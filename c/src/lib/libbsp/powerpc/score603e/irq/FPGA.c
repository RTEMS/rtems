/*  FPGA.c -- Bridge for second and subsequent generations
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>

/*
 *  initialize FPGA
 */
void initialize_PCI_bridge (void)
{
#if (!SCORE603E_USE_DINK)
  uint16_t         mask, shift, data;

  shift = SCORE603E_85C30_0_IRQ - Score_IRQ_First;
  mask = 1 << shift;

  shift = SCORE603E_85C30_1_IRQ - Score_IRQ_First;
  mask  = mask & (1 << shift);

  data = *SCORE603E_FPGA_MASK_DATA;
  data = ~mask;

  *SCORE603E_FPGA_MASK_DATA = data;
#endif

}

void set_irq_mask(
  uint16_t         value
)
{
  uint16_t          *loc;

  loc = (uint16_t*)SCORE603E_FPGA_MASK_DATA;

  *loc = value;
}

uint16_t         get_irq_mask(voi)
{
  uint16_t          *loc;
  uint16_t          value;

  loc =  (uint16_t*)SCORE603E_FPGA_MASK_DATA;

  value = *loc;

  return value;
}

void unmask_irq(
  uint16_t         irq_idx
)
{
  uint16_t         value;
  uint32_t         mask_idx = irq_idx;

  value = get_irq_mask();

#if (HAS_PMC_PSC8)
  switch (irq_idx + Score_IRQ_First ) {
    case SCORE603E_85C30_4_IRQ:
    case SCORE603E_85C30_2_IRQ:
    case SCORE603E_85C30_5_IRQ:
    case SCORE603E_85C30_3_IRQ:
      mask_idx = SCORE603E_PCI_IRQ_0 - Score_IRQ_First;
      break;
    default:
      break;
  }
#endif

  value &= (~(0x1 << mask_idx));
  set_irq_mask( value );
}

void init_irq_data_register(void)
{
  uint32_t         index;
  uint32_t         i;

#if (SCORE603E_USE_DINK)
  set_irq_mask( 0xffff );
#endif

  /*
   * Clear any existing interupts from the vector data register.
   */
  for (i=0; i<20; i++) {
    index =  (*SCORE603E_FPGA_VECT_DATA);
    if ( (index&0x10) != 0x10 )
      break;
  }
}

uint16_t         read_and_clear_PMC_irq(
  uint16_t            irq
)
{
  uint16_t            status_word = irq;

  status_word = (*BSP_PMC_STATUS_ADDRESS);

  return status_word;
}

bool Is_PMC_IRQ(
  uint32_t           pmc_irq,
  uint16_t           status_word
)
{
  bool result = false;

  switch(pmc_irq) {
    case SCORE603E_85C30_4_IRQ:
      result = Is_PMC_85C30_4_IRQ( status_word ) ? true : false;
      break;
    case SCORE603E_85C30_2_IRQ:
      result = Is_PMC_85C30_2_IRQ( status_word ) ? true : false;
      break;
    case SCORE603E_85C30_5_IRQ:
      result = Is_PMC_85C30_5_IRQ( status_word ) ? true : false;
      break;
    case SCORE603E_85C30_3_IRQ:
      result = Is_PMC_85C30_3_IRQ( status_word ) ? true : false;
      break;
    default:
      assert( 0 );
      break;
  }

  return result;
}

uint16_t         read_and_clear_irq(void)
{
  uint16_t            irq;

  irq = (*SCORE603E_FPGA_VECT_DATA);

  if ((irq & 0xffff0) != 0x10) {
    printk( "ERROR:: no irq data\n");
    return (irq | 0x80);
  }

  irq &=0xf;

  return irq;
}
