/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief PCI Auto Configuration Library
 */

/*
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
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

#ifndef __PCI_CFG_AUTO_H__
#define __PCI_CFG_AUTO_H__

#define CFGOPT_NOSETUP_IRQ 0x1 /* Skip IRQ setup */

/* PCI Memory Layout setup, used by the auto-config library in order to
 * determine the addresses of PCI BARs and Buses.
 *
 * All addresses are in PCI address space, the actual address the CPU access
 * may be different, and taken care of elsewhere.
 */
struct pci_auto_setup {
	int options;

	/* PCI prefetchable Memory space (OPTIONAL) */
	uint32_t mem_start;
	uint32_t mem_size; /* 0 = Use MEMIO space for prefetchable mem BARs */

	/* PCI non-prefetchable Memory */
	uint32_t memio_start;
	uint32_t memio_size;

	/* PCI I/O space (OPTIONAL) */
	uint32_t io_start;
	uint32_t io_size; /* 0 = No I/O space */

	/* Get System IRQ connected to a PCI line of a PCI device on bus0.
	 * The return IRQ value zero equals no IRQ (IRQ disabled).
	 */
	uint8_t (*irq_map)(pci_dev_t dev, int irq_pin);

	/* IRQ Bridge routing. Returns the interrupt pin (0..3 = A..D) that
	 * a device is connected to on parent bus.
	 */
	int (*irq_route)(pci_dev_t dev, int irq_pin);
};

/* Do PCI initialization: Enumrate buses, scan buses for devices, assign
 * I/O MEM and MEMIO resources, assign IRQ and so on.
 */
extern int pci_config_auto(void);

/* Register a configuration for the auto library (struct pci_auto_setup *) */
extern void pci_config_auto_register(void *config);

/* PCI memory map */
extern struct pci_auto_setup pci_auto_cfg;

#endif
