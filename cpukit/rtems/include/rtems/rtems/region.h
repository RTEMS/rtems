/**
 * @file rtems/rtems/region.h
 *
 * @brief Constants and Structures Associated with the Region Manager
 *
 *  This include file contains all the constants and structures associated
 *  with the Region Manager.  This manager provides facilities to dynamically
 *  allocate memory in variable sized units which are returned as segments.
 *
 *  Directives provided are:
 *
 *     - create a region
 *     - get an ID of a region
 *     - delete a region
 *     - get a segment from a region
 *     - return a segment to a region
 */

/*  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_REGION_H
#define _RTEMS_RTEMS_REGION_H

#include <rtems/score/object.h>
#include <rtems/score/threadq.h>
#include <rtems/score/heap.h>
#include <rtems/debug.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/types.h>

/**
 *  @defgroup ClassicRegion Regions
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
 *  @brief Instantiate RTEMS Region Data
 *
 *  Region Manager -- Instantiate Data
 *
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_REGION_EXTERN
#define RTEMS_REGION_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following records define the control block used to manage
 *  each region.
 */

typedef struct {
  Objects_Control       Object;
  Thread_queue_Control  Wait_queue;            /* waiting threads        */
  void                 *starting_address;      /* physical start addr    */
  uintptr_t             length;                /* physical length(bytes) */
  uintptr_t             page_size;             /* in bytes               */
  uintptr_t             maximum_segment_size;  /* in bytes               */
  rtems_attribute       attribute_set;
  uint32_t              number_of_used_blocks; /* blocks allocated       */
  Heap_Control          Memory;
}  Region_Control;

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
RTEMS_REGION_EXTERN Objects_Information _Region_Information;

/**
 *  @brief _Region_Manager_initialization
 *
 *  Region Manager
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Region_Manager_initialization(void);

/**
 *  @brief rtems_region_create
 *
 *  Region Manager
 *
 *  This routine implements the rtems_region_create directive.  The
 *  region will have the name name.  The memory area managed by
 *  the region is of length bytes and starts at starting_address.
 *  The memory area will be divided into as many allocatable units of
 *  page_size bytes as possible.   The attribute_set determines which
 *  thread queue discipline is used by the region.  It returns the
 *  id of the created region in ID.
 */
rtems_status_code rtems_region_create(
  rtems_name          name,
  void               *starting_address,
  uintptr_t           length,
  uintptr_t           page_size,
  rtems_attribute     attribute_set,
  rtems_id           *id
);

/**
 *  @brief RTEMS Extend Region
 *
 *  This routine implements the rtems_region_extend directive.  The
 *  region will have the name name.  The memory area managed by
 *  the region will be attempted to be grown by length bytes using
 *  the memory starting at starting_address.
 *
 *  @param[in] id is the id of region to grow
 *  @param[in] starting_address starting address of memory area for extension
 *  @param[in] length is the physical length in bytes to grow the region
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.
 */
rtems_status_code rtems_region_extend(
  rtems_id            id,
  void               *starting_address,
  uintptr_t           length
);

/**
 *  @brief RTEMS Region Name to Id
 *
 *  This routine implements the rtems_region_ident directive.
 *  This directive returns the region ID associated with name.
 *  If more than one region is named name, then the region
 *  to which the ID belongs is arbitrary.
 *
 *  @param[in] name is the user defined region name
 *  @param[in] id is the pointer to region id
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the id will
 *          be filled in with the region id.
 */
rtems_status_code rtems_region_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 *  @brief RTEMS Get Region Information
 *
 *  This routine implements the rtems_region_get_information directive.
 *  This directive returns information about the heap associated with
 *  this region.
 *
 *  @param[in] id is the region id
 *  @param[in] the_info is the pointer to region information block
 *
 *  @return RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 *  *id filled with the region information block
 */
rtems_status_code rtems_region_get_information(
  rtems_id                id,
  Heap_Information_block *the_info
);

/**
 *  @brief RTEMS Get Region Free Information
 *
 *  This routine implements the rtems_region_get_free_information directive.
 *  This directive returns information about the free blocks in the
 *  heap associated with this region.  Information about the used blocks
 *  will be returned as zero.
 *
 *  @param[in] id is the region id
 *  @param[in] the_info is the pointer to region information block
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the the_info will
 *          be filled in with the region information block.
 */
rtems_status_code rtems_region_get_free_information(
  rtems_id                id,
  Heap_Information_block *the_info
);

