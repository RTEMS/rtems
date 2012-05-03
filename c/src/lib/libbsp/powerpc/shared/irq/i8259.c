/*
 *  This file contains the implementation of the function described in irq.h
 *  related to Intel 8259 Programmable Interrupt controller.
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>

/*-------------------------------------------------------------------------+
| Cache for 1st and 2nd PIC IRQ line's status (enabled or disabled) register.
+--------------------------------------------------------------------------*/
/*
 * lower byte is interrupt mask on the master PIC.
 * while upper bits are interrupt on the slave PIC.
 */
volatile rtems_i8259_masks i8259s_cache = 0xfffb;

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_disable_at_i8259s
|      Description: Mask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_cache
|        Arguments: vector_offset - number of IRQ line to mask.
|          Returns: original state or -1 on error.
+--------------------------------------------------------------------------*/
int BSP_irq_disable_at_i8259s    (const rtems_irq_number irqLine)
{
  unsigned short        mask;
  rtems_interrupt_level level;
  int                   rval;

  if ( ((int)irqLine < BSP_ISA_IRQ_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_ISA_IRQ_MAX_OFFSET)
       )
    return -1;

  rtems_interrupt_disable(level);

  mask = 1 << irqLine;
  rval = i8259s_cache & mask ? 0 : 1;
  i8259s_cache |= mask;

  if (irqLine < 8)
  {
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
  }
  else
  {
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  rtems_interrupt_enable(level);

  return rval;
}

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_enable_at_i8259s
|      Description: Unmask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_cache
|        Arguments: irqLine - number of IRQ line to mask.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
int BSP_irq_enable_at_i8259s    (const rtems_irq_number irqLine)
{
  unsigned short        mask;
  rtems_interrupt_level level;

  if ( ((int)irqLine < BSP_ISA_IRQ_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_ISA_IRQ_MAX_OFFSET )
       )
    return 1;

  rtems_interrupt_disable(level);

  mask = ~(1 << irqLine);
  i8259s_cache &= mask;

  if (irqLine < 8)
  {
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
  }
  else
  {
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  rtems_interrupt_enable(level);

  return 0;
} /* mask_irq */

int BSP_irq_enabled_at_i8259s        	(const rtems_irq_number irqLine)
{
  unsigned short mask;

  if ( ((int)irqLine < BSP_ISA_IRQ_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_ISA_IRQ_MAX_OFFSET)
     )
    return 1;

  mask = (1 << irqLine);
  return  (~(i8259s_cache & mask));
}

/*-------------------------------------------------------------------------+
|         Function: BSP_irq_ack_at_i8259s
|      Description: Signal generic End Of Interrupt (EOI) to appropriate PIC.
| Global Variables: None.
|        Arguments: irqLine - number of IRQ line to acknowledge.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
int BSP_irq_ack_at_i8259s  	(const rtems_irq_number irqLine)
{
  if (irqLine >= 8) {
    outport_byte(PIC_MASTER_COMMAND_IO_PORT, SLAVE_PIC_EOSI);
    outport_byte(PIC_SLAVE_COMMAND_IO_PORT, (PIC_EOSI | (irqLine - 8)));
  }
  else {
    outport_byte(PIC_MASTER_COMMAND_IO_PORT, (PIC_EOSI | irqLine));
  }

  return 0;

} /* ackIRQ */

void BSP_i8259s_init(void)
{
  /*
   * init master 8259 interrupt controller
   */
  outport_byte(PIC_MASTER_COMMAND_IO_PORT, 0x11); /* Start init sequence */
  outport_byte(PIC_MASTER_IMR_IO_PORT, 0x00);/* Vector base  = 0 */
  outport_byte(PIC_MASTER_IMR_IO_PORT, 0x04);/* edge tiggered, Cascade (slave) on IRQ2 */
  outport_byte(PIC_MASTER_IMR_IO_PORT, 0x01);/* Select 8086 mode */
  outport_byte(PIC_MASTER_IMR_IO_PORT, 0xFB); /* Mask all except cascade */
  /*
   * init slave  interrupt controller
   */
  outport_byte(PIC_SLAVE_COMMAND_IO_PORT, 0x11); /* Start init sequence */
  outport_byte(PIC_SLAVE_IMR_IO_PORT, 0x08);/* Vector base  = 8 */
  outport_byte(PIC_SLAVE_IMR_IO_PORT, 0x02);/* edge triggered, Cascade (slave) on IRQ2 */
  outport_byte(PIC_SLAVE_IMR_IO_PORT, 0x01); /* Select 8086 mode */
  outport_byte(PIC_SLAVE_IMR_IO_PORT, 0xFF); /* Mask all */

}
