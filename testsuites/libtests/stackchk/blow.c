/*  task1.c
 *
 *  This set of three tasks do some simple task switching for about
 *  15 seconds and then call a routine to "blow the stack".
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>

void b() {}

void blow_stack( void )
{
  volatile unsigned32 *low, *high;
  unsigned char *area;

b();
  /*
   *  Destroy the first and last 16 bytes of our stack... Hope it
   *  does not cause problems :)
   */

 area = (unsigned char *)_Thread_Executing->Start.Initial_stack.area;

 low  = (volatile unsigned32 *) (area + HEAP_OVERHEAD);
 high = (volatile unsigned32 *)
            (area + _Thread_Executing->Start.Initial_stack.size - 16);


 low[0] = 0x11111111;
 low[1] = 0x22222222;
 low[2] = 0x33333333;
 low[3] = 0x44444444;

 high[0] = 0x55555555;
 high[1] = 0x66666666;
 high[2] = 0x77777777;
 high[3] = 0x88888888;

}
