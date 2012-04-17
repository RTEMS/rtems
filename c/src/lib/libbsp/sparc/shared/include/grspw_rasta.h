 /*
 *  Macros used for RASTA PCI GRSPW controller
 *
 *  COPYRIGHT (c) 2006.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __GRSPW_RASTA_H__
#define __GRSPW_RASTA_H__

#include <grspw.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Register GRSPW Driver
 * bus = &ambapp_plb for LEON3
 *
 * Memory setup:
 * ram_base = 128k aligned pointer to memory (as the CPU sees it)
 */

int grspw_rasta_register(
 struct ambapp_bus *bus,
 unsigned int ram_base
 );

/* This function must be called on GRSPW interrupt. Called from the
 * PCI interrupt handler. irq = AMBA IRQ MASK assigned to the GRSPW device,
 * is found by reading pending register on IRQMP connected to GRSPW
 * device.
 *
 */
void grspw_rasta_interrupt_handler(unsigned int status);

/* callback to register interrupt handler */
extern void (*grspw_rasta_int_reg)(void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __GRSPW_RASTA_PCI_H__ */
