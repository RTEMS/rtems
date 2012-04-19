/*
 *  APBUART RASTA via PCI - driver interface
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __APBUART_RASTA_H__
#define __APBUART_RASTA_H__

#include <apbuart.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Register APBUART driver, if APBUART devices are found.
 * bus = pointer to AMBA bus description used to search for APBUART(s).
 *
 */

int apbuart_rasta_register(struct ambapp_bus *bus);

/* This function must be called on APBUART interrupt. Called from the
 * RASTA interrupt handler.
 * irq = AMBA IRQ assigned to the APBUART device, is found by reading
 *       pending register on IRQMP connected to the APBUART device.
 *
 */
void apbuartrasta_interrupt_handler(int irq, void *arg);

extern void (*apbuart_rasta_int_reg)(void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __APBUART_RASTA_H__ */
