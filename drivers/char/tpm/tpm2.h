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

#ifndef __TPM2_H__
#define __TPM2_H__

#include "tpm.h"

enum tpm2_object_attributes {
	TPM2_OA_USER_WITH_AUTH		= BIT(6),
};

enum tpm2_session_attributes {
	TPM2_SA_CONTINUE_SESSION	= BIT(0),
};

struct tpm2_startup_in {
	__be16	startup_type;
} __packed;

struct tpm2_self_test_in {
	u8	full_test;
} __packed;

struct tpm2_pcr_read_in {
	__be32	pcr_selects_cnt;
	__be16	hash_alg;
	u8	pcr_select_size;
	u8	pcr_select[TPM2_PCR_SELECT_MIN];
} __packed;

struct tpm2_pcr_read_out {
	__be32	update_cnt;
	__be32	pcr_selects_cnt;
	__be16	hash_alg;
	u8	pcr_select_size;
	u8	pcr_select[TPM2_PCR_SELECT_MIN];
	__be32	digests_cnt;
	__be16	digest_size;
	u8	digest[TPM_DIGEST_SIZE];
} __packed;

struct tpm2_pcr_selection {
	__be16	hash_alg;
	u8	pcr_select_size;
	u8	pcr_select[0];
} __packed;

struct tpm2_digests {
	__be32			digests_cnt;
	struct tpm2_digest	digests[0];
} __packed;

struct tpm2_get_tpm_pt_in {
	__be32	cap_id;
	__be32	property_id;
	__be32	property_cnt;
} __packed;

struct tpm2_get_tpm_pt_out {
	u8	more_data;
	__be32	subcap_id;
	__be32	property_cnt;
	__be32	property_id;
	__be32	value;
} __packed;

struct tpm2_get_random_in {
	__be16	size;
} __packed;

struct tpm2_get_random_out {
	__be16	size;
	u8	buffer[TPM_MAX_RNG_DATA];
} __packed;

union tpm2_cmd_params {
	struct	tpm2_startup_in		startup_in;
	struct	tpm2_self_test_in	selftest_in;
	struct	tpm2_pcr_read_in	pcrread_in;
	struct	tpm2_pcr_read_out	pcrread_out;
	struct	tpm2_get_tpm_pt_in	get_tpm_pt_in;
	struct	tpm2_get_tpm_pt_out	get_tpm_pt_out;
	struct	tpm2_get_random_in	getrandom_in;
	struct	tpm2_get_random_out	getrandom_out;
};

struct tpm2_cmd {
	tpm_cmd_header		header;
	union tpm2_cmd_params	params;
} __packed;

struct tpm2_hash {
	unsigned int crypto_id;
	unsigned int tpm_id;
};

static inline u16 tpm2_hash_size(enum tpm2_algorithms hash_alg)
{
	switch (hash_alg) {
	case TPM2_ALG_SHA1:
		return SHA1_DIGEST_SIZE;
	case TPM2_ALG_SHA256:
		return SHA256_DIGEST_SIZE;
	case TPM2_ALG_SHA384:
		return SHA384_DIGEST_SIZE;
	case TPM2_ALG_SHA512:
		return SHA512_DIGEST_SIZE;
	case TPM2_ALG_SM3_256:
		return SM3256_DIGEST_SIZE;
	default:
		break;
	}

	return 0;
}

#define TPM2_PCR_MAX_BANKS	5

#endif
