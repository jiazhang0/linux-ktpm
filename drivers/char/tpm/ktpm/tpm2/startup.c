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

static void init_pcr_banks(struct ktpm_context *ctx)
{
	unsigned int hash_algs = 0;

	/* XXX: handle the case if using kernel module */
#ifdef CONFIG_CRYPTO_SHA1
	memset(ctx->pcr_bank_sha1, 0, sizeof(ctx->pcr_bank_sha1));
	++hash_algs;
#endif

#ifdef CONFIG_CRYPTO_SHA256
	memset(ctx->pcr_bank_sha256, 0, sizeof(ctx->pcr_bank_sha256));
	++hash_algs;
#endif

#ifdef CONFIG_CRYPTO_SHA384
	memset(ctx->pcr_bank_sha384, 0, sizeof(ctx->pcr_bank_sha384));
	++hash_algs;
#endif

#ifdef CONFIG_CRYPTO_SHA512
	memset(ctx->pcr_bank_sha512, 0, sizeof(ctx->pcr_bank_sha512));
	++hash_algs;
#endif

#ifdef CONFIG_CRYPTO_SM3_256
	memset(ctx->pcr_bank_sm3_256, 0, sizeof(ctx->pcr_bank_sm3_256));
	++hash_algs;
#endif

	ctx->pcr_hash_algs = hash_algs;

#ifdef CONFIG_TCG_KTPM_TPM2
	ctx->pcrs_per_bank = TPM2_PLATFORM_PCR;
#endif

	ctx->pcr_update_cnt = 0;
}

int
ktpm_tpm2_cmd_init(struct ktpm_context *ctx)
{
	ctx->field_upgrade_mode = false;
	ctx->failure_mode = false;
	ctx->lockout_mode = false;

	init_pcr_banks(ctx);

	//if (0)
	//	ktpm_tpm2_cmd_selftest(ctx);

	return 0;
}

int
ktpm_tpm2_cmd_startup(struct ktpm_context *ctx, struct tpm2_cmd *cmd)
{
	static bool succeeded = false;
	u16 startup_type;

	startup_type = be16_to_cpu(cmd->params.startup_in.startup_type);

	if (succeeded == true) {
		dev_dbg(ctx->dev, "TPM2_Startup() command is not allowed "
			"after it has completed successfully");
		return 0;
	}

	dev_dbg(ctx->dev, "TPM2_Startup(): startupType: %#hx\n",
		startup_type);

	/* FIXME: implement TPM Restart */
	if (startup_type == TPM_SU_CLEAR) {
		/* Most values are placed in a default initial state,
		 * but persistent values are retained. Any value that is
		 * not required by this specification to be kept in NV
		 * memory is reinitialized. In some cases, this means that
		 * values are cleared, in others it means that new random
		 * values are selected.
		 */

		/*
		 * validate the integrity of any NV values before those
		 * values are used before that state is used.
		 */

		/*
		 * When the TPM determines that some NV value required for
		 * proper TPM operation is not valid, the TPM will enter
		 * Failure Mode.
		 */
		if (0)
			ctx->failure_mode = true;
	}

	cmd->header.out.length = cpu_to_be32(sizeof(cmd->header.out));
	cmd->header.out.return_code = cpu_to_be32(TPM2_RC_SUCCESS);

	succeeded = true;

	return 0;
}

int
ktpm_tpm2_cmd_shutdown(struct ktpm_context *ctx, struct tpm2_cmd *cmd)
{
	bool clear = false;	// shutdown clear ?

	if (clear == true)	// case TPM Reset or Restart
		;
	else	// case TPM Resume
		;

	//pstore_save_state();
	return 0;
}
