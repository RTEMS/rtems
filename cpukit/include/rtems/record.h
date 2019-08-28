/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

#ifndef _RTEMS_RECORD_H
#define _RTEMS_RECORD_H

#include "recorddata.h"

#include <rtems/score/atomic.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/intr.h>
#include <rtems/counter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct Record_Control {
  Atomic_Uint       head;
  unsigned int      tail;
  unsigned int      mask;
  Watchdog_Control  Watchdog;
  rtems_record_item Header[ 3 ];
  rtems_record_item Items[ RTEMS_ZERO_LENGTH_ARRAY ];
} Record_Control;

typedef struct {
  unsigned int    item_count;
  size_t          control_size;
  Record_Control *controls;
} Record_Configuration;

typedef struct {
  Record_Control        *control;
  unsigned int           head;
  uint32_t               now;
  rtems_interrupt_level  level;
} rtems_record_context;

extern const Record_Configuration _Record_Configuration;

void _Record_Initialize( void );

bool _Record_Thread_create(
  struct _Thread_Control *executing,
  struct _Thread_Control *created
);

void _Record_Thread_start(
  struct _Thread_Control *executing,
  struct _Thread_Control *started
);

void _Record_Thread_restart(
  struct _Thread_Control *executing,
  struct _Thread_Control *restarted
);

void _Record_Thread_delete(
  struct _Thread_Control *executing,
  struct _Thread_Control *deleted
);

void _Record_Thread_switch(
  struct _Thread_Control *executing,
  struct _Thread_Control *heir
);

void _Record_Thread_begin( struct _Thread_Control *executing );

void _Record_Thread_exitted( struct _Thread_Control *executing );

void _Record_Thread_terminate(
  struct _Thread_Control *executing
);

#define RECORD_EXTENSION \
  { \
    _Record_Thread_create, \
    _Record_Thread_start, \
    _Record_Thread_restart, \
    _Record_Thread_delete, \
    _Record_Thread_switch, \
    _Record_Thread_begin, \
    _Record_Thread_exitted, \
    NULL, \
    _Record_Thread_terminate \
  }

RTEMS_INLINE_ROUTINE unsigned int _Record_Index(
  const Record_Control *control,
  unsigned int          index
)
{
  return index & control->mask;
}

RTEMS_INLINE_ROUTINE unsigned int _Record_Head( const Record_Control *control )
{
  return _Atomic_Load_uint( &control->head, ATOMIC_ORDER_RELAXED );
}

RTEMS_INLINE_ROUTINE unsigned int _Record_Tail( const Record_Control *control )
{
  return control->tail;
}

RTEMS_INLINE_ROUTINE bool _Record_Is_overflow(
  const Record_Control *control,
  unsigned int          tail,
  unsigned int          head
)
{
  return head - tail >= control->mask + 1U;
}

RTEMS_INLINE_ROUTINE unsigned int _Record_Capacity(
  const Record_Control *control,
  unsigned int          tail,
  unsigned int          head
)
{
  return ( tail - head - 1U ) & control->mask;
}

RTEMS_INLINE_ROUTINE rtems_counter_ticks _Record_Now( void )
{
  return rtems_counter_read();
}

typedef struct RTEMS_PACKED {
  uint32_t format;
  uint32_t magic;
  rtems_record_item Version;
  rtems_record_item Processor_maximum;
  rtems_record_item Count;
  rtems_record_item Frequency;
} Record_Stream_header;

void _Record_Stream_header_initialize( Record_Stream_header *header );

/**
 * @addtogroup RTEMSRecord
 *
 * @{
 */

/**
 * @brief Prepares to add and commit record items in a critical section with
 * interrupts disabled.
 *
 * This function does not disable interrupts.  It must be called with
 * interrupts disabled.  Interrupts must be disabled until the corresponding
 * rtems_record_commit_critical() was called.
 *
 * @param context The record context which must be used for the following
 *   rtems_record_add() and rtems_record_commit_critical() calls.  The record
 *   context may have an arbitrary content at function entry.
 * @param cpu_self The control of the current processor.
 */
