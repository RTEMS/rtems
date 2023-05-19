/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRegion
 *
 * @brief This header file defines the Region Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH & Co. KG
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

/* Generated from spec:/rtems/region/if/header */

#ifndef _RTEMS_RTEMS_REGION_H
#define _RTEMS_RTEMS_REGION_H

#include <stdint.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/heapinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/region/if/group */

/**
 * @defgroup RTEMSAPIClassicRegion Region Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Region Manager provides facilities to dynamically allocate memory
 *   in variable sized units.
 */

/* Generated from spec:/rtems/region/if/get-segment-size */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Gets the size of the region segment.
 *
 * @param id is the region identifier.
 *
 * @param segment is the begin address of the segment.
 *
 * @param[out] size is the pointer to a uintptr_t object.  When the directive
 *   call is successful, the size of the segment in bytes will be stored in
 *   this object.
 *
 * This directive obtains the size in bytes of the segment specified by
 * ``segment`` of the region specified by ``id`` in ``size``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``segment`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``size`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The segment was not within the region.
 *
 * @par Notes
 * The actual length of the allocated segment may be larger than the requested
 * size because a segment size is always a multiple of the region's page size.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_get_segment_size(
  rtems_id   id,
  void      *segment,
  uintptr_t *size
);

/* Generated from spec:/rtems/region/if/create */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Creates a region.
 *
 * @param name is the object name of the region.
 *
 * @param starting_address is the starting address of the memory area managed
 *   by the region.
 *
 * @param length is the length in bytes of the memory area managed by the
 *   region.
 *
 * @param page_size is the alignment of the starting address and length of each
 *   allocated segment of the region.
 *
 * @param attribute_set is the attribute set of the region.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the created region will be stored in
 *   this object.
 *
 * This directive creates a region which resides on the local node.  The region
 * has the user-defined object name specified in ``name``.  The assigned object
 * identifier is returned in ``id``.  This identifier is used to access the
 * region with other region related directives.
 *
 * The region manages the **contiguous memory area** which starts at
 * ``starting_address`` and is ``length`` bytes long.  The memory area shall be
 * large enough to contain some internal region administration data.
 *
 * The **starting address** and **length of segments** allocated from the
 * region will be an integral multiple of ``page_size``.  The specified page
 * size will be aligned to an implementation-dependent minimum alignment if
 * necessary.
 *
 * The **attribute set** specified in ``attribute_set`` is built through a
 * *bitwise or* of the attribute constants described below.  Not all
 * combinations of attributes are allowed.  Some attributes are mutually
 * exclusive.  If mutually exclusive attributes are combined, the behaviour is
 * undefined.  Attributes not mentioned below are not evaluated by this
 * directive and have no effect.  Default attributes can be selected by using
 * the #RTEMS_DEFAULT_ATTRIBUTES constant.
 *
 * The **task wait queue discipline** is selected by the mutually exclusive
 * #RTEMS_FIFO and #RTEMS_PRIORITY attributes. The discipline defines the order
 * in which tasks wait for allocatable segments on a currently empty region.
 *
 * * The **FIFO discipline** is the default and can be emphasized through use
 *   of the #RTEMS_FIFO attribute.
 *
 * * The **priority discipline** is selected by the #RTEMS_PRIORITY attribute.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``starting_address`` parameter was NULL.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   region.  The number of regions available to the application is configured
 *   through the @ref CONFIGURE_MAXIMUM_REGIONS application configuration
 *   option.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``page_size`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_SIZE The memory area specified in
 *   ``starting_address`` and ``length`` was too small.
 *
 * @par Notes
 * For control and maintenance of the region, RTEMS allocates a RNCB from the
 * local RNCB free pool and initializes it.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The number of regions available to the application is configured through
 *   the @ref CONFIGURE_MAXIMUM_REGIONS application configuration option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 * @endparblock
 */
rtems_status_code rtems_region_create(
  rtems_name      name,
  void           *starting_address,
  uintptr_t       length,
  uintptr_t       page_size,
  rtems_attribute attribute_set,
  rtems_id       *id
);

/* Generated from spec:/rtems/region/if/ident */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Identifies a region by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the object identifier of an object with the specified
 *   name will be stored in this object.
 *
 * This directive obtains a region identifier associated with the region name
 * specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the local node.
 *
 * @par Notes
 * @parblock
 * If the region name is not unique, then the region identifier will match the
 * first region with that name in the search order.  However, this region
 * identifier is not guaranteed to correspond to the desired region.
 *
 * The objects are searched from lowest to the highest index.  Only the local
 * node is searched.
 *
 * The region identifier is used with other region related directives to access
 * the region.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/region/if/delete */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Deletes the region.
 *
 * @param id is the region identifier.
 *
 * This directive deletes the region specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE There were segments of the region still in
 *   use.
 *
 * @par Notes
 * @parblock
 * The region cannot be deleted if any of its segments are still allocated.
 *
 * The RNCB for the deleted region is reclaimed by RTEMS.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may free memory to the RTEMS Workspace.
 * @endparblock
 */
rtems_status_code rtems_region_delete( rtems_id id );

/* Generated from spec:/rtems/region/if/extend */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Extends the region.
 *
 * @param id is the region identifier.
 *
 * @param starting_address is the starting address of the memory area to extend
 *   the region.
 *
 * @param length is the length in bytes of the memory area to extend the
 *   region.
 *
 * This directive adds the memory area which starts at ``starting_address`` for
 * ``length`` bytes to the region specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``starting_address`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The memory area specified by
 *   ``starting_address`` and ``length`` was insufficient to extend the heap.
 *
 * @par Notes
 * There are no alignment requirements for the memory area.  The memory area
 * must be big enough to contain some maintenance blocks.  It must not overlap
 * parts of the current heap memory areas.  Disconnected memory areas added to
 * the heap will lead to used blocks which cover the gaps.  Extending with an
 * inappropriate memory area will corrupt the heap resulting in undefined
 * behaviour.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_extend(
  rtems_id  id,
  void     *starting_address,
  uintptr_t length
);

