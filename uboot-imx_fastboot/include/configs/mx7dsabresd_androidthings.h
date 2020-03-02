/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MX7DSABRESDBRSD_ANDROIDTHINGS_H
#define __MX7DSABRESDBRSD_ANDROIDTHINGS_H
#include "mx_android_common.h"

#define CONFIG_CMD_FAT

/* For NAND we don't support lock/unlock */
#ifndef CONFIG_NAND_BOOT
#define CONFIG_FASTBOOT_LOCK
#define CONFIG_ENABLE_LOCKSTATUS_SUPPORT
#define FSL_FASTBOOT_FB_DEV "mmc"
#endif

#define CONFIG_ANDROID_AB_SUPPORT
#define FASTBOOT_ENCRYPT_LOCK

#define CONFIG_FSL_CAAM_KB
#define CONFIG_SHA1
#define CONFIG_SHA256


#ifdef CONFIG_SYS_MMC_ENV_DEV
#undef CONFIG_SYS_MMC_ENV_DEV
#define CONFIG_SYS_MMC_ENV_DEV		1   /* USDHC2 */
#endif

#ifdef CONFIG_SYS_MMC_ENV_PART
#undef CONFIG_SYS_MMC_ENV_PART
#define CONFIG_SYS_MMC_ENV_PART		1	/* boot0 area */
#endif

#define CONFIG_SYSTEM_RAMDISK_SUPPORT
#define CONFIG_CMD_FS_GENERIC
#define CONFIG_CMD_EXT4

#define CONFIG_AVB_SUPPORT
#ifdef CONFIG_AVB_SUPPORT
#define CONFIG_SUPPORT_EMMC_RPMB
/* fuse bank size in word */
/* infact 7D have no enough bits
 * set this size to 0 will disable
 * program/read FUSE */
#define CONFIG_AVB_FUSE_BANK_SIZEW 0
#define CONFIG_AVB_FUSE_BANK_START 0
#define CONFIG_AVB_FUSE_BANK_END 0
#endif

#endif

