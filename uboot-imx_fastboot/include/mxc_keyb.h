/*
 * Copyright (C) 2009-2014 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*!
 * @defgroup keypad Keypad Driver
 */

/*!
 * @file mxc_keyb.h
 *
 * @brief MXC keypad header file.
 *
 * @ingroup keypad
 */
#ifndef __MXC_KEYB_H__
#define __MXC_KEYB_H__

/*!
 * Keypad Module Name
 */
#define MOD_NAME  "mxckpd"

/*!
 * Keypad irq number
 */
#define KPP_IRQ  MXC_INT_KPP

/*!
 * XLATE mode selection
 */
#define KEYPAD_XLATE        0

/*!
 * RAW mode selection
 */
#define KEYPAD_RAW          1

/*!
 * Maximum number of keys.
 */
#define MAXROW			8
#define MAXCOL			8
#define MXC_MAXKEY		(MAXROW * MAXCOL)

/*!
 * This define indicates break scancode for every key release. A constant
 * of 128 is added to the key press scancode.
 */
#define  MXC_KEYRELEASE   128

/*
 * _reg_KPP_KPCR   _reg_KPP_KPSR _reg_KPP_KDDR _reg_KPP_KPDR
 * Keypad Control Register Address
 */
#define KPCR    (KPP_BASE_ADDR + 0x00)

/*
 * Keypad Status Register Address
 */
#define KPSR    (KPP_BASE_ADDR + 0x02)

/*
 * Keypad Data Direction Address
 */
#define KDDR    (KPP_BASE_ADDR + 0x04)

/*
 * Keypad Data Register
 */
#define KPDR    (KPP_BASE_ADDR + 0x06)

/*
 * Key Press Interrupt Status bit
 */
#define KBD_STAT_KPKD        0x01

/*
 * Key Release Interrupt Status bit
 */
#define KBD_STAT_KPKR        0x02

/*
 * Key Depress Synchronizer Chain Status bit
 */
#define KBD_STAT_KDSC        0x04

/*
 * Key Release Synchronizer Status bit
 */
#define KBD_STAT_KRSS        0x08

/*
 * Key Depress Interrupt Enable Status bit
 */
#define KBD_STAT_KDIE        0x100

/*
 * Key Release Interrupt Enable
 */
#define KBD_STAT_KRIE        0x200

/*
 * Keypad Clock Enable
 */
#define KBD_STAT_KPPEN       0x400

/*!
 * Buffer size of keypad queue. Should be a power of 2.
 */
#define KPP_BUF_SIZE    128

/*!
 * Test whether bit is set for integer c
 */
#define TEST_BIT(c, n) ((c) & (0x1 << (n)))

/*!
 * Set nth bit in the integer c
 */
#define BITSET(c, n)   ((c) | (1 << (n)))

/*!
 * Reset nth bit in the integer c
 */
#define BITRESET(c, n) ((c) & ~(1 << (n)))

enum KeyEvent {
	KDepress,
	KRelease
};

/*!
 * This enum represents the keypad state machine to maintain debounce logic
 * for key press/release.
 */
enum KeyState {

	/*!
	 * Key press state.
	 */
	KStateUp,

	/*!
	 * Key press debounce state.
	 */
	KStateFirstDown,

	/*!
	 * Key release state.
	 */
	KStateDown,

	/*!
	 * Key release debounce state.
	 */
	KStateFirstUp
};

/*!
 * Keypad Private Data Structure
 */
struct keypad_priv {

	/*!
	 * Keypad state machine.
	 */
	enum KeyState iKeyState;

	/*!
	 * Number of rows configured in the keypad matrix
	 */
	unsigned long kpp_rows;

	/*!
	 * Number of Columns configured in the keypad matrix
	 */
	unsigned long kpp_cols;
};

/*!
 * Keypad Data Structure
 * */
struct kpp_key_info {
	enum KeyEvent evt;
	unsigned short val;
};

int mxc_kpp_init(void);
int mxc_kpp_getc(struct kpp_key_info **);

/*!
 * These functions are used to configure and the GPIO pins for keypad to
 * activate and deactivate it.
 */
void setup_mxc_kpd(void);


#endif				/* __MXC_KEYB_H__ */
