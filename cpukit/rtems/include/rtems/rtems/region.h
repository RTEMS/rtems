/*  region.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Region Manager.  This manager provides facilities to dynamically
 *  allocate memory in variable sized units which are returned as segments.
 *
 *  Directives provided are:
 *
 *     + create a region
 *     + get an ID of a region
 *     + delete a region
 *     + get a segment from a region
 *     + return a segment to a region
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_REGION_h
#define __RTEMS_REGION_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/threadq.h>
#include <rtems/score/heap.h>
#include <rtems/debug.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/types.h>

/*
 *  The following records define the control block used to manage
 *  each region.
 */

typedef struct {
  Objects_Control       Object;
  Thread_queue_Control  Wait_queue;            /* waiting threads        */
  void                 *starting_address;      /* physical start addr    */
  unsigned32            length;                /* physical length(bytes) */
  unsigned32            page_size;             /* in bytes               */
  unsigned32            maximum_segment_size;  /* in bytes               */
  rtems_attribute       attribute_set;
  unsigned32            number_of_used_blocks; /* blocks allocated       */
  Heap_Control          Memory;
}  Region_Control;

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

RTEMS_EXTERN Objects_Information _Region_Information;

/*
 *  _Region_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Region_Manager_initialization(
  unsigned32 maximum_regions
);

/*
 *  rtems_region_create
 *
 *  DESCRIPTION:
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
  unsigned32          length,
  unsigned32          page_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
);

/*
 *  rtems_region_extend
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_extend directive.  The
 *  region will have the name name.  The memory area managed by
 *  the region will be attempted to be grown by length bytes using
 *  the memory starting at starting_address.
 */

rtems_status_code rtems_region_extend(
  Objects_Id          id,
  void               *starting_address,
  unsigned32          length
);

/*
 *  rtems_region_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_ident directive.
 *  This directive returns the region ID associated with name.
 *  If more than one region is named name, then the region
 *  to which the ID belongs is arbitrary.
 */

rtems_status_code rtems_region_ident(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_region_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_get_information directive.
 *  This directive returns information about the heap associated with
 *  this region.
 */

rtems_status_code rtems_region_get_information(
  Objects_Id              id,
  Heap_Information_block *the_info
);

/*
 *  rtems_region_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_delete directive.  The
 *  region indicated by ID is deleted.
 */

rtems_status_code rtems_region_delete(
  Objects_Id id
);

/*
 *  rtems_region_get_segment
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_get_segment directive.  It
 *  attempts to allocate a segment from the region associated with ID.
 *  If a segment of the requested size can be allocated, its address
 *  is returned in segment.  If no segment is available, then the task
 *  may return immediately or block waiting for a segment with an optional
 *  timeout of timeout clock ticks.  Whether the task blocks or returns
 *  immediately is based on the no_wait option in the option_set.
 */

rtems_status_code rtems_region_get_segment(
  Objects_Id         id,
  unsigned32         size,
  rtems_option       option_set,
  rtems_interval     timeout,
  void              **segment
);

/*
 *  rtems_region_get_segment_size
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_get_segment_size directive.  It
 *  returns the size in bytes of the specified user memory area.
 */

rtems_status_code rtems_region_get_segment_size(
  Objects_Id         id,
  void              *segment,
  unsigned32        *size
);

/*
 *  rtems_region_return_segment
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_region_return_segment directive.  It
 *  frees the segment to the region associated with ID.  The segment must
 *  have been previously allocated from the same region.  If freeing the
 *  segment results in enough memory being available to satisfy the
 *  rtems_region_get_segment of the first blocked task, then that task and as
 *  many subsequent tasks as possible will be unblocked with their requests
 *  satisfied.
 */

rtems_status_code rtems_region_return_segment(
  Objects_Id  id,
  void       *segment
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/region.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/regionmp.h>
#endif

/*
 *  _Region_Debug_Walk
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked to verify the integrity of a heap associated
 *  with the_region.
 */

#ifdef RTEMS_DEBUG

#define _Region_Debug_Walk( _the_region, _source ) \
  do { \
    if ( _Debug_Is_enabled( RTEMS_DEBUG_REGION ) ) \
      _Heap_Walk( &(_the_region)->Memory, _source, FALSE ); \
  } while ( 0 )

#else

#define _Region_Debug_Walk( _the_region, _source )

#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
