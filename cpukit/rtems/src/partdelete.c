/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This source file contains the implementation of
 *   rtems_partition_delete().
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
#include <rtems/rtems/attrimpl.h>

rtems_status_code rtems_partition_delete(
  rtems_id id
)
{
  Partition_Control *the_partition;
  ISR_lock_Context   lock_context;

  _Objects_Allocator_lock();
  the_partition = _Partition_Get( id, &lock_context );

  if ( the_partition == NULL ) {
    _Objects_Allocator_unlock();

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Partition_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  _Partition_Acquire_critical( the_partition, &lock_context );

  if ( the_partition->number_of_used_blocks != 0 ) {
    _Partition_Release( the_partition, &lock_context );
    _Objects_Allocator_unlock();
    return RTEMS_RESOURCE_IN_USE;
  }

  _Objects_Close( &_Partition_Information, &the_partition->Object );
  _Partition_Release( the_partition, &lock_context );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( the_partition->attribute_set ) ) {
    _Objects_MP_Close(
      &_Partition_Information,
      the_partition->Object.id
    );

    _Partition_MP_Send_process_packet(
      PARTITION_MP_ANNOUNCE_DELETE,
      the_partition->Object.id,
      0,                         /* Not used */
      0                          /* Not used */
    );
  }
#endif

  _ISR_lock_Destroy( &the_partition->Lock );
  _Objects_Free( &_Partition_Information, &the_partition->Object );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
