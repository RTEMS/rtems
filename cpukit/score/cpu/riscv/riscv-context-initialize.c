/*
 * Copyright (c) 2018 embedded brains GmbH
 *
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * COPYRIGHT (c) 1989-2006.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/cpuimpl.h>
#include <rtems/score/address.h>
#include <rtems/score/tls.h>

void _CPU_Context_Initialize(
  Context_Control *context,
  void            *stack_area_begin,
  size_t           stack_area_size,
  uint32_t         new_level,
  void          ( *entry_point )( void ),
  bool             is_fp,
  void            *tls_area
)
{
  void *stack;

  stack = _Addresses_Add_offset( stack_area_begin, stack_area_size );
  stack = _Addresses_Align_down( stack, CPU_STACK_ALIGNMENT );

  context->ra = (uintptr_t) entry_point;
  context->sp = (uintptr_t) stack;
  context->isr_dispatch_disable = 0;

#if __riscv_flen > 0
  /*
   * According to C11 section 7.6 "Floating-point environment <fenv.h>" the
   * floating-point environment shall be initialized to the current state of
   * the creating thread.
   */
  context->fcsr = _RISCV_Read_FCSR();
#endif

  if ( tls_area != NULL ) {
    void *tls_block;

    tls_block = _TLS_TCB_before_TLS_block_initialize( tls_area );
    context->tp = (uintptr_t) tls_block;
  }
}
