/*
 * RISC-V CPU Dependent Source
 *
 * Copyright (c) 2015 University of York.
 * Hesham ALmatary <hesham@alumni.york.ac.uk>
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

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/cpu.h>

/* bsp_start_vector_table_begin is the start address of the vector table
 * containing addresses to ISR Handlers. It's defined at the BSP linkcmds
 * and may differ from one BSP to another.
 */
extern char bsp_start_vector_table_begin[];

void init(void);
void fini(void);

void _init()
{
}

void _fini()
{
}

/**
 * @brief Performs processor dependent initialization.
 */
void _CPU_Initialize(void)
{
  /* Do nothing */
}

void _CPU_ISR_Set_level(unsigned long level)
{
  /* Do nothing */
}

unsigned long  _CPU_ISR_Get_level( void )
{
  /* Do nothing */
  return 0;
}

void _CPU_ISR_install_raw_handler(
  uint32_t   vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /* Do nothing */
}

void _CPU_ISR_install_vector(
  unsigned long    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr *table =
    (proc_ptr *) bsp_start_vector_table_begin;
  proc_ptr current_handler;

  ISR_Level level;

  _ISR_Local_disable( level );

  current_handler = table [vector];

  /* The current handler is now the old one */
  if (old_handler != NULL) {
    *old_handler = (proc_ptr) current_handler;
  }

  /* Write only if necessary to avoid writes to a maybe read-only
   * memory */
  if (current_handler != new_handler) {
    table [vector] = new_handler;
  }

  _ISR_Local_enable( level );

}

void _CPU_Install_interrupt_stack( void )
{
  /* Do nothing */
}

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  do {
  } while (1);

  return NULL;
}
