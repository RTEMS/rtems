/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Help function, Find a PCI device by VENDOR/DEVICE ID
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
#include <pci/cfg.h>

struct compare_info {
	int index;
	uint16_t vendor;
	uint16_t device;
};

static int compare_dev_id(struct pci_dev *dev, void *arg)
{
	struct compare_info *info = arg;

	if ((dev->vendor != info->vendor) || (dev->device != info->device))
		return 0;
	if (info->index-- == 0)
		return (int)dev;
	else
		return 0;
}

/* Find a Device in PCI device tree located in RAM */
int pci_find_dev(uint16_t ven, uint16_t dev, int index, struct pci_dev **ppdev)
{
	struct compare_info info;
	int result;

	info.index = index;
	info.vendor = ven;
	info.device = dev;

	result = pci_for_each_dev(compare_dev_id, &info);
	if (ppdev)
		*ppdev = (struct pci_dev *)result;
	if (result == 0)
		return -1;
	else
		return 0;
}