/* Generated from spec:/rtems/region/if/get-segment */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Gets a segment from the region.
 *
 * @param id is the region identifier.
 *
 * @param size is the size in bytes of the segment to allocate.
 *
 * @param option_set is the option set.
 *
 * @param timeout is the timeout in clock ticks if the #RTEMS_WAIT option is
 *   set.  Use #RTEMS_NO_TIMEOUT to wait potentially forever.
 *
 * @param[out] segment is the pointer to a ``void`` pointer object.  When the
 *   directive call is successful, the begin address of the allocated segment
 *   will be stored in this object.
 *
 * This directive gets a segment from the region specified by ``id``.
 *
 * The **option set** specified in ``option_set`` is built through a *bitwise
 * or* of the option constants described below.  Not all combinations of
 * options are allowed.  Some options are mutually exclusive.  If mutually
 * exclusive options are combined, the behaviour is undefined.  Options not
 * mentioned below are not evaluated by this directive and have no effect.
 * Default options can be selected by using the #RTEMS_DEFAULT_OPTIONS
 * constant.
 *
 * The calling task can **wait** or **try to get** a segment from the region
 * according to the mutually exclusive #RTEMS_WAIT and #RTEMS_NO_WAIT options.
 *
 * * **Waiting to get** a segment from the region is the default and can be
 *   emphasized through the use of the #RTEMS_WAIT option. The ``timeout``
 *   parameter defines how long the calling task is willing to wait.  Use
 *   #RTEMS_NO_TIMEOUT to wait potentially forever, otherwise set a timeout
 *   interval in clock ticks.
 *
 * * **Trying to get** a segment from the region is selected by the
 *   #RTEMS_NO_WAIT option.  If this option is defined, then the ``timeout``
 *   parameter is ignored.  When a segment from the region cannot be
 *   immediately allocated, then the ::RTEMS_UNSATISFIED status is returned.
 *
 * With either #RTEMS_WAIT or #RTEMS_NO_WAIT if there is a segment of the
 * requested size is available, then it is returned in ``segment`` and this
 * directive returns immediately with the ::RTEMS_SUCCESSFUL status code.
 *
 * If the calling task chooses to return immediately and the region has no
 * segment of the requested size available, then the directive returns
 * immediately with the ::RTEMS_UNSATISFIED status code.  If the calling task
 * chooses to wait for a segment, then the calling task is placed on the region
 * wait queue and blocked.  If the region was created with the #RTEMS_PRIORITY
 * option specified, then the calling task is inserted into the wait queue
 * according to its priority.  But, if the region was created with the
 * #RTEMS_FIFO option specified, then the calling task is placed at the rear of
 * the wait queue.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``segment`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``size`` parameter was zero.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``size`` parameter exceeded the maximum
 *   segment size which is possible for the region.
 *
 * @retval ::RTEMS_UNSATISFIED The region had no segment of the requested size
 *   immediately available.
 *
 * @retval ::RTEMS_TIMEOUT The timeout happened while the calling task was
 *   waiting to get a segment from the region.
 *
 * @par Notes
 * The actual length of the allocated segment may be larger than the requested
 * size because a segment size is always a multiple of the region's page size.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * When the request cannot be immediately satisfied and the #RTEMS_WAIT
 *   option is set, the calling task blocks at some point during the directive
 *   call.
 *
 * * The timeout functionality of the directive requires a clock tick.
 * @endparblock
 */
rtems_status_code rtems_region_get_segment(
  rtems_id       id,
  uintptr_t      size,
  rtems_option   option_set,
  rtems_interval timeout,
  void         **segment
);

/* Generated from spec:/rtems/region/if/return-segment */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Returns the segment to the region.
 *
 * @param id is the region identifier.
 *
 * @param segment is the begin address of the segment to return.
 *
 * This directive returns the segment specified by ``segment`` to the region
 * specified by ``id``.  The returned segment is merged with its neighbors to
 * form the largest possible segment.  The first task on the wait queue is
 * examined to determine if its segment request can now be satisfied.  If so,
 * it is given a segment and unblocked.  This process is repeated until the
 * first task's segment request cannot be satisfied.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The segment was not within the region.
 *
 * @par Notes
 * @parblock
 * This directive will cause the calling task to be preempted if one or more
 * local tasks are waiting for a segment and the following conditions exist:
 *
 * * A waiting task has a higher priority than the calling task.
 *
 * * The size of the segment required by the waiting task is less than or equal
 *   to the size of the segment returned.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock a task.  This may cause the calling task to be
 *   preempted.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_return_segment( rtems_id id, void *segment );

/* Generated from spec:/rtems/region/if/resize-segment */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Changes the size of the segment.
 *
 * @param id is the region identifier.
 *
 * @param segment is the begin address of the segment to resize.
 *
 * @param size is the requested new size of the segment.
 *
 * @param[out] old_size is the pointer to an uintptr_t object.  When the
 *   directive call is successful, the old size of the segment will be stored
 *   in this object.
 *
 * This directive is used to increase or decrease the size of the ``segment``
 * of the region specified by ``id``.  When increasing the size of a segment,
 * it is possible that there is no memory available contiguous to the segment.
 * In this case, the request is unsatisfied.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``old_size`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The segment was not within the region.
 *
 * @retval ::RTEMS_UNSATISFIED The region was unable to resize the segment.
 *
 * @par Notes
 * If an attempt to increase the size of a segment fails, then the application
 * may want to allocate a new segment of the desired size, copy the contents of
 * the original segment to the new, larger segment and then return the original
 * segment.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_resize_segment(
  rtems_id   id,
  void      *segment,
  uintptr_t  size,
  uintptr_t *old_size
);

