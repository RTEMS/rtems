/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Dummy support for just the decrementer interrupt but no PIC.
 *
 *  T. Straumann, 2007/11/30
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq_supp.h>
#include <libcpu/raw_exception.h>

static rtems_irq_connect_data *rtems_hdl_tbl;
static rtems_irq_connect_data  dflt_entry;

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler(
  struct _BSP_Exception_frame *frame,
  unsigned int excNum
)
{
  register uint32_t l_orig;

  if (excNum == ASM_DEC_VECTOR) {

    l_orig = _ISR_Get_level();
    /* re-enable all interrupts */
    _ISR_Set_level(0);

    bsp_irq_dispatch_list(rtems_hdl_tbl, BSP_DECREMENTER, dflt_entry.hdl);

    _ISR_Set_level(l_orig);
    return;
  }
}

void
BSP_enable_irq_at_pic(const rtems_irq_number irq)
{
}

int
BSP_disable_irq_at_pic(const rtems_irq_number irq)
{
  return 0;
}

int
BSP_setup_the_pic(rtems_irq_global_settings *config)
{
  dflt_entry    = config->defaultEntry;
  rtems_hdl_tbl = config->irqHdlTbl;
  return 1;
}
