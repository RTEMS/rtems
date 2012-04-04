/**
 *  @file
 *  
 *  This file was based upon the powerpc and the i386.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/i8259.h>
#include <bsp/pci.h>
#include <bsp/irq-generic.h>


#define DEBUG_8259      1

#define ValidateIrqLine( _irq ) \
   if ( ((int)_irq < 0) ||((int)_irq > 16)) return 1;

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

  ValidateIrqLine(irqLine);

  rtems_interrupt_disable(level);

  /* Recalculate the value */
  mask = 1 << irqLine;
  rval = i8259s_cache & mask ? 0 : 1;
  i8259s_cache |= mask;

  /* Determine which chip and write the value. */
  if (irqLine < 8) {
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_MASTER_IMR_IO_PORT, 
      i8259s_cache & 0xff
    );
  } else {
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_SLAVE_IMR_IO_PORT, 
      ((i8259s_cache & 0xff00) >> 8)
    );
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
  
  ValidateIrqLine( irqLine );

  rtems_interrupt_disable(level);

  /* Calculate the value */
  mask = ~(1 << irqLine);
  i8259s_cache &= mask;

  /* Determine which chip and write the value */
  if (irqLine < 8) {
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_MASTER_IMR_IO_PORT, 
      i8259s_cache & 0xff
    );
  } else {
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_SLAVE_IMR_IO_PORT, 
      ((i8259s_cache & 0xff00) >> 8)
    );
  }

  rtems_interrupt_enable(level);

  return 0;
} /* mask_irq */


int BSP_irq_enabled_at_i8259s(const rtems_irq_number irqLine)
{
  unsigned short mask;

  ValidateIrqLine( irqLine );

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
int BSP_irq_ack_at_i8259s (const rtems_irq_number irqLine)
{
  if (irqLine >= 8) {
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_MASTER_COMMAND_IO_PORT, 
      SLAVE_PIC_EOSI
    );
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_SLAVE_COMMAND_IO_PORT, 
      (PIC_EOSI | (irqLine - 8))
    );
  }else {
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_MASTER_COMMAND_IO_PORT, 
      (PIC_EOSI | irqLine)
    );
  }

  return 0;

} /* ackIRQ */

void BSP_i8259s_init(void)
{
  volatile uint32_t i;

  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_MASTER_IMR_IO_PORT, 0xff );
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT,  0xff );
  

  /*
   * init master 8259 interrupt controller
   */

  /* Start init sequence */
  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_MASTER_COMMAND_IO_PORT, 
    0x11
  );
  /* Vector base  = 0 */
  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_MASTER_IMR_IO_PORT, 
    0x00
  );

  /* edge tiggered, Cascade (slave) on IRQ2 */
  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_MASTER_IMR_IO_PORT, 
    0x04
  );

  /* Select 8086 mode */
  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_MASTER_IMR_IO_PORT, 
    0x01
  );

  /*
   * init slave  interrupt controller
   */

  /* Start init sequence */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_COMMAND_IO_PORT, 0x11); 

  /* Vector base  = 8 */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT, 0x08);

  /* edge triggered, Cascade (slave) on IRQ2 */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT, 0x02);

  /* Select 8086 mode */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT, 0x01); 

  /* Mask all except cascade */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_MASTER_IMR_IO_PORT, 0xFB); 

  /* Mask all */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT, 0xFF); 

  /*
   * Enable all interrupts in debug mode.
   */

  if ( DEBUG_8259 ) {
     i8259s_cache = 0x0101;
  }

  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_MASTER_IMR_IO_PORT, 
    i8259s_cache & 0xff
  );
  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_SLAVE_IMR_IO_PORT, 
    ((i8259s_cache & 0xff00) >> 8)
  );

  for (i=0; i<10000; i++);
}

#define PCI__GEN(bus, off, num)		(((off)^((bus) << 7))+((num) << 4))
#define PCI_INTR_ACK(bus)		PCI__GEN(bus, 0x0c34, 0)

volatile uint8_t  master;
volatile uint8_t  slave;
volatile uint8_t  temp;

void bsp_show_interrupt_regs(void);
void bsp_show_interrupt_regs() {
  unsigned int sr;
  unsigned int cause;
  unsigned int pending;

  mips_get_sr( sr ); 
  mips_get_cause( cause );
  pending = (cause & sr & 0xff00) >> CAUSE_IPSHIFT;

  master = simple_in_8(BSP_8259_BASE_ADDRESS, PIC_MASTER_COMMAND_IO_PORT);
  slave  = simple_in_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_COMMAND_IO_PORT);

  printk("sr: 0x%x cause: 0x%x pending: 0x%x  master: 0x%x slave: 0x%x\n", 
    sr, cause, pending, master, slave 
  );
}

int BSP_i8259s_int_process()
{
  uint8_t           irq;
  volatile uint32_t temp;

  /* Get the Interrupt */
  irq = simple_in_le32(BSP_PCI_BASE_ADDRESS, PCI_INTR_ACK(0) );

  /*
   *  Mask interrupts
   *   + Mask all except cascade on master
   *   + Mask all on slave
   */
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_MASTER_IMR_IO_PORT, 0xFB);
  simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT, 0xFF);

  /* Call the Handler */
  temp = irq + MALTA_SB_IRQ_0;
  bsp_interrupt_handler_dispatch( temp );

  /* Reset the interrupt on the 8259 either the master or the slave chip */
  if (irq & 8) {
    temp = simple_in_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT);

    /* Mask all */
    simple_out_8(BSP_8259_BASE_ADDRESS, PIC_SLAVE_IMR_IO_PORT, 0xFF); 
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_SLAVE_COMMAND_IO_PORT, 
      (PIC_EOSI + (irq&7))
    );
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_MASTER_COMMAND_IO_PORT, 
      SLAVE_PIC_EOSI 
    );
  } else {
    temp = simple_in_8(BSP_8259_BASE_ADDRESS, PIC_MASTER_IMR_IO_PORT);
    /* Mask all except cascade */
    simple_out_8(BSP_8259_BASE_ADDRESS, PIC_MASTER_IMR_IO_PORT, 0xFB);
    simple_out_8(
      BSP_8259_BASE_ADDRESS, 
      PIC_MASTER_COMMAND_IO_PORT, 
      (PIC_EOSI+irq)
    );
  }

  /* Restore the interrupts */
  simple_out_8(BSP_8259_BASE_ADDRESS,PIC_MASTER_IMR_IO_PORT,i8259s_cache&0xff);
  simple_out_8(
    BSP_8259_BASE_ADDRESS, 
    PIC_SLAVE_IMR_IO_PORT, 
    ((i8259s_cache & 0xff00) >> 8)
  );

  return 0;
}
