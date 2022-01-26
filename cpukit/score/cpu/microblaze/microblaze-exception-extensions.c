/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUMicroBlaze
 *
 * @brief MicroBlaze exception extensions implementation
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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
#include <rtems/score/threadimpl.h>

RTEMS_NO_RETURN void _CPU_Exception_resume( CPU_Exception_frame *frame )
{
  /* Break in progress */
  if ( ( frame->msr & MICROBLAZE_MSR_BIP ) != 0 ) {
    _MicroBlaze_Exception_resume_from_break( frame );
  }

  /* Exception in progress */
  if ( ( frame->msr & MICROBLAZE_MSR_EIP ) != 0 ) {
    _MicroBlaze_Exception_resume_from_exception( frame );
  }

  /* Execution should never reach this point */
  rtems_fatal( RTEMS_FATAL_SOURCE_EXCEPTION, (rtems_fatal_code) frame );
}

void _CPU_Exception_disable_thread_dispatch( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  /* Increment interrupt nest and thread dispatch disable level */
  ++cpu_self->thread_dispatch_disable_level;
  ++cpu_self->isr_nest_level;
}

/* -1 means not mappable/recoverable */
int _CPU_Exception_frame_get_signal( CPU_Exception_frame *ef )
{
  uint32_t EC = ef->esr & MICROBLAZE_ESR_EC_MASK;

  /* Break in progress */
  if ( ( ef->msr & MICROBLAZE_MSR_BIP ) != 0 ) {
    return -1;
  }

  switch ( EC ) {
   case 0x0:  /* Stream */
   case 0x7:  /* Privileged or Stack Protection */
     return -1;

   case 0x5:  /* Divide */
   case 0x6:  /* FPU */
     return SIGFPE;

   case 0x3:  /* Instruction Abort */
   case 0x4:  /* Data Abort */
     return SIGSEGV;

   case 0x1:  /* Unaligned access */
   case 0x2:  /* Illegal op-code */
   default:
     return SIGILL;
  }
}

void _CPU_Exception_frame_set_resume( CPU_Exception_frame *ef, void *address )
{
  /* Break in progress */
  if ( ( ef->msr & MICROBLAZE_MSR_BIP ) != 0 ) {
    ef->r16 = address;
    return;
  }

  /* Exception in progress */
  if ( ( ef->msr & MICROBLAZE_MSR_EIP ) != 0 ) {
    ef->r17 = address;
    return;
  }

  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  /* Interrupt in progress must be determined by stack pointer location */
  if (
    ef->r1 >= (uint32_t) cpu_self->interrupt_stack_low
    && ef->r1 < (uint32_t) cpu_self->interrupt_stack_high
  ) {
    ef->r14 = address;
    return;
  }

  /* Default to normal link register */
  ef->r15 = address;
}

/*
 * This returns the target return address, not necessarily the address of the
 * instruction that caused exception. These are the same if it's a MMU exception
 * and the BTR overrides the return address if the exception occurred in a delay
 * slot. */
uint32_t *_MicroBlaze_Get_return_address( CPU_Exception_frame *ef )
{
  /* Break in progress */
  if ( ( ef->msr & MICROBLAZE_MSR_BIP ) != 0 ) {
    return ef->r16;
  }

  /* Exception in progress */
  if ( ( ef->msr & MICROBLAZE_MSR_EIP ) != 0 ) {
    if ( ( ef->esr & MICROBLAZE_ESR_DS ) != 0 ) {
      return ef->btr;
    }

    return ef->r17;
  }

  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  /* Interrupt in progress must be determined by stack pointer location */
  if (
    ef->r1 >= (uint32_t) cpu_self->interrupt_stack_low
    && ef->r1 < (uint32_t) cpu_self->interrupt_stack_high
  ) {
    return ef->r14;
  }

  /* Default to normal link register */
  return ef->r15;
}

/*
 * This can only change the resume address in the case of an exception in a
 * branch delay slot instruction.
 */
void _CPU_Exception_frame_make_resume_next_instruction(
  CPU_Exception_frame *ef
)
{
  uintptr_t ret_addr = (uintptr_t) _MicroBlaze_Get_return_address( ef );

  _CPU_Exception_frame_set_resume( ef, (uint32_t *) ret_addr );
}
