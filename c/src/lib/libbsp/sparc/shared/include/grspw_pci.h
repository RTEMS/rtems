 /*
  *  Macros used for GRSPW controller
  *
  *  COPYRIGHT (c) 2006.
  *  Gaisler Research
  *
  *  The license and distribution terms for this file may be
  *  found in the file LICENSE in this distribution or at
  *  http://www.rtems.com/license/LICENSE.
  *
  */

#ifndef __GRSPW_PCI_H__
#define __GRSPW_PCI_H__

#include <grspw.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Register GRSPW Driver
 * bus = &ambapp_plb for LEON3
 *
 * Memory setup:
 * memarea = 128k aligned pointer to memory (if zero malloc will be used) (as the CPU sees it)
 * hw_address = address that HW must use to access memarea. (used in the translation process)
 */

int grspw_pci_register (struct ambapp_bus *bus,
                        unsigned int memarea, unsigned int hw_address);


/* This function must be called on BRM interrupt. Called from the
 * PCI interrupt handler. irq = AMBA IRQ MASK assigned to the BRM device,
 * is found by reading pending register on IRQMP connected to BRM
 * device.
 *
 * Return 0=not handled. nono-zero=handled
 */
unsigned int grspw_pci_interrupt_handler (int irq, void *arg);

extern void (*grspw_pci_int_reg) (void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __GRSPW_PCI_H__ */
