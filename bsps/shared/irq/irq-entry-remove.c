/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of
 *   rtems_interrupt_entry_remove() and bsp_interrupt_entry_remove().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <bsp/irq-generic.h>

void bsp_interrupt_entry_remove(
  rtems_vector_number     vector,
  rtems_interrupt_entry  *entry,
  rtems_interrupt_entry **previous_next
)
{
  rtems_vector_number    index;
  rtems_interrupt_entry *first;
  rtems_interrupt_entry *entry_next;

  index = bsp_interrupt_dispatch_index( vector );
  first = *bsp_interrupt_get_dispatch_table_slot( index );
  entry_next = entry->next;

  if ( entry == first && entry_next == NULL ) {
    /* We remove the last installed entry */
    bsp_interrupt_vector_disable( vector );
#if defined(bsp_interrupt_vector_remove)
    bsp_interrupt_vector_remove( vector );
#else
    bsp_interrupt_vector_disable( vector );
#endif
    bsp_interrupt_set_handler_unique( index, false );
#if defined(BSP_INTERRUPT_USE_INDEX_TABLE)
    bsp_interrupt_dispatch_index_table[ vector ] = 0;
#endif
  }

  bsp_interrupt_entry_store_release( previous_next, entry_next );
}

static rtems_status_code bsp_interrupt_entry_do_remove(
  rtems_vector_number     vector,
  rtems_interrupt_entry  *entry
)
{
  rtems_interrupt_entry  *installed;
  rtems_interrupt_entry **previous_next;

  installed = bsp_interrupt_entry_find(
    vector,
    entry->handler,
    entry->arg,
    &previous_next
  );

  if ( installed != entry ) {
    return RTEMS_UNSATISFIED;
  }

  bsp_interrupt_entry_remove( vector, entry, previous_next );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_entry_remove(
  rtems_vector_number    vector,
  rtems_interrupt_entry *entry
)
{
  rtems_status_code sc;

  if ( entry == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  sc = bsp_interrupt_check_and_lock( vector, entry->handler );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  sc = bsp_interrupt_entry_do_remove( vector, entry );
  bsp_interrupt_unlock();

  return sc;
}
