/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the interrupt controller support
 *   implementation.
 */

/*
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/tms570.h>
#include <bsp/irq.h>
#include <rtems/score/armv4.h>

#define VIM_CHANCTRL_COUNT 24
#define VIM_CHANMAP_MASK UINT32_C(0x7f)
#define VIM_CHANMAP_SHIFT(i) (24 - (8 * (i)))

static void vim_set_channel_request(uint32_t channel, uint32_t request)
{
  uint32_t chanctrl;
  int shift;

  chanctrl = TMS570_VIM.CHANCTRL[channel / 4];
  shift = VIM_CHANMAP_SHIFT(channel % 4);
  chanctrl &= ~(VIM_CHANMAP_MASK << shift);
  chanctrl |= request << shift;
  TMS570_VIM.CHANCTRL[channel / 4] = chanctrl;
}

rtems_status_code tms570_irq_set_priority(
  rtems_vector_number vector,
  uint32_t            priority
)
{
  rtems_interrupt_level level;
  uint32_t current_channel;
  uint32_t chanctrl;
  size_t i;
  size_t j;

  if (!bsp_interrupt_is_valid_vector(vector)) {
   return RTEMS_INVALID_ID;
  }

  if (priority < 2) {
    return RTEMS_INVALID_PRIORITY;
  }

  if (priority >= BSP_INTERRUPT_VECTOR_COUNT) {
    return RTEMS_INVALID_PRIORITY;
  }

  rtems_interrupt_disable(level);
  current_channel = TMS570_VIM.CHANCTRL[priority / 4];
  current_channel >>= VIM_CHANMAP_SHIFT(priority % 4);
  current_channel &= VIM_CHANMAP_MASK;

  for (i = 0; i < VIM_CHANCTRL_COUNT; ++i) {
    chanctrl = TMS570_VIM.CHANCTRL[i];

    for (j = 0; j < 4; ++j) {
      uint32_t channel_vector;

      channel_vector = (chanctrl >> VIM_CHANMAP_SHIFT(j)) & VIM_CHANMAP_MASK;

      if (channel_vector == vector) {
        vim_set_channel_request(i * 4 + j, current_channel);
        goto set_my_request;
      }
    }
  }

set_my_request:

  vim_set_channel_request(priority, vector);
  rtems_interrupt_enable(level);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code tms570_irq_get_priority(
  rtems_vector_number  vector,
  unsigned            *priority
)
{
  rtems_interrupt_level level;
  size_t i;
  size_t j;

  if (priority == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  if (!bsp_interrupt_is_valid_vector(vector)) {
   return RTEMS_INVALID_ID;
  }

  rtems_interrupt_disable(level);

  for (i = 0; i < VIM_CHANCTRL_COUNT; ++i) {
    uint32_t chanctrl;

    chanctrl = TMS570_VIM.CHANCTRL[i];

    for (j = 0; j < 4; ++j) {
      uint32_t channel_vector;

      channel_vector = (chanctrl >> VIM_CHANMAP_SHIFT(j)) & VIM_CHANMAP_MASK;

      if (channel_vector == vector) {
        rtems_interrupt_enable(level);
        *priority = i * 4 + j;
        return RTEMS_SUCCESSFUL;
      }
    }
  }

  rtems_interrupt_enable(level);
  *priority = UINT32_MAX;
  return RTEMS_NOT_DEFINED;
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
rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(
  rtems_vector_number vector
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  TMS570_VIM.REQENASET[vector >> 5] = 1 << (vector & 0x1f);
  return RTEMS_SUCCESSFUL;
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
rtems_status_code bsp_interrupt_vector_disable(
  rtems_vector_number vector
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  TMS570_VIM.REQENACLR[vector >> 5] = 1 << (vector & 0x1f);
  return RTEMS_SUCCESSFUL;
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
void bsp_interrupt_facility_initialize(void)
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
  for ( i = 0; i < VIM_CHANCTRL_COUNT; i += 1, value += 0x04040404) {
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
}
