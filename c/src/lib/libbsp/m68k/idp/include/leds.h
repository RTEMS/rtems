/*
 * leds.c -- control the led's on a Motorola mc68ec0x0 board.
 *           Written by rob@cygnus.com (Rob Savoye)
 */

#ifndef __LEDS_H__
#define __LEDS_H__

#define LED_ADDR	0xd00003
#define LED_0           ~0x1
#define LED_1           ~0x2
#define LED_2           ~0x4
#define LED_3           ~0x8
#define LED_4           ~0x10
#define LED_5           ~0x20
#define LED_6           ~0x40
#define LED_7           ~0x80
#define LEDS_OFF	0xff
#define LEDS_ON		0x0

#define FUDGE(x) ((x >= 0xa && x <= 0xf) ? (x + 'a') & 0x7f : (x + '0') & 0x7f)

extern void led_putnum( char );

#endif		/* __LEDS_H__ */
