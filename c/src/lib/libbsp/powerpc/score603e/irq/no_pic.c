/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq_supp.h>
#include <bsp/vectors.h>

static rtems_irq_connect_data *rtems_hdl_tbl;
static rtems_irq_connect_data  dflt_entry;

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
int C_dispatch_irq_handler(
  BSP_Exception_frame *frame,
  unsigned int excNum
)
{
  register unsigned int irq;
#if (HAS_PMC_PSC8)
  uint16_t              check_irq;
  uint16_t              status_word;
#endif

  if (excNum == ASM_DEC_VECTOR) {
    bsp_irq_dispatch_list(rtems_hdl_tbl, BSP_DECREMENTER, dflt_entry.hdl);
    return 0;
  }

  irq = read_and_clear_irq();

#if (HAS_PMC_PSC8)
   if (irq ==  SCORE603E_PCI_IRQ_0) {
     status_word = read_and_clear_PMC_irq( irq );
     for (check_irq=SCORE603E_IRQ16; check_irq<=SCORE603E_IRQ19; check_irq++) {
       if ( Is_PMC_IRQ( check_irq, status_word )) {
         bsp_irq_dispatch_list_base(rtems_hdl_tbl, check_irq, dflt_entry.hdl);
       }
     }
   } else
#endif
   {
    bsp_irq_dispatch_list_base(rtems_hdl_tbl, irq, dflt_entry.hdl);
   }

  return 0;
}

void
BSP_enable_irq_at_pic(const rtems_irq_number irq)
{
  uint16_t  vec_idx  = irq - Score_IRQ_First;
  unmask_irq( vec_idx );
}

int
BSP_disable_irq_at_pic(const rtems_irq_number irq)
{
  uint16_t  vec_idx  = irq - Score_IRQ_First;
  unmask_irq( vec_idx );
  return 0;
}

int
BSP_setup_the_pic(rtems_irq_global_settings *config)
{
  dflt_entry    = config->defaultEntry;
  rtems_hdl_tbl = config->irqHdlTbl;
  init_irq_data_register();
  return 1;
}
