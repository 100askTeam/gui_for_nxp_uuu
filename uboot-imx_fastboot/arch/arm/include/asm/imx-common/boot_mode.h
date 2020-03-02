/*
 * Copyright (C) 2012 Boundary Devices Inc.
 * Copyright (C) 2015-2016 Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 *
 * Copyright (C) 2015-2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ASM_BOOT_MODE_H
#define _ASM_BOOT_MODE_H
#define MAKE_CFGVAL(cfg1, cfg2, cfg3, cfg4) \
	((cfg4) << 24) | ((cfg3) << 16) | ((cfg2) << 8) | (cfg1)

enum boot_device {
	WEIM_NOR_BOOT,
	ONE_NAND_BOOT,
	PATA_BOOT,
	SATA_BOOT,
	I2C_BOOT,
	SPI_NOR_BOOT,
	SD1_BOOT,
	SD2_BOOT,
	SD3_BOOT,
	SD4_BOOT,
	MMC1_BOOT,
	MMC2_BOOT,
	MMC3_BOOT,
	MMC4_BOOT,
	NAND_BOOT,
	QSPI_BOOT,
	FLEXSPI_BOOT,
	USB_BOOT,
	UNKNOWN_BOOT,
	BOOT_DEV_NUM = UNKNOWN_BOOT,
};

struct boot_mode {
	const char *name;
	unsigned cfg_val;
};

void add_board_boot_modes(const struct boot_mode *p);
void boot_mode_apply(unsigned cfg_val);
enum boot_device get_boot_device(void);
extern const struct boot_mode soc_boot_modes[];
#endif
