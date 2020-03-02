/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/errno.h>
#include <asm/imx-common/video.h>

int board_video_skip(void)
{
	int i;
	int ret;
	char const *panel = getenv("panel");

	if (!panel) {
		for (i = 0; i < display_count; i++) {
			struct display_info_t const *dev = displays+i;
			if (dev->detect && dev->detect(dev)) {
				panel = dev->mode.name;
				printf("auto-detected panel %s\n", panel);
				break;
			}
		}
		if (!panel) {
			panel = displays[0].mode.name;
			printf("No panel detected: default to %s\n", panel);
			i = 0;
		}
	} else {
		for (i = 0; i < display_count; i++) {
			if (!strcmp(panel, displays[i].mode.name))
				break;
		}
	}

	if (i < display_count) {
#if defined(CONFIG_VIDEO_IPUV3)
		ret = ipuv3_fb_init(&displays[i].mode, displays[i].di ? 1 : 0,
				    displays[i].pixfmt);
#elif defined(CONFIG_VIDEO_IMXDPUV1)
		ret = imxdpuv1_fb_init(&displays[i].mode, displays[i].bus,
					displays[i].pixfmt);
#elif defined(CONFIG_VIDEO_IMXDCSS)
		ret = imx8m_fb_init(&displays[i].mode, displays[i].bus,
					displays[i].pixfmt);
#elif defined(CONFIG_VIDEO_MXS)
		ret = mxs_lcd_panel_setup(displays[i].mode,
					displays[i].pixfmt,
				    displays[i].bus);
#endif
		if (!ret) {
			if (displays[i].enable)
				displays[i].enable(displays + i);

			printf("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		if (strcmp(panel, "NULL"))
			printf("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	return 0;
}

#ifdef CONFIG_IMX_HDMI
#include <asm/arch/mxc_hdmi.h>
#include <asm/io.h>
int detect_hdmi(struct display_info_t const *dev)
{
	struct hdmi_regs *hdmi	= (struct hdmi_regs *)HDMI_ARB_BASE_ADDR;
	return readb(&hdmi->phy_stat0) & HDMI_DVI_STAT;
}
#endif
