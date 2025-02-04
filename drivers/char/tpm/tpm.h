/*
 * Copyright (C) 2004 IBM Corporation
 * Copyright (C) 2015 Intel Corporation
 *
 * Authors:
 * Leendert van Doorn <leendert@watson.ibm.com>
 * Dave Safford <safford@watson.ibm.com>
 * Reiner Sailer <sailer@watson.ibm.com>
 * Kylene Hall <kjhall@us.ibm.com>
 *
 * Maintained by: <tpmdd-devel@lists.sourceforge.net>
 *
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 */

#ifndef __TPM_H__
#define __TPM_H__

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/tpm.h>
#include <linux/acpi.h>
#include <linux/cdev.h>
#include <linux/highmem.h>
#include <crypto/hash_info.h>

enum tpm_const {
	TPM_MINOR = 224,	/* officially assigned */
	TPM_BUFSIZE = 4096,
	TPM_NUM_DEVICES = 65536,
	TPM_RETRY = 50,		/* 5 seconds */
	TPM_NUM_EVENT_LOG_FILES = 3,
};

enum tpm_timeout {
	TPM_TIMEOUT = 5,	/* msecs */
	TPM_TIMEOUT_RETRY = 100 /* msecs */
};

/* TPM addresses */
enum tpm_addr {
	TPM_SUPERIO_ADDR = 0x2E,
	TPM_ADDR = 0x4E,
};

/* Indexes the duration array */
enum tpm_duration {
	TPM_SHORT = 0,
	TPM_MEDIUM = 1,
	TPM_LONG = 2,
	TPM_UNDEFINED,
};

#define TPM_WARN_RETRY          0x800
#define TPM_WARN_DOING_SELFTEST 0x802
#define TPM_ERR_DEACTIVATED     0x6
#define TPM_ERR_DISABLED        0x7
#define TPM_ERR_INVALID_POSTINIT 38

#define TPM_HEADER_SIZE		10

enum tpm2_const {
	TPM2_PLATFORM_PCR	= 24,
	TPM2_PCR_SELECT_MIN	= ((TPM2_PLATFORM_PCR + 7) / 8),
	TPM2_TIMEOUT_A		= 750,
	TPM2_TIMEOUT_B		= 2000,
	TPM2_TIMEOUT_C		= 200,
	TPM2_TIMEOUT_D		= 30,
	TPM2_DURATION_SHORT	= 20,
	TPM2_DURATION_MEDIUM	= 750,
	TPM2_DURATION_LONG	= 2000,
	TPM2_DIGEST_SIZE_MAX	= SHA512_DIGEST_SIZE,
};

enum tpm2_structures {
	TPM2_ST_NO_SESSIONS	= 0x8001,
	TPM2_ST_SESSIONS	= 0x8002,
};

