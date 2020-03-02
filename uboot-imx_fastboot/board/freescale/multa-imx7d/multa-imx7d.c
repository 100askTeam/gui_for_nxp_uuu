/*
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx7-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <common.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <miiphy.h>
#include <netdev.h>
#include <axp152.h>
#include "../common/pfuze.h"
#include <i2c.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/arch/crm_regs.h>
#include <usb.h>
#if defined(CONFIG_MXC_EPDC)
#include <lcd.h>
#include <mxc_epdc_fb.h>
#endif
#include <asm/imx-common/video.h>

#ifdef CONFIG_FSL_FASTBOOT
#include <fsl_fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FSL_FASTBOOT*/

DECLARE_GLOBAL_DATA_PTR;

#define PHY_NRESET_GPIO		IMX_GPIO_NR(4, 22)

#define UART_PAD_CTRL  (PAD_CTL_DSE_3P3V_49OHM | \
	PAD_CTL_PUS_PU100KOHM | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
	PAD_CTL_HYS | PAD_CTL_PUE | PAD_CTL_PUS_PU47KOHM)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_PU100KOHM | PAD_CTL_DSE_3P3V_49OHM)
#define ENET_PAD_CTRL_MII  (PAD_CTL_DSE_3P3V_32OHM)

#define ENET_RX_PAD_CTRL  (PAD_CTL_PUS_PU100KOHM | PAD_CTL_DSE_3P3V_49OHM)

#define I2C_PAD_CTRL    (PAD_CTL_DSE_3P3V_32OHM | PAD_CTL_SRE_SLOW | \
	PAD_CTL_HYS | PAD_CTL_PUE | PAD_CTL_PUS_PU100KOHM)

#define LCD_PAD_CTRL    (PAD_CTL_HYS | PAD_CTL_PUS_PU100KOHM | \
	PAD_CTL_DSE_3P3V_49OHM)

#define QSPI_PAD_CTRL	\
	(PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_PUE | PAD_CTL_PUS_PU47KOHM)

#define SPI_PAD_CTRL (PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_SRE_SLOW | PAD_CTL_HYS)

#define BUTTON_PAD_CTRL    (PAD_CTL_PUS_PU5KOHM | PAD_CTL_DSE_3P3V_98OHM)

#define NAND_PAD_CTRL (PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_SRE_SLOW | PAD_CTL_HYS)

#define NAND_PAD_READY0_CTRL (PAD_CTL_DSE_3P3V_49OHM | PAD_CTL_PUS_PU5KOHM)

#define EPDC_PAD_CTRL	0x0

#ifdef CONFIG_SYS_I2C_MXC
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)

/* I2C4 */
struct i2c_pads_info i2c_pad_info4 = {
	.scl = {
		.i2c_mode = MX7D_PAD_ENET1_RGMII_TD2__I2C4_SCL | MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gpio_mode = MX7D_PAD_ENET1_RGMII_TD2__GPIO7_IO8 | MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(7, 8),
	},
	.sda = {
		.i2c_mode = MX7D_PAD_ENET1_RGMII_TD3__I2C4_SDA | MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gpio_mode = MX7D_PAD_ENET1_RGMII_TD3__GPIO7_IO9 | MUX_PAD_CTRL(I2C_PAD_CTRL),
		.gp = IMX_GPIO_NR(7, 9),
	},
};
#endif

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_SIZE;

	return 0;
}

