/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
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

/*PAGE
 *
 *  _POSIX_signals_Clear_process_signals
 */

void _POSIX_signals_Clear_process_signals(
  sigset_t   mask
)
{
  ISR_Level  level;

  _ISR_Disable( level );
    _POSIX_signals_Pending &= ~mask;
    if ( !_POSIX_signals_Pending )
      _Thread_Do_post_task_switch_extension--;
  _ISR_Enable( level );
}
