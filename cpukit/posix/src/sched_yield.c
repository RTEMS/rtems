/**
 * @file
 *
 * @brief Yield Processor
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>

#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

int sched_yield( void )
{
  _Thread_Disable_dispatch();
    _Thread_Yield( _Thread_Executing );
  _Thread_Enable_dispatch();
  return 0;
}