static iomux_v3_cfg_t const wdog_pads[] = {
	MX7D_PAD_GPIO1_IO00__WDOG1_WDOG_B | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const uart1_pads[] = {
	MX7D_PAD_UART1_TX_DATA__UART1_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX7D_PAD_UART1_RX_DATA__UART1_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc1_pads[] = {
	MX7D_PAD_SD1_CLK__SD1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_CMD__SD1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA0__SD1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA1__SD1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA2__SD1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_DATA3__SD1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	MX7D_PAD_SD1_CD_B__GPIO5_IO0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD1_RESET_B__GPIO5_IO2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc3_emmc_pads[] = {
	MX7D_PAD_SD3_CLK__SD3_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_CMD__SD3_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA4__SD3_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA5__SD3_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA6__SD3_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_DATA7__SD3_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX7D_PAD_SD3_STROBE__SD3_STROBE	 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	MX7D_PAD_SD3_RESET_B__GPIO6_IO11 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

#define IOX_SDI IMX_GPIO_NR(1, 9)
#define IOX_STCP IMX_GPIO_NR(1, 12)
#define IOX_SHCP IMX_GPIO_NR(1, 13)

static iomux_v3_cfg_t const iox_pads[] = {
	/* IOX_SDI */
	MX7D_PAD_GPIO1_IO09__GPIO1_IO9	| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_STCP */
	MX7D_PAD_GPIO1_IO12__GPIO1_IO12	| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_SHCP */
	MX7D_PAD_GPIO1_IO13__GPIO1_IO13	| MUX_PAD_CTRL(NO_PAD_CTRL),
};

/*
 * PCIE_DIS_B --> Q0
 * PCIE_RST_B --> Q1
 * HDMI_RST_B --> Q2
 * PERI_RST_B --> Q3
 * SENSOR_RST_B --> Q4
 * ENET_RST_B --> Q5
 * PERI_3V3_EN --> Q6
 * LCD_PWR_EN --> Q7
 */
enum qn {
	PCIE_DIS_B,
	PCIE_RST_B,
	HDMI_RST_B,
	PERI_RST_B,
	SENSOR_RST_B,
	ENET_RST_B,
	PERI_3V3_EN,
	LCD_PWR_EN,
};

enum qn_func {
	qn_reset,
	qn_enable,
	qn_disable,
};

enum qn_level {
	qn_low = 0,
	qn_high = 1,
};

static enum qn_level seq[3][2] = {
	{0, 1}, {1, 1}, {0, 0}
};

static enum qn_func qn_output[8] = {
	qn_disable, qn_reset, qn_reset, qn_reset, qn_reset, qn_reset, qn_enable,
	qn_disable
};

static void iox74lv_init(void)
{
	int i;

	for (i = 7; i >= 0; i--) {
		gpio_direction_output(IOX_SHCP, 0);
		gpio_direction_output(IOX_SDI, seq[qn_output[i]][0]);
		udelay(500);
		gpio_direction_output(IOX_SHCP, 1);
		udelay(500);
	}

	gpio_direction_output(IOX_STCP, 0);
	udelay(500);
	/*
	  * shift register will be output to pins
	  */
	gpio_direction_output(IOX_STCP, 1);

	for (i = 7; i >= 0; i--) {
		gpio_direction_output(IOX_SHCP, 0);
		gpio_direction_output(IOX_SDI, seq[qn_output[i]][1]);
		udelay(500);
		gpio_direction_output(IOX_SHCP, 1);
		udelay(500);
	}
	gpio_direction_output(IOX_STCP, 0);
	udelay(500);
	/*
	  * shift register will be output to pins
	  */
	gpio_direction_output(IOX_STCP, 1);
};

void iox74lv_set(int index)
{
	int i;
	for (i = 7; i >= 0; i--) {
		gpio_direction_output(IOX_SHCP, 0);

		if (i == index)
			gpio_direction_output(IOX_SDI, seq[qn_output[i]][0]);
		else
			gpio_direction_output(IOX_SDI, seq[qn_output[i]][1]);
		udelay(500);
		gpio_direction_output(IOX_SHCP, 1);
		udelay(500);
	}

	gpio_direction_output(IOX_STCP, 0);
	udelay(500);
	/*
	  * shift register will be output to pins
	  */
	gpio_direction_output(IOX_STCP, 1);

	for (i = 7; i >= 0; i--) {
		gpio_direction_output(IOX_SHCP, 0);
		gpio_direction_output(IOX_SDI, seq[qn_output[i]][1]);
		udelay(500);
		gpio_direction_output(IOX_SHCP, 1);
		udelay(500);
	}

	gpio_direction_output(IOX_STCP, 0);
	udelay(500);
	/*
	  * shift register will be output to pins
	  */
	gpio_direction_output(IOX_STCP, 1);
};

#define BOARD_REV_C  0x300
#define BOARD_REV_B  0x200
#define BOARD_REV_A  0x100

static int mx7sabre_rev(void)
{
	/*
	 * Get Board ID information from OCOTP_GP1[15:8]
	 * i.MX7D SDB RevA: 0x41
	 * i.MX7D SDB RevB: 0x42
	 */
	struct ocotp_regs *ocotp = (struct ocotp_regs *)OCOTP_BASE_ADDR;
	struct fuse_bank *bank = &ocotp->bank[14];
	int reg = readl(&bank->fuse_regs[0]);
	int ret;

	if (reg != 0) {
		switch (reg >> 8 & 0x0F) {
		case 0x3:
			ret = BOARD_REV_C;
			break;
		case 0x02:
			ret = BOARD_REV_B;
			break;
		case 0x01:
		default:
			ret = BOARD_REV_A;
			break;
		}
	} else {
		/* If the gp1 fuse is not burn, we have to use TO rev for the board rev */
		if (is_soc_rev(CHIP_REV_1_0))
			ret = BOARD_REV_A;
		else if (is_soc_rev(CHIP_REV_1_1))
			ret = BOARD_REV_B;
		else
			ret = BOARD_REV_C;
	}

	return ret;
}

u32 get_board_rev(void)
{
	int rev = mx7sabre_rev();

	return (get_cpu_rev() & ~(0xF << 8)) | rev;
}

#ifdef CONFIG_NAND_MXS
static iomux_v3_cfg_t const gpmi_pads[] = {
	MX7D_PAD_SD3_DATA0__NAND_DATA00 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA1__NAND_DATA01 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA2__NAND_DATA02 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA3__NAND_DATA03 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA4__NAND_DATA04 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA5__NAND_DATA05 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA6__NAND_DATA06 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_DATA7__NAND_DATA07 | MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_CLK__NAND_CLE	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_CMD__NAND_ALE	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_STROBE__NAND_RE_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SD3_RESET_B__NAND_WE_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_MCLK__NAND_WP_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_RX_BCLK__NAND_CE3_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_RX_SYNC__NAND_CE2_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_RX_DATA__NAND_CE1_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_TX_BCLK__NAND_CE0_B	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_TX_SYNC__NAND_DQS	| MUX_PAD_CTRL(NAND_PAD_CTRL),
	MX7D_PAD_SAI1_TX_DATA__NAND_READY_B	| MUX_PAD_CTRL(NAND_PAD_READY0_CTRL),
};

static void setup_gpmi_nand(void)
{
	imx_iomux_v3_setup_multiple_pads(gpmi_pads, ARRAY_SIZE(gpmi_pads));

	/* NAND_USDHC_BUS_CLK is set in rom */
	set_clk_nand();
}
#endif

#ifdef CONFIG_VIDEO_MXS
static iomux_v3_cfg_t const lcd_pads[] = {
	MX7D_PAD_LCD_CLK__LCD_CLK | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_EPDC_BDR1__LCD_ENABLE | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_HSYNC__LCD_HSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_VSYNC__LCD_VSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA00__LCD_DATA0 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA01__LCD_DATA1 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA02__LCD_DATA2 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA03__LCD_DATA3 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA04__LCD_DATA4 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA05__LCD_DATA5 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA06__LCD_DATA6 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA07__LCD_DATA7 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA08__LCD_DATA8 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA09__LCD_DATA9 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA10__LCD_DATA10 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA11__LCD_DATA11 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA12__LCD_DATA12 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA13__LCD_DATA13 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA14__LCD_DATA14 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA15__LCD_DATA15 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA16__LCD_DATA16 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA17__LCD_DATA17 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA18__LCD_DATA18 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA19__LCD_DATA19 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA20__LCD_DATA20 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA21__LCD_DATA21 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA22__LCD_DATA22 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX7D_PAD_LCD_DATA23__LCD_DATA23 | MUX_PAD_CTRL(LCD_PAD_CTRL),

	MX7D_PAD_LCD_RESET__GPIO3_IO4	| MUX_PAD_CTRL(LCD_PAD_CTRL),
};

static iomux_v3_cfg_t const pwm_pads[] = {
	/* Use GPIO for Brightness adjustment, duty cycle = period */
	MX7D_PAD_GPIO1_IO01__GPIO1_IO1 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

void do_enable_parallel_lcd(struct display_info_t const *dev)
{
	imx_iomux_v3_setup_multiple_pads(lcd_pads, ARRAY_SIZE(lcd_pads));

	imx_iomux_v3_setup_multiple_pads(pwm_pads, ARRAY_SIZE(pwm_pads));

	/* Reset LCD */
	gpio_direction_output(IMX_GPIO_NR(3, 4) , 0);
	udelay(500);
	gpio_direction_output(IMX_GPIO_NR(3, 4) , 1);

	/* Set Brightness to high */
	gpio_direction_output(IMX_GPIO_NR(1, 1) , 1);
}

struct display_info_t const displays[] = {{
	.bus = ELCDIF1_IPS_BASE_ADDR,
	.addr = 0,
	.pixfmt = 24,
	.detect = NULL,
	.enable	= do_enable_parallel_lcd,
	.mode	= {
		.name			= "TFT43AB",
		.xres           = 480,
		.yres           = 272,
		.pixclock       = 108695,
		.left_margin    = 8,
		.right_margin   = 4,
		.upper_margin   = 2,
		.lower_margin   = 4,
		.hsync_len      = 41,
		.vsync_len      = 10,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} } };
size_t display_count = ARRAY_SIZE(displays);
#endif

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

#ifdef CONFIG_FSL_ESDHC

#define USDHC1_CD_GPIO	IMX_GPIO_NR(5, 0)
#define USDHC1_PWR_GPIO	IMX_GPIO_NR(5, 2)
#define USDHC3_PWR_GPIO IMX_GPIO_NR(6, 11)

static struct fsl_esdhc_cfg usdhc_cfg[3] = {
	{USDHC1_BASE_ADDR, 0, 4},
	{USDHC3_BASE_ADDR},
};

int board_mmc_get_env_dev(int devno)
{
	if (devno == 2)
		devno--;

	return devno;
}

int mmc_map_to_kernel_blk(int dev_no)
{
	if (dev_no == 1)
		dev_no++;

	return dev_no;
}

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		ret = !gpio_get_value(USDHC1_CD_GPIO);
		break;
	case USDHC3_BASE_ADDR:
		ret = 1; /* Assume uSDHC3 emmc is always present */
		break;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	int i, ret;
	/*
	 * According to the board_mmc_init() the following map is done:
	 * (U-Boot device node)    (Physical Port)
	 * mmc0                    USDHC1
	 * mmc2                    USDHC3 (eMMC)
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
			gpio_request(USDHC1_CD_GPIO, "usdhc1_cd");
			gpio_direction_input(USDHC1_CD_GPIO);
			gpio_request(USDHC1_PWR_GPIO, "usdhc1_pwr");
			gpio_direction_output(USDHC1_PWR_GPIO, 0);
			udelay(500);
			gpio_direction_output(USDHC1_PWR_GPIO, 1);
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
			break;
		case 1:
			imx_iomux_v3_setup_multiple_pads(
				usdhc3_emmc_pads, ARRAY_SIZE(usdhc3_emmc_pads));
			gpio_request(USDHC3_PWR_GPIO, "usdhc3_pwr");
			gpio_direction_output(USDHC3_PWR_GPIO, 0);
			udelay(500);
			gpio_direction_output(USDHC3_PWR_GPIO, 1);
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) than supported by the board\n", i + 1);
			return -EINVAL;
			}

			ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
			if (ret)
				return ret;
	}

	return 0;
}
#endif

iomux_v3_cfg_t const fec2_en_pads[] = {
	MX7D_PAD_GPIO1_IO04__GPIO1_IO4 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
#ifdef CONFIG_FEC_MXC
/* Currently incompleted since on the demo client fec1 is not used */
static const iomux_v3_cfg_t fec1_pads[] = {
	MX7D_PAD_ENET1_TX_CLK__CCM_ENET_REF_CLK1 | MUX_PAD_CTRL(ENET_PAD_CTRL) | MUX_MODE_SION,
	MX7D_PAD_LCD_DATA15__GPIO3_IO20	| MUX_PAD_CTRL(NO_PAD_CTRL),
};

static const iomux_v3_cfg_t fec2_pads[] = {
	MX7D_PAD_GPIO1_IO14__ENET2_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_GPIO1_IO15__ENET2_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),

	MX7D_PAD_EPDC_GDRL__ENET2_RGMII_TX_CTL | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_EPDC_SDCE2__ENET2_RGMII_TD0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX7D_PAD_EPDC_SDCE3__ENET2_RGMII_TD1 | MUX_PAD_CTRL(ENET_PAD_CTRL),

	MX7D_PAD_EPDC_SDCE0__ENET2_RGMII_RX_CTL | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_EPDC_SDCLK__ENET2_RGMII_RD0 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_EPDC_SDLE__ENET2_RGMII_RD1 | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),
	MX7D_PAD_EPDC_SDCE1__ENET2_RX_ER | MUX_PAD_CTRL(ENET_RX_PAD_CTRL),

	MX7D_PAD_EPDC_BDR0__CCM_ENET_REF_CLK2 | MUX_PAD_CTRL(ENET_PAD_CTRL) | MUX_MODE_SION,

	/* NRES_ETH pin */
	MX7D_PAD_SD1_CD_B__GPIO5_IO0 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(fec1_pads, ARRAY_SIZE(fec1_pads));
	imx_iomux_v3_setup_multiple_pads(fec2_pads, ARRAY_SIZE(fec2_pads));
}

int board_eth_init(bd_t *bis)
{
	int ret;

	setup_iomux_fec();

	ret = fecmxc_initialize_multi(bis, CONFIG_FEC_ENET_DEV,
		CONFIG_FEC_MXC_PHYADDR, IMX_FEC_BASE);
	if (ret)
		printf("FEC1 MXC: %s:failed\n", __func__);

	return ret;
}

static int setup_fec(int fec_id)
{
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *) IOMUXC_GPR_BASE_ADDR;
	int ret;

	if (fec_id == 0) {
		/* FEC1 */
		clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
				(IOMUXC_GPR_GPR1_GPR_ENET1_TX_CLK_SEL_MASK | IOMUXC_GPR_GPR1_GPR_ENET1_CLK_DIR_MASK),
				(IOMUXC_GPR_GPR1_GPR_ENET1_CLK_DIR_MASK));
	} else if (fec_id == 1) {
		/* FEC2 */
		clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
				(IOMUXC_GPR_GPR1_GPR_ENET2_TX_CLK_SEL_MASK | IOMUXC_GPR_GPR1_GPR_ENET2_CLK_DIR_MASK),
				(IOMUXC_GPR_GPR1_GPR_ENET2_CLK_DIR_MASK));
	} else {
		return -EINVAL;
	}

	ret = set_clk_enet(ENET_50MHz);
	if (ret) {
		printf("%s: set_clk_enet() failed\n", __func__);
		return ret;
	}

	return 0;
}


