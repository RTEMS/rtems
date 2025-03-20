/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicBarrier
 *
 * @brief This source file contains the implementation of
 *   rtems_barrier_get_number_waiting().
 */

/*
 *  Copyright (C) 2025 Mazen Adel Elmessady
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/barrierimpl.h>

rtems_status_code rtems_barrier_get_number_waiting(
  rtems_id          id,
  uint32_t         *waiting
)
{
  Barrier_Control      *the_barrier;
  Thread_queue_Context  queue_context;

  if ( waiting == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_barrier = _Barrier_Get( id, &queue_context );

  if ( the_barrier == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _CORE_barrier_Acquire_critical( &the_barrier->Barrier, &queue_context );

  *waiting = _CORE_barrier_Get_number_waiting( &the_barrier->Barrier );

  _CORE_barrier_Release( &the_barrier->Barrier, &queue_context );

  return RTEMS_SUCCESSFUL;
}
