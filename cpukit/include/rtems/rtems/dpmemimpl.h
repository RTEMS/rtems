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

#include <rtems/rtems/dpmemdata.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicDPMEMImpl Dual Ported Memory Manager Implementation
 *
 * @ingroup RTEMSInternalClassic
 *
 * @{
 */

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

RTEMS_INLINE_ROUTINE Dual_ported_memory_Control *_Dual_ported_memory_Get(
  Objects_Id        id,
  ISR_lock_Context *lock_context
)
{
  return (Dual_ported_memory_Control *)
    _Objects_Get( id, lock_context, &_Dual_ported_memory_Information );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_DPMEM_INL */
/* end of include file */
