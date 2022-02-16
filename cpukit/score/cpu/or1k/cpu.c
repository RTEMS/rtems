/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Opencore OR1K CPU Dependent Source
 *
 *  COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
 *
 */

#include <rtems/score/cpuimpl.h>
#include <rtems/score/isr.h>

/* bsp_start_vector_table_begin is the start address of the vector table
 * containing addresses to ISR Handlers. It's defined at the BSP linkcmds
 * and may differ from one BSP to another. 
 */
extern char bsp_start_vector_table_begin[];

/**
 * @brief Performs processor dependent initialization.
 */
void _CPU_Initialize(void)
{
  /* Do nothing */
}

void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error )
{
  ISR_Level level;

  _CPU_ISR_Disable( level );
  (void) level;

  _OR1KSIM_CPU_Halt();

  while ( true ) {
    /* Do nothing */
  }
}

/* end of Fatal Error manager macros */

/**
 * @brief Sets the hardware interrupt level by the level value.
 *
 * @param[in] level for or1k can only range over two values:
 * 0 (enable interrupts) and 1 (disable interrupts). In future
 * implementations if fast context switch is implemented, the level
 * can range from 0 to 15. @see OpenRISC architecture manual.
 *
 */
void _CPU_ISR_Set_level(uint32_t level)
{
  uint32_t sr = 0;
  level = (level > 0)? 1 : 0;

  /* map level bit to or1k interrupt enable/disable bit in sr register */
  level <<= CPU_OR1K_SPR_SR_SHAMT_IEE;

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);

  if (level == 0){ /* Enable all interrupts */
    sr |= CPU_OR1K_SPR_SR_IEE | CPU_OR1K_SPR_SR_TEE;

  } else{
    sr &= ~CPU_OR1K_SPR_SR_IEE;
  }

  _OR1K_mtspr(CPU_OR1K_SPR_SR, sr);
 }

uint32_t  _CPU_ISR_Get_level( void )
{
  uint32_t sr = 0;

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);

  return (sr & CPU_OR1K_SPR_SR_IEE)? 0 : 1;
}

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
)
{
   CPU_ISR_raw_handler *table =
     (CPU_ISR_raw_handler *) bsp_start_vector_table_begin;
   CPU_ISR_raw_handler current_handler;

   ISR_Level level;

  _ISR_Local_disable( level );

  current_handler = table [vector];

  /* The current handler is now the old one */
  if (old_handler != NULL) {
    *old_handler = current_handler;
  }

  /* Write only if necessary to avoid writes to a maybe read-only memory */
  if (current_handler != new_handler) {
    table [vector] = new_handler;
  }

   _ISR_Local_enable( level );
}

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  do {
     _OR1K_CPU_Sleep();
  } while (1);
}
