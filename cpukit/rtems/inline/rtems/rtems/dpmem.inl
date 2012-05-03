/**
 * @file rtems/rtems/dpmem.inl
 *
 *  This include file contains the inline routine used in conjunction
 *  with the Dual Ported Memory Manager
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_DPMEM_H
# error "Never use <rtems/rtems/dpmem.inl> directly; include <rtems/rtems/dpmem.h> instead."
#endif

#ifndef _RTEMS_RTEMS_DPMEM_INL
#define _RTEMS_RTEMS_DPMEM_INL

/**
 *  @addtogroup ClassicDPMEM
 *  @{
 */

/**
 *  @brief Dual_ported_memory_Allocate
 *
 *  This routine allocates a port control block from the inactive chain
 *  of free port control blocks.
 */
RTEMS_INLINE_ROUTINE Dual_ported_memory_Control
   *_Dual_ported_memory_Allocate ( void )
{
  return (Dual_ported_memory_Control *)
     _Objects_Allocate( &_Dual_ported_memory_Information );
}

/**
 *  @brief Dual_ported_memory_Free
 *
 *  This routine frees a port control block to the inactive chain
 *  of free port control blocks.
 */
RTEMS_INLINE_ROUTINE void _Dual_ported_memory_Free (
   Dual_ported_memory_Control *the_port
)
{
  _Objects_Free( &_Dual_ported_memory_Information, &the_port->Object );
}

/**
 *  @brief Dual_ported_memory_Get
 *
 *  This function maps port IDs to port control blocks.  If ID
 *  corresponds to a local port, then it returns the_port control
 *  pointer which maps to ID and location is set to OBJECTS_LOCAL.
 *  Global ports are not supported, thus if ID  does not map to a
 *  local port, location is set to OBJECTS_ERROR and the_port is
 *  undefined.
 */
RTEMS_INLINE_ROUTINE Dual_ported_memory_Control *_Dual_ported_memory_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Dual_ported_memory_Control *)
     _Objects_Get( &_Dual_ported_memory_Information, id, location );
}

/**
 *  @brief Dual_ported_memory_Is_null
 *
 *  This function returns true if the_port is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Dual_ported_memory_Is_null(
  Dual_ported_memory_Control *the_port
)
{
  return ( the_port == NULL );
}

/**@}*/

#endif
/* end of include file */