int board_phy_config(struct phy_device *phydev)
{
	/* Reset the PHY */
	gpio_direction_output(PHY_NRESET_GPIO, 0);
	mdelay(5);
	gpio_set_value(PHY_NRESET_GPIO, 1);

	if (phydev->drv->config)
		phydev->drv->config(phydev);
	return 0;
}
#endif

#ifdef CONFIG_FSL_QSPI
static iomux_v3_cfg_t const quadspi_pads[] = {
	MX7D_PAD_EPDC_DATA00__QSPI_A_DATA0 | MUX_PAD_CTRL(QSPI_PAD_CTRL),
	MX7D_PAD_EPDC_DATA01__QSPI_A_DATA1 | MUX_PAD_CTRL(QSPI_PAD_CTRL),
	MX7D_PAD_EPDC_DATA02__QSPI_A_DATA2 | MUX_PAD_CTRL(QSPI_PAD_CTRL),
	MX7D_PAD_EPDC_DATA03__QSPI_A_DATA3 | MUX_PAD_CTRL(QSPI_PAD_CTRL),
	MX7D_PAD_EPDC_DATA05__QSPI_A_SCLK  | MUX_PAD_CTRL(QSPI_PAD_CTRL),
	MX7D_PAD_EPDC_DATA06__QSPI_A_SS0_B | MUX_PAD_CTRL(QSPI_PAD_CTRL),
};

