/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of
 *   rtems_interrupt_get_attributes(), rtems_interrupt_vector_is_enabled(),
 *   rtems_interrupt_vector_enable() and rtems_interrupt_vector_disable().
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

#include <string.h>

rtems_status_code rtems_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  rtems_status_code sc;

  if ( attributes == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  memset( attributes, 0, sizeof( *attributes ) );

  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  sc = bsp_interrupt_get_attributes( vector, attributes );
#if !defined(RTEMS_SMP)
  attributes->can_raise_on = attributes->can_raise;
  attributes->can_get_affinity = true;
  attributes->can_set_affinity = true;
#endif

  return sc;
}

rtems_status_code rtems_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  if ( enabled == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  return bsp_interrupt_vector_is_enabled( vector, enabled );
}

rtems_status_code rtems_interrupt_vector_enable( rtems_vector_number vector )
{
  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  return bsp_interrupt_vector_enable( vector );
}

rtems_status_code rtems_interrupt_vector_disable( rtems_vector_number vector )
{
  if ( !bsp_interrupt_is_valid_vector( vector ) ) {
    return RTEMS_INVALID_ID;
  }

  return bsp_interrupt_vector_disable( vector );
}
