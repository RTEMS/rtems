/*
 *  This file contains the implementation of the function described in irq.h
 */

/*
 *  Copyright (c) 2009 embedded brains GmbH
 *  Copyright (C) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <rtems/score/cpu.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>


#include "elcr.h"

RTEMS_INTERRUPT_LOCK_DEFINE( static, rtems_i8259_access_lock, "rtems_i8259_access_lock" );

/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from teh prioruty table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
static rtems_i8259_masks irq_mask_or_tbl[BSP_IRQ_LINES_NUMBER];

/*
 * Stats of interrupts dispatched.
 */
static uint32_t irq_count[BSP_IRQ_VECTOR_NUMBER] = {0};
static uint32_t spurious_count;

/*
 * Edge or level trigger interrupts.
 */
static enum intr_trigger irq_trigger[BSP_IRQ_LINES_NUMBER];

/*-------------------------------------------------------------------------+
| Cache for 1st and 2nd PIC IRQ line's mssk (enabled or disabled) register.
+--------------------------------------------------------------------------*/
/*
 * lower byte is interrupt mask on the master PIC.
 * while upper bits are interrupt on the slave PIC.
 * This cache is initialized in ldseg.s
 */
static rtems_i8259_masks i8259a_imr_cache = 0xFFFB;
static rtems_i8259_masks i8259a_in_progress = 0;

static inline
void BSP_i8259a_irq_update_master_imr( void )
{
  rtems_i8259_masks mask = i8259a_in_progress | i8259a_imr_cache;
  outport_byte( PIC_MASTER_IMR_IO_PORT, mask & 0xff );
}

static inline
void BSP_i8259a_irq_update_slave_imr( void )
{
  rtems_i8259_masks mask = i8259a_in_progress | i8259a_imr_cache;
  outport_byte( PIC_SLAVE_IMR_IO_PORT, ( mask >> 8 ) & 0xff );
}

/*
 * Print the stats.
 */
uint32_t BSP_irq_count_dump(FILE *f)
{
  uint32_t tot = 0;
  int      i;
 if ( !f )
   f = stdout;
 fprintf(f,"SPURIOUS: %9"PRIu32"\n", spurious_count);
 for ( i = 0; i < BSP_IRQ_VECTOR_NUMBER; i++ ) {
   char type = '-';
   if (i < BSP_IRQ_LINES_NUMBER)
     type = irq_trigger[i] == INTR_TRIGGER_EDGE ? 'E' : 'L';
   tot += irq_count[i];
   fprintf(f,"IRQ %2u: %c %9"PRIu32"\n", i, type, irq_count[i]);
 }
 return tot;
}

/*
 * Is the IRQ valid?
 */
static inline bool BSP_i8259a_irq_valid(const rtems_irq_number irqLine)
{
  return ((int)irqLine >= BSP_IRQ_VECTOR_LOWEST_OFFSET) &&
    ((int)irqLine <= BSP_IRQ_MAX_ON_i8259A);
}

/*
 * Read the IRR register. The default.
 */
static inline uint8_t BSP_i8259a_irq_int_request_reg(uint32_t ioport)
{
  uint8_t isr;
  inport_byte(ioport, isr);
  return isr;
}

/*
 * Read the ISR register. Keep the default of the IRR.
 */
