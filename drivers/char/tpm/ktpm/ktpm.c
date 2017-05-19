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

#include "ktpm.h"

typedef enum {
	KTPM_STS_COMPLETE = BIT(0),
} ktpm_status_t;

static struct platform_device *ktpm_pdev;

static u8 ktpm_status(struct tpm_chip *chip)
{
	struct device *dev = &chip->dev;

	dev_dbg(dev, "ktpm_status(): \n");
#if 0
	//struct crb_priv *priv = dev_get_drvdata(&chip->dev);
	u8 sts = 0;

	if ((ioread32(&priv->cca->start) & CRB_START_INVOKE) !=
	    CRB_START_INVOKE)
		sts |= CRB_DRV_STS_COMPLETE;
#endif
	return (u8)KTPM_STS_COMPLETE;
}

static int ktpm_recv(struct tpm_chip *chip, u8 *buf, size_t len)
{
	struct ktpm_context *ctx = dev_get_drvdata(&chip->dev);
	struct tpm_output_header *header = (struct tpm_output_header *)buf;
	int rc;

	rc = ktpm_tpm2_retrieve_response(ctx, buf, len);

	dev_dbg(&chip->dev, "ktpm_recv(): tag %#hx, length %#x/%#lx, "
		"retcode %#x\n", be32_to_cpu(header->tag),
		be32_to_cpu(header->length), len,
		be32_to_cpu(header->return_code));

	return !rc ? be32_to_cpu(header->length) : rc;
}

static int ktpm_send(struct tpm_chip *chip, u8 *buf, size_t len)
{
	struct ktpm_context *ctx = dev_get_drvdata(&chip->dev);
	enum tpm2_return_codes rc;

	rc = ktpm_tpm2_dispatch_cmd(ctx, buf, len);

	return rc == TPM2_RC_SUCCESS ? 0 : -(int)rc;
}

static void ktpm_cancel(struct tpm_chip *chip)
{
	struct device *dev = &chip->dev;

	dev_dbg(dev, "ktpm_cancel: \n");
}

static bool ktpm_req_canceled(struct tpm_chip *chip, u8 status)
{
	struct device *dev = &chip->dev;

	dev_dbg(dev, "ktpm_req_canceled: \n");

	return false;
}

static bool ktpm_update_timeouts(struct tpm_chip *chip,
				 unsigned long *timeout_cap)
{
	struct device *dev = &chip->dev;

	dev_dbg(dev, "ktpm_update_timeouts: \n");

	return false;
}

static const struct tpm_class_ops ktpm_ops = {
	.flags = TPM_OPS_AUTO_STARTUP,
	.req_complete_mask = KTPM_STS_COMPLETE,
	.req_complete_val = KTPM_STS_COMPLETE,
	.req_canceled = ktpm_req_canceled,
	.recv = ktpm_recv,
	.send = ktpm_send,
	.cancel = ktpm_cancel,
	.status = ktpm_status,
	.update_timeouts = ktpm_update_timeouts,
};

static int ktpm_plat_probe(struct platform_device *pdev)
{
	struct tpm_chip *chip;
	struct ktpm_context *ctx;
	struct device *dev;
	int rc;

	chip = tpmm_chip_alloc(&pdev->dev, &ktpm_ops);
	if (IS_ERR(chip))
		return PTR_ERR(chip);

	dev = &chip->dev;

	ctx = devm_kmalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx) {
		devm_kfree(&pdev->dev, chip);
		return -ENOMEM;
	}

	ctx->dev = dev;
	dev_set_drvdata(dev, ctx);
	chip->flags = TPM_CHIP_FLAG_TPM2;

	/*
	 * _TPM_init command must be issued before tpm_chip_register()
	 * which calls an automatic startup.
	 */
	ktpm_tpm2_cmd_init(ctx);

	rc = tpm_chip_register(chip);
	if (!rc)
		dev_info(dev, "kTPM driver initialized\n");
	else {
		devm_kfree(dev, ctx);
		dev_err(dev, "Failed to initialize kTPM driver (err: %#x)\n",
			rc);
	}

	return rc;
}

static int ktpm_plat_remove(struct platform_device *pdev)
{
	struct tpm_chip *chip = dev_get_drvdata(&pdev->dev);
	struct device *dev = &chip->dev;
	struct ktpm_context *ctx;

	ctx = dev_get_drvdata(dev);
	devm_kfree(dev, ctx);
	tpm_chip_unregister(chip);

	return 0;
}

static struct platform_driver ktpm_drv = {
	.probe = ktpm_plat_probe,
	.remove = ktpm_plat_remove,
	.driver = {
		.name = "ktpm",
	},
};

static int __init ktpm_init(void)
{
	int rc;

	ktpm_pdev = platform_device_register_simple("ktpm", -1, NULL, 0);
	if (IS_ERR(ktpm_pdev))
		return PTR_ERR(ktpm_pdev);

	rc = platform_driver_register(&ktpm_drv);
	if (rc)
		platform_device_unregister(ktpm_pdev);

	return rc;
}

static void __exit ktpm_cleanup(void)
{
	platform_driver_unregister(&ktpm_drv);
	platform_device_unregister(ktpm_pdev);
}

module_init(ktpm_init);
module_exit(ktpm_cleanup);

MODULE_AUTHOR("Lans Zhang (jia.zhang@windriver.com)");
MODULE_DESCRIPTION("Kernel TPM (kTPM) Driver");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