enum tpm2_return_codes {
	TPM2_RC_SUCCESS		= 0x000,
	/* defined for compatibility with TPM 1.2 */
	TPM2_RC_BAD_TAG		= 0x01e,
	/* Format 0 response codes */
	TPM2_RC_VER1		= 0x100,
	TPM2_RC_INITIALIZE	= (TPM2_RC_VER1 + 0x00),
	TPM2_RC_FAILURE		= (TPM2_RC_VER1 + 0x01),
	TPM2_RC_SEQUENCE	= (TPM2_RC_VER1 + 0x03),
	TPM2_RC_PRIVATE		= (TPM2_RC_VER1 + 0x0b),
	TPM2_RC_HMAC		= (TPM2_RC_VER1 + 0x19),
	TPM2_RC_DISABLED	= (TPM2_RC_VER1 + 0x20),
	TPM2_RC_EXCLUSIVE	= (TPM2_RC_VER1 + 0x21),
	TPM2_RC_AUTH_TYPE	= (TPM2_RC_VER1 + 0x24),
	TPM2_RC_AUTH_MISSING	= (TPM2_RC_VER1 + 0x25),
	TPM2_RC_POLICY		= (TPM2_RC_VER1 + 0x26),
	TPM2_RC_PCR		= (TPM2_RC_VER1 + 0x27),
	TPM2_RC_PCR_CHANGED	= (TPM2_RC_VER1 + 0x28),
	TPM2_RC_UPGRADE		= (TPM2_RC_VER1 + 0x2d),
	TPM2_RC_TOO_MANY_CONTEXTS = (TPM2_RC_VER1 + 0x2e),
	TPM2_RC_AUTH_UNAVAILABLE = (TPM2_RC_VER1 + 0x2f),
	TPM2_RC_REBOOT		= (TPM2_RC_VER1 + 0x30),
	TPM2_RC_UNBALANCED	= (TPM2_RC_VER1 + 0x31),
	TPM2_RC_COMMAND_SIZE	= (TPM2_RC_VER1 + 0x42),
	TPM2_RC_COMMAND_CODE	= (TPM2_RC_VER1 + 0x43),
	TPM2_RC_AUTHSIZE	= (TPM2_RC_VER1 + 0x44),
	TPM2_RC_AUTH_CONTEXT	= (TPM2_RC_VER1 + 0x45),
	TPM2_RC_NV_RANGE	= (TPM2_RC_VER1 + 0x46),
	TPM2_RC_NV_SIZE		= (TPM2_RC_VER1 + 0x47),
	TPM2_RC_NV_LOCKED	= (TPM2_RC_VER1 + 0x48),
	TPM2_RC_NV_AUTHORIZATION = (TPM2_RC_VER1 + 0x49),
	TPM2_RC_NV_UNINITIALIZED = (TPM2_RC_VER1 + 0x4a),
	TPM2_RC_NV_SPACE	= (TPM2_RC_VER1 + 0x4b),
	TPM2_RC_NV_DEFINED	= (TPM2_RC_VER1 + 0x4c),
	TPM2_RC_BAD_CONTEXT	= (TPM2_RC_VER1 + 0x50),
	TPM2_RC_CPHASH		= (TPM2_RC_VER1 + 0x51),
	TPM2_RC_PARENT		= (TPM2_RC_VER1 + 0x52),
	TPM2_RC_NEEDS_TEST	= (TPM2_RC_VER1 + 0x53),
	TPM2_RC_NO_RESULT	= (TPM2_RC_VER1 + 0x54),
	TPM2_RC_SENSITIVE	= (TPM2_RC_VER1 + 0x55),
	TPM2_MAX_FM0		= (TPM2_RC_VER1 + 0x7f),
	/* Format 1 response codes */
	TPM2_RC_FMT1		= 0x080,
	TPM2_RC_ASYMMETRIC	= (TPM2_RC_FMT1 + 0x01),
	TPM2_RC_ATTRIBUTES	= (TPM2_RC_FMT1 + 0x02),
	TPM2_RC_HASH		= (TPM2_RC_FMT1 + 0x03),
	TPM2_RC_VALUE		= (TPM2_RC_FMT1 + 0x04),
	TPM2_RC_HIERARCHY	= (TPM2_RC_FMT1 + 0x05),
	TPM2_RC_KEY_SIZE	= (TPM2_RC_FMT1 + 0x07),
	TPM2_RC_MGF		= (TPM2_RC_FMT1 + 0x08),
	TPM2_RC_MODE		= (TPM2_RC_FMT1 + 0x09),
	TPM2_RC_TYPE		= (TPM2_RC_FMT1 + 0x0a),
	TPM2_RC_HANDLE		= (TPM2_RC_FMT1 + 0x0b),
	TPM2_RC_KDF		= (TPM2_RC_FMT1 + 0x0c),
	TPM2_RC_RANGE		= (TPM2_RC_FMT1 + 0x0d),
	TPM2_RC_AUTH_FAIL	= (TPM2_RC_FMT1 + 0x0e),
	TPM2_RC_NONCE		= (TPM2_RC_FMT1 + 0x0f),
	TPM2_RC_PP		= (TPM2_RC_FMT1 + 0x10),
	TPM2_RC_SCHEME		= (TPM2_RC_FMT1 + 0x12),
	TPM2_RC_SIZE		= (TPM2_RC_FMT1 + 0x15),
	TPM2_RC_SYMMETRIC	= (TPM2_RC_FMT1 + 0x16),
	TPM2_RC_TAG		= (TPM2_RC_FMT1 + 0x17),
	TPM2_RC_SELECTOR	= (TPM2_RC_FMT1 + 0x18),
	TPM2_RC_INSUFFICIENT	= (TPM2_RC_FMT1 + 0x1a),
	TPM2_RC_SIGNATURE	= (TPM2_RC_FMT1 + 0x1b),
	TPM2_RC_KEY		= (TPM2_RC_FMT1 + 0x1c),
	TPM2_RC_POLICY_FAIL	= (TPM2_RC_FMT1 + 0x1d),
	TPM2_RC_INTEGRITY	= (TPM2_RC_FMT1 + 0x1f),
	TPM2_RC_TICKET		= (TPM2_RC_FMT1 + 0x20),
	TPM2_RC_RESERVED_BITS	= (TPM2_RC_FMT1 + 0x21),
	TPM2_RC_BAD_AUTH	= (TPM2_RC_FMT1 + 0x22),
	TPM2_RC_EXPIRED		= (TPM2_RC_FMT1 + 0x23),
	TPM2_RC_POLICY_CC	= (TPM2_RC_FMT1 + 0x24),
	TPM2_RC_BINDING		= (TPM2_RC_FMT1 + 0x25),
	TPM2_RC_CURVE		= (TPM2_RC_FMT1 + 0x26),
	TPM2_RC_ECC_POINT	= (TPM2_RC_FMT1 + 0x27),
	/* Warning response codes */
	TPM2_RC_WARN		= 0x900,
	TPM2_RC_CONTEXT_GAP	= (TPM2_RC_WARN + 0x01),
	TPM2_RC_OBJECT_MEMORY	= (TPM2_RC_WARN + 0x02),
	TPM2_RC_SESSION_MEMORY	= (TPM2_RC_WARN + 0x03),
	TPM2_RC_MEMORY		= (TPM2_RC_WARN + 0x04),
	TPM2_RC_SESSION_HANDLES	= (TPM2_RC_WARN + 0x05),
	TPM2_RC_OBJECT_HANDLES	= (TPM2_RC_WARN + 0x06),
	TPM2_RC_LOCALITY	= (TPM2_RC_WARN + 0x07),
	TPM2_RC_YIELDED		= (TPM2_RC_WARN + 0x08),
	TPM2_RC_CANCELED	= (TPM2_RC_WARN + 0x09),
	TPM2_RC_TESTING		= (TPM2_RC_WARN + 0x0a),
	TPM2_RC_REFERENCE_H0	= (TPM2_RC_WARN + 0x10),
	TPM2_RC_REFERENCE_H1	= (TPM2_RC_WARN + 0x11),
	TPM2_RC_REFERENCE_H2	= (TPM2_RC_WARN + 0x12),
	TPM2_RC_REFERENCE_H3	= (TPM2_RC_WARN + 0x13),
	TPM2_RC_REFERENCE_H4	= (TPM2_RC_WARN + 0x14),
	TPM2_RC_REFERENCE_H5	= (TPM2_RC_WARN + 0x15),
	TPM2_RC_REFERENCE_H6	= (TPM2_RC_WARN + 0x16),
	TPM2_RC_REFERENCE_S0	= (TPM2_RC_WARN + 0x18),
	TPM2_RC_REFERENCE_S1	= (TPM2_RC_WARN + 0x19),
	TPM2_RC_REFERENCE_S2	= (TPM2_RC_WARN + 0x1a),
	TPM2_RC_REFERENCE_S3	= (TPM2_RC_WARN + 0x1b),
	TPM2_RC_REFERENCE_S4	= (TPM2_RC_WARN + 0x1c),
	TPM2_RC_REFERENCE_S5	= (TPM2_RC_WARN + 0x1d),
	TPM2_RC_REFERENCE_S6	= (TPM2_RC_WARN + 0x1e),
	TPM2_RC_NV_RATE		= (TPM2_RC_WARN + 0x20),
	TPM2_RC_LOCKOUT		= (TPM2_RC_WARN + 0x21),
	TPM2_RC_RETRY		= (TPM2_RC_WARN + 0x22),
	TPM2_RC_NV_UNAVAILABLE	= (TPM2_RC_WARN + 0x23),
	TPM2_RC_NOT_USED	= (TPM2_RC_WARN + 0x7f),
	/* Additional Defines */
	TPM2_RC_H		= 0x000,
	TPM2_RC_P		= 0x040,
	TPM2_RC_S		= 0x800,
	TPM2_RC_1		= 0x100,
	TPM2_RC_2		= 0x200,
	TPM2_RC_3		= 0x300,
	TPM2_RC_4		= 0x400,
	TPM2_RC_5		= 0x500,
	TPM2_RC_6		= 0x600,
	TPM2_RC_7		= 0x700,
	TPM2_RC_8		= 0x800,
	TPM2_RC_9		= 0x900,
	TPM2_RC_A		= 0xa00,
	TPM2_RC_B		= 0xb00,
	TPM2_RC_C		= 0xc00,
	TPM2_RC_D		= 0xd00,
	TPM2_RC_E		= 0xe00,
	TPM2_RC_F		= 0xf00,
	TPM2_RC_N_MASK		= 0xf00,
};

