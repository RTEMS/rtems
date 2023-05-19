/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of the interrupt event
 *   recording support.
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <rtems/record.h>

/* The regular interrupt entries are registered in this table */
static rtems_interrupt_entry *
_Record_Interrupt_dispatch_table[ BSP_INTERRUPT_DISPATCH_TABLE_SIZE ];

/*
 * Provide one interrupt entry for the _Record_Interrupt_handler() interrupt
 * dispatch wrapper for each interrupt vector.
 */
static rtems_interrupt_entry
_Record_Interrupt_entry_table[ BSP_INTERRUPT_DISPATCH_TABLE_SIZE ];

rtems_interrupt_entry **bsp_interrupt_get_dispatch_table_slot(
  rtems_vector_number index
)
{
  return &_Record_Interrupt_dispatch_table[ index ];
}

static void _Record_Interrupt_handler( void *arg )
{
  uintptr_t              vector;
  rtems_interrupt_entry *entry;

  vector = (uintptr_t) arg;
  rtems_record_produce( RTEMS_RECORD_INTERRUPT_ENTRY, vector );

  entry = bsp_interrupt_entry_load_acquire(
    &_Record_Interrupt_dispatch_table[ vector ]
  );

  if ( RTEMS_PREDICT_TRUE( entry != NULL ) ) {
    bsp_interrupt_dispatch_entries( entry );
  } else {
#if defined(RTEMS_SMP)
    bsp_interrupt_spurious( vector );
#else
    bsp_interrupt_handler_default( vector );
#endif
  }

  rtems_record_produce( RTEMS_RECORD_INTERRUPT_EXIT, vector );
}

void _Record_Interrupt_initialize( void )
{
  uintptr_t i;

  /*
   * Let each interrupt dispatch table slot reference the
   * _Record_Interrupt_handler() interrupt dispatch wrapper.
   */
  for ( i = 0; i < BSP_INTERRUPT_DISPATCH_TABLE_SIZE; ++i ) {
    _Record_Interrupt_entry_table[ i ].handler = _Record_Interrupt_handler;
    _Record_Interrupt_entry_table[ i ].arg = (void *) i;
    bsp_interrupt_dispatch_table[ i ] = &_Record_Interrupt_entry_table[ i ];
  }
}
