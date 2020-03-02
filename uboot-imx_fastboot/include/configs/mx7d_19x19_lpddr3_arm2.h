/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX7D 19x19 LPDDR3 ARM2 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MX7D_19X19_LPDDR3_ARM2_CONFIG_H
#define __MX7D_19X19_LPDDR3_ARM2_CONFIG_H

#define CONFIG_SYS_FSL_USDHC_NUM	1
#define CONFIG_SYS_MMC_ENV_DEV		0	/* USDHC1 */
#define CONFIG_SYS_MMC_ENV_PART		0	/* user area */
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"  /* USDHC1 */

#ifdef CONFIG_TARGET_MX7D_19X19_LPDDR2_ARM2
#define PHYS_SDRAM_SIZE			SZ_512M
#else
#define PHYS_SDRAM_SIZE			SZ_2G
#endif

#define CONFIG_FEC_MXC
#define CONFIG_MII
#define CONFIG_FEC_XCV_TYPE             RGMII
#ifdef CONFIG_DM_ETH
#define CONFIG_ETHPRIME                 "eth0"
#else
#define CONFIG_ETHPRIME                 "FEC"
#endif
#define CONFIG_FEC_MXC_PHYADDR          0

#define CONFIG_PHYLIB
#define CONFIG_PHY_ATHEROS

/* ENET2 */
#define IMX_FEC_BASE			ENET2_IPS_BASE_ADDR

#define CONFIG_FEC_MXC_MDIO_BASE	ENET_IPS_BASE_ADDR

/* QSPI conflict with EIMNOR */
/* FEC0 conflict with EIMNOR */
/* ECSPI conflict with UART */
#ifdef CONFIG_QSPI_BOOT
#define CONFIG_FSL_QSPI
#define CONFIG_ENV_IS_IN_SPI_FLASH
#elif defined CONFIG_SPI_BOOT
#define CONFIG_MXC_SPI
#define CONFIG_ENV_IS_IN_SPI_FLASH
#elif defined CONFIG_NOR_BOOT
#define CONFIG_MTD_NOR_FLASH
#define CONFIG_ENV_IS_IN_FLASH
#undef CONFIG_FEC_MXC
#elif defined CONFIG_NAND_BOOT
#define CONFIG_NAND_MXS
#define CONFIG_ENV_IS_IN_NAND
#else
#define CONFIG_MTD_NOR_FLASH
#undef CONFIG_FEC_MXC
#define CONFIG_ENV_IS_IN_MMC
#endif

#ifndef CONFIG_DM_I2C
#define CONFIG_SYS_I2C
#endif
#ifdef CONFIG_CMD_I2C
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#endif

/* PMIC */
#ifndef CONFIG_DM_PMIC
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE3000
#define CONFIG_POWER_PFUZE3000_I2C_ADDR	0x08
#endif

#ifdef CONFIG_MXC_SPI
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_ATMEL
#define CONFIG_SF_DEFAULT_BUS  0
#define CONFIG_SF_DEFAULT_SPEED 20000000
#define CONFIG_SF_DEFAULT_MODE (SPI_MODE_0)
#define CONFIG_SF_DEFAULT_CS   0
#endif

#include "mx7d_arm2.h"

#endif
