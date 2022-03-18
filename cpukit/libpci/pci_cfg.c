/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Configuration Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
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

#include <pci/cfg.h>

/* Number of buses. This is set from respective library */
int pci_bus_cnt = 0;

/* PCI Address assigned to BARs which failed to fit into the PCI Window or
 * is disabled by any other cause.
 */
uint32_t pci_invalid_address = 0;

/* PCI System type. Configuration Library setup this */
enum pci_system_type pci_system_type = PCI_SYSTEM_NONE;

/* PCI Endianness.
 *
 * Host driver or BSP must override this be writing here if bus is defined
 * as non-standard big-endian.
 */
int pci_endian = PCI_LITTLE_ENDIAN;

/* Configure PCI devices and bridges, and setup the RAM data structures
 * describing the PCI devices currently present in the system
 */
int pci_config_init(void)
{
	if (pci_config_lib_init)
		return pci_config_lib_init();
	else
		return 0;
}

void pci_config_register(void *config)
{
	if (pci_config_lib_register)
		pci_config_lib_register(config);
}

/* Return the number of PCI busses available in the system, note that
 * there are always one bus (bus0) after the PCI library has been
 * initialized and a driver has been registered.
 */
int pci_bus_count(void)
{
	return pci_bus_cnt;
}