static inline uint8_t BSP_i8259a_irq_in_service_reg(uint32_t ioport)
{
  uint8_t isr;
  outport_byte(ioport, PIC_OCW3_SEL | PIC_OCW3_RR | PIC_OCW3_RIS);
  inport_byte(ioport, isr);
  outport_byte(ioport, PIC_OCW3_SEL | PIC_OCW3_RR);
  return isr;
}

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_disable_at_i8259a
|      Description: Mask IRQ line in appropriate PIC chip.
| Global Variables: i8259a_imr_cache, i8259a_in_progress
|        Arguments: vector_offset - number of IRQ line to mask.
|          Returns: 0 is OK.
+--------------------------------------------------------------------------*/
static int BSP_irq_disable_at_i8259a(const rtems_irq_number irqLine)
{
  unsigned short        mask;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(&rtems_i8259_access_lock, &lock_context);

  mask = 1 << irqLine;
  i8259a_imr_cache |= mask;

  if (irqLine < 8)
  {
    BSP_i8259a_irq_update_master_imr();
  }
  else
  {
    BSP_i8259a_irq_update_slave_imr();
  }

  rtems_interrupt_lock_release(&rtems_i8259_access_lock, &lock_context);

  return 0;
}

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_enable_at_i8259a
|      Description: Unmask IRQ line in appropriate PIC chip.
| Global Variables: i8259a_imr_cache, i8259a_in_progress
|        Arguments: irqLine - number of IRQ line to mask.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
static int BSP_irq_enable_at_i8259a(const rtems_irq_number irqLine)
{
  unsigned short        mask;
  rtems_interrupt_lock_context lock_context;
  uint8_t               isr;
  uint8_t               irr;

  rtems_interrupt_lock_acquire(&rtems_i8259_access_lock, &lock_context);

  mask = 1 << irqLine;
  i8259a_imr_cache &= ~mask;

  if (irqLine < 8)
  {
    isr = BSP_i8259a_irq_in_service_reg(PIC_MASTER_COMMAND_IO_PORT);
    irr = BSP_i8259a_irq_int_request_reg(PIC_MASTER_COMMAND_IO_PORT);
    BSP_i8259a_irq_update_master_imr();
  }
  else
  {
    isr = BSP_i8259a_irq_in_service_reg(PIC_SLAVE_COMMAND_IO_PORT);
    irr = BSP_i8259a_irq_int_request_reg(PIC_SLAVE_COMMAND_IO_PORT);
    BSP_i8259a_irq_update_slave_imr();
  }

  if (((isr ^ irr) & mask) != 0)
    printk("i386: isr=%x irr=%x\n", isr, irr);

  rtems_interrupt_lock_release(&rtems_i8259_access_lock, &lock_context);

  return 0;
} /* mask_irq */

/*-------------------------------------------------------------------------+
|         Function: BSP_irq_ack_at_i8259a
|      Description: Signal generic End Of Interrupt (EOI) to appropriate PIC.
| Global Variables: None.
|        Arguments: irqLine - number of IRQ line to acknowledge.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
static int BSP_irq_ack_at_i8259a(const rtems_irq_number irqLine)
{
  uint8_t slave_isr = 0;

  if (irqLine >= 8) {
   outport_byte(PIC_SLAVE_COMMAND_IO_PORT, PIC_EOI);
   slave_isr = BSP_i8259a_irq_in_service_reg(PIC_SLAVE_COMMAND_IO_PORT);
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

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

static rtems_irq_prio irqPrioTable[BSP_IRQ_LINES_NUMBER]={
  /*
   * actual priorities for each interrupt source:
   *	0   means that only current interrupt is masked
   *	255 means all other interrupts are masked
   * The second entry has a priority of 255 because
   * it is the slave pic entry and is should always remain
   * unmasked.
   */
  0,0,
  255,
  0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0
};

static void compute_i8259_masks_from_prio (void)
{
  rtems_interrupt_lock_context lock_context;
  unsigned int i;
  unsigned int j;

  rtems_interrupt_lock_acquire(&rtems_i8259_access_lock, &lock_context);

  /*
   * Always mask at least current interrupt to prevent re-entrance
   */
  for (i=0; i < BSP_IRQ_LINES_NUMBER; i++) {
    * ((unsigned short*) &irq_mask_or_tbl[i]) = (1 << i);
    for (j = 0; j < BSP_IRQ_LINES_NUMBER; j++) {
      /*
       * Mask interrupts at i8259 level that have a lower priority
       */
      if (irqPrioTable [i] > irqPrioTable [j]) {
	* ((unsigned short*) &irq_mask_or_tbl[i]) |= (1 << j);
      }
    }
  }

  rtems_interrupt_lock_release(&rtems_i8259_access_lock, &lock_context);
}

