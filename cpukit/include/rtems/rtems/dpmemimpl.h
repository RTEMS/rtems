/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicDPMem
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicDPMem
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_DPMEM_INL
#define _RTEMS_RTEMS_DPMEM_INL

#include <rtems/rtems/dpmemdata.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicDPMem Dual-Ported Memory Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Dual-Ported Memory Manager implementation.
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
static inline Dual_ported_memory_Control
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
static inline void _Dual_ported_memory_Free (
   Dual_ported_memory_Control *the_port
)
{
  _Objects_Free( &_Dual_ported_memory_Information, &the_port->Object );
}

static inline Dual_ported_memory_Control *_Dual_ported_memory_Get(
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
