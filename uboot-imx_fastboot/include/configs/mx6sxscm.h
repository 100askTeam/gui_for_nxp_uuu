/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX6SXSCM EVB board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef __MX6SXSCM_H
#define __MX6SXSCM_H

#include "mx6sxsabresd.h"

#undef CONFIG_MXC_UART_BASE
#define CONFIG_MXC_UART_BASE		UART3_BASE

#undef CONFIG_PHY_ATHEROS
#undef CONFIG_FEC_MXC_PHYADDR
#define CONFIG_PHY_MICREL
#define CONFIG_FEC_MXC_PHYADDR          3



#if defined CONFIG_512MB_LPDDR2
#if defined CONFIG_MX6SXSCM_EMMC
#define FDT_FILE		"imx6sxscm-epop-evb-ldo.dtb"
#else
#define FDT_FILE		"imx6sxscm-512mb-evb-ldo.dtb"
#endif
#else
#define FDT_FILE		"imx6sxscm-1gb-evb-ldo.dtb"
#endif

#ifndef CONFIG_FASTBOOT
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	UPDATE_M4_ENV \
	"script=boot.scr\0" \
	"image=zImage\0" \
	"console=ttymxc2\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_file="FDT_FILE"\0" \
	"fdt_addr=0x83000000\0" \
	"boot_fdt=try\0" \
	"ip_dyn=yes\0" \
	"panel=Hannstar-XGA\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=1\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot}\0" \
	"loadbootscript=" \
		"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run loadfdt; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0" \
	"netargs=setenv bootargs console=${console},${baudrate} " \
		"root=/dev/nfs " \
	"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
		"netboot=echo Booting from net ...; " \
		"run netargs; " \
		"if test ${ip_dyn} = yes; then " \
			"setenv get_cmd dhcp; " \
		"else " \
			"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${image}; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if ${get_cmd} ${fdt_addr} ${fdt_file}; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0"
#endif
#endif				/* __MX6SXSCM_H */

