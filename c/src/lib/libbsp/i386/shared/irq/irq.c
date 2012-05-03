/* irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (c) 2009 embedded brains GmbH
 *  Copyright (C) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* so we can see _API_extensions_Run_postswitch */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <stdlib.h>
#include <rtems/score/apiext.h>
#include <stdio.h>
#include <inttypes.h>

/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from teh prioruty table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
rtems_i8259_masks 	irq_mask_or_tbl[BSP_IRQ_LINES_NUMBER];

uint32_t            irq_count[BSP_IRQ_LINES_NUMBER] = {0};

uint32_t
BSP_irq_count_dump(FILE *f)
{
uint32_t tot = 0;
int      i;
	if ( !f )
		f = stdout;
	for ( i=0; i<BSP_IRQ_LINES_NUMBER; i++ ) {
		tot += irq_count[i];
		fprintf(f,"IRQ %2u: %9"PRIu32"\n", i, irq_count[i]);
	}
	return tot;
}

/*-------------------------------------------------------------------------+
| Cache for 1st and 2nd PIC IRQ line's status (enabled or disabled) register.
+--------------------------------------------------------------------------*/
/*
 * lower byte is interrupt mask on the master PIC.
 * while upper bits are interrupt on the slave PIC.
 * This cache is initialized in ldseg.s
 */
rtems_i8259_masks i8259s_cache = 0xFFFB;

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_disable_at_i8259s
|      Description: Mask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_cache
|        Arguments: vector_offset - number of IRQ line to mask.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
int BSP_irq_disable_at_i8259s    (const rtems_irq_number irqLine)
{
  unsigned short        mask;
  rtems_interrupt_level level;

  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_ON_i8259S )
       )
    return 1;

  rtems_interrupt_disable(level);

  mask = 1 << irqLine;
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

  return 0;
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

  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_ON_i8259S )
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

  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_ON_i8259S )
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
  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_ON_i8259S )
       )
    return 1;

  if (irqLine >= 8) {
   outport_byte(PIC_SLAVE_COMMAND_IO_PORT, PIC_EOI);
  }
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
  0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static void compute_i8259_masks_from_prio (void)
{
  rtems_interrupt_level level;
  unsigned int i;
  unsigned int j;

  rtems_interrupt_disable(level);

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

  rtems_interrupt_enable(level);
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  BSP_irq_enable_at_i8259s(vector);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  BSP_irq_disable_at_i8259s(vector);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /*
   * set up internal tables used by rtems interrupt prologue
   */
  compute_i8259_masks_from_prio();

  /*
   * must enable slave pic anyway
   */
  BSP_irq_enable_at_i8259s(2);

  return RTEMS_SUCCESSFUL;
}

void C_dispatch_isr(int vector)
{
  irq_count[vector]++;
  bsp_interrupt_handler_dispatch(vector);
}
