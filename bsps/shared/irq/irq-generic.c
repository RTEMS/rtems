/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the generic interrupt controller support
 *   implementation.
 */

/*
 * Copyright (C) 2008, 2021 embedded brains GmbH & Co. KG
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

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  bsp_interrupt_dispatch_index_type bsp_interrupt_dispatch_index_table
    [BSP_INTERRUPT_VECTOR_COUNT];
#endif

rtems_interrupt_entry *
bsp_interrupt_dispatch_table[ BSP_INTERRUPT_DISPATCH_TABLE_SIZE ];

RTEMS_WEAK rtems_interrupt_entry **bsp_interrupt_get_dispatch_table_slot(
  rtems_vector_number index
)
{
  return &bsp_interrupt_dispatch_table[ index ];
}

/* The last entry indicates if everything is initialized */
uint8_t bsp_interrupt_handler_unique_table
  [ ( BSP_INTERRUPT_DISPATCH_TABLE_SIZE + 7 + 1 ) / 8 ];

static inline void bsp_interrupt_set_initialized(void)
{
  bsp_interrupt_set_handler_unique(BSP_INTERRUPT_DISPATCH_TABLE_SIZE, true);
}

#if defined(BSP_INTERRUPT_USE_INDEX_TABLE)
static inline rtems_vector_number bsp_interrupt_allocate_handler_index( void )
{
  rtems_vector_number i;

  /* The first entry will remain empty */
  for ( i = 1; i < BSP_INTERRUPT_DISPATCH_TABLE_SIZE; ++i ) {
    if (  bsp_interrupt_dispatch_table[ i ] == NULL ) {
      break;
    }
  }

  return i;
}
#endif

#if defined(RTEMS_SMP)
RTEMS_STATIC_ASSERT(
  sizeof( Atomic_Uintptr ) == sizeof( rtems_interrupt_entry * ),
  rtems_interrupt_entry_pointer_size
);

void bsp_interrupt_spurious( rtems_vector_number vector )
{
  Atomic_Uintptr        *ptr;
  rtems_interrupt_entry *first;

  /*
   * In order to get the last written pointer value to the first entry, we have
   * to carry out an atomic read-modify-write operation.
   */
  ptr = (Atomic_Uintptr *) bsp_interrupt_get_dispatch_table_slot(
    bsp_interrupt_dispatch_index( vector )
  );
  first = (rtems_interrupt_entry *)
    _Atomic_Fetch_add_uintptr( ptr, 0, ATOMIC_ORDER_ACQUIRE );

  if ( first == NULL ) {
    bsp_interrupt_handler_default( vector );
  } else {
    bsp_interrupt_dispatch_entries( first );
  }
}
#endif

rtems_status_code bsp_interrupt_check_and_lock(
  rtems_vector_number     vector,
  rtems_interrupt_handler routine
)
{
  if ( !bsp_interrupt_is_initialized() ) {
    return RTEMS_INCORRECT_STATE;
  }

  if ( routine == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  if ( rtems_interrupt_is_in_progress() ) {
    return RTEMS_CALLED_FROM_ISR;
  }

  bsp_interrupt_lock();

  return RTEMS_SUCCESSFUL;
}

rtems_interrupt_entry *bsp_interrupt_entry_find(
  rtems_vector_number      vector,
  rtems_interrupt_handler  routine,
  void                    *arg,
  rtems_interrupt_entry ***previous_next
)
{
  rtems_vector_number    index;
  rtems_interrupt_entry *entry;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  index = bsp_interrupt_dispatch_index( vector );
  *previous_next = bsp_interrupt_get_dispatch_table_slot( index );
  entry = **previous_next;

  while ( entry != NULL ) {
    if ( entry->handler == routine && entry->arg == arg ) {
      return entry;
    }

    *previous_next = &entry->next;
    entry = entry->next;
  }

  return NULL;
}

void bsp_interrupt_initialize( void )
{
  bsp_interrupt_facility_initialize();
  bsp_interrupt_set_initialized();
}

static rtems_status_code bsp_interrupt_entry_install_first(
  rtems_vector_number       vector,
  rtems_option              options,
  rtems_interrupt_entry    *entry
)
{
  rtems_vector_number index;

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  index = bsp_interrupt_allocate_handler_index();

  if ( index == BSP_INTERRUPT_DISPATCH_TABLE_SIZE ) {
    /* Handler table is full */
    return RTEMS_NO_MEMORY;
  }
#else
  index = vector;
#endif

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  bsp_interrupt_dispatch_index_table[ vector ] = index;
#endif
  bsp_interrupt_entry_store_release(
    bsp_interrupt_get_dispatch_table_slot( index ),
    entry
  );

  bsp_interrupt_set_handler_unique(
    index,
    RTEMS_INTERRUPT_IS_UNIQUE( options )
  );
#if defined(bsp_interrupt_vector_install)
  bsp_interrupt_vector_install( vector );
#else
  bsp_interrupt_vector_enable( vector );
#endif

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code bsp_interrupt_entry_install(
  rtems_vector_number    vector,
  rtems_option           options,
  rtems_interrupt_entry *entry
)
{
  rtems_vector_number     index;
  rtems_interrupt_entry  *first;
  rtems_interrupt_entry  *other;
  rtems_interrupt_entry **previous_next;

  if ( RTEMS_INTERRUPT_IS_REPLACE( options ) ) {
    return RTEMS_INVALID_NUMBER;
  }

  index = bsp_interrupt_dispatch_index( vector );
  first = *bsp_interrupt_get_dispatch_table_slot( index );

  if ( first == NULL ) {
    return bsp_interrupt_entry_install_first( vector, options, entry );
  }

  if ( RTEMS_INTERRUPT_IS_UNIQUE( options ) ) {
    /* Cannot install a unique entry if there is already an entry installed */
    return RTEMS_RESOURCE_IN_USE;
  }

  if ( bsp_interrupt_is_handler_unique( index ) ) {
    /*
     * Cannot install another entry if there is already an unique entry
     * installed.
     */
    return RTEMS_RESOURCE_IN_USE;
  }

  other = bsp_interrupt_entry_find(
    vector,
    entry->handler,
    entry->arg,
    &previous_next
  );

  if ( other != NULL ) {
    /*
     * Cannot install an entry which has the same routine and argument as an
     * already installed entry.
     */
    return RTEMS_TOO_MANY;
  }

  bsp_interrupt_entry_store_release( previous_next, entry );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_entry_install(
  rtems_vector_number    vector,
  rtems_option           options,
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

  sc = bsp_interrupt_entry_install( vector, options, entry );
  bsp_interrupt_unlock();

  return sc;
}
