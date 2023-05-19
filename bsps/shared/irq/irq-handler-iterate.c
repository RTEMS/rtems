/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of
 *   rtems_interrupt_handler_iterate().
 */

/*
 * Copyright (C) 2017, 2021 embedded brains GmbH & Co. KG
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

rtems_status_code rtems_interrupt_handler_iterate(
  rtems_vector_number                 vector,
  rtems_interrupt_per_handler_routine routine,
  void                               *arg
)
{
  rtems_status_code      sc;
  rtems_vector_number    index;
  rtems_option           options;
  rtems_interrupt_entry *entry;

  sc = bsp_interrupt_check_and_lock(
    vector,
    (rtems_interrupt_handler) routine
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  index = bsp_interrupt_dispatch_index( vector );
  entry = *bsp_interrupt_get_dispatch_table_slot( index );
  options = bsp_interrupt_is_handler_unique( index ) ?
    RTEMS_INTERRUPT_UNIQUE : RTEMS_INTERRUPT_SHARED;

  while ( entry != NULL ) {
    ( *routine )( arg, entry->info, options, entry->handler, entry->arg );
    entry = entry->next;
  }

  bsp_interrupt_unlock();

  return RTEMS_SUCCESSFUL;
}
