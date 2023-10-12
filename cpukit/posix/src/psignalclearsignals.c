/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_SIGNALS POSIX Signals Support
 *
 * @brief POSIX Signals Clear Signals
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <stdio.h>

/*
 *  _POSIX_signals_Clear_signals
 */

bool _POSIX_signals_Clear_signals(
  POSIX_API_Control  *api,
  int                 signo,
  siginfo_t          *info,
  bool                is_global,
  bool                check_blocked,
  bool                do_signals_acquire_release
)
{
  sigset_t                    mask;
  sigset_t                    signals_unblocked;
  Thread_queue_Context        queue_context;
  bool                        do_callout;
  POSIX_signals_Siginfo_node *psiginfo;

  mask = signo_to_mask( signo );

  do_callout = false;

  /* set blocked signals based on if checking for them, SIGNAL_ALL_MASK
   * insures that no signals are blocked and all are checked.
   */

  if ( check_blocked )
    signals_unblocked = api->signals_unblocked;
  else
    signals_unblocked = SIGNAL_ALL_MASK;

  /* XXX is this right for siginfo type signals? */
  /* XXX are we sure they can be cleared the same way? */

  if ( do_signals_acquire_release ) {
    _Thread_queue_Context_initialize( &queue_context );
    _POSIX_signals_Acquire( &queue_context );
  }

    if ( is_global ) {
       if ( mask & (_POSIX_signals_Pending & signals_unblocked) ) {
         if ( _POSIX_signals_Vectors[ signo ].sa_flags == SA_SIGINFO ) {
           psiginfo = (POSIX_signals_Siginfo_node *)
             _Chain_Get_unprotected( &_POSIX_signals_Siginfo[ signo ] );
           _POSIX_signals_Clear_process_signals( signo );
           /*
            *  It may be impossible to get here with an empty chain
            *  BUT until that is proven we need to be defensive and
            *  protect against it.
            */
           if ( psiginfo ) {
             *info = psiginfo->Info;
             _Chain_Append_unprotected(
               &_POSIX_signals_Inactive_siginfo,
               &psiginfo->Node
             );
           } else
             do_callout = false;
         }
         _POSIX_signals_Clear_process_signals( signo );
         do_callout = true;
       }
    } else {
      if ( mask & (api->signals_pending & signals_unblocked) ) {
        api->signals_pending &= ~mask;
        do_callout = true;
      }
    }

  if ( do_signals_acquire_release ) {
    _POSIX_signals_Release( &queue_context );
  }

  return do_callout;
}
