/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX7D SABRESD board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __PICO_IMX7D_CONFIG_H
#define __PICO_IMX7D_CONFIG_H

#include <asm/arch/imx-regs.h>
#include <linux/sizes.h>
#include "mx7_common.h"
#include <asm/imx-common/gpio.h>

#define CONFIG_SYS_BOOTM_LEN 0x1000000

#define CONFIG_DBG_MONITOR
/* uncomment for PLUGIN mode support */
/* #define CONFIG_USE_PLUGIN */

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

#ifdef CONFIG_SECURE_BOOT
#ifndef CONFIG_CSF_SIZE
#define CONFIG_CSF_SIZE 0x4000
#endif
#endif

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN           (32 * SZ_1M)

#define CONFIG_MXC_UART_BASE            UART1_IPS_BASE_ADDR

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_BOARD_LATE_INIT
#define CONFIG_MXC_GPIO

#define CONFIG_MXC_UART
#ifdef CONFIG_MXC_UART_BASE
#undef CONFIG_MXC_UART_BASE
#define CONFIG_MXC_UART_BASE		UART5_IPS_BASE_ADDR
#endif
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0

#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */

#define CONFIG_FEC_MXC
#define CONFIG_MII
#define CONFIG_FEC_XCV_TYPE             RGMII
#define CONFIG_ETHPRIME                 "FEC"
#define CONFIG_FEC_MXC_PHYADDR          1

#define CONFIG_PHYLIB
#define CONFIG_PHY_BROADCOM

/* ENET1 + MDIO2 */
#define IMX_FEC_BASE			ENET_IPS_BASE_ADDR
#define CONFIG_FEC_MXC_MDIO_BASE 	ENET2_IPS_BASE_ADDR

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE3000
#define CONFIG_POWER_PFUZE3000_I2C_ADDR	0x08

#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS

#undef CONFIG_CMD_EXPORTENV
#undef CONFIG_CMD_IMPORTENV

/* I2C configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1                /* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2                /* enable I2C bus 2 */
#define CONFIG_SYS_I2C_MXC_I2C3                /* enable I2C bus 3 */
#define CONFIG_SYS_I2C_MXC_I2C4                /* enable I2C bus 4 */
#define CONFIG_SYS_I2C_SPEED		100000


#define CONFIG_LOADADDR			0x80800000
#define CONFIG_SYS_TEXT_BASE		0x87800000

#define CONFIG_SYS_AUXCORE_BOOTDATA 0x60000000 /* Set to QSPI1 A flash at default */

#ifdef CONFIG_CMD_BOOTAUX

#define UPDATE_M4_ENV \
	"m4image=m4_qspi.bin\0" \
	"loadm4image=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${m4image}\0" \
	"update_m4_from_sd=" \
		"if sf probe 0:0; then " \
			"if run loadm4image; then " \
				"setexpr fw_sz ${filesize} + 0xffff; " \
				"setexpr fw_sz ${fw_sz} / 0x10000; "	\
				"setexpr fw_sz ${fw_sz} * 0x10000; "	\
				"sf erase 0x0 ${fw_sz}; " \
				"sf write ${loadaddr} 0x0 ${filesize}; " \
			"fi; " \
		"fi\0" \
	"m4boot=sf probe 0:0; bootaux "__stringify(CONFIG_SYS_AUXCORE_BOOTDATA)"\0"
#else
#define UPDATE_M4_ENV ""
#endif

#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#define CONFIG_MFG_ENV_SETTINGS \
	"mfgtool_args=setenv bootargs console=${console},${baudrate} " \
		"rdinit=/linuxrc " \
		"g_mass_storage.stall=0 g_mass_storage.removable=1 " \
		"g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF "\
		"g_mass_storage.iSerialNumber=\"\" "\
		"clk_ignore_unused "\
		"\0" \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffff\0" \
	"bootcmd_mfg=run mfgtool_args;bootz ${loadaddr} ${initrd_addr} ${fdt_addr};\0" \

#if defined(CONFIG_NAND_BOOT)
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	"panel=EJ050NA\0" \
	"fdt_addr=0x83000000\0" \
	"fdt_high=0xffffffff\0"	  \
	"console=ttymxc4\0" \
	"bootargs=console=ttymxc4,115200 ubi.mtd=3 "  \
		"root=ubi0:rootfs rootfstype=ubifs "		     \
		"mtdparts=gpmi-nand:64m(boot),16m(kernel),16m(dtb),-(rootfs)\0"\
	"bootcmd=nand read ${loadaddr} 0x4000000 0x800000;"\
		"nand read ${fdt_addr} 0x5000000 0x100000;"\
		"bootz ${loadaddr} - ${fdt_addr}\0"

#else
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS \
	UPDATE_M4_ENV \
	"panel=EJ050NA\0" \
	"script=boot.scr\0" \
	"image=zImage\0" \
	"console=ttymxc4\0" \
	"splashpos=m,m\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_file=imx7d-pico_dwarf.dtb\0" \
	"fdt_addr=0x83000000\0" \
	"boot_fdt=try\0" \
	"ip_dyn=yes\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=" __stringify(CONFIG_SYS_MMC_IMG_LOAD_PART) "\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot} 2\0" \
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

