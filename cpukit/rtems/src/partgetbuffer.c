/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This source file contains the implementation of
 *   rtems_partition_get_buffer().
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
#include <rtems/score/chainimpl.h>

static void *_Partition_Allocate_buffer( Partition_Control *the_partition )
{
  return _Chain_Get_unprotected( &the_partition->Memory );
}

rtems_status_code rtems_partition_get_buffer(
  rtems_id   id,
  void     **buffer
)
{
  Partition_Control *the_partition;
  ISR_lock_Context   lock_context;
  void              *the_buffer;

  if ( buffer == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_partition = _Partition_Get( id, &lock_context );

  if ( the_partition == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Partition_MP_Get_buffer( id, buffer );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _Partition_Acquire_critical( the_partition, &lock_context );
  the_buffer = _Partition_Allocate_buffer( the_partition );

  if ( the_buffer == NULL ) {
    _Partition_Release( the_partition, &lock_context );
    return RTEMS_UNSATISFIED;
  }

  the_partition->number_of_used_blocks += 1;
  _Partition_Release( the_partition, &lock_context );
  *buffer = the_buffer;
  return RTEMS_SUCCESSFUL;
}
