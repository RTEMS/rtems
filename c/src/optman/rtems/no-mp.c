/*
 *  Multiprocessing Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#if !defined(RTEMS_MULTIPROCESSING)
/* char rtems_no_multiprocessing; */
#else
#include <rtems/rtems/status.h>
#include <rtems/rtems/mp.h>
#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/interr.h>

void _Multiprocessing_Manager_initialization ( void )
{
}
#endif
