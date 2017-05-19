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

#include "../ktpm.h"

bool tpm2_state_in_field_upgrade_mode(struct ktpm_context *ctx)
{
	return ctx->field_upgrade_mode;
}

bool tpm2_state_in_failure_mode(struct ktpm_context *ctx)
{
	return ctx->failure_mode;
}

bool tpm2_state_in_lockout_mode(struct ktpm_context *ctx)
{
	return ctx->lockout_mode;
}
