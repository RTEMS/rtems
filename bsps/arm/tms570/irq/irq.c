/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief TMS570 interrupt support functions definitions.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/tms570-vim.h>
#include <bsp/irq.h>
#include <rtems/score/armv4.h>

unsigned int priorityTable[BSP_INTERRUPT_VECTOR_MAX+1];

/**
 * @brief Set priority of the interrupt vector.
 *
 * This function is here because of compability. It should set
 * priority of the interrupt vector.
 * @warning It does not set any priority at HW layer. It is nearly imposible to
 * @warning set priority of the interrupt on TMS570 in a nice way.
 * @param[in] vector vector of isr
 * @param[in] priority new priority assigned to the vector
 * @return Void
 */
void tms570_irq_set_priority(
  rtems_vector_number vector,
  unsigned priority
)
{
  if ( bsp_interrupt_is_valid_vector(vector) ) {
    priorityTable[vector] = priority;
  }
}

/**
 * @brief Gets priority of the interrupt vector.
 *
 * This function is here because of compability. It returns priority
 * of the isr vector last set by tms570_irq_set_priority function.
 *
 * @warning It does not return any real priority of the HW layer.
 * @param[in] vector vector of isr
 * @retval 0 vector is invalid.
 * @retval priority priority of the interrupt
 */
unsigned tms570_irq_get_priority(
  rtems_vector_number vector
)
{
  if ( bsp_interrupt_is_valid_vector(vector) ) {
   return priorityTable[vector];
 }
 return 0;
}

/**
 * @brief Interrupt dispatch
 *
 * Called by OS to determine which interrupt occured.
 * Function passes control to interrupt handler.
 *
 * @return Void
 */
void bsp_interrupt_dispatch(void)
{
  rtems_vector_number vector = TMS570_VIM.IRQINDEX-1;

  bsp_interrupt_handler_dispatch(vector);
}

/**
 * @brief enables interrupt vector in the HW
 *
 * Enables HW interrupt for specified vector
 *
 * @param[in] vector vector of the isr which needs to be enabled.
 * @retval RTEMS_INVALID_ID vector is invalid.
 * @retval RTEMS_SUCCESSFUL interrupt source enabled.
 */
void bsp_interrupt_vector_enable(
  rtems_vector_number vector
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  TMS570_VIM.REQENASET[vector >> 5] = 1 << (vector & 0x1f);
}

/**
 * @brief disables interrupt vector in the HW
 *
 * Disables HW interrupt for specified vector
 *
 * @param[in] vector vector of the isr which needs to be disabled.
 * @retval RTEMS_INVALID_ID vector is invalid.
 * @retval RTEMS_SUCCESSFUL interrupt source disabled.
 */
void bsp_interrupt_vector_disable(
  rtems_vector_number vector
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  TMS570_VIM.REQENACLR[vector >> 5] = 1 << (vector & 0x1f);
}

/**
 * @brief Init function of interrupt module
 *
 * Resets vectored interrupt interface to default state.
 * Disables all interrupts.
 * Set all sources as IRQ (not FIR).
 *
 * @retval RTEMS_SUCCESSFUL All is set
 */
rtems_status_code bsp_interrupt_facility_initialize(void)
{
  void (**vim_vec)(void) = (void (**)(void)) 0xFFF82000;
  unsigned int value = 0x00010203;
  unsigned int i = 0;
  uint32_t sctlr;

  /* Disable interrupts */
  for ( i = 0; i < 3; i++ ) {
    TMS570_VIM.REQENACLR[i] = 0xffffffff;
  }
  /* Map default events on interrupt vectors */
  for ( i = 0; i < 24; i += 1, value += 0x04040404) {
    TMS570_VIM.CHANCTRL[i] = value;
  }
  /* Set all vectors as IRQ (not FIR) */
  TMS570_VIM.FIRQPR[0] = 3;
  TMS570_VIM.FIRQPR[1] = 0;
  TMS570_VIM.FIRQPR[2] = 0;

  /*
    _CPU_ISR_install_vector(
        ARM_EXCEPTION_IRQ,
        _ARMV4_Exception_interrupt,
        NULL
    );

    Call to setup of interrupt entry in CPU level exception vectors table
    is not used (necessary/possible) because the table is provided
    by c/src/lib/libbsp/arm/shared/start/start.S and POM overlay
    solution remaps that to address zero.
  */

  for ( i = 0; i <= 94; ++i ) {
    vim_vec[i] = _ARMV4_Exception_interrupt;
  }
  /* Clear bit VE in SCTLR register to not use VIM IRQ exception bypass*/
  asm volatile ("mrc p15, 0, %0, c1, c0, 0\n": "=r" (sctlr));
  /*
   * Disable bypass of CPU level exception table for interrupt entry which
   * can be provided by VIM hardware
   */
  sctlr &= ~(1 << 24);
  #if 0
    /*
     * Option to enable exception table bypass for interrupts
     *
     * Because RTEMS requires all interrupts to be serviced through
     * common _ARMV4_Exception_interrupt handler to allow task switching
     * on exit from interrupt working correctly, vim_vec cannot point
     * directly to individual vector handlers and need to point
     * to single entry path. But if TMS570_VIM.IRQINDEX is then used
     * to target execution to corresponding service then for some
     * peripherals (i.e. EMAC) interrupt is already acknowledged
     * by VIM and IRQINDEX is read as zero which leads to spurious
     * interrupt and peripheral not serviced/blocked.
     *
     * To analyze this behavior we used trampolines which setup
     * bsp_interrupt_vector_inject and pass execution to
     * _ARMV4_Exception_interrupt. It works but is more ugly than
     * use of POM remap for these cases where application does not
     * start at address 0x00000000. If RTEMS image is placed at
     * memory space beginning then no of these constructs is necessary.
     */
    sctlr |= 1 << 24;
  #endif
  asm volatile ("mcr p15, 0, %0, c1, c0, 0\n": : "r" (sctlr));

  return RTEMS_SUCCESSFUL;
}
