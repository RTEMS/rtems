/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the definition of ::_Copyright_Notice,
 *   ::_Objects_Information_table, the flexible per-CPU data linker set limits,
 *   the system initialization linker set limits and the implementation of
 *   rtems_initialize_executive().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#include <rtems/config.h>
#include <rtems/extensionimpl.h>
#include <rtems/init.h>
#include <rtems/sysinit.h>
#include <rtems/score/sysstate.h>

#include <rtems/score/copyrt.h>
#include <rtems/score/heap.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/priority.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>

RTEMS_SECTION(".rtemsroset.copyright") const char _Copyright_Notice[] =
  "Copyright (C) 1989, 2021 RTEMS Project and contributors";

static Objects_Information *
_Internal_Objects[ OBJECTS_INTERNAL_CLASSES_LAST + 1 ];

static Objects_Information *_RTEMS_Objects[ OBJECTS_RTEMS_CLASSES_LAST + 1 ];

static Objects_Information *_POSIX_Objects[ OBJECTS_POSIX_CLASSES_LAST + 1 ];

Objects_Information ** const
_Objects_Information_table[ OBJECTS_APIS_LAST + 1 ] = {
  NULL,
  &_Internal_Objects[ 0 ],
  &_RTEMS_Objects[ 0 ],
  &_POSIX_Objects[ 0 ]
};

static void rtems_initialize_data_structures(void)
{
  /*
   *  Dispatching and interrupts are disabled until the end of the
   *  initialization sequence.  This prevents an inadvertent context
   *  switch before the executive is initialized.
   *
   *  WARNING: Interrupts should have been disabled by the BSP and
   *           are disabled by boot_card().
   */

  /*
   * Initialize any target architecture specific support as early as possible
   */
  _CPU_Initialize();

  _Thread_Dispatch_initialization();

  _ISR_Handler_initialization();

  _Thread_Handler_initialization();

  _Scheduler_Handler_initialization();

  _SMP_Handler_initialize();
}

RTEMS_LINKER_ROSET( _Sysinit, rtems_sysinit_item );

RTEMS_SYSINIT_ITEM(
  rtems_initialize_data_structures,
  RTEMS_SYSINIT_DATA_STRUCTURES,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/*
 *  No threads should be created before this point!!!
 *  _Thread_Executing and _Thread_Heir are not set.
 *
 *  At this point all API extensions are in place.  After the call to
 *  _Thread_Create_idle() _Thread_Executing and _Thread_Heir will be set.
 *
 *  Scheduling can properly occur afterwards as long as we avoid dispatching.
 */
RTEMS_SYSINIT_ITEM(
  _Thread_Create_idle,
  RTEMS_SYSINIT_IDLE_THREADS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

void rtems_initialize_executive(void)
{
  const rtems_sysinit_item *item;

  /* Invoke the registered system initialization handlers */
  RTEMS_LINKER_SET_FOREACH( _Sysinit, item ) {
    ( *item->handler )();
  }

  _System_state_Set( SYSTEM_STATE_UP );

  _SMP_Request_start_multitasking();

  _Thread_Start_multitasking();

  /*******************************************************************
   *******************************************************************
   *******************************************************************
   ******                 APPLICATION RUNS HERE                 ******
   ******              THE FUNCTION NEVER RETURNS               ******
   *******************************************************************
   *******************************************************************
   *******************************************************************/
}
