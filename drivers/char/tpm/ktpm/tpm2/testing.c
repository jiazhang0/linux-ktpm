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

enum tpm2_return_codes
do_full_test(struct ktpm_context *ctx)
{
	return TPM2_RC_SUCCESS;
}

/*
 * Perform a test of TPM's capabilities.
 * If the fullTest is YES, the TPM will test all functions.
 * If fullTest = NO, the TPM will only test those functions
 * that have not previously been tested.
 */
int
ktpm_tpm2_cmd_selftest(struct ktpm_context *ctx, struct tpm2_cmd *cmd)
{
	u8 full_test = cmd->params.selftest_in.full_test;
	struct tpm_output_header *out;
	enum tpm2_return_codes rc;

	dev_dbg(ctx->dev, "TPM2_SelfTest(): fullTest: 0x%hhx\n",
		full_test);

	if (full_test == YES)
		rc = do_full_test(ctx);
	else {
		/*
		 * If fullTest is NO, and all functions have been tested, the
		 * TPM shall return TPM_RC_SUCCESS.
		 */
		rc = TPM2_RC_SUCCESS;
	}

	if (rc != TPM2_RC_SUCCESS)
		ctx->failure_mode = true;

	out = &cmd->header.out;
	out->length = cpu_to_be32(sizeof(*out));
	out->return_code = cpu_to_be32(TPM2_RC_SUCCESS);

	return 0;
}
