/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief PCI Library
 *
 * Defines in this file was taken from FreeBSD and auto-generated
 * pci_ids.h reused from RTEMS.
 *
 */

/*
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PCI_H__
#define __PCI_H__

#include <pci/pcireg.h>
#include <pci/ids.h>

#define PCI_INVALID_VENDORDEVICEID    0xffffffff

#define PCID_CLASS(class, dev) ((class << 8) | dev)
#define PCID_PCI2PCI_BRIDGE PCID_CLASS(PCIC_BRIDGE, PCIS_BRIDGE_PCI)

#include <pci/access.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The PCI Library have the following build time configuration options. It is
 * up to the BSP header file (bsp.h) to set options properly.
 *
 * BSP_PCI_BIG_ENDIAN    - Access inline routines will be for a big-endian PCI
 *                         bus, if not defined the routines will assume that
 *                         PCI is as the standard defines: little-endian.
 *
 *                         Note that drivers may be run-time configurable,
 *                         meaning that they may adopt to either big-endian or
 *                         little-endian PCI bus, the host driver or BSP may
 *                         detect endianness during run-time.
 */

/* Error return values */
enum {
	PCISTS_ERR         = -1, /* Undefined Error */
	PCISTS_OK          = 0,
	PCISTS_EINVAL      = 1, /* Bad input arguments */
	PCISTS_MSTABRT     = 2, /* CFG space access error (can be ignored) */
};

/* PCI System type can be used to determine system for drivers. Normally
 * the system is Host, but the peripheral configuration library also supports
 * being PCI peripheral not allowed to access configuration space.
 *
 * The active configuration Library set this variable.
 */
enum pci_system_type {
	PCI_SYSTEM_NONE = 0,
	PCI_SYSTEM_HOST = 1,
	PCI_SYSTEM_PERIPHERAL = 2,
};
extern enum pci_system_type pci_system_type;

/* PCI Bus Endianness. The PCI specification is little endian, however on some
 * embedded systems (AT697-LEON2 for example) the PCI bus is defined as big
 * endian (non-standard) in order to avoid byte-twisting.
 */
enum {
	PCI_LITTLE_ENDIAN = 0,
	PCI_BIG_ENDIAN = 1,
};
extern int pci_endian;

/* Return the number of PCI busses in the system */
extern int pci_bus_count(void);

/* Scan the PCI bus and print the PCI device/functions/bridges and their
 * current resources and size to the system console.
 */
extern void pci_print(void);

/* Print current configuration of a single PCI device by reading PCI
 * configuration space
 */
extern void pci_print_dev(pci_dev_t dev);
extern void pci_print_device(int bus, int slot, int function);

/*** PCI Configuration Space direct access routines ***/

/* Function iterates over all PCI buses/devices/functions and calls
 * func(PCIDEV,arg) for each present device. The iteration is stopped if
 * func() returns non-zero result the same result is returned. As long
 * as func() returns zero the function will keep on iterating, when all
 * devices has been processed the function return zero.
 *
 * The function iterates over all devices/functions on all buses by accessing
 * configuration space directly (PCI RAM data structures not used). This
 * function is valid to call after PCI buses have been enumrated.
 */
extern int pci_for_each(int (*func)(pci_dev_t, void*), void *arg);

/* Get PCI Configuration space BUS|SLOT|FUNC for a device matching PCI
 * Vendor, Device and instance number 'index'.
 *
 * Return Values
 * -1  pci_find_dev did not find a device matching the criterion.
 *  0  device was found, *pdev was updated with the device's BUS|SLOT|FUNC
 */
extern int pci_find(uint16_t ven, uint16_t dev, int index, pci_dev_t *pdev);

#ifdef __cplusplus
}
#endif

#endif /* __PCI_H__ */
