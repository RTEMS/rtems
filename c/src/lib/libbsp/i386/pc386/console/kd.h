/*
 *  by: Rosimildo da Silva:  rdasilva@connecttel.com
 *
 *
 */

#ifndef _LINUX_KD_H
#define _LINUX_KD_H
#include <sys/types.h>

/* 0x4B is 'K', to avoid collision with termios and vt */

#define KIOCSOUND	0x4B2F	/* start sound generation (0 for off) */
#define KDMKTONE	0x4B30	/* generate tone */

#define KDGETLED	0x4B31	/* return current led state */
#define KDSETLED	0x4B32	/* set led state [lights, not flags] */
#define 	LED_SCR		0x01	/* scroll lock led */
#define 	LED_CAP		0x04	/* caps lock led */
#define 	LED_NUM		0x02	/* num lock led */

#define  KDGKBTYPE	0x4B33	/* get keyboard type */
#define 	KB_84		   0x01
#define 	KB_101		0x02 	/* this is what we always answer */
#define 	KB_OTHER	   0x03

#define     KDSETMODE	0x4B3A	/* set text/graphics mode */
#define		KD_TEXT		0x00
#define		KD_GRAPHICS	0x01
#define		KD_TEXT0	0x02	/* obsolete */
#define		KD_TEXT1	0x03	/* obsolete */
#define     KDGETMODE	0x4B3B	/* get current mode */

#define		K_RAW		   0x00
#define		K_XLATE		0x01
#define		K_MEDIUMRAW	0x02
#define		K_UNICODE	0x03
#define     KDGKBMODE	0x4B44	/* gets current keyboard mode */
#define     KDSKBMODE	0x4B45	/* sets current keyboard mode */

#define		K_METABIT	0x03
#define		K_ESCPREFIX	0x04
#define     KDGKBMETA	0x4B62	/* gets meta key handling mode */
#define     KDSKBMETA	0x4B63	/* sets meta key handling mode */

#define		K_SCROLLLOCK	0x01
#define		K_CAPSLOCK	0x02
#define		K_NUMLOCK	0x04
#define	   KDGKBLED	0x4B64	/* get led flags (not lights) */
#define     KDSKBLED	0x4B65	/* set led flags (not lights) */

struct kbentry {
	unsigned char kb_table;
	unsigned char kb_index;
	unsigned short kb_value;
};
#define		K_NORMTAB	0x00
#define		K_SHIFTTAB	0x01
#define		K_ALTTAB	0x02
#define		K_ALTSHIFTTAB	0x03

#define KDGKBENT	0x4B46	/* gets one entry in translation table */
#define KDSKBENT	0x4B47	/* sets one entry in translation table */

struct kbsentry {
	unsigned char kb_func;
	unsigned char kb_string[512];
};

struct kbdiacr {
        unsigned char diacr, base, result;
};
struct kbdiacrs {
        unsigned int kb_cnt;    /* number of entries in following array */
	struct kbdiacr kbdiacr[256];    /* MAX_DIACR from keyboard.h */
};
#define KDGKBDIACR      0x4B4A  /* read kernel accent table */
#define KDSKBDIACR      0x4B4B  /* write kernel accent table */

struct kbkeycode {
	unsigned int scancode, keycode;
};
#define KDGETKEYCODE	0x4B4C	/* read kernel keycode table entry */
#define KDSETKEYCODE	0x4B4D	/* write kernel keycode table entry */

#define KDSIGACCEPT	0x4B4E	/* accept kbd generated signals */

#define KDGHWCLK        0x4B50	/* get hardware clock */
#define KDSHWCLK        0x4B51  /* set hardware clock */

struct kbd_repeat {
	int delay;	/* in msec; <= 0: don't change */
	int rate;	/* in msec; <= 0: don't change */
};

#define KDKBDREP        0x4B52  /* set keyboard delay/repeat rate;
                                 * actually used values are returned */

/* note: 0x4B00-0x4B4E all have had a value at some time;
   don't reuse for the time being */
/* note: 0x4B60-0x4B6D, 0x4B70-0x4B72 used above */

#endif /* _LINUX_KD_H */
