/*
 * (C) Copyright 2016 Toradex
 * Author: Stefan Agner <stefan.agner@toradex.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <spl.h>
#include <usb.h>
#include <g_dnl.h>
#include <sdp.h>

DECLARE_GLOBAL_DATA_PTR;

static int spl_sdp_load_image(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev)
{
	int ret;
	const int controller_index = 0;

	g_dnl_clear_detach();
	g_dnl_register("usb_dnl_sdp");

	ret = sdp_init(controller_index);
	if (ret) {
		error("SDP init failed: %d\n", ret);
		return -ENODEV;
	}

	/* This command typically does not return but jumps to an image */
	sdp_handle(controller_index);
	error("SDP ended\n");

	return -EINVAL;
}
SPL_LOAD_IMAGE_METHOD("USB SDP", 0, BOOT_DEVICE_BOARD, spl_sdp_load_image);
