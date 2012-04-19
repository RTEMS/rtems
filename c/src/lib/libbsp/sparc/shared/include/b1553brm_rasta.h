 /*
 *  Macros used for brm controller
 *
 *  COPYRIGHT (c) 2006.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __B1553BRM_RASTA_H__
#define __B1553BRM_RASTA_H__

#include <b1553brm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Register BRM driver
 * See (struct brm_reg).w_ctrl for clksel and clkdiv.
 * See Enhanced register (the least signinficant 2 bits) in BRM Core for brm_freq
 * bus = &ambapp_plb for LEON3. (LEON2 not yet supported for this driver)
 *
 * Memory setup:
 * memarea = 128k aligned pointer to memory (if zero malloc will be used) (as the CPU sees it)
 * hw_address = address that HW must use to access memarea. (used in the translation process)
 */

int b1553brm_rasta_register(
 struct ambapp_bus *bus,
 unsigned int clksel,
 unsigned int clkdiv,
 unsigned int brm_freq,
 unsigned int memarea,
 unsigned int hw_address
 );


/* This function must be called on BRM interrupt. Called from the
 * PCI interrupt handler. irq = AMBA IRQ MASK assigned to the BRM device,
 * is found by reading pending register on IRQMP connected to BRM
 * device.
 *
 * Return 0=not handled. nono-zero=handled
 */
int b1553brm_rasta_interrupt_handler(int irq, void *arg);

extern void (*b1553brm_rasta_int_reg)(void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __B1553BRM_RASTA_H__ */
