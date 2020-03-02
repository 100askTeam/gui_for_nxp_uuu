/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __PFUZE_BOARD_HELPER__
#define __PFUZE_BOARD_HELPER__

#ifdef CONFIG_DM_PMIC_PFUZE100
struct udevice *pfuze_common_init(void);
int pfuze_mode_init(struct udevice *dev, u32 mode);

#elif defined(CONFIG_POWER_PFUZE100)
struct pmic *pfuze_common_init(unsigned char i2cbus);
int pfuze_mode_init(struct pmic *p, u32 mode);
#endif

#endif
