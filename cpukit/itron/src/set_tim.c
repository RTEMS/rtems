/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/itron/time.h>

/*
 *  set_tim - Set System Clock
 */

ER set_tim(
  SYSTIME *pk_tim __attribute__((unused))
)
{
#if 0
 struct timespec   time;

/* convert *pk_tim which is 48 bits integer in binary into an ordinary
 integer in milliseconds */

/* XXX */ temp = 0;
  if(temp > 0) {
    _Thread_Disable_dispatch();
      _TOD_Set(&time);
    _Thread_Enable_dispatch();
    return E_OK;
  }
#endif
  return E_SYS;
}