int board_qspi_init(void)
{
	/* Set the iomux */
	imx_iomux_v3_setup_multiple_pads(quadspi_pads,
					 ARRAY_SIZE(quadspi_pads));

	/* Set the clock */
	set_clk_qspi();

	return 0;
}
#endif

#ifdef CONFIG_MXC_EPDC
static iomux_v3_cfg_t const epdc_enable_pads[] = {
	MX7D_PAD_EPDC_DATA00__EPDC_DATA0	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA01__EPDC_DATA1	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA02__EPDC_DATA2	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA03__EPDC_DATA3	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA04__EPDC_DATA4	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA05__EPDC_DATA5	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA06__EPDC_DATA6	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_DATA07__EPDC_DATA7	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_SDCLK__EPDC_SDCLK		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_SDLE__EPDC_SDLE		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_SDOE__EPDC_SDOE		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_SDSHR__EPDC_SDSHR		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_SDCE0__EPDC_SDCE0		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_SDCE1__EPDC_SDCE1		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_GDCLK__EPDC_GDCLK		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_GDOE__EPDC_GDOE		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_GDRL__EPDC_GDRL		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_GDSP__EPDC_GDSP		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_BDR0__EPDC_BDR0		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX7D_PAD_EPDC_BDR1__EPDC_BDR1		| MUX_PAD_CTRL(EPDC_PAD_CTRL),
};

