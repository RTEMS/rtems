/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Help Function, iterate over all PCI devices. Find devices by cfg access.
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

#include <pci.h>
#include <pci/access.h>

/*#define DEBUG*/

#ifdef DEBUG
#include <rtems/bspIo.h>
#define DBG(args...) printk(args)
#else
#define DBG(args...)
#endif

int pci_for_each(int (*func)(pci_dev_t, void*), void *arg)
{
	uint32_t id;
	uint8_t hd;
	int bus, dev, fun, result, fail;
	int maxbus = pci_bus_count();
	pci_dev_t pcidev;

	for (bus = 0; bus < maxbus ; bus++) {
		for (dev = 0; dev <= PCI_SLOTMAX; dev++) {
			pcidev = PCI_DEV(bus, dev, 0);

			for (fun = 0; fun <= PCI_FUNCMAX; fun++, pcidev++) {
				fail = pci_cfg_r32(pcidev, PCIR_VENDOR, &id);
				if (fail || (0xffffffff == id) || (0 == id)) {
					if (fun == 0)
						break;
					else
						continue;
				}

				DBG("pcibus_for_each: found 0x%08lx at"
				    " %d/%d/%d\n", id, bus, dev, fun);
				result = func(pcidev, arg);
				if (result != 0)
					return result; /* Stopped */

				/* Stop if not a multi-function device */
				if (fun == 0) {
					pci_cfg_r8(pcidev, PCIR_HDRTYPE,
							&hd);
					if ((hd & PCIM_MFDEV) == 0)
						break;
				}
			}
		}
	}

	return 0; /* scanned all */
}
