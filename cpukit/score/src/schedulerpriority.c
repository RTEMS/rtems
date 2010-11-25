/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
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
#include <rtems/config.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>

/* Instantiate any global variables needed by the priority scheduler */
volatile Priority_bit_map_Control _Priority_Major_bit_map;

Priority_bit_map_Control          _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

/*
 *  _Scheduler_priority_Initialize
 *
 * Initializes the scheduler for priority scheduling.
 *
 *  Input parameters:
 *    the_scheduler - pointer to scheduler control
 *
 *  Output parameters: NONE
 */

void _Scheduler_priority_Initialize (
    Scheduler_Control       *the_scheduler
)
{
  /* the operations table is a jump table to redirect generic scheduler 
   * function calls to scheduler implementation specific functions.  The 
   * main purpose of scheduler initialization is to set up the jump table 
   * for the scheduler.  Every scheduler implementation provides its own
   * scheduler operations table.
   */
  the_scheduler->Operations.schedule           = &_Scheduler_priority_Schedule;
  the_scheduler->Operations.yield              = &_Scheduler_priority_Yield;
  the_scheduler->Operations.block              = &_Scheduler_priority_Block;
  the_scheduler->Operations.unblock            = &_Scheduler_priority_Unblock;
  the_scheduler->Operations.scheduler_allocate = 
      &_Scheduler_priority_Thread_scheduler_allocate;
  the_scheduler->Operations.scheduler_free     = 
      &_Scheduler_priority_Thread_scheduler_free;
  the_scheduler->Operations.scheduler_update   = 
      &_Scheduler_priority_Thread_scheduler_update;

  _Scheduler_priority_Ready_queue_initialize( the_scheduler );
  _Priority_bit_map_Handler_initialization( );
}
