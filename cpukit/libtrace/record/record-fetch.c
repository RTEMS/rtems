/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSRecord
 *
 * @brief This source file contains the implementation of the record fetching.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#include <rtems/record.h>

#include <rtems.h>
#include <string.h>

/*
 * One for RTEMS_RECORD_PROCESSOR, one for the optional
 * RTEMS_RECORD_PER_CPU_OVERFLOW.
 */
#define RECORD_FETCH_HEADER_ITEMS 2

size_t rtems_record_get_item_count_for_fetch( void )
{
  return _Record_Configuration.item_count + RECORD_FETCH_HEADER_ITEMS
#ifdef RTEMS_SMP
   /* See red zone comment below */
   - 1
#endif
   ;
}

void rtems_record_fetch_initialize(
  rtems_record_fetch_control *control,
  rtems_record_item          *items,
  size_t                      count
)
{
  control = memset( control, 0, sizeof( *control ) );
  control->internal.storage_items = items;
  control->internal.storage_item_count = count;
}


rtems_record_fetch_status rtems_record_fetch(
  rtems_record_fetch_control *control
)
{
  rtems_record_fetch_status status;
  rtems_record_item        *items;
  size_t                    count;
  uint32_t                  cpu_index;
  Per_CPU_Control          *cpu;
  Record_Control           *record_control;
  rtems_record_item        *item;
  unsigned int              mask;
  unsigned int              red_zone;
  unsigned int              capacity;
  unsigned int              tail;
  unsigned int              head;
  unsigned int              available;
  unsigned int              overflow;
  unsigned int              new_tail;
  unsigned int              new_items;
  rtems_record_item        *fetched_items;
  size_t                    fetched_count;

  items = control->internal.storage_items;
  count = control->internal.storage_item_count;

  if ( count < RECORD_FETCH_HEADER_ITEMS + 1 ) {
    control->fetched_items = 0;
    return RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT;
  }

  /*
   * The red zone indicates an area of items before the head where new items
   * are produced.
   *
   * In uniprocessor configurations, the record data and the record head is
   * atomically produced so that there is no red zone.
   *
   * In SMP configurations, the atomic store release to update the head and the
   * atomic fetch acquire to get the head guarantees that we read fully produced
   * items and that the head is the last value stored by the producer.
   * However, one item may be already in production which could be observed
   * before the new head is stored.
   */
#ifdef RTEMS_SMP
  red_zone = 1;
#else
  red_zone = 0;
#endif

#ifdef RTEMS_SMP
  cpu_index = control->internal.cpu_index;
#else
  cpu_index = 0;
#endif
  cpu = _Per_CPU_Get_by_index( cpu_index );
  record_control = cpu->record;
  mask = record_control->mask;
  capacity = mask + 1 - red_zone;
  tail = _Record_Tail( record_control );
  head = _Record_Head( record_control );

  available = control->internal.cpu_todo;
  control->internal.cpu_todo = 0;

  if ( available == 0 ) {
    available = head - tail;
  }

  if ( available > capacity ) {
    overflow = available - capacity;
    available = capacity;
    tail = head - capacity;
  } else {
    overflow = 0;
  }

  if ( available + RECORD_FETCH_HEADER_ITEMS > count ) {
    control->internal.cpu_todo = available - count + RECORD_FETCH_HEADER_ITEMS;
    available = count - RECORD_FETCH_HEADER_ITEMS;
    status = RTEMS_RECORD_FETCH_CONTINUE;
  } else {
#ifdef RTEMS_SMP
    if ( cpu_index + 1 < rtems_scheduler_get_processor_maximum() ) {
      control->internal.cpu_index = cpu_index + 1;
      status = RTEMS_RECORD_FETCH_CONTINUE;
    } else {
      control->internal.cpu_index = 0;
      status = RTEMS_RECORD_FETCH_DONE;
    }
#else
    status = RTEMS_RECORD_FETCH_DONE;
#endif
  }

  new_tail = tail + available;
  record_control->tail = new_tail;
  item = items + 2;

  while ( tail != new_tail ) {
    *item = record_control->Items[ tail & mask ];
    ++item;
    ++tail;
  }

  fetched_items = items + RECORD_FETCH_HEADER_ITEMS;
  fetched_count = available;
  new_items = _Record_Head( record_control ) - head;

  if ( available + new_items > capacity ) {
    unsigned int overwritten;

    overwritten = available + new_items - capacity;
    overflow += overwritten;

    if ( overwritten > available ) {
      overwritten = available;
    }

    fetched_items += overwritten;
    fetched_count -= overwritten;
  }

  if ( overflow > 0 ) {
    --fetched_items;
    ++fetched_count;
    fetched_items->event = RTEMS_RECORD_PER_CPU_OVERFLOW;
    fetched_items->data = overflow;
  }

  --fetched_items;
  ++fetched_count;
  fetched_items->event = RTEMS_RECORD_PROCESSOR;
  fetched_items->data = cpu_index;

  control->fetched_items = fetched_items;
  control->fetched_count = fetched_count;
  return status;
}
