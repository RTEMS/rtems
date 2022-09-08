/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicBarrier
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicBarrier.
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_BARRIERIMPL_H
#define _RTEMS_RTEMS_BARRIERIMPL_H

#include <rtems/rtems/barrierdata.h>
#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicBarrier Barrier Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Barrier Manager implementation.
 *
 * @{
 */

/**
 *  @brief _Barrier_Allocate
 *
 *  This function allocates a barrier control block from
 *  the inactive chain of free barrier control blocks.
 */
static inline Barrier_Control *_Barrier_Allocate( void )
{
  return (Barrier_Control *) _Objects_Allocate( &_Barrier_Information );
}

/**
 *  @brief _Barrier_Free
 *
 *  This routine frees a barrier control block to the
 *  inactive chain of free barrier control blocks.
 */
static inline void _Barrier_Free (
  Barrier_Control *the_barrier
)
{
  _CORE_barrier_Destroy( &the_barrier->Barrier );
  _Objects_Free( &_Barrier_Information, &the_barrier->Object );
}

static inline Barrier_Control *_Barrier_Get(
  Objects_Id            id,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_initialize( queue_context );
  return (Barrier_Control *) _Objects_Get(
    id,
    &queue_context->Lock_context.Lock_context,
    &_Barrier_Information
  );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
