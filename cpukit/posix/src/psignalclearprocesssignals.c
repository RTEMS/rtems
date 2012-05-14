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
#include <rtems/score/tqdata.h>
#include <rtems/score/wkspace.h>
#include <rtems/seterr.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/time.h>
#include <stdio.h>

/*
 *  _POSIX_signals_Clear_process_signals
 */

void _POSIX_signals_Clear_process_signals(
  int signo
)
{
  sigset_t   mask;
  bool       clear_signal;

  clear_signal = true;
  mask         = signo_to_mask( signo );

  ISR_Level  level;

  _ISR_Disable( level );
    if ( _POSIX_signals_Vectors[ signo ].sa_flags == SA_SIGINFO ) {
      if ( !_Chain_Is_empty( &_POSIX_signals_Siginfo[ signo ] ) )
       clear_signal = false;
    }
    if ( clear_signal ) {
      _POSIX_signals_Pending &= ~mask;
    }
  _ISR_Enable( level );
}
