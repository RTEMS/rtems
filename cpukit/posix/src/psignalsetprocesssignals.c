/**
 *  @file
 *
 *  @brief POSIX Signals Set Process Signals
 *  @ingroup POSIX_SIGNALS
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/seterr.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/time.h>
#include <stdio.h>

void _POSIX_signals_Set_process_signals(
  sigset_t   mask
)
{
  ISR_Level  level;

  _POSIX_signals_Acquire( level );
    _POSIX_signals_Pending |= mask;
  _POSIX_signals_Release( level );
}