enum tpm2_algorithms {
	TPM2_ALG_ERROR		= 0x0000,
	TPM2_ALG_SHA1		= 0x0004,
	TPM2_ALG_KEYEDHASH	= 0x0008,
	TPM2_ALG_SHA256		= 0x000B,
	TPM2_ALG_SHA384		= 0x000C,
	TPM2_ALG_SHA512		= 0x000D,
	TPM2_ALG_NULL		= 0x0010,
	TPM2_ALG_SM3_256	= 0x0012,
};

enum tpm2_command_codes {
	TPM2_CC_FIRST			= 0x011F,
	TPM2_CC_FIELD_UPGRADE_DATA	= 0x0141,
	TPM2_CC_SELF_TEST		= 0x0143,
	TPM2_CC_STARTUP			= 0x0144,
	TPM2_CC_SHUTDOWN		= 0x0145,
	TPM2_CC_CREATE			= 0x0153,
	TPM2_CC_LOAD			= 0x0157,
	TPM2_CC_UNSEAL			= 0x015E,
	TPM2_CC_FLUSH_CONTEXT		= 0x0165,
	TPM2_CC_GET_CAPABILITY		= 0x017A,
	TPM2_CC_GET_RANDOM		= 0x017B,
	TPM2_CC_GET_TEST_RESULT		= 0x017C,
	TPM2_CC_PCR_READ		= 0x017E,
	TPM2_CC_POLICY_RESTART		= 0x0180,
	TPM2_CC_PCR_EXTEND		= 0x0182,
	TPM2_CC_LAST			= 0x018F,
};

