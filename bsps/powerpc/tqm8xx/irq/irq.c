/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS TQM8xx BSP
 *
 * This file contains the console driver.
 *
 * Derived from: generic MPC83xx BSP
 */

/*
 * Copyright (c) 2008 Thomas Doerfler, embedded brains GmbH & Co. KG
 * All rights reserved.
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

#include <rtems.h>
#include <mpc8xx.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
/*
 * functions to enable/disable a source at the SIU/CPM irq controller
 */

static rtems_status_code bsp_irq_disable_at_SIU(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_SIU_IRQ_LOWEST_OFFSET;
  m8xx.simask &= ~(1 << (31 - vecnum));
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code bsp_irq_enable_at_SIU(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_SIU_IRQ_LOWEST_OFFSET;
  m8xx.simask |= (1 << (31 - vecnum));
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code bsp_irq_disable_at_CPM(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_CPM_IRQ_LOWEST_OFFSET;
  m8xx.cimr &= ~(1 << (vecnum));
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code bsp_irq_enable_at_CPM(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_CPM_IRQ_LOWEST_OFFSET;
  m8xx.cimr |= (1 << (vecnum));
  return RTEMS_SUCCESSFUL;
}

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

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (BSP_IS_CPM_IRQ(vector)) {
    bsp_irq_enable_at_CPM(vector);
  } else if (BSP_IS_SIU_IRQ(vector)) {
    bsp_irq_enable_at_SIU(vector);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (BSP_IS_CPM_IRQ(vector)) {
    bsp_irq_disable_at_CPM(vector);
  } else if (BSP_IS_SIU_IRQ(vector)) {
    bsp_irq_disable_at_SIU(vector);
  }

  return RTEMS_SUCCESSFUL;
}

/*
 *  IRQ Handler: this is called from the primary exception dispatcher
 */
static int BSP_irq_handle_at_cpm(void)
{
  int32_t  cpvecnum;
  uint32_t msr;

  /* Get vector number: write IACK=1, then read vectir */
  m8xx.civr = 1;
  cpvecnum  = (m8xx.civr >> 11) + BSP_CPM_IRQ_LOWEST_OFFSET;

  /*
   * Check the vector number,
   * enable exceptions and dispatch the handler.
   * NOTE: lower-prio interrupts are automatically masked in CPIC
   */
  if (BSP_IS_CPM_IRQ(cpvecnum)) {
    /* Enable all interrupts */
    msr = ppc_external_exceptions_enable();
    /* Dispatch interrupt handlers */
    bsp_interrupt_handler_dispatch(cpvecnum);
    /* Restore machine state */
    ppc_external_exceptions_disable(msr);
  }
  else {
    /* not valid vector */
    bsp_interrupt_handler_default(cpvecnum);
  }
  /*
   * clear "in-service" bit
   */
  m8xx.cisr = 1 << (cpvecnum - BSP_CPM_IRQ_LOWEST_OFFSET);

  return 0;
}

static int BSP_irq_handle_at_siu( unsigned excNum)
{
  int32_t  sivecnum;
  uint32_t msr;
  bool  is_cpm_irq;
  uint32_t simask_save;
  /*
   * check, if interrupt is pending
   * and repeat as long as valid interrupts are pending
   */
  while (0 != (m8xx.simask & m8xx.sipend)) {
    /* Get vector number */
    sivecnum     = (m8xx.sivec >> 26);
    is_cpm_irq = (sivecnum == BSP_CPM_INTERRUPT);
    /*
     * Check the vector number, mask lower priority interrupts, enable
     * exceptions and dispatch the handler.
     */
    if (BSP_IS_SIU_IRQ(sivecnum)) {
      simask_save = m8xx.simask;
      /*
       * if this is the CPM interrupt, mask lower prio interrupts at SIU
       * else mask lower and same priority interrupts
       */
      m8xx.simask &= ~0 << (32
			    - sivecnum
			    - ((is_cpm_irq) ? 1 : 0));

      if (is_cpm_irq) {
	BSP_irq_handle_at_cpm();
      }
      else {
	/* Enable all interrupts */
	msr = ppc_external_exceptions_enable();
	/* Dispatch interrupt handlers */
	bsp_interrupt_handler_dispatch(sivecnum + BSP_SIU_IRQ_LOWEST_OFFSET);
	/* Restore machine state */
	ppc_external_exceptions_disable(msr);
	/*
	 * clear pending bit, if edge triggered interrupt input
	 */
	m8xx.sipend = 1 << (31 - sivecnum);
      }


      /* Restore initial masks */
      m8xx.simask = simask_save;
    } else {
      /* not valid vector */
      bsp_interrupt_handler_default(sivecnum);
    }
  }
  return 0;
}

/*
 * Activate the CPIC
 */
static void mpc8xx_cpic_initialize( void)
{
  rtems_status_code sc;

  /*
   * mask off all interrupts
   */
  m8xx.cimr   = 0;
  /*
   * make sure CPIC request proper level at SIU interrupt controller
   */
  m8xx.cicr  = (0x00e41f80 |
		((BSP_CPM_INTERRUPT/2) << 13));
  /*
   * enable CPIC interrupt in SIU interrupt controller
   */
  sc = bsp_irq_enable_at_SIU(BSP_CPM_INTERRUPT);
  _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);
}

/*
 * Activate the SIU interrupt controller
 */
static void mpc8xx_siu_int_initialize( void)
{
  /*
   * mask off all interrupts
   */
  m8xx.simask = 0;
}

static int mpc8xx_exception_handler(BSP_Exception_frame *frame,
			     unsigned exception_number)
{
  return BSP_irq_handle_at_siu(exception_number);
}

void bsp_interrupt_facility_initialize()
{
  rtems_status_code sc;

  /* Install exception handler */
  sc = ppc_exc_set_handler(ASM_EXT_VECTOR, mpc8xx_exception_handler);
  _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);

  /* Initialize the SIU interrupt controller */
  mpc8xx_siu_int_initialize();

  /* Initialize the CPIC interrupt controller */
  mpc8xx_cpic_initialize();
}
