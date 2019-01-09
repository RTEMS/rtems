/*  task1.c
 *
 *  This set of three tasks do some simple task switching for about
 *  15 seconds and then call a routine to "blow the stack".
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/stackchk.h>

#include "system.h"

void blow_stack(void)
{
  Thread_Control *executing;
  char *area;
  volatile uintptr_t *low;
  volatile uintptr_t *high;

  executing = _Thread_Get_executing();

  /*
   *  Destroy the first and last 4 words of our stack area... Hope it
   *  does not cause problems :)
   */

  area = (char *) executing->Start.Initial_stack.area;
  low  = (uintptr_t *) area;
  high = (uintptr_t *)
    (area + executing->Start.Initial_stack.size - 4 * sizeof(*high));

  low[0] = 0x11111111;
  low[1] = 0x22222222;
  low[2] = 0x33333333;
  low[3] = 0x44444444;

  high[0] = 0x55555555;
  high[1] = 0x66666666;
  high[2] = 0x77777777;
  high[3] = 0x88888888;

  rtems_stack_checker_report_usage();
}
