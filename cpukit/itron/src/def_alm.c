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
 *  def_alm - Define Alarm Handler
 */

ER def_alm(
  HNO     almno,
  T_DALM *pk_dalm
)
{
#if 0
  Objects_Control *objectAlrm;
  Watchdog_Interval timeInterval;

  timeInterval = pk_dalm->almtim / 1000;
  (void) _Watchdog_Remove(&objectAlrm);
  _Watchdog_Initialize(
           objectAlrm,
           pk_dalm->almhdr,
           almno,
           pk_dalm->exinf);
  _Watchdong_Insert_seconds(objectAlrm,timeInterval);
  _Thread_Enable_dispatch();
#endif

  return E_OK;
}