static iomux_v3_cfg_t const epdc_disable_pads[] = {
	MX7D_PAD_EPDC_DATA00__GPIO2_IO0,
	MX7D_PAD_EPDC_DATA01__GPIO2_IO1,
	MX7D_PAD_EPDC_DATA02__GPIO2_IO2,
	MX7D_PAD_EPDC_DATA03__GPIO2_IO3,
	MX7D_PAD_EPDC_DATA04__GPIO2_IO4,
	MX7D_PAD_EPDC_DATA05__GPIO2_IO5,
	MX7D_PAD_EPDC_DATA06__GPIO2_IO6,
	MX7D_PAD_EPDC_DATA07__GPIO2_IO7,
	MX7D_PAD_EPDC_SDCLK__GPIO2_IO16,
	MX7D_PAD_EPDC_SDLE__GPIO2_IO17,
	MX7D_PAD_EPDC_SDOE__GPIO2_IO18,
	MX7D_PAD_EPDC_SDSHR__GPIO2_IO19,
	MX7D_PAD_EPDC_SDCE0__GPIO2_IO20,
	MX7D_PAD_EPDC_SDCE1__GPIO2_IO21,
	MX7D_PAD_EPDC_GDCLK__GPIO2_IO24,
	MX7D_PAD_EPDC_GDOE__GPIO2_IO25,
	MX7D_PAD_EPDC_GDRL__GPIO2_IO26,
	MX7D_PAD_EPDC_GDSP__GPIO2_IO27,
	MX7D_PAD_EPDC_BDR0__GPIO2_IO28,
	MX7D_PAD_EPDC_BDR1__GPIO2_IO29,
};

