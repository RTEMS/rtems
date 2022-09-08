/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicPartition.
 */

/*  COPYRIGHT (c) 1989-2008.
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

#ifndef _RTEMS_RTEMS_PARTIMPL_H
#define _RTEMS_RTEMS_PARTIMPL_H

#include <rtems/rtems/partdata.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicPartition Partition Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Partition Manager implementation.
 *
 * @{
 */

/**
 * @brief Calls _Objects_Get() using the ::_Partition_Information.
 *
 * @param id is the object identifier.
 * @param[out] lock_context is the lock context.
 *
 * @return See _Objects_Get().
 */
static inline Partition_Control *_Partition_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
)
{
  return (Partition_Control *) _Objects_Get(
    id,
    lock_context,
    &_Partition_Information
  );
}

/**
 * @brief Acquires the partition lock in an ISR disabled section.
 *
 * @param[in, out] the_partition is the partition control block.
 *
 * @param[in, out] lock_context is the lock context set up by _Partition_Get().
 */
static inline void _Partition_Acquire_critical(
  Partition_Control *the_partition,
  ISR_lock_Context  *lock_context
)
{
  _ISR_lock_Acquire( &the_partition->Lock, lock_context );
}

/**
 * @brief Releases the partition lock and restores the ISR level.
 *
 * @param[in, out] the_partition is the partition control block.
 *
 * @param[in, out] lock_context is the lock context set up by _Partition_Get().
 */
static inline void _Partition_Release(
  Partition_Control *the_partition,
  ISR_lock_Context  *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_partition->Lock, lock_context );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/partmp.h>
#endif

#endif
/* end of include file */