enum tpm2_permanent_handles {
	TPM2_RS_PW		= 0x40000009,
};

enum tpm2_capabilities {
	TPM2_CAP_PCRS		= 5,
	TPM2_CAP_TPM_PROPERTIES = 6,
};

enum tpm2_startup_types {
	TPM2_SU_CLEAR	= 0x0000,
	TPM2_SU_STATE	= 0x0001,
};

#define TPM_VID_INTEL    0x8086
#define TPM_VID_WINBOND  0x1050
#define TPM_VID_STM      0x104A

#define TPM_PPI_VERSION_LEN		3

enum tpm_chip_flags {
	TPM_CHIP_FLAG_TPM2		= BIT(1),
	TPM_CHIP_FLAG_IRQ		= BIT(2),
	TPM_CHIP_FLAG_VIRTUAL		= BIT(3),
	TPM_CHIP_FLAG_HAVE_TIMEOUTS	= BIT(4),
};

struct tpm_bios_log {
	void *bios_event_log;
	void *bios_event_log_end;
};

struct tpm_chip_seqops {
	struct tpm_chip *chip;
	const struct seq_operations *seqops;
};

struct tpm_chip {
	struct device dev;
	struct cdev cdev;

	/* A driver callback under ops cannot be run unless ops_sem is held
	 * (sometimes implicitly, eg for the sysfs code). ops becomes null
	 * when the driver is unregistered, see tpm_try_get_ops.
	 */
	struct rw_semaphore ops_sem;
	const struct tpm_class_ops *ops;

