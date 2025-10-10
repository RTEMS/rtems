/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling
 */

/*
 *  Copyright (C) 1999 eric.valette@free.fr
 *
 *  Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 *  to make it valid for MVME2300 Motorola boards.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <psim.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>
#include <bsp/openpic.h>
#include <bsp/irq-generic.h>

static rtems_irq_connect_data      rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings   initial_config;
static rtems_irq_connect_data      defaultIrq = {
  /* vectorIdex, hdl  , handle  , on  , off , isOn */
      0,          NULL, NULL    , NULL, NULL, NULL
};
static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER]={
  /*
   * Processor exceptions handled as interrupts
   */
  0
};

  /*
   * This code assumes the exceptions management setup has already
   * been done. We just need to replace the exceptions that will
   * be handled like interrupt. On mcp750/mpc750 and many PPC processors
   * this means the decrementer exception and the external exception.
   */
void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  (void) cpuId;

  int i;

  /*
   * First initialize the Interrupt management hardware
   */
  OpenPIC = (void*)PSIM.OpenPIC;
  openpic_init(1,0,0,16,0,0);

  /*
   * Initialize RTEMS management interrupt table
   */
  /*
   * re-init the rtemsIrq table
   */
  for (i = 0; i < BSP_IRQ_NUMBER; i++) {
    rtemsIrq[i]      = defaultIrq;
    rtemsIrq[i].name = i;
  }
  /*
   * Init initial Interrupt management config
   */
  initial_config.irqNb        = BSP_IRQ_NUMBER;
  initial_config.defaultEntry = defaultIrq;
  initial_config.irqHdlTbl    = rtemsIrq;
  initial_config.irqBase      = BSP_LOWEST_OFFSET;
  initial_config.irqPrioTbl   = irqPrioTable;

  for (i = BSP_PCI_IRQ_LOWEST_OFFSET; i< BSP_PCI_IRQ_NUMBER; i++ ) {
  	irqPrioTable[i] = 8;
  }

  if (!BSP_rtems_irq_mngt_set(&initial_config)) {
    /*
     * put something here that will show the failure...
     */
    rtems_panic(
      "Unable to initialize RTEMS interrupt Management!!! System locked\n"
    );
  }

  #ifdef TRACE_IRQ_INIT
    printk("RTEMS IRQ management is now operationnal\n");
  #endif
}

static int psim_exception_handler(
  BSP_Exception_frame *frame,
  unsigned exception_number
)
{
  (void) frame;
  (void) exception_number;

  rtems_panic("Unexpected interrupt occured");
  return 0;
}

/*
 * functions to enable/disable a source at the ipic
 */
rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;
  (void) attributes;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
  (void) irqnum;

  /* FIXME: do something */
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(irqnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
  (void) irqnum;

  /* FIXME: do something */
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(irqnum));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}

void bsp_interrupt_facility_initialize(void)
{
  rtems_status_code sc;

  /* Install exception handler */
  sc = ppc_exc_set_handler( ASM_EXT_VECTOR, psim_exception_handler);
  _Assert_Unused_variable_equals( sc, RTEMS_SUCCESSFUL);
}
