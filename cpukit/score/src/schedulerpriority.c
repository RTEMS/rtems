/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/prioritybitmap.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>

/* Instantiate any global variables needed by the priority scheduler */
volatile Priority_bit_map_Control _Priority_Major_bit_map;

Priority_bit_map_Control          _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

void _Scheduler_priority_Initialize(void)
{
  _Scheduler_priority_Ready_queue_initialize();
  _Priority_bit_map_Handler_initialization();
}