	struct tpm_bios_log log;
	struct tpm_chip_seqops bin_log_seqops;
	struct tpm_chip_seqops ascii_log_seqops;

	unsigned int flags;

	int dev_num;		/* /dev/tpm# */
	unsigned long is_open;	/* only one allowed */

	struct mutex tpm_mutex;	/* tpm is processing */

	unsigned long timeout_a; /* jiffies */
	unsigned long timeout_b; /* jiffies */
	unsigned long timeout_c; /* jiffies */
	unsigned long timeout_d; /* jiffies */
	bool timeout_adjusted;
	unsigned long duration[3]; /* jiffies */
	bool duration_adjusted;

	struct dentry *bios_dir[TPM_NUM_EVENT_LOG_FILES];

	const struct attribute_group *groups[3];
	unsigned int groups_cnt;

	u16 active_banks[7];
#ifdef CONFIG_ACPI
	acpi_handle acpi_dev_handle;
	char ppi_version[TPM_PPI_VERSION_LEN + 1];
#endif /* CONFIG_ACPI */
};

#define to_tpm_chip(d) container_of(d, struct tpm_chip, dev)

struct tpm_input_header {
	__be16	tag;
	__be32	length;
	__be32	ordinal;
} __packed;

struct tpm_output_header {
	__be16	tag;
	__be32	length;
	__be32	return_code;
} __packed;

#define TPM_TAG_RQU_COMMAND cpu_to_be16(193)

struct	stclear_flags_t {
	__be16	tag;
	u8	deactivated;
	u8	disableForceClear;
	u8	physicalPresence;
	u8	physicalPresenceLock;
	u8	bGlobalLock;
} __packed;

struct	tpm_version_t {
	u8	Major;
	u8	Minor;
	u8	revMajor;
	u8	revMinor;
} __packed;

struct	tpm_version_1_2_t {
	__be16	tag;
	u8	Major;
	u8	Minor;
	u8	revMajor;
	u8	revMinor;
} __packed;

struct	timeout_t {
	__be32	a;
	__be32	b;
	__be32	c;
	__be32	d;
} __packed;

struct duration_t {
	__be32	tpm_short;
	__be32	tpm_medium;
	__be32	tpm_long;
} __packed;

struct permanent_flags_t {
	__be16	tag;
	u8	disable;
	u8	ownership;
	u8	deactivated;
	u8	readPubek;
	u8	disableOwnerClear;
	u8	allowMaintenance;
	u8	physicalPresenceLifetimeLock;
	u8	physicalPresenceHWEnable;
	u8	physicalPresenceCMDEnable;
	u8	CEKPUsed;
	u8	TPMpost;
	u8	TPMpostLock;
	u8	FIPS;
	u8	operator;
	u8	enableRevokeEK;
	u8	nvLocked;
	u8	readSRKPub;
	u8	tpmEstablished;
	u8	maintenanceDone;
	u8	disableFullDALogicInfo;
} __packed;

typedef union {
	struct	permanent_flags_t perm_flags;
	struct	stclear_flags_t	stclear_flags;
	__u8	owned;
	__be32	num_pcrs;
	struct	tpm_version_t	tpm_version;
	struct	tpm_version_1_2_t tpm_version_1_2;
	__be32	manufacturer_id;
	struct timeout_t  timeout;
	struct duration_t duration;
} cap_t;

