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

#ifndef __KTPM_TPM2_H__
#define __KTPM_TPM2_H__

#include "../ktpm.h"
#include "sapi/tpm20.h"

#if 0
struct tpm2_buffer {
	struct tpm_input_header *cmd_header;
	struct tpm_output_header *resp_header;
	void *parameter;
};

static inline void
tb_init(struct tpm2_buffer *tb, u8 *buf)
{
	tpm_cmd_header *header = (tpm_cmd_header *)buf;

	tb->cmd_header = &header->in;
	tb->resp_header = &header->out;
	tb->parameter = tb->cmd_header + 1;
}

static inline struct tpm_input_header *
tb_cmd_header(struct tpm2_buffer *tb)
{
	return tb->cmd_header;
}

static inline struct tpm_output_header *
tb_resp_header(struct tpm2_buffer *tb)
{
	return tb->resp_header;
}

#define tb_next_parameter(tb, type)	\
	({	\
		type *__p = (type *)(tb)->parameter;	\
		(tb)->parameter += sizeof(type);	\
		__p;	\
	})
#endif

int
ktpm_tpm2_cmd_init(struct ktpm_context *ctx);

int
ktpm_tpm2_cmd_startup(struct ktpm_context *ctx, struct tpm2_cmd *cmd);

int
ktpm_tpm2_cmd_shutdown(struct ktpm_context *ctx, struct tpm2_cmd *cmd);

int
ktpm_tpm2_cmd_selftest(struct ktpm_context *ctx,
		       struct tpm2_cmd *cmd);

int
ktpm_tpm2_cmd_pcr_read(struct ktpm_context *ctx,
		       struct tpm2_cmd *cmd);

int
ktpm_tpm2_cmd_get_capability(struct ktpm_context *ctx,
			     struct tpm2_cmd *cmd);

#endif