static inline bool bsp_interrupt_vector_is_valid(rtems_vector_number vector)
{
  return BSP_i8259a_irq_valid((const rtems_irq_number) vector);
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_irq_enable_at_i8259a(vector);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_irq_disable_at_i8259a(vector);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  int i;

  /*
   * set up internal tables used by rtems interrupt prologue
   */
  compute_i8259_masks_from_prio();

  /*
   * must enable slave pic anyway
   */
  BSP_irq_enable_at_i8259a(2);

  /*
   * Probe the ELCR.
   */
  elcr_probe();

  for (i = 0; i < BSP_IRQ_LINES_NUMBER; i++)
    irq_trigger[i] = elcr_read_trigger(i);

  return RTEMS_SUCCESSFUL;
}

/*
 * Global so the asm handler can call it.
 */
void BSP_dispatch_isr(int vector);

void BSP_dispatch_isr(int vector)
{
  rtems_interrupt_lock_context lock_context;
  rtems_i8259_masks in_progress_save = 0;

  if (vector < BSP_IRQ_VECTOR_NUMBER) {
    /*
     * Hardware?
     */
    if (vector <= BSP_IRQ_MAX_ON_i8259A) {

      rtems_interrupt_lock_acquire_isr(&rtems_i8259_access_lock, &lock_context);

      /*
       * See if this is a spurious interrupt.
       */
      if ((vector == 7 || vector == 15)) {
        /*
         * Only check it there no handler for 7 or 15.
         */
        if (bsp_interrupt_handler_is_empty(vector)) {
          /*
           * Read the ISR register to see if IRQ 7/15 is really pending.
           */
          uint8_t isr = BSP_i8259a_irq_in_service_reg(PIC_MASTER_COMMAND_IO_PORT);
          if ((isr & (1 << 7)) == 0) {
            ++spurious_count;
            rtems_interrupt_lock_release_isr(&rtems_i8259_access_lock, &lock_context);
            return;
          }
        }
      }

      /*
       * Save the current cached value for the IMR. It will have the bit for this
       * vector clear.
       */
      if (vector <= BSP_IRQ_MAX_ON_i8259A) {
        in_progress_save = i8259a_in_progress;
        i8259a_in_progress |= irq_mask_or_tbl[vector];
        BSP_i8259a_irq_update_master_imr();
        BSP_i8259a_irq_update_slave_imr();
      }

      /*
       * Do not use auto-EOI as some slave PIC do not work correctly.
       */
      BSP_irq_ack_at_i8259a(vector);

      rtems_interrupt_lock_release_isr(&rtems_i8259_access_lock, &lock_context);
    }

    /*
     * Count the interrupt.
     */
    irq_count[vector]++;

    RTEMS_COMPILER_MEMORY_BARRIER();
    /*
     * Allow nesting.
     */
    __asm__ __volatile__("sti");

    bsp_interrupt_handler_dispatch(vector);

    /*
     * Disallow nesting.
     */
    __asm__ __volatile__("cli");

    RTEMS_COMPILER_MEMORY_BARRIER();

    if (vector <= BSP_IRQ_MAX_ON_i8259A) {

      rtems_interrupt_lock_acquire_isr(&rtems_i8259_access_lock, &lock_context);

      /*
       * Put the mask back but keep this vector masked if the trigger type is
       * level. The driver or a thread level interrupt server needs to enable it
       * again.
       */
      if (vector <= BSP_IRQ_MAX_ON_i8259A) {
        i8259a_in_progress = in_progress_save;
        BSP_i8259a_irq_update_master_imr();
        BSP_i8259a_irq_update_slave_imr();
      }

      rtems_interrupt_lock_release_isr(&rtems_i8259_access_lock, &lock_context);
    }
  }
}
