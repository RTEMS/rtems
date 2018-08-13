/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#ifndef _RTEMS_SCORE_CPU_ASM_H
#define _RTEMS_SCORE_CPU_ASM_H

#if !ASM

#include <rtems/score/basedefs.h>

RTEMS_INLINE_ROUTINE uint8_t inport_byte(uint16_t port)
{
  uint8_t ret;
  __asm__ volatile ( "inb %1, %0"
                     : "=a" (ret)
                     : "Nd" (port) );
  return ret;
}

RTEMS_INLINE_ROUTINE void outport_byte(uint16_t port, uint8_t val)
{
  __asm__ volatile ( "outb %0, %1" : : "a" (val), "Nd" (port) );
}

RTEMS_INLINE_ROUTINE uint16_t amd64_get_cs(void)
{
  uint16_t segment = 0;

  __asm__ volatile ( "movw %%cs, %0" : "=r" (segment) : "0" (segment) );

  return segment;
}

RTEMS_INLINE_ROUTINE void amd64_set_cr3(uint64_t segment)
{
  __asm__ volatile ( "movq %0, %%cr3" : "=r" (segment) : "0" (segment) );
}

RTEMS_INLINE_ROUTINE void cpuid(
  uint32_t code, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx
) {
  __asm__ volatile ( "cpuid"
                     : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                     : "a" (code) );
}

RTEMS_INLINE_ROUTINE uint64_t rdmsr(uint32_t msr)
{
  uint32_t low, high;
  __asm__ volatile ( "rdmsr" :
                     "=a" (low), "=d" (high) :
                     "c" (msr) );
   return low | (uint64_t) high << 32;
}

RTEMS_INLINE_ROUTINE void wrmsr(uint32_t msr, uint32_t low, uint32_t high)
{
  __asm__ volatile ( "wrmsr" : :
                     "a" (low), "d" (high), "c" (msr) );
}

RTEMS_INLINE_ROUTINE void amd64_enable_interrupts(void)
{
  __asm__ volatile ( "sti" );
}

RTEMS_INLINE_ROUTINE void amd64_disable_interrupts(void)
{
  __asm__ volatile ( "cli" );
}

RTEMS_INLINE_ROUTINE void stub_io_wait(void)
{
  /* XXX: This likely won't be required on any modern boards, but this function
   * exists so it's easier to find all the places it may be used.
   */
}

#endif /* !ASM */

#endif
