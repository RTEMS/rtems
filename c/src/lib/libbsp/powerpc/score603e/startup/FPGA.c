/*  FPGA.c -- Bridge for second and subsequent generations
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: 
 */

#include <bsp.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 *  initialize FPGA
 */
void initialize_PCI_bridge ()
{
#if (!SCORE603E_USE_DINK)
  rtems_unsigned16 mask, shift, data;

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
  rtems_unsigned16 value
)
{
  rtems_unsigned16  *loc;

  loc = (rtems_unsigned16  *)SCORE603E_FPGA_MASK_DATA;

  *loc = value;
}

rtems_unsigned16 get_irq_mask()
{
  rtems_unsigned16  *loc;
  rtems_unsigned16  value;

  loc =  (rtems_unsigned16  *)SCORE603E_FPGA_MASK_DATA;

  value = *loc;

  return value;
}

void unmask_irq( 
  rtems_unsigned16 irq_idx
)
{
  rtems_unsigned16 value;
  rtems_unsigned32 mask_idx = irq_idx;

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


void init_irq_data_register()
{
  rtems_unsigned32 index;
  rtems_unsigned32 i;

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

rtems_unsigned16 read_and_clear_PMC_irq(
  rtems_unsigned16    irq
) 
{
  rtems_unsigned16    status_word = irq;

  status_word = (*SCORE603E_PMC_STATUS_ADDRESS);

  return status_word;
}

rtems_boolean Is_PMC_IRQ(
  rtems_unsigned32   pmc_irq,
  rtems_unsigned16   status_word
)
{
  rtems_boolean   result= FALSE;

  switch(pmc_irq) {
    case SCORE603E_85C30_4_IRQ:
      result = Is_PMC_85C30_4_IRQ( status_word );
      break;
    case SCORE603E_85C30_2_IRQ:
      result = Is_PMC_85C30_2_IRQ( status_word );
      break;
    case SCORE603E_85C30_5_IRQ:
      result = Is_PMC_85C30_5_IRQ( status_word );
      break;
    case SCORE603E_85C30_3_IRQ:
      result = Is_PMC_85C30_3_IRQ( status_word );
      break;
    default:
      assert( 0 );
      break;
  }

  return result;
}

rtems_unsigned16 read_and_clear_irq()
{
  rtems_unsigned16    irq;

  irq = (*SCORE603E_FPGA_VECT_DATA);

  if ((irq & 0xffff0) != 0x10) {
    DEBUG_puts( "ERROR:: no irq data\n");    
    return (irq | 0x80);
  }

  irq &=0xf;

  return irq;
}
