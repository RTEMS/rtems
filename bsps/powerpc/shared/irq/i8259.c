/*
 *  This file contains the implementation of the function described in irq.h
 *  related to Intel 8259 Programmable Interrupt controller.
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>

#define PIC_EOSI        0x60    ///< End of Specific Interrupt (EOSI)
#define PIC_EOI         0x20    ///< Generic End of Interrupt (EOI)

/* Operation control word type 3.  Bit 3 (0x08) must be set. Even address. */
#define PIC_OCW3_RIS        0x01            /* 1 = read IS, 0 = read IR */
#define PIC_OCW3_RR         0x02            /* register read */
#define PIC_OCW3_P          0x04            /* poll mode command */
/* 0x08 must be 1 to select OCW3 vs OCW2 */
#define PIC_OCW3_SEL        0x08            /* must be 1 */
/* 0x10 must be 0 to select OCW3 vs ICW1 */
#define PIC_OCW3_SMM        0x20            /* special mode mask */
#define PIC_OCW3_ESMM       0x40            /* enable SMM */

/*-------------------------------------------------------------------------+
| Cache for 1st and 2nd PIC IRQ line's status (enabled or disabled) register.
+--------------------------------------------------------------------------*/
/*
 * lower byte is interrupt mask on the master PIC.
 * while upper bits are interrupt on the slave PIC.
 */
static rtems_i8259_masks i8259s_imr_cache = 0xFFFB;
static rtems_i8259_masks i8259s_in_progress = 0;

static inline
void BSP_i8259s_irq_update_master_imr( void )
{
  rtems_i8259_masks mask = i8259s_in_progress | i8259s_imr_cache;
  outport_byte( PIC_MASTER_IMR_IO_PORT, mask & 0xff );
}

static inline
void BSP_i8259s_irq_update_slave_imr( void )
{
  rtems_i8259_masks mask = i8259s_in_progress | i8259s_imr_cache;
  outport_byte( PIC_SLAVE_IMR_IO_PORT, ( mask >> 8 ) & 0xff );
}

/*
 * Is the IRQ valid?
 */
static inline bool BSP_i8259s_irq_valid(const rtems_irq_number irqLine)
{
  return ((int)irqLine >= BSP_ISA_IRQ_LOWEST_OFFSET) &&
    ((int)irqLine <= BSP_ISA_IRQ_MAX_OFFSET);
}

/*
 * Read the IRR register. The default.
 */
static inline uint8_t BSP_i8259s_irq_int_request_reg(uint32_t ioport)
{
  uint8_t isr;
  inport_byte(ioport, isr);
  return isr;
}

/*
 * Read the ISR register. Keep the default of the IRR.
 */
static inline uint8_t BSP_i8259s_irq_in_service_reg(uint32_t ioport)
{
  uint8_t isr;
  outport_byte(ioport, PIC_OCW3_SEL | PIC_OCW3_RR | PIC_OCW3_RIS);
  inport_byte(ioport, isr);
  outport_byte(ioport, PIC_OCW3_SEL | PIC_OCW3_RR);
  return isr;
}

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_disable_at_i8259s
|      Description: Mask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_imr_cache, i8259s_in_progress
|        Arguments: vector_offset - number of IRQ line to mask.
|          Returns: 0 is OK.
+--------------------------------------------------------------------------*/
int BSP_irq_disable_at_i8259s(const rtems_irq_number irqLine)
{
  unsigned short        mask;
  rtems_interrupt_level level;

  if (!BSP_i8259s_irq_valid(irqLine))
    return -1;

  rtems_interrupt_disable(level);

  mask = 1 << irqLine;
  i8259s_imr_cache |= mask;

  if (irqLine < 8)
  {
    BSP_i8259s_irq_update_master_imr();
  }
  else
  {
    BSP_i8259s_irq_update_slave_imr();
  }

  rtems_interrupt_enable(level);

  return 0;
}

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_enable_at_i8259s
|      Description: Unmask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_imr_cache, i8259s_in_progress
|        Arguments: irqLine - number of IRQ line to mask.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
int BSP_irq_enable_at_i8259s(const rtems_irq_number irqLine)
{
  rtems_interrupt_level level;
  unsigned short        mask;

  if (!BSP_i8259s_irq_valid(irqLine))
    return 1;

  rtems_interrupt_disable(level);

  mask = 1 << irqLine;
  i8259s_imr_cache &= ~mask;

  if (irqLine < 8)
  {
    BSP_i8259s_irq_update_master_imr();
  }
  else
  {
    BSP_i8259s_irq_update_slave_imr();
  }

  rtems_interrupt_enable(level);

  return 0;
} /* mask_irq */

int BSP_irq_enabled_at_i8259s(const rtems_irq_number irqLine)
{
  unsigned short mask;

  if (!BSP_i8259s_irq_valid(irqLine))
    return 1;

  mask = (1 << irqLine);
  return  (~(i8259s_imr_cache & mask));
}

/*-------------------------------------------------------------------------+
|         Function: BSP_irq_ack_at_i8259s
|      Description: Signal generic End Of Interrupt (EOI) to appropriate PIC.
| Global Variables: None.
|        Arguments: irqLine - number of IRQ line to acknowledge.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
int BSP_irq_ack_at_i8259s(const rtems_irq_number irqLine)
{
  uint8_t slave_isr = 0;

  if (irqLine >= 8) {
   outport_byte(PIC_SLAVE_COMMAND_IO_PORT, PIC_EOI);
   slave_isr = BSP_i8259s_irq_in_service_reg(PIC_SLAVE_COMMAND_IO_PORT);
  }

  /*
   * Only issue the EOI to the master if there are no more interrupts in
   * service for the slave. i8259a data sheet page 18, The Special Fully Nested
   * Mode, b.
   */
  if (slave_isr == 0)
    outport_byte(PIC_MASTER_COMMAND_IO_PORT, PIC_EOI);

  return 0;

} /* ackIRQ */

unsigned short BSP_irq_suspend_i8259s(unsigned short mask)
{
  unsigned short in_progress_save = i8259s_in_progress;
  i8259s_in_progress |= mask;
  BSP_i8259s_irq_update_master_imr();
  BSP_i8259s_irq_update_slave_imr();
  return in_progress_save;
}

void BSP_irq_resume_i8259s(unsigned short in_progress_save)
{
  i8259s_in_progress = in_progress_save;
  BSP_i8259s_irq_update_master_imr();
  BSP_i8259s_irq_update_slave_imr();
}

void BSP_i8259s_init(void)
{
  /*
   * Always mask at least current interrupt to prevent re-entrance
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
