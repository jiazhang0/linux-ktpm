/*
 * Copyright (C) 2017 Lans Zhang <jia.zhang@windriver.com>
 *
 * Author:
 *	Lans Zhang <jia.zhang@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ktpm-tpm2.h"

#if 0
struct tpm2_get_tpm_pt_out {
	u8	more_data;
	__be32	subcap_id;
	__be32	property_cnt;
	__be32	property_id;
	__be32	value;
} __packed;

union tpm2_cmd_params {
	struct	tpm2_get_tpm_pt_out	get_tpm_pt_out;
};

state, capabilities, and properties

TPM_CAP (u32) capability
group selection; determines the format of the response

UINT32 property 
further definition of information

UINT32 propertyCount
number of properties of the indicated type to return

#endif

/*
 * Returned data
 * @u8 moreData: flag to indicate if there are more values of this type
 * @TPMS_CAPABILITY_DATA capabilityData: the capability data
	u32	capability;	// the capability
	TPMU_CAPABILITIES	data;	 the capability data

typedef union {
        TPML_ALG_PROPERTY       algorithms;
        TPML_HANDLE     handles;
        TPML_CCA        command;
        TPML_CC ppCommands;
        TPML_CC auditCommands;
        TPML_TAGGED_TPM_PROPERTY        tpmProperties;
        TPML_TAGGED_PCR_PROPERTY        pcrProperties;
        TPML_ECC_CURVE  eccCurves;
} TPMU_CAPABILITIES;

typedef struct {
        u32 count;
        TPMS_PCR_SELECTION pcr_selections[HASH_COUNT];
} TPML_PCR_SELECTION;

typedef struct {
        u16   hash;
        u8      sizeofSelect;
        u8      pcrSelect[PCR_SELECT_MAX];
} TPMS_PCR_SELECTION
 */

static void
set_pcr_bank_allocation(struct tpm_buf *resp, u16 hash_alg, u8 size_of_select,
			u8 last_select)
{
	u8 i;

	tpm_buf_append_u16(resp, hash_alg);
	tpm_buf_append_u8(resp, size_of_select);

	for (i = 0; i < size_of_select - 1; ++i)
		tpm_buf_append_u8(resp, 0xff);

	if (!last_select)
		tpm_buf_append_u8(resp, 0xff);
	else
		tpm_buf_append_u8(resp, last_select);
}

/*
 * Always respond to this command with the full PCR allocation.
 */
static void
capability_get_pcr_allocation(struct ktpm_context *ctx, u32 capability,
			      struct tpm_buf *resp)
{
	u8 size_of_select;
	u8 last_select;

	/* moreData is always NO */
	tpm_buf_append_u8(resp, 0);
	tpm_buf_append_u32(resp, capability);
	tpm_buf_append_u32(resp, ctx->pcr_hash_algs);

	size_of_select = ALIGN(ctx->pcrs_per_bank, 8) / 8;
	last_select = (1 << (ctx->pcrs_per_bank & 0x7)) - 1;

#ifdef CONFIG_CRYPTO_SHA1
	set_pcr_bank_allocation(resp, TPM2_ALG_SHA1, size_of_select,
				last_select);
#endif

#ifdef CONFIG_CRYPTO_SHA256
	set_pcr_bank_allocation(resp, TPM2_ALG_SHA256, size_of_select,
				last_select);
#endif

#ifdef CONFIG_CRYPTO_SHA384
	set_pcr_bank_allocation(resp, TPM2_ALG_SHA384, size_of_select,
				last_select);
#endif

#ifdef CONFIG_CRYPTO_SHA512
	set_pcr_bank_allocation(resp, TPM2_ALG_SHA512, size_of_select,
				last_select);
#endif

#ifdef CONFIG_CRYPTO_SM3_256
	set_pcr_bank_allocation(resp, TPM2_ALG_SM3_256, size_of_select,
				last_select);
#endif
}

/*
 * TPM2_GetCapability() allows reporting of multiple values in a single
 * call. The values are grouped according to type.
 *
 * @capability: determines the category of data returned.
 * @property: selects the first value of the selected category to be
 *            returned. If there is no property that corresponds to the
 *            value of property, the next higher value is returned, if
 *            it exists.
 * @propertyCount: indicates the number of capabilities in the indicated
 *                 group that are requested. The TPM will return no more
 *                 than the number of requested values (propertyCount) or
 *                 until the last property of the requested type has been
 *                 returned.
 */
int
ktpm_tpm2_cmd_get_capability(struct ktpm_context *ctx,
			     struct tpm2_cmd *cmd)
{
	struct tpm_input_header *header_in;
	struct tpm_buf resp;
	struct tpm2_get_tpm_pt_in *cap_in;
	u32 capability, property, property_count;
	int rc;

	header_in = &cmd->header.in;
	rc = tpm_buf_init(&resp);
        if (rc)
		return rc;

	tpm_buf_init_resp(&resp, be16_to_cpu(header_in->tag),
			  be32_to_cpu(TPM2_RC_SUCCESS));

	cap_in = &cmd->params.get_tpm_pt_in;
	capability = be32_to_cpu(cap_in->cap_id);
	property = be32_to_cpu(cap_in->property_id);
	property_count = be32_to_cpu(cap_in->property_cnt);

	dev_dbg(ctx->dev, "TPM2_GetCapability(): capability: %#x, "
		"property %#x, propertyCount %d\n", capability, property,
		property_count);

	if (!property_count)
		dev_warn(ctx->dev, "The requesting propertyCount is zero?\n");

	switch (capability) {
	case TPM2_CAP_PCRS:
		/*
		 * According to the spec, ignore the check against
		 * propertyCount.
		 */
		if (property) {
			tpm_buf_init_resp(&resp, be16_to_cpu(header_in->tag),
					  TPM2_RC_VALUE | TPM2_RC_P |
					  TPM2_RC_2);
			return 0;
		}

		capability_get_pcr_allocation(ctx, capability, &resp);
		break;
//	case TPM2_CAP_TPM_PROPERTIES:
//		break;
	default:
		rc = -ENOENT;
	}

	memcpy(cmd, resp.data, tpm_buf_length(&resp));
	tpm_buf_destroy(&resp);

	return rc;
}

/*
 * TPM2_TestParms() is used to determine if a TPM supports a particular
 * combination of algorithm parameters.
 */
int
ktpm_tpm2_cmd_test_parms(struct ktpm_context *ctx,
			 struct tpm2_cmd *cmd)
{
	return 0;
}
