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
 *  _POSIX_signals_Clear_signals
 */

bool _POSIX_signals_Clear_signals(
  POSIX_API_Control  *api,
  int                 signo,
  siginfo_t          *info,
  bool                is_global,
  bool                check_blocked
)
{
  sigset_t                    mask;
  sigset_t                    signals_blocked;
  ISR_Level                   level;
  bool                        do_callout;
  POSIX_signals_Siginfo_node *psiginfo;

  mask = signo_to_mask( signo );

  do_callout = false;

  /* set blocked signals based on if checking for them, SIGNAL_ALL_MASK
   * insures that no signals are blocked and all are checked.
   */

  if ( check_blocked )
    signals_blocked = ~api->signals_blocked;
  else
    signals_blocked = SIGNAL_ALL_MASK;

  /* XXX is this right for siginfo type signals? */
  /* XXX are we sure they can be cleared the same way? */

  _ISR_Disable( level );
    if ( is_global ) {
       if ( mask & (_POSIX_signals_Pending & signals_blocked) ) {
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
      if ( mask & (api->signals_pending & signals_blocked) ) {
        api->signals_pending &= ~mask;
        do_callout = true;
      }
    }
  _ISR_Enable( level );
  return do_callout;
}
