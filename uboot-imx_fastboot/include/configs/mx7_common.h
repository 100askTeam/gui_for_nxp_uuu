/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX7.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MX7_COMMON_H
#define __MX7_COMMON_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#ifndef CONFIG_MX7
#define CONFIG_MX7
#endif

/* Timer settings */
#define CONFIG_MXC_GPT_HCLK
#define CONFIG_SYSCOUNTER_TIMER
#define CONFIG_SC_TIMER_CLK 8000000 /* 8Mhz */
#define CONFIG_TIMER_CLK_FREQ CONFIG_SC_TIMER_CLK
#define CONFIG_SYS_FSL_CLK

#define CONFIG_SYS_BOOTM_LEN	0x1000000

/* Enable iomux-lpsr support */
#define CONFIG_IOMUX_LPSR

#define CONFIG_LOADADDR                 0x80800000
#define CONFIG_SYS_TEXT_BASE            0x87800000

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX               1
#define CONFIG_BAUDRATE                 115200

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_MAXARGS		32
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#ifndef CONFIG_SYS_DCACHE_OFF
#endif

/* GPIO */
#define CONFIG_MXC_GPIO

/* UART */
#define CONFIG_MXC_UART

/* MMC */
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC

/* Fuses */
#define CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP

#define CONFIG_ARMV7_SECURE_BASE	0x00900000

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

/* Secure boot (HAB) support */
#ifdef CONFIG_SECURE_BOOT
#define CONFIG_CSF_SIZE			0x4000
#endif

#ifdef CONFIG_IMX_OPTEE
#define CONFIG_SYS_MEM_TOP_HIDE	SZ_32M
#define TEE_ENV "tee=yes\0"
#else
#define TEE_ENV "tee=no\0"
#endif
#endif
