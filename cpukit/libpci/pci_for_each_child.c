/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Help function, iterate all PCI device children of PCI bus.
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

/* Iterate over all PCI devices on a bus (not child buses) and call func(),
 * iteration is stopped if a non-zero value is returned by func().
 *
 * search options: 0 (no child buses), 1 (depth first), 2 (breadth first)
 */
int pci_for_each_child(
	struct pci_bus *bus,
	int (*func)(struct pci_dev *, void *arg),
	void *arg,
	int search)
{
	struct pci_dev *dev = bus->devs;
	int ret;

	while (dev) {
		ret = func(dev, arg);
		if (ret)
			return ret;
		if (search == SEARCH_DEPTH && (dev->flags & PCI_DEV_BRIDGE)) {
			ret = pci_for_each_child((struct pci_bus *)dev,
							func, arg, search);
			if (ret)
				return ret;
		}
		dev = dev->next;
	}

	return 0;
}
