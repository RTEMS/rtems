/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the rtems_interrupt_handler_install()
 *   implementation.
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
#include <rtems/malloc.h>

#include <stdlib.h>

static rtems_status_code bsp_interrupt_handler_do_replace(
  rtems_vector_number     vector,
  const char             *info,
  rtems_interrupt_handler routine,
  void                   *arg
)
{
  rtems_interrupt_entry  *entry;
  rtems_interrupt_entry **unused;

  entry = bsp_interrupt_entry_find( vector, routine, arg, &unused );

  if ( entry == NULL ) {
    return RTEMS_UNSATISFIED;
  }

  entry->handler = routine;
  entry->info = info;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code bsp_interrupt_handler_replace(
  rtems_vector_number     vector,
  const char             *info,
  rtems_interrupt_handler routine,
  void                   *arg
)
{
  rtems_status_code sc;

  sc = bsp_interrupt_check_and_lock( vector, routine );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  sc = bsp_interrupt_handler_do_replace( vector, info, routine, arg );
  bsp_interrupt_unlock();

  return sc;
}

rtems_status_code rtems_interrupt_handler_install(
  rtems_vector_number     vector,
  const char             *info,
  rtems_option            options,
  rtems_interrupt_handler routine,
  void                   *arg
)
{
  rtems_interrupt_entry *entry;
  rtems_status_code      sc;

  if ( RTEMS_INTERRUPT_IS_REPLACE( options ) ) {
    return bsp_interrupt_handler_replace( vector, info, routine, arg );
  }

  entry = rtems_malloc( sizeof( *entry ) );

  if ( entry == NULL ) {
    return RTEMS_NO_MEMORY;
  }

  rtems_interrupt_entry_initialize( entry, routine, arg, info );
  sc = rtems_interrupt_entry_install( vector, options, entry );

  if ( sc != RTEMS_SUCCESSFUL ) {
    free( entry );
  }

  return sc;
}
