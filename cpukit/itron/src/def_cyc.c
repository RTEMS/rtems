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
 *  def_cyc - Define Cyclic Handler
 */

ER def_cyc(
  HNO     cycno,
  T_DCYC *pk_dcyc
)
{
  Watchdog_Control *object;
#if 0
  Watchdog_Interval timeInterval;
#endif

/* XXX */ object = 0;
  if ( pk_dcyc->cycatr != TA_HLNG)
     return E_RSATR;
#if 0
  timeInterval = pk_dcyc->cyctim / 1000;
  object->delta_interval = timeInterval;
#endif
  _Watchdog_Initialize(
         object,
         pk_dcyc->cychdr,
         cycno,
         pk_dcyc->exinf);
  _Watchdog_Deactivate(object);

  return E_OK;
}
