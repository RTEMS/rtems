/*  AT697 PCI host bridge driver
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef __AT697PCI_H__
#define __AT697PCI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Register AT697 PCI host bridge driver to Driver Manager */
void at697pci_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
