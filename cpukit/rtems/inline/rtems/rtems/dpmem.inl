/*  inline/dpmem.inl
 *
 *  This include file contains the inline routine used in conjunction
 *  with the Dual Ported Memory Manager
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __INLINE_DPMEM_inl
#define __INLINE_DPMEM_inl


/*PAGE
 *
 *  _Dual_ported_memory_Allocate
 *
 *  DESCRIPTION:
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

/*PAGE
 *
 *  _Dual_ported_memory_Free
 *
 *  DESCRIPTION:
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

/*PAGE
 *
 *  _Dual_ported_memory_Get
 *
 *  DESCRIPTION:
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

/*PAGE
 *
 *  _Dual_ported_memory_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_port is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Dual_ported_memory_Is_null(
  Dual_ported_memory_Control *the_port
)
{
  return ( the_port == NULL );
}

#endif
/* end of include file */
