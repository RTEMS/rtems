/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2018, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/rtems/intr.h>
#include <rtems/score/atomic.h>
#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/watchdog.h>
#include <rtems/counter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct Record_Control {
  Atomic_Uint       head;
  unsigned int      tail;
  unsigned int      mask;
  Watchdog_Control  Watchdog;
  RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )
    rtems_record_item Items[ RTEMS_ZERO_LENGTH_ARRAY ];
} Record_Control;

typedef struct {
  unsigned int    item_count;
  Record_Control *controls;
} Record_Configuration;

typedef struct {
  Record_Control *control;
  unsigned int    head;
  uint32_t        now;
  uint32_t        level;
} rtems_record_context;

extern const Record_Configuration _Record_Configuration;

void _Record_Initialize( void );

void _Record_Interrupt_initialize( void );

extern rtems_interrupt_entry *_Record_Interrupt_dispatch_table[];

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

void _Record_Fatal_dump_base64(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
);

void _Record_Fatal_dump_base64_zlib(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
);

void _Record_Thread_terminate(
  struct _Thread_Control *executing
);

static inline unsigned int _Record_Index(
  const Record_Control *control,
  unsigned int          index
)
{
  return index & control->mask;
}

static inline unsigned int _Record_Head( Record_Control *control )
{
#ifdef RTEMS_SMP
  /*
   * Use a read-modify-write operation to get the last value stored by the
   * record producer.
   */
  return _Atomic_Fetch_add_uint( &control->head, 0, ATOMIC_ORDER_ACQUIRE );
#else
  return _Atomic_Load_uint( &control->head, ATOMIC_ORDER_ACQUIRE );
#endif
}

static inline unsigned int _Record_Tail( const Record_Control *control )
{
  return control->tail;
}

static inline bool _Record_Is_overflow(
  const Record_Control *control,
  unsigned int          tail,
  unsigned int          head
)
{
  return head - tail >= control->mask + 1U;
}

static inline unsigned int _Record_Capacity(
  const Record_Control *control,
  unsigned int          tail,
  unsigned int          head
)
{
  return ( tail - head - 1U ) & control->mask;
}

static inline rtems_counter_ticks _Record_Now( void )
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
  rtems_record_item Info[64];
} Record_Stream_header;

size_t _Record_Stream_header_initialize( Record_Stream_header *header );

size_t _Record_String_to_items(
  rtems_record_event  event,
  const char         *str,
  size_t              len,
  rtems_record_item  *items,
  size_t              item_count
);

void _Record_Caller( void *return_address );

void _Record_Caller_3(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data
);

void _Record_Caller_4(
  void               *return_address,
  rtems_record_event  event_0,
  rtems_record_data   data_0,
  rtems_record_event  event_1,
  rtems_record_data   data_1
);

void _Record_Caller_arg( void *return_address, rtems_record_data data );

void _Record_Caller_arg_2(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1
);

void _Record_Caller_arg_3(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2
);

void _Record_Caller_arg_4(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3
);

void _Record_Caller_arg_5(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4
);

void _Record_Caller_arg_6(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5
);

void _Record_Caller_arg_7(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6
);

void _Record_Caller_arg_8(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7
);

void _Record_Caller_arg_9(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8
);

void _Record_Caller_arg_10(
  void              *return_address,
  rtems_record_data  data_0,
  rtems_record_data  data_1,
  rtems_record_data  data_2,
  rtems_record_data  data_3,
  rtems_record_data  data_4,
  rtems_record_data  data_5,
  rtems_record_data  data_6,
  rtems_record_data  data_7,
  rtems_record_data  data_8,
  rtems_record_data  data_9
);

void _Record_Entry_2(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1
);

void _Record_Entry_3(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2
);

void _Record_Entry_4(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3
);

void _Record_Entry_5(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4
);

void _Record_Entry_6(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5
);

void _Record_Entry_7(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6
);

void _Record_Entry_8(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7
);

void _Record_Entry_9(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8
);

void _Record_Entry_10(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8,
  rtems_record_data   data_9
);

