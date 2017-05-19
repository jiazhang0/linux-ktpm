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

u8 *
pcr_get(struct ktpm_context *ctx, enum tpm2_algorithms hash_alg,
	unsigned int idx)
{
	switch (hash_alg) {
#ifdef CONFIG_CRYPTO_SHA1
	case TPM2_ALG_SHA1:
		return ctx->pcr_bank_sha1[idx];
#endif
#ifdef CONFIG_CRYPTO_SHA256
	case TPM2_ALG_SHA256:
		return ctx->pcr_bank_sha256[idx];
#endif
#ifdef CONFIG_CRYPTO_SHA384
	case TPM2_ALG_SHA384:
		return ctx->pcr_bank_sha384[idx];
#endif
#ifdef CONFIG_CRYPTO_SHA512
	case TPM2_ALG_SHA512:
		return ctx->pcr_bank_sha512[idx];
#endif
#ifdef CONFIG_CRYPTO_SM3_256
	case TPM2_ALG_SM3_256:
		return ctx->pcr_bank_sm3_256[idx];
#endif
	default:
		break;
	}

	return NULL;
}

static void
pcr_read_bank(struct ktpm_context *ctx, enum tpm2_algorithms hash_alg,
	      u8 *pcr_select, u8 pcr_select_size, struct tpm_buf *resp)
{
	u16 hash_size = tpm2_hash_size(hash_alg);
	u8 i;

	for (i = 0; i < pcr_select_size; ++i) {
		unsigned long idx;
		unsigned long select = (unsigned long)pcr_select[i];

		if (!select)
			continue;

		idx = 0;

		while (1) {
			unsigned long pcr_idx;

			if (idx)
				idx = find_next_bit(&select, 8,
						    idx);
			else
				idx = find_first_bit(&select, 8);

			if (idx == 8)
				break;

			pcr_idx = i * 8 + idx;

			if (!resp)
				pr_debug(" %ld", pcr_idx);
			else
				tpm_buf_append(resp,
					       pcr_get(ctx, hash_alg, pcr_idx),
					       hash_size);

			if (++idx == 8)
				break;
		}
	}
}

int
ktpm_tpm2_cmd_pcr_read(struct ktpm_context *ctx,
		       struct tpm2_cmd *cmd)
{
	struct tpm_input_header *header_in;
	struct tpm_buf resp;
	struct tpm2_pcr_read_in *pcrs_in;
	struct tpm2_pcr_selection *pcr_selection;
	u32 i, count, *p_digests_cnt;
	int rc;

	header_in = &cmd->header.in;
	rc = tpm_buf_init(&resp);
        if (rc)
		return rc;

	tpm_buf_init_resp(&resp, be16_to_cpu(header_in->tag),
			  be32_to_cpu(TPM2_RC_SUCCESS));

	pcrs_in = &cmd->params.pcrread_in;
	count = be32_to_cpu(pcrs_in->pcr_selects_cnt);

	dev_dbg(ctx->dev, "TPM2_PCR_Read(): count: %d\n", count);

	tpm_buf_append_u32(&resp, ctx->pcr_update_cnt);
	tpm_buf_append_u32(&resp, count);

	pcr_selection = (struct tpm2_pcr_selection *)&pcrs_in->hash_alg;

	for (i = 0; i < count; ++i) {
		enum tpm2_algorithms hash_alg;
		u8 pcr_select_size;
		u16 hash_size;

		hash_alg = be16_to_cpu(pcr_selection->hash_alg);
		tpm_buf_append_u16(&resp, hash_alg);

		pcr_select_size = pcr_selection->pcr_select_size;
		tpm_buf_append_u8(&resp, pcr_select_size);

		dev_dbg(ctx->dev, "TPM2_PCR_Read(): [%d] hash: 0x%x, "
			"pcr selection size %d, selected PCRs", i, hash_alg,
			pcr_select_size);

		hash_size = tpm2_hash_size(hash_alg);

		if (hash_size) {
			tpm_buf_append(&resp, pcr_selection->pcr_select,
				       pcr_select_size);

			pcr_read_bank(ctx, hash_alg, pcr_selection->pcr_select,
				      pcr_select_size, NULL);
		} else
			tpm_buf_memset(&resp, 0, pcr_select_size);

		pcr_selection = (struct tpm2_pcr_selection *)
				((u8 *)(pcr_selection + 1) + pcr_select_size);
	}

	p_digests_cnt = (u32 *)tpm_buf_reserve(&resp, sizeof(*p_digests_cnt));
	if (!p_digests_cnt) {
		tpm_buf_destroy(&resp);
		return -EFAULT;
	}

	*p_digests_cnt = 0;

	pcr_selection = (struct tpm2_pcr_selection *)&pcrs_in->hash_alg;

	for (i = 0; i < count; ++i) {
		enum tpm2_algorithms hash_alg;
		u8 pcr_select_size;
		u16 hash_size;

		hash_alg = be16_to_cpu(pcr_selection->hash_alg);
		pcr_select_size = pcr_selection->pcr_select_size;

		hash_size = tpm2_hash_size(hash_alg);

		if (hash_size) {
			tpm_buf_append_u16(&resp, hash_size);

			pcr_read_bank(ctx, hash_alg, pcr_selection->pcr_select,
				      pcr_select_size, &resp);

			++(*p_digests_cnt);
		}

		pcr_selection = (struct tpm2_pcr_selection *)
				((u8 *)(pcr_selection + 1) + pcr_select_size);
	}

	*p_digests_cnt = cpu_to_be32(*p_digests_cnt);
	memcpy(cmd, resp.data, tpm_buf_length(&resp));

	tpm_buf_destroy(&resp);

	return 0;
}
