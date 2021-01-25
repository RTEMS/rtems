/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This header file provides the Partition Manager API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/part/if/header */

#ifndef _RTEMS_RTEMS_PART_H
#define _RTEMS_RTEMS_PART_H

#include <stddef.h>
#include <stdint.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/part/if/group */

/**
 * @defgroup RTEMSAPIClassicPart Partition Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Partition Manager provides facilities to dynamically allocate
 *   memory in fixed-size units.
 */

/* Generated from spec:/rtems/part/if/alignment */

/**
 * @ingroup RTEMSAPIClassicPart
 *
 * @brief This constant defines the minimum alignment of a partition buffer in
 *   bytes.
 *
 * @par Notes
 * Use it with RTEMS_ALIGNED() to define the alignment of partition buffer
 * types or statically allocated partition buffer areas.
 */
#define RTEMS_PARTITION_ALIGNMENT CPU_SIZEOF_POINTER

/* Generated from spec:/rtems/part/if/create */

/**
 * @ingroup RTEMSAPIClassicPart
 *
 * @brief Creates a partition.
 *
 * @param name is the name of the partition.
 *
 * @param starting_address is the starting address of the buffer area used by
 *   the partition.
 *
 * @param length is the length in bytes of the buffer area used by the
 *   partition.
 *
 * @param buffer_size is the size in bytes of a buffer managed by the
 *   partition.
 *
 * @param attribute_set is the attribute set of the partition.
 *
 * @param[out] id is the pointer to an object identifier variable.  The
 *   identifier of the created partition object will be stored in this
 *   variable, in case of a successful operation.
 *
 * This directive creates a partition of fixed size buffers from a physically
 * contiguous memory space which starts at ``starting_address`` and is
 * ``length`` bytes in size.  Each allocated buffer is to be of ``buffer_size``
 * in bytes.  The assigned object identifier is returned in ``id``.  This
 * identifier is used to access the partition with other partition related
 * directives.
 *
 * The **attribute set** specified in ``attribute_set`` is built through a
 * *bitwise or* of the attribute constants described below.  Attributes not
 * mentioned below are not evaluated by this directive and have no effect.
 *
 * The partition can have **local** or **global** scope in a multiprocessing
 * network (this attribute does not refer to SMP systems).
 *
 * * A **local** scope is the default and can be emphasized through the use of
 *   the #RTEMS_LOCAL attribute.  A local partition can be only used by the
 *   node which created it.
 *
 * * A **global** scope is established if the #RTEMS_GLOBAL attribute is set.
 *   The memory space used for the partition must reside in shared memory.
 *   Setting the global attribute in a single node system has no effect.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The partition name was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``length`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``buffer_size`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``length`` parameter was less than the
 *   ``buffer_size`` parameter.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``buffer_size`` parameter was not an
 *   integral multiple of the pointer size.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``buffer_size`` parameter was less than two
 *   times the pointer size.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``starting_address`` parameter was not
 *   on a pointer size boundary.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   new partition.  The number of partitions available to the application is
 *   configured through the #CONFIGURE_MAXIMUM_PARTITIONS configuration option.
 *
 * @par Notes
 * @parblock
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 *
 * The partition buffer area specified by the ``starting_address`` must be
 * properly aligned.  It must be possible to directly store target architecture
 * pointers and also the user data.  For example, if the user data contains
 * some long double or vector data types, the partition buffer area and the
 * buffer size must take the alignment of these types into account which is
 * usually larger than the pointer alignment.  A cache line alignment may be
 * also a factor.  Use #RTEMS_PARTITION_ALIGNMENT to specify the minimum
 * alignment of a partition buffer type.
 *
 * The ``buffer_size`` parameter must be an integral multiple of the pointer
 * size on the target architecture.  Additionally, ``buffer_size`` must be
 * large enough to hold two pointers on the target architecture.  This is
 * required for RTEMS to manage the buffers when they are free.
 *
 * For control and maintenance of the partition, RTEMS allocates a PTCB from
 * the local PTCB free pool and initializes it. Memory from the partition
 * buffer area is not used by RTEMS to store the PTCB.
 *
 * The PTCB for a global partition is allocated on the local node.  Partitions
 * should not be made global unless remote tasks must interact with the
 * partition.  This is to avoid the overhead incurred by the creation of a
 * global partition.  When a global partition is created, the partition's name
 * and identifier must be transmitted to every node in the system for insertion
 * in the local copy of the global object table.
 *
 * The total number of global objects, including partitions, is limited by the
 * value of the #CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application configuration
 * option.
 * @endparblock
 */
rtems_status_code rtems_partition_create(
  rtems_name      name,
  void           *starting_address,
  uintptr_t       length,
  size_t          buffer_size,
  rtems_attribute attribute_set,
  rtems_id       *id
);

/* Generated from spec:/rtems/part/if/ident */

