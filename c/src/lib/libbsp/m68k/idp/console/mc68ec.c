/*
 * mc68ec.c -- Low level support for the Motorola mc68ec0x0 board.
 *             Written by rob@cygnus.com (Rob Savoye)
 */
#include "leds.h"

/*
 * delay -- delay execution. This is an ugly hack. It should
 *          use the timer, but I'm waiting for docs. (sigh)
 */
void delay(num)
int num;
{
  while (num--)
    {
      asm ("nop");
    }
}