vidinfo_t panel_info = {
	.vl_refresh = 85,
	.vl_col = 1024,
	.vl_row = 758,
	.vl_pixclock = 40000000,
	.vl_left_margin = 12,
	.vl_right_margin = 76,
	.vl_upper_margin = 4,
	.vl_lower_margin = 5,
	.vl_hsync = 12,
	.vl_vsync = 2,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};

struct epdc_timing_params panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 524,
	.gdsp_offs = 327,
	.gdoe_offs = 0,
	.gdclk_offs = 19,
	.num_ce = 1,
};

static void setup_epdc_power(void)
{
	/* IOMUX_GPR1: bit30: Disable On-chip RAM EPDC Function */
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *) IOMUXC_GPR_BASE_ADDR;

	clrsetbits_le32(&iomuxc_gpr_regs->gpr[1],
		IOMUXC_GPR_GPR1_GPR_ENABLE_OCRAM_EPDC_MASK, 0);

	/* Setup epdc voltage */

	/* EPDC_PWRSTAT - GPIO2[31] for PWR_GOOD status */
	imx_iomux_v3_setup_pad(MX7D_PAD_EPDC_PWR_STAT__GPIO2_IO31 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	gpio_direction_input(IMX_GPIO_NR(2, 31));

	/* EPDC_VCOM0 - GPIO4[14] for VCOM control */
	imx_iomux_v3_setup_pad(MX7D_PAD_I2C4_SCL__GPIO4_IO14 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(4, 14), 1);

	/* EPDC_PWRWAKEUP - GPIO2[23] for EPD PMIC WAKEUP */
	imx_iomux_v3_setup_pad(MX7D_PAD_EPDC_SDCE3__GPIO2_IO23 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 23), 1);

	/* EPDC_PWRCTRL0 - GPIO2[30] for EPD PWR CTL0 */
	imx_iomux_v3_setup_pad(MX7D_PAD_EPDC_PWR_COM__GPIO2_IO30 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 30), 1);
}