/**
 * @ingroup RTEMSAPIClassicPart
 *
 * @brief Identifies a partition by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param node is the node or node set to search for a matching object.
 *
 * @param[out] id is the pointer to an object identifier variable.  The object
 *   identifier of an object with the specified name will be stored in this
 *   variable, in case of a successful operation.
 *
 * This directive obtains a partition identifier associated with the partition
 * name specified in ``name``.
 *
 * The node to search is specified in ``node``.  It shall be
 *
 * * a valid node number,
 *
 * * the constant #RTEMS_SEARCH_ALL_NODES to search in all nodes,
 *
 * * the constant #RTEMS_SEARCH_LOCAL_NODE to search in the local node only, or
 *
 * * the constant #RTEMS_SEARCH_OTHER_NODES to search in all nodes except the
 *   local node.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the specified nodes.
 *
 * @retval ::RTEMS_INVALID_NODE In multiprocessing configurations, the
 *   specified node was invalid.
 *
 * @par Notes
 * @parblock
 * If the partition name is not unique, then the partition identifier will
 * match the first partition with that name in the search order.  However, this
 * partition identifier is not guaranteed to correspond to the desired
 * partition.  The partition identifier is used with other partition related
 * directives to access the partition.
 *
 * If node is #RTEMS_SEARCH_ALL_NODES, all nodes are searched with the local
 * node being searched first.  All other nodes are searched with the lowest
 * numbered node searched first.
 *
 * If node is a valid node number which does not represent the local node, then
 * only the partitions exported by the designated node are searched.
 *
 * This directive does not generate activity on remote nodes.  It accesses only
 * the local copy of the global object table.
 * @endparblock
 */
rtems_status_code rtems_partition_ident(
  rtems_name name,
  uint32_t   node,
  rtems_id  *id
);

/* Generated from spec:/rtems/part/if/delete */

/**
 * @ingroup RTEMSAPIClassicPart
 *
 * @brief Deletes the partition.
 *
 * @param id is the partition identifier.
 *
 * This directive deletes the partition specified by the ``id`` parameter.  The
 * partition cannot be deleted if any of its buffers are still allocated.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no partition associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The partition resided on a remote
 *   node.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE There were buffers of the partition still in
 *   use.
 *
 * @par Notes
 * @parblock
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 *
 * The PTCB for the deleted partition is reclaimed by RTEMS.
 *
 * The calling task does not have to be the task that created the partition.
 * Any local task that knows the partition identifier can delete the partition.
 *
 * When a global partition is deleted, the partition identifier must be
 * transmitted to every node in the system for deletion from the local copy of
 * the global object table.
 *
 * The partition must reside on the local node, even if the partition was
 * created with the #RTEMS_GLOBAL attribute.
 * @endparblock
 */
rtems_status_code rtems_partition_delete( rtems_id id );

/* Generated from spec:/rtems/part/if/get-buffer */

/**
 * @ingroup RTEMSAPIClassicPart
 *
 * @brief Tries to get a buffer from the partition.
 *
 * @param id is the partition identifier.
 *
 * @param[out] buffer is the pointer to a buffer pointer variable.  The pointer
 *   to the allocated buffer will be stored in this variable, in case of a
 *   successful operation.
 *
 * This directive allows a buffer to be obtained from the partition specified
 * in the ``id`` parameter.  The address of the allocated buffer is returned
 * through the ``buffer`` parameter.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no partition associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``buffer`` parameter was NULL.
 *
 * @retval ::RTEMS_UNSATISFIED There was no free buffer available to allocate
 *   and return.
 *
 * @par Notes
 * @parblock
 * This directive will not cause the running task to be preempted.
 *
 * The buffer start alignment is determined by the memory area and buffer size
 * used to create the partition.
 *
 * A task cannot wait on a buffer to become available.
 *
 * Getting a buffer from a global partition which does not reside on the local
 * node will generate a request telling the remote node to allocate a buffer
 * from the partition.
 * @endparblock
 */
rtems_status_code rtems_partition_get_buffer( rtems_id id, void **buffer );

/* Generated from spec:/rtems/part/if/return-buffer */

/**
 * @ingroup RTEMSAPIClassicPart
 *
 * @brief Returns the buffer to the partition.
 *
 * @param id is the partition identifier.
 *
 * @param buffer is the pointer to the buffer to return.
 *
 * This directive returns the buffer specified by ``buffer`` to the partition
 * specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no partition associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The buffer referenced by ``buffer`` was not
 *   in the partition.
 *
 * @par Notes
 * @parblock
 * This directive will not cause the running task to be preempted.
 *
 * Returning a buffer to a global partition which does not reside on the local
 * node will generate a request telling the remote node to return the buffer to
 * the partition.
 *
 * Returning a buffer multiple times is an error.  It will corrupt the internal
 * state of the partition.
 * @endparblock
 */
rtems_status_code rtems_partition_return_buffer( rtems_id id, void *buffer );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_PART_H */
