/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of _AArch64_Exception_default
 *
 * This file implements _AArch64_Exception_default for use as part of the
 * default exception handling code which dumps all system registers.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <rtems/fatal.h>
#include <rtems/score/aarch64-system-registers.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>

void _AArch64_Exception_default( CPU_Exception_frame *frame )
{
  uint64_t EC = AARCH64_ESR_EL1_EC_GET( frame->register_syndrome );

  /* Emulate FPSR flags for FENV if a FPU exception occurred */
  if ( EC == 0x2c ) {
    /*
     * This must be done because FENV depends on FPSR values, but trapped FPU
     * exceptions don't set FPSR bits. In the case where a signal is mapped, the
     * signal code executes after the exception frame is restored and FENV
     * functions executed in that context will need this information to be
     * accurate.
     */
    uint64_t ISS = AARCH64_ESR_EL1_EC_GET( frame->register_syndrome );

    /* If the exception bits are valid, use them */
    if ( ( ISS & ( 1 << 23 ) ) != 0 ) {
      /* The bits of the lower byte match the FPSR exception bits */
      frame->register_fpsr |= ( ISS & 0xff );
    }
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXCEPTION, (rtems_fatal_code) frame );
}

void _CPU_Exception_disable_thread_dispatch( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  /* Increment interrupt nest and thread dispatch disable level */
  ++cpu_self->thread_dispatch_disable_level;
  ++cpu_self->isr_nest_level;
}

void _AArch64_Exception_frame_copy(
  CPU_Exception_frame *new_ef,
  CPU_Exception_frame *old_ef
)
{
  *new_ef = *old_ef;
}

int _CPU_Exception_frame_get_signal( CPU_Exception_frame *ef )
{
  uint64_t EC = AARCH64_ESR_EL1_EC_GET( ef->register_syndrome );

  switch ( EC ) {
    case 0x1:  /* WFI */
    case 0x7:  /* SVE/SIMD/FP */
    case 0xa:  /* LD64B/ST64B* */
    case 0x18: /* MSR/MRS/system instruction */
    case 0x19: /* SVE */
    case 0x15: /* Supervisor call */
    case 0x26: /* SP Alignment */
    case 0x31: /* Breakpoint */
    case 0x33: /* Step */
    case 0x35: /* Watchpoint */
    case 0x3c: /* Break Instruction */
      return -1;
    case 0x2c: /* FPU */
      return SIGFPE;
    case 0x21: /* Instruction Abort */
    case 0x25: /* Data Abort */
      return SIGSEGV;
    default:
      return SIGILL;
  }
}

void _CPU_Exception_frame_set_resume( CPU_Exception_frame *ef, void *address )
{
  ef->register_pc = address;
}

#define AARCH64_INSTRUCTION_SIZE 4
void  _CPU_Exception_frame_make_resume_next_instruction( CPU_Exception_frame *ef )
{
  ef->register_pc += AARCH64_INSTRUCTION_SIZE;
}