enum tpm_capabilities {
	TPM_CAP_FLAG = 4,
	TPM_CAP_PROP = 5,
	TPM_CAP_VERSION_1_1 = 0x06,
	TPM_CAP_VERSION_1_2 = 0x1A,
};

enum tpm_sub_capabilities {
	TPM_CAP_PROP_PCR = 0x101,
	TPM_CAP_PROP_MANUFACTURER = 0x103,
	TPM_CAP_FLAG_PERM = 0x108,
	TPM_CAP_FLAG_VOL = 0x109,
	TPM_CAP_PROP_OWNER = 0x111,
	TPM_CAP_PROP_TIS_TIMEOUT = 0x115,
	TPM_CAP_PROP_TIS_DURATION = 0x120,
};

struct	tpm_getcap_params_in {
	__be32	cap;
	__be32	subcap_size;
	__be32	subcap;
} __packed;

struct	tpm_getcap_params_out {
	__be32	cap_size;
	cap_t	cap;
} __packed;

struct	tpm_readpubek_params_out {
	u8	algorithm[4];
	u8	encscheme[2];
	u8	sigscheme[2];
	__be32	paramsize;
	u8	parameters[12]; /*assuming RSA*/
	__be32	keysize;
	u8	modulus[256];
	u8	checksum[20];
} __packed;

typedef union {
	struct	tpm_input_header in;
	struct	tpm_output_header out;
} tpm_cmd_header;

struct tpm_pcrread_out {
	u8	pcr_result[TPM_DIGEST_SIZE];
} __packed;

struct tpm_pcrread_in {
	__be32	pcr_idx;
} __packed;

struct tpm_pcrextend_in {
	__be32	pcr_idx;
	u8	hash[TPM_DIGEST_SIZE];
} __packed;

/* 128 bytes is an arbitrary cap. This could be as large as TPM_BUFSIZE - 18
 * bytes, but 128 is still a relatively large number of random bytes and
 * anything much bigger causes users of struct tpm_cmd_t to start getting
 * compiler warnings about stack frame size. */
#define TPM_MAX_RNG_DATA	128

struct tpm_getrandom_out {
	__be32 rng_data_len;
	u8     rng_data[TPM_MAX_RNG_DATA];
} __packed;

struct tpm_getrandom_in {
	__be32 num_bytes;
} __packed;

struct tpm_startup_in {
	__be16	startup_type;
} __packed;

typedef union {
	struct	tpm_getcap_params_out getcap_out;
	struct	tpm_readpubek_params_out readpubek_out;
	u8	readpubek_out_buffer[sizeof(struct tpm_readpubek_params_out)];
	struct	tpm_getcap_params_in getcap_in;
	struct	tpm_pcrread_in	pcrread_in;
	struct	tpm_pcrread_out	pcrread_out;
	struct	tpm_pcrextend_in pcrextend_in;
	struct	tpm_getrandom_in getrandom_in;
	struct	tpm_getrandom_out getrandom_out;
	struct	tpm_startup_in startup_in;
} tpm_cmd_params;

struct tpm_cmd_t {
	tpm_cmd_header	header;
	tpm_cmd_params	params;
} __packed;

struct tpm2_digest {
	u16 alg_id;
	u8 digest[TPM2_DIGEST_SIZE_MAX];
} __packed;

/* A string buffer type for constructing TPM commands. This is based on the
 * ideas of string buffer code in security/keys/trusted.h but is heap based
 * in order to keep the stack usage minimal.
 */

enum tpm_buf_flags {
	TPM_BUF_OVERFLOW	= BIT(0),
	TPM_BUF_INITIALIZED	= BIT(01),
};

struct tpm_buf {
	struct page *data_page;
	unsigned int flags;
	u8 *data;
};

