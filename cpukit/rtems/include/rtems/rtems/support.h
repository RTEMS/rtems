/*  support.h
 *
 *  This include file contains information about support functions for
 *  the RTEMS API.
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

#ifndef __RTEMS_RTEMS_SUPPORT_h
#define __RTEMS_RTEMS_SUPPORT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/status.h>

/*
 *  rtems_build_name
 *
 *  DESCRIPTION:
 *
 *  This function returns an object name composed of the four characters
 *  C1, C2, C3, and C4.
 *
 *  NOTE:
 *
 *  This must be implemented as a macro for use in Configuration Tables.
 *
 */
 
#define rtems_build_name( _C1, _C2, _C3, _C4 ) \
  ( (unsigned32)(_C1) << 24 | (unsigned32)(_C2) << 16 | (unsigned32)(_C3) << 8 | (unsigned32)(_C4) )
 
/*
 *  rtems_get_class
 *
 *  DESCRIPTION:
 *
 *  This function returns the class portion of the ID.
 *
 */
 
#define rtems_get_class( _id ) \
  _Objects_Get_class( _id )
 
/*
 *  rtems_get_node
 *
 *  DESCRIPTION:
 *
 *  This function returns the node portion of the ID.
 *
 */
 
#define rtems_get_node( _id ) \
  _Objects_Get_node( _id )
 
/*
 *  rtems_get_index
 *
 *  DESCRIPTION:
 *
 *  This function returns the index portion of the ID.
 *
 */
 
#define rtems_get_index( _id ) \
  _Objects_Get_index( _id )

/*
 *  Time related
 */

#define RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) \
        TOD_MILLISECONDS_TO_MICROSECONDS(_ms)
#define RTEMS_MILLISECONDS_TO_TICKS(_ms) \
        TOD_MILLISECONDS_TO_TICKS(_ms)
#define RTEMS_MICROSECONDS_TO_TICKS(_ms) \
        TOD_MICROSECONDS_TO_TICKS(_ms)

/*
 *  Workspace Related
 */

/*
 *  rtems_workspace_get_information
 *
 *  DESCRIPTION:
 *
 *  This directive will return information about the RTEMS
 *  executive workspace.
 */

rtems_status_code rtems_workspace_get_information(
  Heap_Information_block *the_info
);

/*
 *  rtems_workspace_free_space
 *
 *  DESCRIPTION:
 *
 *  This directive will return the number of bytes available in the RTEMS
 *  executive workspace.  This area is probably not be contiguous.
 */

size_t rtems_workspace_free_space(void);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/support.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
