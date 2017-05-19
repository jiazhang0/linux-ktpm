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

#ifndef __KTPM_H__
#define __KTPM_H__

#define DEBUG

#include "../tpm.h"

#ifdef CONFIG_TCG_KTPM_TPM2
#include "../tpm2.h"

struct ktpm_context {
	struct device *dev;

	bool failure_mode;
	bool lockout_mode;
	bool field_upgrade_mode;
	/* PCR */
	unsigned int pcrs_per_bank;
	u32 pcr_hash_algs;
	u32 pcr_update_cnt;
#ifdef CONFIG_CRYPTO_SHA1
	u8 pcr_bank_sha1[TPM2_PLATFORM_PCR][SHA1_DIGEST_SIZE];
#endif
#ifdef CONFIG_CRYPTO_SHA256
	u8 pcr_bank_sha256[TPM2_PLATFORM_PCR][SHA256_DIGEST_SIZE];
#endif
#ifdef CONFIG_CRYPTO_SHA384
	u8 pcr_bank_sha384[TPM2_PLATFORM_PCR][SHA384_DIGEST_SIZE];
#endif
#ifdef CONFIG_CRYPTO_SHA512
	u8 pcr_bank_sha512[TPM2_PLATFORM_PCR][SHA512_DIGEST_SIZE];
#endif
#ifdef CONFIG_CRYPTO_SM3_256
	u8 pcr_bank_sm3_256[TPM2_PLATFORM_PCR][SM3256_DIGEST_SIZE];
#endif
};
#endif

int
ktpm_tpm2_cmd_init(struct ktpm_context *ctx);

int
ktpm_tpm2_dispatch_cmd(struct ktpm_context *ctx, u8 *buf, size_t len);

int
ktpm_tpm2_retrieve_response(struct ktpm_context *ctx, u8 *buf, size_t len);

bool
tpm2_state_in_field_upgrade_mode(struct ktpm_context *ctx);

bool
tpm2_state_in_failure_mode(struct ktpm_context *ctx);

bool
tpm2_state_in_lockout_mode(struct ktpm_context *ctx);

#endif	/* __KTPM_H__ */