static inline int tpm_buf_init(struct tpm_buf *buf)
{
	buf->data_page = alloc_page(GFP_HIGHUSER);
	if (!buf->data_page)
		return -ENOMEM;

	buf->flags = TPM_BUF_INITIALIZED;
	buf->data = kmap(buf->data_page);

	return 0;
}

static inline int tpm_buf_init_req(struct tpm_buf *buf, u16 tag, u32 ordinal)
{
	struct tpm_input_header *head;

	if (!(buf->flags & TPM_BUF_INITIALIZED)) {
		int rc = tpm_buf_init(buf);

		if (rc)
			return rc;
	}

	head = (struct tpm_input_header *) buf->data;

	head->tag = cpu_to_be16(tag);
	head->length = cpu_to_be32(sizeof(*head));
	head->ordinal = cpu_to_be32(ordinal);

	return 0;
}

static inline int tpm_buf_init_resp(struct tpm_buf *buf, u16 tag, u32 retcode)
{
	struct tpm_output_header *head;

	if (!(buf->flags & TPM_BUF_INITIALIZED)) {
		int rc = tpm_buf_init(buf);

		if (rc)
			return rc;
	}

	head = (struct tpm_output_header *) buf->data;

	head->tag = cpu_to_be16(tag);
	head->length = cpu_to_be32(sizeof(*head));
	head->return_code = cpu_to_be32(retcode);

	return 0;
}

static inline void tpm_buf_destroy(struct tpm_buf *buf)
{
	kunmap(buf->data_page);
	__free_page(buf->data_page);
}

static inline u32 tpm_buf_length(struct tpm_buf *buf)
{
	struct tpm_input_header *head = (struct tpm_input_header *) buf->data;

	return be32_to_cpu(head->length);
}

static inline u16 tpm_buf_tag(struct tpm_buf *buf)
{
	struct tpm_input_header *head = (struct tpm_input_header *) buf->data;

	return be16_to_cpu(head->tag);
}

static inline void tpm_buf_append(struct tpm_buf *buf,
				  const unsigned char *new_data,
				  unsigned int new_len)
{
	struct tpm_input_header *head = (struct tpm_input_header *) buf->data;
	u32 len = tpm_buf_length(buf);

	/* Return silently if overflow has already happened. */
	if (buf->flags & TPM_BUF_OVERFLOW)
		return;

	if ((len + new_len) > PAGE_SIZE) {
		WARN(1, "tpm_buf: overflow\n");
		buf->flags |= TPM_BUF_OVERFLOW;
		return;
	}

	memcpy(&buf->data[len], new_data, new_len);
	head->length = cpu_to_be32(len + new_len);
}

static inline void tpm_buf_append_u8(struct tpm_buf *buf, const u8 value)
{
	tpm_buf_append(buf, &value, 1);
}

static inline void tpm_buf_append_u16(struct tpm_buf *buf, const u16 value)
{
	__be16 value2 = cpu_to_be16(value);

	tpm_buf_append(buf, (u8 *) &value2, 2);
}

static inline void tpm_buf_append_u32(struct tpm_buf *buf, const u32 value)
{
	__be32 value2 = cpu_to_be32(value);

	tpm_buf_append(buf, (u8 *) &value2, 4);
}

static inline u8 *tpm_buf_reserve(struct tpm_buf *buf, unsigned int new_len)
{
	struct tpm_input_header *head = (struct tpm_input_header *) buf->data;
	u32 len = tpm_buf_length(buf);

	/* Return silently if overflow has already happened. */
	if (buf->flags & TPM_BUF_OVERFLOW)
		return NULL;

	if ((len + new_len) > PAGE_SIZE) {
		WARN(1, "tpm_buf: overflow\n");
		buf->flags |= TPM_BUF_OVERFLOW;
		return NULL;
	}

	head->length = cpu_to_be32(len + new_len);

	return buf->data + len;
}

