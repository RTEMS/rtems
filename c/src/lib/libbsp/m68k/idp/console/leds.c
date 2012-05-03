/*
 * leds.c -- control the led's on a Motorola mc68ec0x0 board.
 *           Written by rob@cygnus.com (Rob Savoye)
 */
#include "leds.h"

void zylons(void);
void clear_leds(void);

/*
 * led_putnum --
 *      print a hex number on the LED. the value of num must be a char with
 *      the ascii value. ie... number 0 is '0', a is 'a', ' ' (null) clears
 *	the led display.
 *	Setting the bit to 0 turns it on, 1 turns it off.
 * 	the LED's are controlled by setting the right bit mask in the base
 * 	address.
 *	The bits are:
 *	[d.p | g | f | e | d | c | b | a ] is the byte.
 *
 *	The locations are:
 *
 *			 a
 *		       -----
 *		    f |     | b
 *		      |  g  |
 *		       -----
 *                    |     |
 *                  e |     | c
 *                     -----
 *                       d                . d.p (decimal point)
 */
void
led_putnum ( char num )
{
    static unsigned char *leds = (unsigned char *)LED_ADDR;
    static unsigned char num_bits [18] = {
      0xff,						/* clear all */
      0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x98, /* numbers 0-9 */
      0x98, 0x20, 0x3, 0x27, 0x21, 0x4, 0xe 		/* letters a-f */
    };

    if (num >= '0' && num <= '9')
      num = (num - '0') + 1;

    if (num >= 'a' && num <= 'f')
      num = (num - 'a') + 12;

    if (num == ' ')
      num = 0;

    *leds = num_bits[(int)num];
}

/* This procedure added by Doug McBride, Colorado Space Grant College --
   Probably should be a macro instead */
void
clear_leds ( )
{
    static unsigned char *leds = (unsigned char *)LED_ADDR;
    *leds = 0xFF;
}

void rtems_bsp_delay( int );

/*
 * zylons -- draw a rotating pattern. NOTE: this function never returns.
 */
void
zylons()
{
  unsigned char *leds 	= (unsigned char *)LED_ADDR;
  unsigned char curled = 0xfe;

  while (1)
    {
      *leds = curled;
      curled = (curled >> 1) | (curled << 7);
      rtems_bsp_delay ( 8000 );
    }
}
