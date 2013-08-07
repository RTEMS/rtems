/**
 * @file
 *
 * @brief Removes the Thread from Ready Queue
 *
 * @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>

void _Scheduler_EDF_Block(
  Thread_Control    *the_thread
)
{
  _Scheduler_Generic_block(
    _Scheduler_EDF_Extract,
    _Scheduler_EDF_Schedule_body,
    the_thread
  );
}