void _Record_Exit_2(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1
);

void _Record_Exit_3(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2
);

void _Record_Exit_4(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3
);

void _Record_Exit_5(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4
);

void _Record_Exit_6(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5
);

void _Record_Exit_7(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6
);

void _Record_Exit_8(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7
);

void _Record_Exit_9(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8
);

void _Record_Exit_10(
  void               *return_address,
  rtems_record_event  event,
  rtems_record_data   data_0,
  rtems_record_data   data_1,
  rtems_record_data   data_2,
  rtems_record_data   data_3,
  rtems_record_data   data_4,
  rtems_record_data   data_5,
  rtems_record_data   data_6,
  rtems_record_data   data_7,
  rtems_record_data   data_8,
  rtems_record_data   data_9
);

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
static inline void rtems_record_prepare_critical(
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
static inline void rtems_record_prepare( rtems_record_context *context )
{
  uint32_t               level;
  const Per_CPU_Control *cpu_self;
  Record_Control        *control;
  unsigned int           head;

  _CPU_ISR_Disable( level );
  RTEMS_COMPILER_MEMORY_BARRIER();
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
static inline void rtems_record_add(
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
static inline void rtems_record_commit_critical( rtems_record_context *context )
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
static inline void rtems_record_commit( rtems_record_context *context )
{
  rtems_record_commit_critical( context );
  RTEMS_COMPILER_MEMORY_BARRIER();
  _CPU_ISR_Enable( context->level );
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

/**
 * @brief Generates an RTEMS_RECORD_LINE event.
 *
 * The event data is the address of the call to this function.
 */
void rtems_record_line( void );

/**
 * @brief Generates an RTEMS_RECORD_LINE event and an extra event.
 *
 * The event data is the address of the call to this function.
 *
 * @param event The record event without a time stamp for the second item.
 * @param data The record data for the second item.
 */
void rtems_record_line_2(
  rtems_record_event event,
  rtems_record_data  data
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and two extra events.
 *
 * The event data is the address of the call to this function.
 *
 * @param event_0 The record event without a time stamp for the second item.
 * @param data_0 The record data for the second item.
 * @param event_1 The record event without a time stamp for the third item.
 * @param data_1 The record data for the third item.
 */
void rtems_record_line_3(
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and one argument event.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data The RTEMS_RECORD_ARG_0 event data.
 */
void rtems_record_line_arg( rtems_record_data data );

/**
 * @brief Generates an RTEMS_RECORD_LINE event and two argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 */
void rtems_record_line_arg_2(
  rtems_record_data data_0,
  rtems_record_data data_1
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and three argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 */
void rtems_record_line_arg_3(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and four argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 */
void rtems_record_line_arg_4(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and five argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 */
void rtems_record_line_arg_5(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and six argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 */
void rtems_record_line_arg_6(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and seven argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 */
void rtems_record_line_arg_7(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and eight argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 */
void rtems_record_line_arg_8(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and nine argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 * @param data_8 The RTEMS_RECORD_ARG_8 event data.
 */
void rtems_record_line_arg_9(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7,
  rtems_record_data data_8
);

/**
 * @brief Generates an RTEMS_RECORD_LINE event and ten argument events.
 *
 * The event data of the RTEMS_RECORD_LINE event is the address of the call to
 * this function.
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 * @param data_8 The RTEMS_RECORD_ARG_8 event data.
 * @param data_9 The RTEMS_RECORD_ARG_9 event data.
 */
void rtems_record_line_arg_10(
  rtems_record_data data_0,
  rtems_record_data data_1,
  rtems_record_data data_2,
  rtems_record_data data_3,
  rtems_record_data data_4,
  rtems_record_data data_5,
  rtems_record_data data_6,
  rtems_record_data data_7,
  rtems_record_data data_8,
  rtems_record_data data_9
);

/**
 * @brief Generates an RTEMS_RECORD_CALLER and an RTEMS_RECORD_LINE event.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller().
 */
#define rtems_record_caller() _Record_Caller( RTEMS_RETURN_ADDRESS() )

/**
 * @brief Generates an RTEMS_RECORD_CALLER, an RTEMS_RECORD_LINE event, and an
 * extra event.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_3().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_3().
 *
 * @param event The record event without a time stamp for the third item.
 * @param data The record data for the third item.
 */
#define rtems_record_caller_3( event, data ) \
  _Record_Caller_3( RTEMS_RETURN_ADDRESS(), event, data )

/**
 * @brief Generates an RTEMS_RECORD_CALLER, an RTEMS_RECORD_LINE event, and two
 * extra events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_4().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_4().
 *
 * @param event_0 The record event without a time stamp for the third item.
 * @param data_0 The record data for the third item.
 * @param event_1 The record event without a time stamp for the fourth item.
 * @param data_1 The record data for the fourth item.
 */
#define rtems_record_caller_4( event_0, data_0, event_1, data_1 ) \
  _Record_Caller_4( \
    RTEMS_RETURN_ADDRESS(), \
    event_0, \
    data_0, \
    event_1, \
    data_1 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and one argument event.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg().
 *
 * @param data The RTEMS_RECORD_ARG_0 event data.
 */
#define rtems_record_caller_arg( data ) \
  _Record_Caller_arg( RTEMS_RETURN_ADDRESS(), data )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and two argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_2().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_2().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 */
#define rtems_record_caller_arg_2( data_0, data_1 ) \
  _Record_Caller_arg_2( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and three argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_3().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_3().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 */
#define rtems_record_caller_arg_3( data_0, data_1, data_2 ) \
  _Record_Caller_arg_3( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and four argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_4().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_4().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 */
#define rtems_record_caller_arg_4( data_0, data_1, data_2, data_3 ) \
  _Record_Caller_arg_4( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and five argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_5().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_5().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 */
#define rtems_record_caller_arg_5( data_0, data_1, data_2, data_3, data_4 ) \
  _Record_Caller_arg_5( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and six argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_6().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_6().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 */
#define rtems_record_caller_arg_6( \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5 \
) \
  _Record_Caller_arg_6( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and seven argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_7().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_7().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 */
#define rtems_record_caller_arg_7( \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6 \
) \
  _Record_Caller_arg_7( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and eight argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_8().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_8().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 */
#define rtems_record_caller_arg_8( \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7 \
) \
  _Record_Caller_arg_8( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and nine argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_9().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_9().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 * @param data_8 The RTEMS_RECORD_ARG_8 event data.
 */
#define rtems_record_caller_arg_9( \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7, \
  data_8 \
) \
  _Record_Caller_arg_9( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7, \
    data_8 \
  )

/**
 * @brief Generates an RTEMS_RECORD_CALLER event, an RTEMS_RECORD_LINE event,
 * and ten argument events.
 *
 * The RTEMS_RECORD_CALLER event data is the return address of the function
 * calling rtems_record_caller_arg_10().
 *
 * The RTEMS_RECORD_LINE event data is the address of the call to
 * rtems_record_caller_arg_10().
 *
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 * @param data_8 The RTEMS_RECORD_ARG_8 event data.
 * @param data_10 The RTEMS_RECORD_ARG_10 event data.
 */
#define rtems_record_caller_arg_10( \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7, \
  data_8, \
  data_9 \
) \
  _Record_Caller_arg_10( \
    RTEMS_RETURN_ADDRESS(), \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7, \
    data_8, \
    data_9 \
  )

/**
 * @brief Generates the specified entry event.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry().
 *
 * @param event The entry event.
 */
#define rtems_record_entry( event ) \
  rtems_record_produce( event, (rtems_record_data) RTEMS_RETURN_ADDRESS() )

/**
 * @brief Generates the specified entry event and an argument event.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_1().
 *
 * @param event The entry event.
 * @param data The RTEMS_RECORD_ARG_0 event data.
 */
#define rtems_record_entry_1( event, data ) \
  rtems_record_produce_2( \
    event, \
    (rtems_record_data) RTEMS_RETURN_ADDRESS(), \
    RTEMS_RECORD_ARG_0, \
    data \
  )

/**
 * @brief Generates the specified entry event and two argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_2().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 */
#define rtems_record_entry_2( event, data_0, data_1 ) \
  _Record_Entry_2( RTEMS_RETURN_ADDRESS(), event, data_0, data_1 )

/**
 * @brief Generates the specified entry event and three argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_3().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 */
#define rtems_record_entry_3( event, data_0, data_1, data_2 ) \
  _Record_Entry_3( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2 \
  )

/**
 * @brief Generates the specified entry event and four argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_4().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 */
#define rtems_record_entry_4( event, data_0, data_1, data_2, data_3 ) \
  _Record_Entry_4( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3 \
  )

/**
 * @brief Generates the specified entry event and five argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_5().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 */
#define rtems_record_entry_5( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4 \
) \
  _Record_Entry_5( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4 \
  )

/**
 * @brief Generates the specified entry event and six argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_6().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 */
#define rtems_record_entry_6( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5 \
) \
  _Record_Entry_6( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5 \
  )

/**
 * @brief Generates the specified entry event and seven argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_7().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 */
#define rtems_record_entry_7( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6 \
) \
  _Record_Entry_7( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6 \
  )

/**
 * @brief Generates the specified entry event and eight argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_8().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 */
#define rtems_record_entry_8( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7 \
) \
  _Record_Entry_8( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7 \
  )

/**
 * @brief Generates the specified entry event and nine argument events.
 *
 * The event data of the specified entry event is the return address of the
 * function calling rtems_record_entry_9().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 * @param data_8 The RTEMS_RECORD_ARG_8 event data.
 */
#define rtems_record_entry_9( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7, \
  data_8 \
) \
  _Record_Entry_9( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7, \
    data_8 \
  )

/**
 * @brief Generates the specified entry event and ten argument events.
 *
 * The event data of the specified entry event is the return address of the function
 * calling rtems_record_entry_10().
 *
 * @param event The entry event.
 * @param data_0 The RTEMS_RECORD_ARG_0 event data.
 * @param data_1 The RTEMS_RECORD_ARG_1 event data.
 * @param data_2 The RTEMS_RECORD_ARG_2 event data.
 * @param data_3 The RTEMS_RECORD_ARG_3 event data.
 * @param data_4 The RTEMS_RECORD_ARG_4 event data.
 * @param data_5 The RTEMS_RECORD_ARG_5 event data.
 * @param data_6 The RTEMS_RECORD_ARG_6 event data.
 * @param data_7 The RTEMS_RECORD_ARG_7 event data.
 * @param data_8 The RTEMS_RECORD_ARG_8 event data.
 * @param data_10 The RTEMS_RECORD_ARG_10 event data.
 */
#define rtems_record_entry_10( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7, \
  data_8, \
  data_9 \
) \
  _Record_Entry_10( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7, \
    data_8, \
    data_9 \
  )

/**
 * @brief Generates the specified exit event.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit().
 *
 * @param event The exit event.
 */
#define rtems_record_exit( event ) \
  rtems_record_produce( event, (rtems_record_data) RTEMS_RETURN_ADDRESS() )

/**
 * @brief Generates the specified exit event and a return event.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_1().
 *
 * @param event The exit event.
 * @param data The RTEMS_RECORD_RETURN_0 event data.
 */
#define rtems_record_exit_1( event, data ) \
  rtems_record_produce_2( \
    event, \
    (rtems_record_data) RTEMS_RETURN_ADDRESS(), \
    RTEMS_RECORD_RETURN_0, \
    data \
  )

/**
 * @brief Generates the specified exit event and two argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_2().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 */
#define rtems_record_exit_2( event, data_0, data_1 ) \
  _Record_Entry_2( RTEMS_RETURN_ADDRESS(), event, data_0, data_1 )

/**
 * @brief Generates the specified exit event and three argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_3().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 */
#define rtems_record_exit_3( event, data_0, data_1, data_2 ) \
  _Record_Exit_3( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2 \
  )

/**
 * @brief Generates the specified exit event and four argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_4().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 */
#define rtems_record_exit_4( event, data_0, data_1, data_2, data_3 ) \
  _Record_Exit_4( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3 \
  )

/**
 * @brief Generates the specified exit event and five argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_5().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 * @param data_4 The RTEMS_RECORD_RETURN_4 event data.
 */
#define rtems_record_exit_5( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4 \
) \
  _Record_Exit_5( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4 \
  )

/**
 * @brief Generates the specified exit event and six argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_6().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 * @param data_4 The RTEMS_RECORD_RETURN_4 event data.
 * @param data_5 The RTEMS_RECORD_RETURN_5 event data.
 */
#define rtems_record_exit_6( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5 \
) \
  _Record_Exit_6( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5 \
  )

/**
 * @brief Generates the specified exit event and seven argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_7().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 * @param data_4 The RTEMS_RECORD_RETURN_4 event data.
 * @param data_5 The RTEMS_RECORD_RETURN_5 event data.
 * @param data_6 The RTEMS_RECORD_RETURN_6 event data.
 */
#define rtems_record_exit_7( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6 \
) \
  _Record_Exit_7( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6 \
  )

/**
 * @brief Generates the specified exit event and eight argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_8().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 * @param data_4 The RTEMS_RECORD_RETURN_4 event data.
 * @param data_5 The RTEMS_RECORD_RETURN_5 event data.
 * @param data_6 The RTEMS_RECORD_RETURN_6 event data.
 * @param data_7 The RTEMS_RECORD_RETURN_7 event data.
 */
#define rtems_record_exit_8( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7 \
) \
  _Record_Exit_8( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7 \
  )

/**
 * @brief Generates the specified exit event and nine argument events.
 *
 * The event data of the specified exit event is the return address of the
 * function calling rtems_record_exit_9().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 * @param data_4 The RTEMS_RECORD_RETURN_4 event data.
 * @param data_5 The RTEMS_RECORD_RETURN_5 event data.
 * @param data_6 The RTEMS_RECORD_RETURN_6 event data.
 * @param data_7 The RTEMS_RECORD_RETURN_7 event data.
 * @param data_8 The RTEMS_RECORD_RETURN_8 event data.
 */
#define rtems_record_exit_9( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7, \
  data_8 \
) \
  _Record_Exit_9( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7, \
    data_8 \
  )

/**
 * @brief Generates the specified exit event and ten argument events.
 *
 * The event data of the specified exit event is the return address of the function
 * calling rtems_record_exit_10().
 *
 * @param event The exit event.
 * @param data_0 The RTEMS_RECORD_RETURN_0 event data.
 * @param data_1 The RTEMS_RECORD_RETURN_1 event data.
 * @param data_2 The RTEMS_RECORD_RETURN_2 event data.
 * @param data_3 The RTEMS_RECORD_RETURN_3 event data.
 * @param data_4 The RTEMS_RECORD_RETURN_4 event data.
 * @param data_5 The RTEMS_RECORD_RETURN_5 event data.
 * @param data_6 The RTEMS_RECORD_RETURN_6 event data.
 * @param data_7 The RTEMS_RECORD_RETURN_7 event data.
 * @param data_8 The RTEMS_RECORD_RETURN_8 event data.
 * @param data_10 The RTEMS_RECORD_RETURN_10 event data.
 */
#define rtems_record_exit_10( \
  event, \
  data_0, \
  data_1, \
  data_2, \
  data_3, \
  data_4, \
  data_5, \
  data_6, \
  data_7, \
  data_8, \
  data_9 \
) \
  _Record_Exit_10( \
    RTEMS_RETURN_ADDRESS(), \
    event, \
    data_0, \
    data_1, \
    data_2, \
    data_3, \
    data_4, \
    data_5, \
    data_6, \
    data_7, \
    data_8, \
    data_9 \
  )

/**
 * @brief Disables interrupts and generates an RTEMS_RECORD_ISR_DISABLE event.
 *
 * @return The previous interrupt level.
 */
uint32_t rtems_record_interrupt_disable( void );

/**
 * @brief Restores the previous interrupt level and generates an
 *   RTEMS_RECORD_ISR_ENABLE event.
 *
 * @param level The previous interrupt level.  Must be the return value of the
 *   corresponding rtems_record_interrupt_disable().
 */
void rtems_record_interrupt_enable( uint32_t level );

/**
 * @brief This structure controls the record fetching performed by rtems_record_fetch().
 *
 * The structure shall be initialized by rtems_record_fetch_initialize().
 */
typedef struct {
  /**
   * @brief This member references the first item fetched by the last call to
   *   rtems_record_fetch().
   */
  rtems_record_item *fetched_items;

  /**
   * @brief This member contains the count of items fetched by the last call to
   *   rtems_record_fetch().
   */
  size_t fetched_count;

  /**
   * @brief The following members should only be accessed by
   *   rtems_record_fetch_initialize() and rtems_record_fetch().
   */
  struct {
#ifdef RTEMS_SMP
    /**
     * @brief This member contains the index of the processor from which the next
     *   records are fetched.
     */
    uint32_t cpu_index;
#endif

    /**
     * @brief This member contains the count of records which need to be fetched
     *   from the current processor before the next processor is selected.
     */
    size_t cpu_todo;

    /**
     * @brief This member references the item array used to store fetched items.
     */
    rtems_record_item *storage_items;

    /**
     * @brief This member contains the count of items of the array referenced by
     *   ``storage_items``.
     */
    size_t storage_item_count;
  } internal;
} rtems_record_fetch_control;

/**
 * @brief This enumeration provides status codes returned by
 *   rtems_record_fetch().
 */
typedef enum {
  /**
   * @brief This enumerator indicates that the current round of record fetches
   *   for all configure processors is done.
   */
  RTEMS_RECORD_FETCH_DONE,

  /**
   * @brief This enumerator indicates that the current round of record fetches
   *   for all configure processors has to continue.
   */
  RTEMS_RECORD_FETCH_CONTINUE,

  /**
   * @brief This enumerator indicates that the item count passed to
   *   rtems_record_fetch() is invalid.
   */
  RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT
} rtems_record_fetch_status;

/**
 * @brief Returns the count of items which allows getting all available items
 *   for one processor through one call to rtems_record_fetch().
 *
 * The value depends on @ref CONFIGURE_RECORD_PER_PROCESSOR_ITEMS and
 * implementation details fo rtems_record_fetch().
 */
size_t rtems_record_get_item_count_for_fetch( void );

/**
 * @brief Initializes the record fetch control structure.
 *
 * This function shall be called before a record fetch control structure is
 * passed to rtems_record_fetch().
 *
 * @param[out] control is the structure to initialize.
 *
 * @param[out] items is a reference to an item array which is used to store the
 *   fetched items.
 *
 * @param count is the count of items in the referenced array.  See
 *   rtems_record_get_item_count_for_fetch().
 */
void rtems_record_fetch_initialize(
  rtems_record_fetch_control *control,
  rtems_record_item          *items,
  size_t                      count
);

/**
 * @brief Fetches records from the record buffers of the processors.
 *
 * The fetched items and count of fetched items is returned through the control
 * structure.
 *
 * @param[in, out] control is the structure used to control the record
 *   fetching.  Use rtems_record_fetch_initialize() to initialize it before the
 *   first call to rtems_record_fetch().
 *
 * @retval RTEMS_RECORD_FETCH_DONE This return status indicates that the
 *   current round of record fetches for all configure processors is done.
 *
 * @retval RTEMS_RECORD_FETCH_CONTINUE This return status indicates that the
 *   current round of record fetches for all configure processors has to
 *   continue.
 *
 * @retval RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT This return status indicates
 *   that the specified item count was invalid.
 */
rtems_record_fetch_status rtems_record_fetch(
  rtems_record_fetch_control *control
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORD_H */
