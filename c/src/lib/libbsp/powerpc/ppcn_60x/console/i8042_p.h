/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */

#ifndef _I8042_P_H_
#define _I8042_P_H_

#ifdef __cplusplus
extern "C" {
#endif

#define KBD_CTR_WRITE_COMMAND   0x60
#define KBD_CTR_READ_COMMAND    0x20
#define KBD_CTR_TEST_PASSWORD   0xA4
#define KBD_CTR_LOAD_PASSWORD   0xA5
#define KBD_CTR_ENABLE_PASSWORD 0xA6
#define KBD_CTR_DISABLE_AUX     0xA7
#define KBD_CTR_ENABLE_AUX      0xA8
#define KBD_CTR_AUXLINES_TEST   0xA9
#define KBD_CTR_SELFTEST        0xAA
#define KBD_CTR_KBDLINES_TEST   0xAB
#define KBD_CTR_DISABLE_KBD     0xAD
#define KBD_CTR_ENABLE_KBD      0xAE
#define KBD_CTR_WRITE_AUX       0xD4
#define KBD_CTR_READ_REV        0xD5
#define KBD_CTR_READ_VER        0xD6
#define KBD_CTR_READ_MODEL      0xD7

/* Keyboard Controller Data */
#define KBD_CTR_SELFTEST_PASSED 0x55
#define KBD_CTR_PASSWORD_INSTALLED 0xFA
#define KBD_CTR_PASSWORD_NOT_INSTALLED 0xF1

/* Controller Command Byte bit definitions. */
#define KBD_CMD_BYTE_DISABLE_AUX 0x20
#define KBD_CMD_BYTE_DISABLE_KBD 0x10
#define KBD_CMD_ENABLE_AUX_INT   0x02
#define KBD_CMD_ENABLE_KBD_INT   0x01

/* Keyboard Controller Status byte masks */
#define KBD_OBF_MASK        0x1         /* Output buffer full */
#define KBD_IBF_MASK        0x2         /* Input buffer full */
#define KBD_FROM_AUX_MASK   0x20        /* Byte from Aux Port. */

/* Interface Test Results */
#define INTERFACE_NO_ERROR  0x00
#define CLOCK_STUCK_LOW     0x01
#define CLOCK_STUCK_HIGH    0x02
#define DATA_STUCK_LOW      0x03
#define DATA_STUCK_HIGH     0x04

/* Timeout */
#define KBD_TIMEOUT         500000	/* Effectively ISA read access times */

/* Keyboard Commands */
#define KBD_CMD_SET_LEDS	0xed	/* Set/Reset LEDs */
#define KBD_CMD_ECHO		0xee    /* request keyboard echo resp. "EE" */
#define KBD_CMD_SEL_SCAN_CODE	0xf0    /* Scan codes 1,2,3 or 0 = rquest current. */
#define KBD_CMD_READ_ID		0xf2    /* Request for two byte response */
#define KBD_CMD_SET_RATE	0xf3    /* Set tellematic Rate */
#define KBD_CMD_ENABLE		0xf4    /* Clears Buffer and Starts Scanning. */
#define KBD_CMD_DISABLE		0xf5    /* reset to power up */

#define KBD_CMD_SET_DEFAULT	0xf6    
#define KBD_CMD_SET_ALL_TLMTIC	0xf7    /* Set all keys telematic */
#define KBD_CMD_SET_ALL_MKBR	0xf8    /* Set all keys Make /Break */
#define KBD_CMD_SET_ALL_MAKE	0xf9    /* Set all keys Make only */
#define KBD_CMD_SET_KEY_TLMTIC	0xfb    /* Set individual key telemativ */
#define KBD_CMD_SET_KEY_MKBR	0xfc    /* set individual key make/break */
#define KBD_CMD_SET_KEY_MK	0xfd    /* set individual key make only */
#define KBD_CMD_RESEND		0xfe    /* request to resend last transfer */
#define KBD_CMD_RESET		0xff    /* request to start a program reset */
#define KBD_CMD_ACK		0xfa    /* keyboard ack after reset */
#define KBD_CMD_BAT		0xaa    /* Keyboard Bat completion Response */

/*
 * Define LED encodings for use with the KbdSetLEDs command
 */
#define KBD_LED_CAPS		0x04
#define KBD_LED_NUM		0x02
#define KBD_LED_SCROLL		0x01

/*
 * Define two code scan codes in keycode order
 */
#define KEY_TWO_KEY       0xe0
#define KEY_ALT           0x38
#define KEY_CONTROL       0x1d
#define KEY_INSERT        0x52
#define KEY_DELETE        0x53
#define KEY_LEFT_ARROW    0x4b
#define KEY_HOME          0x47
#define KEY_END           0x4F
#define KEY_UP_ARROW      0x48
#define KEY_DOWN_ARROW    0x50
#define KEY_PAGE_UP       0x49
#define KEY_PAGE_DOWN     0x51
#define KEY_RIGHT_ARROW   0x4d
#define KEY_KEYPAD_SLASH  0x35
#define KEY_KEYPAD_ENTER  0x1c
#define KEY_SYS_REQUEST   0x2a
#define KEY_PRINT_SCREEN  0x37

#define KEY_LEFT_SHIFT    0x2a
#define KEY_RIGHT_SHIFT   0x36
#define KEY_TAB		  0x0f
#define KEY_CAPS_LOCK     0x3a
#define KEY_NUM_LOCK      0x45
#define KEY_SCROLL_LOCK   0x46
#define KEY_ESC           0x01
#define KEY_F1            0x3b
#define KEY_F2            0x3c
#define KEY_F3            0x3d
#define KEY_F4            0x3e
#define KEY_F5            0x3f
#define KEY_F6            0x40
#define KEY_F7            0x41
#define KEY_F8            0x42
#define KEY_F9            0x43
#define KEY_F10           0x44
#define KEY_F11           0x57
#define KEY_F12           0x58

/*
 * ASCII control codes
 */
#define ASCII_NUL 0x00
#define ASCII_SOH 0x01
#define ASCII_STX 0x02
#define ASCII_ETX 0x03
#define ASCII_EOT 0x04
#define ASCII_ENQ 0x05
#define ASCII_ACK 0x06
#define ASCII_BEL 0x07
#define ASCII_BS  0x08
#define ASCII_HT  0x09
#define ASCII_LF  0x0a
#define ASCII_VT  0x0b
#define ASCII_FF  0x0c
#define ASCII_CR  0x0d
#define ASCII_SO  0x0e
#define ASCII_SI  0x0f
#define ASCII_DLE 0x10
#define ASCII_XON 0x11
#define ASCII_DC1 0x11
#define ASCII_DC2 0x12
#define ASCII_XOFF 0x13
#define ASCII_DC3 0x13
#define ASCII_DC4 0x14
#define ASCII_NAK 0x15
#define ASCII_SYN 0x16
#define ASCII_ETB 0x17
#define ASCII_CAN 0x18
#define ASCII_EM  0x19
#define ASCII_SUB 0x1a
#define ASCII_ESC 0x1b
#define ASCII_FS  0x1c
#define ASCII_GS  0x1d
#define ASCII_RS  0x1e
#define ASCII_US  0x1f
#define ASCII_DEL 0x7f
#define ASCII_SYSRQ 0x80
#define ASCII_CSI 0x9b

/*
 * Exported functions
 */
extern boolean i8042_probe(int minor);

extern void i8042_init(int minor);
#if CONSOLE_USE_INTERRUPTS
extern void i8042_initialize_interrupts(int minor);

#else
extern int i8042_inbyte_nonblocking_polled(
	int minor
);
#endif /* CONSOLE_USE_INTERRUPTS */

#ifdef __cplusplus
}
#endif

#endif /* _I8042_P_H_ */
