/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: generic MPC83xx BSP */

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

rtems_status_code bsp_irq_disable_at_SIU(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_SIU_IRQ_LOWEST_OFFSET;
  m8xx.simask &= ~(1 << (31 - vecnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_irq_enable_at_SIU(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_SIU_IRQ_LOWEST_OFFSET;
  m8xx.simask |= (1 << (31 - vecnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_irq_disable_at_CPM(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_CPM_IRQ_LOWEST_OFFSET;
  m8xx.cimr &= ~(1 << (vecnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_irq_enable_at_CPM(rtems_vector_number irqnum)
{
  rtems_vector_number vecnum = irqnum - BSP_CPM_IRQ_LOWEST_OFFSET;
  m8xx.cimr |= (1 << (vecnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
  if (BSP_IS_CPM_IRQ(irqnum)) {
    bsp_irq_enable_at_CPM(irqnum);
    return RTEMS_SUCCESSFUL;
  }
  else if (BSP_IS_SIU_IRQ(irqnum)) {
    bsp_irq_enable_at_SIU(irqnum);
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_INVALID_ID;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
  if (BSP_IS_CPM_IRQ(irqnum)) {
    bsp_irq_disable_at_CPM(irqnum);
    return RTEMS_SUCCESSFUL;
  }
  else if (BSP_IS_SIU_IRQ(irqnum)) {
    bsp_irq_disable_at_SIU(irqnum);
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_INVALID_ID;
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
rtems_status_code mpc8xx_cpic_initialize( void)
{
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
  return bsp_irq_enable_at_SIU(BSP_CPM_INTERRUPT);
}

/*
 * Activate the SIU interrupt controller
 */
rtems_status_code mpc8xx_siu_int_initialize( void)
{
  /*
   * mask off all interrupts
   */
  m8xx.simask = 0;

  return RTEMS_SUCCESSFUL;
}

int mpc8xx_exception_handler(BSP_Exception_frame *frame,
			     unsigned exception_number)
{
  return BSP_irq_handle_at_siu(exception_number);
}

rtems_status_code bsp_interrupt_facility_initialize()
{
  /* Install exception handler */
  if (ppc_exc_set_handler(ASM_EXT_VECTOR, mpc8xx_exception_handler)) {
    return RTEMS_IO_ERROR;
  }
  /* Initialize the SIU interrupt controller */
  if (mpc8xx_siu_int_initialize()) {
    return RTEMS_IO_ERROR;
  }
  /* Initialize the CPIC interrupt controller */
  return mpc8xx_cpic_initialize();
}
