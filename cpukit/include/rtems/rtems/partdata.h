/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define ::_Partition_Information.
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_PARTDATA_H
#define _RTEMS_RTEMS_PARTDATA_H

#include <rtems/rtems/part.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/objectdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSImplClassicPartition
 *
 * @{
 */

/**
 * @brief The Partition Control Block (PTCB) represents a partition.
 */
typedef struct {
  /**
   * @brief This member turns the PTCB into an object.
   */
  Objects_Control Object;

#if defined(RTEMS_SMP)
  /**
   * @brief This lock protects the chain of unallocated buffers and the number
   *   of allocated buffers.
   */
  ISR_lock_Control Lock;
#endif

  /**
   * @brief This member contains the base address of the buffer area.
   *
   * The base address is the address of the first byte contained in the buffer
   * area.
   */
  const void *base_address;

  /**
   * @brief This member contains the limit address of the buffer area.
   *
   * The limit address is the address of the last byte contained in the buffer
   * area.
   */
  const void *limit_address;

  /**
   * @brief This member contains the size of each buffer in bytes.
  */
  size_t buffer_size;

  /**
   * @brief This member contains the attribute set provided at creation time.
   */
  rtems_attribute attribute_set;

  /**
   * @brief This member contains the count of allocated buffers.
   */
  uintptr_t number_of_used_blocks;

  /**
   * @brief This chain is used to manage unallocated buffers.
   */
  Chain_Control Memory;
} Partition_Control;

/**
 * @brief The Partition Manager objects information is used to manage the
 *   objects of this class.
 *
 * If #CONFIGURE_MAXIMUM_PARTITIONS is greater than zero, then the object
 * information is defined by PARTITION_INFORMATION_DEFINE(), otherwise it is
 * defined by OBJECTS_INFORMATION_DEFINE_ZERO().
 */
extern Objects_Information _Partition_Information;

#if defined(RTEMS_MULTIPROCESSING)
struct _Thread_Control;

/**
 * @brief Sends the extract proxy request.
 *
 * This routine is invoked when a task is deleted and it has a proxy which must
 * be removed from a thread queue and the remote node must be informed of this.
 *
 * @param[in, out] the_thread is the thread proxy.
 * @param id is the partition identifier.
 */
void _Partition_MP_Send_extract_proxy (
  struct _Thread_Control *the_thread,
  Objects_Id              id
);
#endif

/**
 * @brief Defines the Partition Manager objects information.
 *
 * This macro should only be used by <rtems/confdefs/objectsclassic.h>.
 *
 * @param _max is the configured object maximum (the #OBJECTS_UNLIMITED_OBJECTS
 *   flag may be set).
 */
#define PARTITION_INFORMATION_DEFINE( _max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Partition, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_PARTITIONS, \
    Partition_Control, \
    _max, \
    OBJECTS_NO_STRING_NAME, \
    _Partition_MP_Send_extract_proxy \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