#define CONFIG_BOOTCOMMAND \
	   "mmc dev ${mmcdev};" \
	   "mmc dev ${mmcdev}; if mmc rescan; then " \
		   "if run loadbootscript; then " \
			   "run bootscript; " \
		   "else " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "else run netboot; " \
			   "fi; " \
		   "fi; " \
	   "else run netboot; fi"
#endif

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_PROMPT		"=> "
#define CONFIG_AUTO_COMPLETE

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x20000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#define CONFIG_CMDLINE_EDITING
#define CONFIG_STACKSIZE		SZ_128K

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)


#define CONFIG_ENV_SIZE			SZ_8K

#ifdef CONFIG_SYS_BOOT_QSPI
#define CONFIG_SYS_USE_QSPI
#define CONFIG_ENV_IS_IN_SPI_FLASH
#elif defined CONFIG_NAND_BOOT
#define CONFIG_SYS_USE_NAND
#define CONFIG_ENV_IS_IN_NAND
#else
#define CONFIG_ENV_IS_IN_MMC
#endif

#ifdef CONFIG_SYS_USE_NAND
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_TRIMFFS

/* NAND stuff */
#define CONFIG_NAND_MXS
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_ONFI_DETECTION

/* DMA stuff, needed for GPMI/MXS NAND support */
#define CONFIG_APBH_DMA
#define CONFIG_APBH_DMA_BURST
#define CONFIG_APBH_DMA_BURST8
#endif

#ifdef CONFIG_SYS_USE_QSPI
#define CONFIG_FSL_QSPI    /* enable the QUADSPI driver */
#define CONFIG_QSPI_BASE		QSPI1_IPS_BASE_ADDR
#define CONFIG_QSPI_MEMMAP_BASE		QSPI0_ARB_BASE_ADDR

#define CONFIG_CMD_SF
#define	CONFIG_SPI_FLASH
#define	CONFIG_SPI_FLASH_MACRONIX
#define	CONFIG_SPI_FLASH_BAR
#define	CONFIG_SF_DEFAULT_BUS		0
#define	CONFIG_SF_DEFAULT_CS		0
#define	CONFIG_SF_DEFAULT_SPEED		40000000
#define	CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#endif

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_ENV_OFFSET		(13 * SZ_64K)
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
#define CONFIG_ENV_OFFSET		(768 * 1024)
#define CONFIG_ENV_SECT_SIZE		(64 * 1024)
#define CONFIG_ENV_SPI_BUS		CONFIG_SF_DEFAULT_BUS
#define CONFIG_ENV_SPI_CS		CONFIG_SF_DEFAULT_CS
#define CONFIG_ENV_SPI_MODE		CONFIG_SF_DEFAULT_MODE
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#elif defined(CONFIG_ENV_IS_IN_NAND)
#undef CONFIG_ENV_SIZE
#define CONFIG_ENV_OFFSET		(3 << 18)
#define CONFIG_ENV_SECT_SIZE		(128 << 10)
#define CONFIG_ENV_SIZE			CONFIG_ENV_SECT_SIZE
#endif

#ifdef CONFIG_SYS_USE_NAND
#define CONFIG_SYS_FSL_USDHC_NUM	1
#else
#define CONFIG_SYS_FSL_USDHC_NUM	2
#endif
#define CONFIG_SYS_MMC_ENV_DEV		0   /* USDHC1 */
#define CONFIG_SYS_MMC_ENV_PART		0	/* user area */
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"  /* USDHC1 */

#define CONFIG_CMD_BMODE

#ifndef CONFIG_SYS_DCACHE_OFF
#endif

#ifdef CONFIG_VIDEO
#define	CONFIG_VIDEO_MXS
#define	CONFIG_VIDEO_LOGO
#define	CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define	CONFIG_CMD_BMP
#define	CONFIG_BMP_16BPP
#define	CONFIG_VIDEO_BMP_RLE8
#define CONFIG_VIDEO_BMP_LOGO
#endif

/* #define CONFIG_SPLASH_SCREEN*/
/* #define CONFIG_MXC_EPDC*/

/*
 * SPLASH SCREEN Configs
 */
#if defined(CONFIG_SPLASH_SCREEN) && defined(CONFIG_MXC_EPDC)
/*
 * Framebuffer and LCD
 */
#define	CONFIG_CFB_CONSOLE
#define CONFIG_CMD_BMP
#define CONFIG_LCD
#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#undef LCD_TEST_PATTERN
/* #define CONFIG_SPLASH_IS_IN_MMC			1 */
#define LCD_BPP					LCD_MONOCHROME
/* #define CONFIG_SPLASH_SCREEN_ALIGN		1 */

#define CONFIG_WAVEFORM_BUF_SIZE		0x400000
#endif

/* USB Configs */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX7
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_MXC_USB_PORTSC  (PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS   0
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2

#define CONFIG_IMX_THERMAL

#if defined(CONFIG_MXC_EPDC) && defined(CONFIG_SYS_USE_QSPI)
#error "EPDC Pins conflicts QSPI, Either EPDC or QSPI can be enabled!"
#endif

#if defined(CONFIG_ANDROID_THINGS_SUPPORT)
#include "pico-imx7dandroidthings.h"
#endif

#define PRODUCT_NAME "imx7d"
#define VARIANT_NAME "imx7d_pico"

#endif	/* __CONFIG_H */
