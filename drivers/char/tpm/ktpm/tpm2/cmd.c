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

int
ktpm_tpm2_dispatch_cmd(struct ktpm_context *ctx, u8 *buf, size_t len)
{
	struct tpm2_cmd *cmd = (struct tpm2_cmd *)buf;
	struct tpm_input_header *header;
	u32 ordinal;
	int rc;

	header = &cmd->header.in;
	ordinal = be32_to_cpu(header->ordinal);

	dev_dbg(ctx->dev, "ktpm_send(): tag %#hx, length %#x/%#lx, "
		"ordinal %#x\n", be32_to_cpu(header->tag),
		be32_to_cpu(header->length), len,
		ordinal);

	/* FIXME: move this logic into the internal */
	if (tpm2_state_in_field_upgrade_mode(ctx) == false) {
		if (ordinal != TPM2_CC_STARTUP) {
			if (tpm2_state_in_failure_mode(ctx) == true) {
				if (ordinal != TPM2_CC_GET_TEST_RESULT &&
				    ordinal != TPM2_CC_GET_CAPABILITY) {
					dev_err(ctx->dev, "the next expected "
						"command after _TPM_init must "
						"be TPM2_Startup() rather "
						"than %#x\n", ordinal);
					return -EFAULT;
				}
			}
		}
	} else if (ordinal != TPM2_CC_FIELD_UPGRADE_DATA) {
		if (ordinal != TPM2_CC_GET_TEST_RESULT &&
		    ordinal != TPM2_CC_GET_CAPABILITY) {
			dev_err(ctx->dev, "the next expected command "
				"after _TPM_init must be "
				"TPM2_FieldUpgradeData() rather than "
				"%#x\n", ordinal);
			return -EFAULT;
		}
	}

	switch (ordinal) {
	case TPM2_CC_GET_CAPABILITY:
		rc = ktpm_tpm2_cmd_get_capability(ctx, cmd);
		break;
	case TPM2_CC_PCR_READ:
		rc = ktpm_tpm2_cmd_pcr_read(ctx, cmd);
		break;
	case TPM2_CC_STARTUP:
		rc = ktpm_tpm2_cmd_startup(ctx, cmd);
		break;
	case TPM2_CC_SELF_TEST:
		rc = ktpm_tpm2_cmd_selftest(ctx, cmd);
		break;
	default:
		dev_err(ctx->dev, "unsupported ordinal %#x\n", ordinal);
		return -ENOENT;
	}

	return rc;
}

int
ktpm_tpm2_retrieve_response(struct ktpm_context *ctx, u8 *buf, size_t len)
{
	//struct tpm_output_header *header = (struct tpm_output_header *)buf;

	//header->tag = cpu_to_be16(ctx->tag);
	//header->length = cpu_to_be32(sizeof(*header));
	//header->return_code = cpu_to_be32(TPM2_RC_SUCCESS);
	return 0;
}