static void epdc_enable_pins(void)
{
	/* epdc iomux settings */
	imx_iomux_v3_setup_multiple_pads(epdc_enable_pads,
				ARRAY_SIZE(epdc_enable_pads));
}

static void epdc_disable_pins(void)
{
	/* Configure MUX settings for EPDC pins to GPIO  and drive to 0 */
	imx_iomux_v3_setup_multiple_pads(epdc_disable_pads,
				ARRAY_SIZE(epdc_disable_pads));
}

static void setup_epdc(void)
{
	/*** epdc Maxim PMIC settings ***/

	/* EPDC_PWRSTAT - GPIO2[31] for PWR_GOOD status */
	imx_iomux_v3_setup_pad(MX7D_PAD_EPDC_PWR_STAT__GPIO2_IO31 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* EPDC_VCOM0 - GPIO4[14] for VCOM control */
	imx_iomux_v3_setup_pad(MX7D_PAD_I2C4_SCL__GPIO4_IO14 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* EPDC_PWRWAKEUP - GPIO4[23] for EPD PMIC WAKEUP */
	imx_iomux_v3_setup_pad(MX7D_PAD_EPDC_SDCE3__GPIO2_IO23 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* EPDC_PWRCTRL0 - GPIO4[20] for EPD PWR CTL0 */
	imx_iomux_v3_setup_pad(MX7D_PAD_EPDC_PWR_COM__GPIO2_IO30 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* Set pixel clock rates for EPDC in clock.c */

	panel_info.epdc_data.wv_modes.mode_init = 0;
	panel_info.epdc_data.wv_modes.mode_du = 1;
	panel_info.epdc_data.wv_modes.mode_gc4 = 3;
	panel_info.epdc_data.wv_modes.mode_gc8 = 2;
	panel_info.epdc_data.wv_modes.mode_gc16 = 2;
	panel_info.epdc_data.wv_modes.mode_gc32 = 2;

	panel_info.epdc_data.epdc_timings = panel_timings;

	setup_epdc_power();
}

void epdc_power_on(void)
{
	unsigned int reg;
	struct gpio_regs *gpio_regs = (struct gpio_regs *)GPIO2_BASE_ADDR;

	/* Set EPD_PWR_CTL0 to high - enable EINK_VDD (3.15) */
	gpio_set_value(IMX_GPIO_NR(2, 30), 1);
	udelay(1000);

	/* Enable epdc signal pin */
	epdc_enable_pins();

	/* Set PMIC Wakeup to high - enable Display power */
	gpio_set_value(IMX_GPIO_NR(2, 23), 1);

	/* Wait for PWRGOOD == 1 */
	while (1) {
		reg = readl(&gpio_regs->gpio_psr);
		if (!(reg & (1 << 31)))
			break;

		udelay(100);
	}

	/* Enable VCOM */
	gpio_set_value(IMX_GPIO_NR(4, 14), 1);

	udelay(500);
}

void epdc_power_off(void)
{
	/* Set PMIC Wakeup to low - disable Display power */
	gpio_set_value(IMX_GPIO_NR(2, 23), 0);

	/* Disable VCOM */
	gpio_set_value(IMX_GPIO_NR(4, 14), 0);

	epdc_disable_pins();

	/* Set EPD_PWR_CTL0 to low - disable EINK_VDD (3.15) */
	gpio_set_value(IMX_GPIO_NR(2, 30), 0);
}
#endif

#ifdef CONFIG_USB_EHCI_MX7
static iomux_v3_cfg_t const usb_otg1_pads[] = {
	MX7D_PAD_GPIO1_IO05__USB_OTG1_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const usb_otg2_pads[] = {
	MX7D_PAD_UART3_CTS_B__USB_OTG2_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
};

/* On RevB board, the GPIO_IO07 is muxed for OTG2 PWR */
iomux_v3_cfg_t const usb_otg2_revB_pads[] = {
	MX7D_PAD_GPIO1_IO07__USB_OTG2_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_usb(void)
{
	imx_iomux_v3_setup_multiple_pads(usb_otg1_pads,
						 ARRAY_SIZE(usb_otg1_pads));

	if (mx7sabre_rev() >= BOARD_REV_B)
		imx_iomux_v3_setup_multiple_pads(usb_otg2_revB_pads,
						 ARRAY_SIZE(usb_otg2_revB_pads));
	else
		imx_iomux_v3_setup_multiple_pads(usb_otg2_pads,
						 ARRAY_SIZE(usb_otg2_pads));
}

extern int usb_phy_mode(int port);
int board_usb_phy_mode(int port)
{
	if (port == 0)
		return usb_phy_mode(port);
	else
		return USB_INIT_HOST;
}

#endif

int board_early_init_f(void)
{
	setup_iomux_uart();

#ifdef CONFIG_SYS_I2C_MXC
	setup_i2c(3, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info4);
#endif

#ifdef CONFIG_USB_EHCI_MX7
	setup_usb();
#endif

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	imx_iomux_v3_setup_multiple_pads(iox_pads, ARRAY_SIZE(iox_pads));

	iox74lv_init();

#ifdef CONFIG_FEC_MXC
	setup_fec(CONFIG_FEC_ENET_DEV);
#endif

#ifdef CONFIG_NAND_MXS
	setup_gpmi_nand();
#endif

#ifdef CONFIG_FSL_QSPI
	board_qspi_init();
#endif

#ifdef CONFIG_MXC_EPDC
	if (mx7sabre_rev() >= BOARD_REV_B) {
		/*  On RevB, GPIO1_IO04 is used for ENET2 EN,
		*  so set its output to high to isolate the ENET2 signals for EPDC
		*/
		imx_iomux_v3_setup_multiple_pads(fec2_en_pads,
			ARRAY_SIZE(fec2_en_pads));
		gpio_direction_output(IMX_GPIO_NR(1, 4), 1);
	} else {
		qn_output[5] = qn_disable;
		iox74lv_set(5);
	}
	setup_epdc();
#endif

	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"sd1", MAKE_CFGVAL(0x10, 0x10, 0x00, 0x00)},
	{"emmc", MAKE_CFGVAL(0x10, 0x2a, 0x00, 0x00)},
	/* TODO: Nand */
	{"qspi", MAKE_CFGVAL(0x00, 0x40, 0x00, 0x00)},
	{NULL,   0},
};
#endif

#ifdef CONFIG_POWER
#define I2C_PMIC	0
int power_init_board(void)
{
	i2c_set_bus_num(3);

	axp_init();
	/* Disable LDO0 and ALDO2 */
	axp_disable_ldo0();
	axp_set_power_output(0xFB);
	mdelay(20);

	axp_set_ldo0(AXP152_LDO0_3V3, AXP152_LDO0_CURR_1500MA);
	axp_set_aldo2(AXP152_ALDO_3V3);

	axp_set_dcdc3(1350);
	axp_set_power_output(0xFF);

	return 0;
}
#endif

int board_late_init(void)
{
	struct wdog_regs *wdog = (struct wdog_regs *)WDOG1_BASE_ADDR;
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_env_init();
#endif

	imx_iomux_v3_setup_multiple_pads(wdog_pads, ARRAY_SIZE(wdog_pads));

	set_wdog_reset(wdog);

	return 0;
}

int checkboard(void)
{
	printf("Board: multa imx7d\n");

	return 0;
}

#ifdef CONFIG_FSL_FASTBOOT
#ifdef CONFIG_ANDROID_RECOVERY
/* Use S3 button for recovery key */
#define GPIO_VOL_DN_KEY IMX_GPIO_NR(5, 10)
iomux_v3_cfg_t const recovery_key_pads[] = {
	(MX7D_PAD_SD2_WP__GPIO5_IO10 | MUX_PAD_CTRL(BUTTON_PAD_CTRL)),
};

int is_recovery_key_pressing(void)
{
	int button_pressed = 0;

	/* Check Recovery Combo Button press or not. */
	imx_iomux_v3_setup_multiple_pads(recovery_key_pads,
		ARRAY_SIZE(recovery_key_pads));

	gpio_direction_input(GPIO_VOL_DN_KEY);

	if (gpio_get_value(GPIO_VOL_DN_KEY) == 0) { /* VOL_DN key is low assert */
		button_pressed = 1;
		printf("Recovery key pressed\n");
	}

	return button_pressed;
}
#endif /*CONFIG_ANDROID_RECOVERY*/
#endif /*CONFIG_FSL_FASTBOOT*/