/* Generated from spec:/rtems/region/if/get-information */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Gets the region information.
 *
 * @param id is the region identifier.
 *
 * @param[out] the_info is the pointer to a Heap_Information_block object.
 *   When the directive call is successful, the information of the region will
 *   be stored in this object.
 *
 * This directive is used to obtain information about the used and free memory
 * in the region specified by ``id``. This is a snapshot at the time of the
 * call. The information will be returned in the structure pointed to by
 * ``the_info``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``the_info`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @par Notes
 * @parblock
 * This is primarily intended as a mechanism to obtain a diagnostic
 * information. This method forms am O(n) scan of the free and an O(n) scan of
 * the used blocks in the region to calculate the information provided. Given
 * that the execution time is driven by the number of used and free blocks, it
 * can take a non-deterministic time to execute.
 *
 * To get only the free information of the region use
 * rtems_region_get_free_information().
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_get_information(
  rtems_id                id,
  Heap_Information_block *the_info
);

/* Generated from spec:/rtems/region/if/get-free-information */

/**
 * @ingroup RTEMSAPIClassicRegion
 *
 * @brief Gets the region free information.
 *
 * @param id is the region identifier.
 *
 * @param[out] the_info is the pointer to a Heap_Information_block object.
 *   When the directive call is successful, the free information of the region
 *   will be stored in this object.
 *
 * This directive is used to obtain information about the free memory in the
 * region specified by ``id``. This is a snapshot at the time of the call. The
 * information will be returned in the structure pointed to by ``the_info``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``the_info`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no region associated with the
 *   identifier specified by ``id``.
 *
 * @par Notes
 * @parblock
 * This directive uses the same structure to return information as the
 * rtems_region_get_information() directive but does not fill in the used
 * information.
 *
 * This is primarily intended as a mechanism to obtain a diagnostic
 * information. This method forms am O(n) scan of the free in the region to
 * calculate the information provided. Given that the execution time is driven
 * by the number of used and free blocks, it can take a non-deterministic time
 * to execute. Typically, there are many used blocks and a much smaller number
 * of used blocks making a call to this directive less expensive than a call to
 * rtems_region_get_information().
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_region_get_free_information(
  rtems_id                id,
  Heap_Information_block *the_info
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_REGION_H */
