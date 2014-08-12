/*
 *  Opencore OR1K CPU Dependent Source
 *
 *  COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <bsp/linker-symbols.h>
#include <rtems/score/cpu.h>

/**
 * @brief Performs processor dependent initialization.
 */
void _CPU_Initialize(void)
{
  /* Do nothing */
}

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
  uint32_t   vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
}

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   proc_ptr *table =
     (proc_ptr *) bsp_start_vector_table_begin;
   proc_ptr current_handler;

   ISR_Level level;

  _ISR_Disable( level );

  current_handler = table [vector];

  /* The current handler is now the old one */
  if (old_handler != NULL) {
    *old_handler = (proc_ptr) current_handler;
  }

  /* Write only if necessary to avoid writes to a maybe read-only memory */
  if (current_handler != new_handler) {
    table [vector] = new_handler;
  }

   _ISR_Enable( level );
}

void _CPU_Install_interrupt_stack( void )
{
}

void _CPU_Thread_Idle_body( void )
{
  do {
     _OR1K_CPU_Sleep();
  } while (1);
}
