/*
 * Epiphany CPU Dependent Source
 *
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
 *
 * COPYRIGHT (c) 1989-1999.
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

#include <rtems/score/cpu.h>

void _init(void);
void _fini(void);

void _init(void)
{
}

void _fini(void)
{
}

void _CPU_Exception_frame_print (const CPU_Exception_frame *ctx)
{
  /* Do nothing */
}
/**
 * @brief Performs processor dependent initialization.
 */
void _CPU_Initialize(void)
{
  /* Do nothing */
}

void _CPU_ISR_Set_level(uint32_t level)
{
  /* Do nothing */
}

uint32_t  _CPU_ISR_Get_level( void )
{
  /* Do nothing */
  return 0;
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  static CPU_Counter_ticks counter;

  CPU_Counter_ticks snapshot;

  snapshot = counter;
  counter = snapshot + 1;

  return snapshot;
}

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  do {
    __asm__ __volatile__ ("idle");
  } while (1);
  
  return NULL;
}
