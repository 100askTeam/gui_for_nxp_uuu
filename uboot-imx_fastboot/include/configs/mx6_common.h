/*
 * Copyright (C) 2012-2016 Freescale Semiconductor, Inc.
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef __MX6_COMMON_H
#define __MX6_COMMON_H

#ifndef CONFIG_MX6UL
#define CONFIG_ARM_ERRATA_743622
#if (defined(CONFIG_MX6QP) || defined(CONFIG_MX6Q) ||\
defined(CONFIG_MX6DL)) && !defined(CONFIG_MX6S)
#define CONFIG_ARM_ERRATA_751472
#define CONFIG_ARM_ERRATA_794072
#define CONFIG_ARM_ERRATA_761320
#define CONFIG_ARM_ERRATA_845369
#endif

#ifndef CONFIG_SYS_L2CACHE_OFF
#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_PL310_BASE	L2_PL310_BASE
#endif

#define CONFIG_MP
#define CONFIG_GPT_TIMER
#else
#define CONFIG_SYSCOUNTER_TIMER
#define CONFIG_SC_TIMER_CLK 8000000 /* 8Mhz */
#endif
#define CONFIG_BOARD_POSTCLK_INIT
#define CONFIG_MXC_GPT_HCLK

#ifdef CONFIG_MX6QP
#define CONFIG_MX6Q
#endif

#ifdef CONFIG_MX6S
#define CONFIG_MX6DL
#endif

#define CONFIG_SYS_BOOTM_LEN	0x1000000

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#ifndef CONFIG_MX6
#define CONFIG_MX6
#endif

#define CONFIG_SYS_FSL_CLK

/* ATAGs */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

/* Boot options */
#if (defined(CONFIG_MX6SX) || defined(CONFIG_MX6SL) || \
	defined(CONFIG_MX6UL) || defined(CONFIG_MX6SLL))
#define CONFIG_LOADADDR		0x80800000
#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE	0x87800000
#endif
#else
#define CONFIG_LOADADDR		0x12000000
#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE	0x17800000
#endif
#endif
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX       1
#define CONFIG_BAUDRATE         115200

/* Filesystems and image support */
#define CONFIG_SUPPORT_RAW_INITRD

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE	512
#define CONFIG_SYS_MAXARGS	32
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#ifndef CONFIG_SYS_DCACHE_OFF
#endif

/* GPIO */
#define CONFIG_MXC_GPIO

/* MMC */
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SUPPORT_EMMC_BOOT

/* Fuses */
#define CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

/* Secure boot (HAB) support */
#ifdef CONFIG_SECURE_BOOT
#define CONFIG_CSF_SIZE			0x4000
#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_DRIVERS_MISC_SUPPORT
#endif
#endif

/* LDO Bypass */
#ifndef CONFIG_MX6SLL
#define CONFIG_LDO_BYPASS_CHECK
#endif

#ifdef CONFIG_IMX_OPTEE
#define CONFIG_SYS_MEM_TOP_HIDE	SZ_32M
#define TEE_ENV "tee=yes\0"
#else
#define TEE_ENV "tee=no\0"
#endif
#endif
