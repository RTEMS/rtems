/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
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

#include <rtems/system.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>

void _Scheduler_priority_Unblock (
  Thread_Control          *the_thread
)
{
  _Scheduler_priority_Unblock_body(the_thread);
}
