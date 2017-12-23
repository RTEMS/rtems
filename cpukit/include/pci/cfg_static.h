/* Static PCI Auto Configuration Library
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __PCI_CFG_STATIC_H__
#define __PCI_CFG_STATIC_H__

/* This function initializes all buses and device accorind to a user defined
 * "static" configuration. The configuration can manually created with C
 * data structures. Or it can be automatically created on a running target
 * using the pci_cfg_print() routine after the AUTO or READ Configuration
 * Library has setup the PCI bus
 */
extern int pci_config_static(void);

#endif
