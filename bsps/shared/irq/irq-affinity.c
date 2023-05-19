/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of
 *   rtems_interrupt_get_affinity() and rtems_interrupt_set_affinity().
 */

/*
 * Copyright (C) 2017, 2022 embedded brains GmbH & Co. KG
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

#include <bsp/irq-generic.h>

#include <rtems/score/processormask.h>
#include <rtems/score/smpimpl.h>

rtems_status_code rtems_interrupt_set_affinity(
  rtems_vector_number  vector,
  size_t               affinity_size,
  const cpu_set_t     *affinity
)
{
  Processor_mask             set;
  Processor_mask_Copy_status status;

  if ( affinity == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  status = _Processor_mask_From_cpu_set_t( &set, affinity_size, affinity );
  if ( !_Processor_mask_Is_at_most_partial_loss( status ) ) {
    return RTEMS_INVALID_NUMBER;
  }

  _Processor_mask_And( &set, _SMP_Get_online_processors(), &set );
  if ( _Processor_mask_Is_zero( &set ) ) {
    return RTEMS_INVALID_NUMBER;
  }

#if defined(RTEMS_SMP)
  return bsp_interrupt_set_affinity( vector, &set );
#else
  return RTEMS_SUCCESSFUL;
#endif
}

rtems_status_code rtems_interrupt_get_affinity(
  rtems_vector_number  vector,
  size_t               affinity_size,
  cpu_set_t           *affinity
)
{
  rtems_status_code          sc;
  Processor_mask             set;
  Processor_mask_Copy_status status;

  if ( affinity == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  _Processor_mask_Zero( &set );

  if ( bsp_interrupt_is_valid_vector( vector ) ) {
#if defined(RTEMS_SMP)
    sc = bsp_interrupt_get_affinity( vector, &set );
#else
    _Processor_mask_From_index( &set, 0 );
    sc = RTEMS_SUCCESSFUL;
#endif
  } else {
    sc = RTEMS_INVALID_ID;
  }

  status = _Processor_mask_To_cpu_set_t( &set, affinity_size, affinity );
  if ( sc == RTEMS_SUCCESSFUL && status != PROCESSOR_MASK_COPY_LOSSLESS ) {
    return RTEMS_INVALID_SIZE;
  }

  return sc;
}
