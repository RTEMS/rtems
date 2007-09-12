/**
 *  @file irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 */
/*
 *  RTEMS GBA BSP
 *
 *  CopyRight (C) 2000 Canon Research Centre France SA.
 *      Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <stdint.h>
#include <bsp.h>
#include <irq.h>
#include <rtems/bspIo.h>

/**  default int vector */
extern void _ISR_Handler(void);

/** max number of vectors, defined in linkcmds  */
extern  void _irq_max_vector;

/**
 *  @brief default_int_handler BSP routine is default int_handler
 *
 *  @param  None
 *  @return None
 */
void default_int_handler(void)
{
    printk("raw_idt_notify has been called \n");
}

/**
 *  @brief rtems_irq_mngt_init BSP routine initialize rtems_irq_mngt
 *
 *  @param  None
 *  @return None
 */
void  rtems_irq_mngt_init(void)
{
    int   i;
    uint32_t              *vectorTable;
    rtems_interrupt_level  level;

    vectorTable = (uint32_t *)VECTOR_TABLE;

    rtems_interrupt_disable(level);

    /* @todo Can't use exception vectors in GBA because they are already in GBA ROM BIOS */
    /* First, connect the ISR_Handler for IRQ and FIQ interrupts */
    /*_CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ISR_Handler, NULL);*/
    /*_CPU_ISR_install_vector(ARM_EXCEPTION_FIQ, _ISR_Handler, NULL);*/

    /* Initialize the vector table contents with default handler */
    for (i=0 ; i < (uint32_t)&_irq_max_vector ; i++) {
       *(vectorTable + i) = (uint32_t)(default_int_handler);
    }
    /* Initialize the INT at the BSP level */
    BSP_rtems_irq_mngt_init();
}

