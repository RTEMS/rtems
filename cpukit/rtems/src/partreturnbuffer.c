/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This source file contains the implementation of
 *   rtems_partition_return_buffer().
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

#include <rtems/rtems/partimpl.h>
#include <rtems/score/address.h>
#include <rtems/score/chainimpl.h>

static bool _Partition_Is_address_on_buffer_boundary(
  const Partition_Control *the_partition,
  const void              *the_buffer
)
{
  intptr_t offset;

  offset = _Addresses_Subtract(
    the_buffer,
    the_partition->base_address
  );

  return ( offset % the_partition->buffer_size ) == 0;
}

static bool _Partition_Is_address_a_buffer_begin(
   const Partition_Control *the_partition,
   const void              *the_buffer
)
{
  const void *base;
  const void *limit;

  base = the_partition->base_address;
  limit = the_partition->limit_address;

  if ( !_Addresses_Is_in_range( the_buffer, base, limit ) ) {
    return false;
  }

  return _Partition_Is_address_on_buffer_boundary( the_partition, the_buffer );
}

static void _Partition_Free_buffer(
  Partition_Control *the_partition,
  void              *the_buffer
)
{
  _Chain_Append_unprotected( &the_partition->Memory, the_buffer );
}

rtems_status_code rtems_partition_return_buffer(
  rtems_id  id,
  void     *buffer
)
{
  Partition_Control *the_partition;
  ISR_lock_Context   lock_context;

  the_partition = _Partition_Get( id, &lock_context );

  if ( the_partition == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Partition_MP_Return_buffer( id, buffer );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _Partition_Acquire_critical( the_partition, &lock_context );

  if ( !_Partition_Is_address_a_buffer_begin( the_partition, buffer ) ) {
    _Partition_Release( the_partition, &lock_context );
    return RTEMS_INVALID_ADDRESS;
  }

  _Partition_Free_buffer( the_partition, buffer );
  the_partition->number_of_used_blocks -= 1;
  _Partition_Release( the_partition, &lock_context );
  return RTEMS_SUCCESSFUL;
}