/**
 *  @brief RTEMS Delete Region
 *
 *  This routine implements the rtems_region_delete directive.  The
 *  region indicated by ID is deleted, provided that none of its segments are
 *  still allocated.
 *
 *  @param[in] id is the region id
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.
 */
rtems_status_code rtems_region_delete(
  rtems_id   id
);

/**
 *  @brief RTEMS Get Region Segment
 *
 *  This routine implements the rtems_region_get_segment directive.  It
 *  attempts to allocate a segment from the region associated with @a id.
 *  If a segment of the requested @a size size can be allocated, its address
 *  is returned in @a segment.  If no segment is available, then the task
 *  may return immediately or block waiting for a segment with an optional
 *  timeout of @a timeout clock ticks.  Whether the task blocks or returns
 *  immediately is based on the no_wait option in the @a option_set.
 *
 *  @param[in] id is the region id
 *  @param[in] size is the segment size in bytes
 *  @param[in] option_set is the wait option
 *  @param[in] timeout is the number of ticks to wait (0 means wait forever)
 *  @param[in] segment is the pointer to segment address
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the segment will
 *          be filled in with the segment address.
 */
rtems_status_code rtems_region_get_segment(
  rtems_id           id,
  uintptr_t          size,
  rtems_option       option_set,
  rtems_interval     timeout,
  void             **segment
);

/**
 *  @brief RTEMS Get Region Segment Size
 *
 *  This routine implements the rtems_region_get_segment_size directive.  It
 *  returns the size in bytes of the specified user memory area.
 *
 *  @param[in] id is the region id
 *  @param[in] segment is the segment address
 *  @param[in] size is the pointer to segment size in bytes
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the size will
 *          be filled in with the segment size in bytes.
 */
rtems_status_code rtems_region_get_segment_size(
  rtems_id           id,
  void              *segment,
  uintptr_t         *size
);

/**
 *  @brief RTEMS Return Region Segment
 *
 *  This routine implements the rtems_region_return_segment directive.  It
 *  frees the segment to the region associated with ID.  The segment must
 *  have been previously allocated from the same region.  If freeing the
 *  segment results in enough memory being available to satisfy the
 *  rtems_region_get_segment of the first blocked task, then that task and as
 *  many subsequent tasks as possible will be unblocked with their requests
 *  satisfied.
 *
 *  @param[in] id is the region id
 *  @param[in] segment is the pointer to segment address
 *
 *  @return RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_region_return_segment(
  rtems_id    id,
  void       *segment
);

/**
 *  @brief Resize RTEMS Region Segment
 *
 *  This routine implements the rtems_region_resize_segment directive.  It
 *  tries to resize segment in the region associated with 'id' to the new size
 *  'size' in place. The first 'size' or old size bytes of the segment
 *  (whatever is less) are guaranteed to remain unmodified. The segment must
 *  have been previously allocated from the same region.  If resizing the
 *  segment results in enough memory being available to satisfy the
 *  rtems_region_get_segment of the first blocked task, then that task and as
 *  many subsequent tasks as possible will be unblocked with their requests
 *  satisfied.
 *
 *  @param[in] id is the region id
 *  @param[in] segmet is the pointer to segment address
 *  @param[in] size is the new required size
 *  @return RTEMS_SUCCESSFUL if operation successful, RTEMS_UNSATISFIED if the
 * 	the segment can't be resized in place or any other code atfailure
 *
 *  @note On RTEMS_SUCCESSFUL or RTEMS_UNSATISFIED exit it returns into the
 *  	'old_size' the old size in bytes of the user memory area of the
 * 	specified segment.
 */
rtems_status_code rtems_region_resize_segment(
  rtems_id    id,
  void       *segment,
  uintptr_t   size,
  uintptr_t  *old_size
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/region.inl>
/**
 *  @brief Process Region Queue
 *
 *  This is a helper routine which is invoked any time memory is
 *  freed.  It looks at the set of waiting tasks and attempts to
 *  satisfy all outstanding requests.
 *
 *  @param[in] the_region is the the region
 */
extern void _Region_Process_queue(Region_Control *the_region);

#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/regionmp.h>
#endif

/**
 *  @brief _Region_Debug_Walk
 *
 *  This routine is invoked to verify the integrity of a heap associated
 *  with the_region.
 */
#ifdef RTEMS_DEBUG

#define _Region_Debug_Walk( _the_region, _source ) \
  do { \
    if ( rtems_debug_is_enabled( RTEMS_DEBUG_REGION ) ) \
      _Heap_Walk( &(_the_region)->Memory, _source, false ); \
  } while ( 0 )

#else

#define _Region_Debug_Walk( _the_region, _source )

#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