static inline void tpm_buf_memset(struct tpm_buf *buf,
				  u8 value, unsigned int new_len)
{
	struct tpm_input_header *head = (struct tpm_input_header *) buf->data;
	u32 len = tpm_buf_length(buf);

	/* Return silently if overflow has already happened. */
	if (buf->flags & TPM_BUF_OVERFLOW)
		return;

	if ((len + new_len) > PAGE_SIZE) {
		WARN(1, "tpm_buf: overflow\n");
		buf->flags |= TPM_BUF_OVERFLOW;
		return;
	}

	memset(&buf->data[len], value, new_len);
	head->length = cpu_to_be32(len + new_len);
}

extern struct class *tpm_class;
extern dev_t tpm_devt;
extern const struct file_operations tpm_fops;
extern struct idr dev_nums_idr;

enum tpm_transmit_flags {
	TPM_TRANSMIT_UNLOCKED	= BIT(0),
};

ssize_t tpm_transmit(struct tpm_chip *chip, const u8 *buf, size_t bufsiz,
		     unsigned int flags);
ssize_t tpm_transmit_cmd(struct tpm_chip *chip, const void *buf, size_t bufsiz,
			 size_t min_rsp_body_len, unsigned int flags,
			 const char *desc);
ssize_t tpm_getcap(struct tpm_chip *chip, u32 subcap_id, cap_t *cap,
		   const char *desc, size_t min_cap_length);
int tpm_get_timeouts(struct tpm_chip *);
int tpm1_auto_startup(struct tpm_chip *chip);
int tpm_do_selftest(struct tpm_chip *chip);
unsigned long tpm_calc_ordinal_duration(struct tpm_chip *chip, u32 ordinal);
int tpm_pm_suspend(struct device *dev);
int tpm_pm_resume(struct device *dev);
int wait_for_tpm_stat(struct tpm_chip *chip, u8 mask, unsigned long timeout,
		      wait_queue_head_t *queue, bool check_cancel);

struct tpm_chip *tpm_chip_find_get(int chip_num);
__must_check int tpm_try_get_ops(struct tpm_chip *chip);
void tpm_put_ops(struct tpm_chip *chip);

struct tpm_chip *tpm_chip_alloc(struct device *dev,
				const struct tpm_class_ops *ops);
struct tpm_chip *tpmm_chip_alloc(struct device *pdev,
				 const struct tpm_class_ops *ops);
int tpm_chip_register(struct tpm_chip *chip);
void tpm_chip_unregister(struct tpm_chip *chip);

void tpm_sysfs_add_device(struct tpm_chip *chip);

int tpm_pcr_read_dev(struct tpm_chip *chip, int pcr_idx, u8 *res_buf);

#ifdef CONFIG_ACPI
extern void tpm_add_ppi(struct tpm_chip *chip);
#else
static inline void tpm_add_ppi(struct tpm_chip *chip)
{
}
#endif

static inline inline u32 tpm2_rc_value(u32 rc)
{
	return (rc & BIT(7)) ? rc & 0xff : rc;
}

int tpm2_pcr_read(struct tpm_chip *chip, int pcr_idx, u8 *res_buf);
int tpm2_pcr_extend(struct tpm_chip *chip, int pcr_idx, u32 count,
		    struct tpm2_digest *digests);
int tpm2_get_random(struct tpm_chip *chip, u8 *out, size_t max);
int tpm2_seal_trusted(struct tpm_chip *chip,
		      struct trusted_key_payload *payload,
		      struct trusted_key_options *options);
int tpm2_unseal_trusted(struct tpm_chip *chip,
			struct trusted_key_payload *payload,
			struct trusted_key_options *options);
ssize_t tpm2_get_tpm_pt(struct tpm_chip *chip, u32 property_id,
			u32 *value, const char *desc);

int tpm2_auto_startup(struct tpm_chip *chip);
void tpm2_shutdown(struct tpm_chip *chip, u16 shutdown_type);
unsigned long tpm2_calc_ordinal_duration(struct tpm_chip *chip, u32 ordinal);
int tpm2_probe(struct tpm_chip *chip);
#endif