RTEMS_INLINE_ROUTINE void rtems_record_prepare_critical(
  rtems_record_context  *context,
  const Per_CPU_Control *cpu_self
)
{
  Record_Control *control;
  unsigned int    head;

  context->now = RTEMS_RECORD_TIME_EVENT( _Record_Now(), 0 );
  control = cpu_self->record;
  context->control = control;
  head = _Record_Head( control );
  context->head = head;
}

/**
 * @brief Prepares to add and commit record items.
 *
 * This function disables interrupts.
 *
 * @param context The record context which must be used for the following
 *   rtems_record_add() and rtems_record_commit() calls.  The record context
 *   may have an arbitrary content at function entry.
 *
 * @see rtems_record_produce().
 */
RTEMS_INLINE_ROUTINE void rtems_record_prepare( rtems_record_context *context )
{
  rtems_interrupt_level  level;
  const Per_CPU_Control *cpu_self;
  Record_Control        *control;
  unsigned int           head;

  rtems_interrupt_local_disable( level );
  context->now = RTEMS_RECORD_TIME_EVENT( _Record_Now(), 0 );
  context->level = level;
  cpu_self = _Per_CPU_Get();
  control = cpu_self->record;
  context->control = control;
  head = _Record_Head( control );
  context->head = head;
}

/**
 * @brief Adds a record item.
 *
 * @param context The record context initialized via rtems_record_prepare().
 * @param event The record event without a time stamp for the item.
 * @param data The record data for the item.
 */
RTEMS_INLINE_ROUTINE void rtems_record_add(
  rtems_record_context *context,
  rtems_record_event    event,
  rtems_record_data     data
)
{
  Record_Control    *control;
  rtems_record_item *item;
  unsigned int       head;

  control = context->control;
  head = context->head;
  item = &control->Items[ _Record_Index( control, head ) ];
  context->head = head + 1;

  item->event = context->now | event;
  item->data = data;
}

/**
 * @brief Commits a set of record items in a critical section with interrupts
 * disabled.
 *
 * @param context The record context initialized via
 *   rtems_record_prepare_critical().
 */
RTEMS_INLINE_ROUTINE void rtems_record_commit_critical( rtems_record_context *context )
{
  _Atomic_Store_uint(
    &context->control->head,
    context->head,
    ATOMIC_ORDER_RELEASE
  );
}

/**
 * @brief Commits a set of record items.
 *
 * @param context The record context initialized via rtems_record_prepare().
 */
RTEMS_INLINE_ROUTINE void rtems_record_commit( rtems_record_context *context )
{
  rtems_record_commit_critical( context );
  rtems_interrupt_local_enable( context->level );
}

/**
 * @brief Produces a record item.
 *
 * @param event The record event without a time stamp for the item.
 * @param data The record data for the item.
 */
void rtems_record_produce( rtems_record_event event, rtems_record_data data );

/**
 * @brief Produces two record items.
 *
 * @param event_0 The record event without a time stamp for the first item.
 * @param data_0 The record data for the first item.
 * @param event_1 The record event without a time stamp for the second item.
 * @param data_1 The record data for the second item.
 */
void rtems_record_produce_2(
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
);

/**
 * @brief Produces n record items.
 *
 * @param item The record items without a time stamps.
 * @param n The count of record items.
 */
void rtems_record_produce_n(
  const rtems_record_item *items,
  size_t                   n
);

typedef void ( *rtems_record_drain_visitor )(
  const rtems_record_item *items,
  size_t                   count,
  void                    *arg
);

/**
 * @brief Drains the record items on all processors.
 *
 * Calls the visitor function for each drained item set.
 *
 * @param visitor The visitor function.
 * @param arg The argument for the visitor function.
 */
void rtems_record_drain( rtems_record_drain_visitor visitor, void *arg );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORD_H */
