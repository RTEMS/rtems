/**
 * @file
 *
 * @ingroup ClassicDPMEMImpl
 *
 * @brief Dual Ported Memory Manager Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_DPMEM_INL
#define _RTEMS_RTEMS_DPMEM_INL

#include <rtems/rtems/dpmem.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicDPMEMImpl Dual Ported Memory Manager Implementation
 *
 * @ingroup ClassicDPMEM
 *
 * @{
 */

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_DPMEM_EXTERN
#define RTEMS_DPMEM_EXTERN extern
#endif

/**
 *  @brief Define the internal Dual Ported Memory information
 *  The following define the internal Dual Ported Memory information.
 */
RTEMS_DPMEM_EXTERN Objects_Information  _Dual_ported_memory_Information;

/**
 *  @brief Dual Ported Memory Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Dual_ported_memory_Manager_initialization(void);

/**
 *  @brief Allocates a port control block from the inactive chain
 *  of free port control blocks.
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
 *  @brief Frees a port control block to the inactive chain
 *  of free port control blocks.
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
 *  @brief Maps port IDs to port control blocks.
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

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_DPMEM_INL */
/* end of include file */
