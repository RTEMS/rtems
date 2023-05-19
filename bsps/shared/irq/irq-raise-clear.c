/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of
 *   rtems_interrupt_clear().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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
#include <rtems/config.h>

rtems_status_code rtems_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  if ( pending == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  return bsp_interrupt_is_pending( vector, pending );
}

rtems_status_code rtems_interrupt_raise( rtems_vector_number vector )
{
  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  return bsp_interrupt_raise( vector );
}

rtems_status_code rtems_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  if ( cpu_index >= rtems_configuration_get_maximum_processors() ) {
    return RTEMS_NOT_CONFIGURED;
  }

#if defined(RTEMS_SMP)
  if ( !_Processor_mask_Is_set( _SMP_Get_online_processors(), cpu_index ) ) {
    return RTEMS_INCORRECT_STATE;
  }

  return bsp_interrupt_raise_on( vector, cpu_index );
#else
  return bsp_interrupt_raise( vector );
#endif
}

rtems_status_code rtems_interrupt_clear( rtems_vector_number vector )
{
  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  return bsp_interrupt_clear( vector );
}
