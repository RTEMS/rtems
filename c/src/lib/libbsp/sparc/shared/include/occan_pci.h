 /*
 *  OC_CAN controller via PCI - driver interface
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __OCCAN_PCI_H__
#define __OCCAN_PCI_H__

#include <occan.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Register OC_CAN driver
 * bus = pointer to AMBA bus description used to search for OC_CAN contrller(s).
 */

int occan_pci_register(struct ambapp_bus *bus);

/* This function must be called on OC_CAN interrupt. Called from the
 * PCI interrupt handler. irq = AMBA IRQ assigned to the OC_CAN device,
 * is found by reading pending register on IRQMP connected to the OC_CAN
 * device.
 *
 */
void occanpci_interrupt_handler(int irq, void *arg);

extern void (*occan_pci_int_reg)(void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __OCCAN_PCI_H__ */
