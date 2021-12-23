/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUMicroBlaze
 *
 * @brief MicroBlaze architecture support implementation
 */

/*
 * Copyright (c) 2015, Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <rtems/score/isr.h>
#include <rtems/score/tls.h>
#include <rtems/score/wkspace.h>

void _CPU_Initialize( void )
{
}

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  uint32_t stack = (uint32_t) stack_area_begin;
  uint32_t stack_high = stack + stack_area_size;

  memset( context, 0, sizeof(*context) ) ;

  context->r1 = stack_high - 64;
  context->r15 = (uint32_t) entry_point;

  uint32_t msr;
  _CPU_MSR_GET( msr );
  context->rmsr = msr;

  if ( tls_area != NULL ) {
    _TLS_TCB_at_area_begin_initialize( tls_area );
  }
}

void _CPU_Exception_frame_print( const CPU_Exception_frame *ctx )
{
}

void _CPU_ISR_Set_level( uint32_t level )
{
  uint32_t microblaze_switch_reg;

  _CPU_MSR_GET( microblaze_switch_reg );

  if ( level == 0 ) {
    microblaze_switch_reg |= (MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE);
  } else {
    microblaze_switch_reg &= ~(MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE);
  }

  _CPU_MSR_SET( microblaze_switch_reg );
}

uint32_t _CPU_ISR_Get_level( void )
{
  uint32_t level;

  _CPU_MSR_GET( level );

  /* This is unique. The MSR register contains an interrupt enable flag where
   * most other architectures have an interrupt disable flag. */
  return ( level & (MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE) ) == 0;
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
  *old_handler = _ISR_Vector_table[ vector ];
  _ISR_Vector_table[ vector ] = new_handler;
}

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  while ( true ) {
  }
}
