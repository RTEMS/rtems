/*
 * mc68ec.c -- Low level support for the Motorola mc68ec0x0 board.
 *             Written by rob@cygnus.com (Rob Savoye)
 */

#include "leds.h"

/*
 * rtems_bsp_delay -- delay execution. This is an ugly hack. It should
 *          use the timer, but I'm waiting for docs. (sigh)
 */
void rtems_bsp_delay(int num)
{
  while (num--)
    {
      __asm__ ("nop");
    }
}
