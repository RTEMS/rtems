/**
 * @file
 *
 * @brief POSIX_signals clear_process_signals
 * @ingroup POSIXAPI
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

#include <rtems/posix/psignalimpl.h>
#include <rtems/score/assert.h>

void _POSIX_signals_Clear_process_signals(
  int signo
)
{
  sigset_t   mask;
  bool       clear_signal;

  _Assert( _ISR_Get_level() != 0 );

  clear_signal = true;
  mask         = signo_to_mask( signo );

  if ( _POSIX_signals_Vectors[ signo ].sa_flags == SA_SIGINFO ) {
    if ( !_Chain_Is_empty( &_POSIX_signals_Siginfo[ signo ] ) )
      clear_signal = false;
  }

  if ( clear_signal ) {
    _POSIX_signals_Pending &= ~mask;
  }
}
