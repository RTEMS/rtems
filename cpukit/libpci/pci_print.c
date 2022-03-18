/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Print Current Configuration To Terminal
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

#include <inttypes.h>
#include <stdio.h>
#include <pci.h>
#include <pci/access.h>

/* PCI Access Library shortcuts */
#define PCI_CFG_R8(dev, args...) pci_cfg_r8(dev, args)
#define PCI_CFG_R16(dev, args...) pci_cfg_r16(dev, args)
#define PCI_CFG_R32(dev, args...) pci_cfg_r32(dev, args)
#define PCI_CFG_W8(dev, args...) pci_cfg_w8(dev, args)
#define PCI_CFG_W16(dev, args...) pci_cfg_w16(dev, args)
#define PCI_CFG_W32(dev, args...) pci_cfg_w32(dev, args)

void pci_print_dev(pci_dev_t dev)
{
	int maxbars, pos, romadrs;
	uint32_t tmp, tmp2, id;
	uint16_t irq;
	uint8_t irq_pin;
	char *str, *str2;
	uint32_t base, limit;

	maxbars = 6;
	romadrs = 0x30;
	str = "";
	PCI_CFG_R32(dev, PCIR_REVID, &tmp);
	tmp >>= 16;
	if (tmp == PCID_PCI2PCI_BRIDGE) {
		maxbars = 2;
		romadrs = 0x38;
		str = "(BRIDGE)";
	}

	PCI_CFG_R32(dev, PCIR_VENDOR, &id);
	printf("\nBus %x Slot %x function: %x [0x%x] %s\n",
		PCI_DEV_EXPAND(dev), dev, str);
	printf("\tVendor id: 0x%" PRIx32 ", device id: 0x%" PRIx32 "\n",
		id & 0xffff, id >> 16);
	if (maxbars == 2) {
		PCI_CFG_R32(dev, PCIR_PRIBUS_1, &tmp);
		printf("\tPrimary: %" PRIx32 "  Secondary: %" PRIx32
			"  Subordinate: %" PRIx32 "\n",
			tmp & 0xff, (tmp >> 8) & 0xff, (tmp >> 16) & 0xff);
	}

	PCI_CFG_R16(dev, PCIR_INTLINE, &irq);
	irq_pin = irq >> 8;
	if ((irq_pin > 0) && (irq_pin < 5))
		printf("\tIRQ INT%c#  LINE: %d\n",
			(irq_pin - 1) + 'A', (irq & 0xff));

	/* Print standard BARs */
	for (pos = 0; pos < maxbars; pos++) {
		PCI_CFG_R32(dev, PCIR_BAR(0) + pos*4, &tmp);
		PCI_CFG_W32(dev, PCIR_BAR(0) + pos*4, 0xffffffff);
		PCI_CFG_R32(dev, PCIR_BAR(0) + pos*4, &tmp2);
		PCI_CFG_W32(dev, PCIR_BAR(0) + pos*4, tmp);

		if (tmp2 != 0 && tmp2 != 0xffffffff && ((tmp2 & 0x1) ||
		    ((tmp2 & 0x6) == 0))) {
			uint32_t mask = ~0xf;
			if ((tmp2 & 0x1) == 1) {
				/* I/O Bar */
				mask = ~3;
				tmp2 = tmp2 | 0xffffff00;
			}
			tmp2 &= mask;
			tmp2 = ~tmp2+1; /* Size of BAR */
			if (tmp2 < 0x1000) {
				str = "B";
			} else if (tmp2 < 0x100000) {
				str = "kB";
				tmp2 = tmp2 / 1024;
			} else {
				str = "MB";
				tmp2 = tmp2 / (1024*1024);
			}
			printf("\tBAR %d: %" PRIx32 " [%" PRIu32 "%s]\n",
				pos, tmp, tmp2, str);
		}
	}

	/* Print ROM BARs */
	PCI_CFG_R32(dev, romadrs, &tmp);
	PCI_CFG_W32(dev, romadrs, 0xffffffff);
	PCI_CFG_R32(dev, romadrs, &tmp2);
	PCI_CFG_W32(dev, romadrs, tmp);
	if (tmp2 & 1) {
		/* ROM BAR available */
		tmp2 &= PCIM_BIOS_ADDR_MASK;
		tmp2 = (~tmp2 + 1); /* Size of BAR */
		if (tmp2 < 0x1000) {
			str = "B";
		} else if (tmp2 < 0x100000) {
			str = "kB";
			tmp2 = tmp2 / 1024;
		} else {
			str = "MB";
			tmp2 = tmp2 / (1024*1024);
		}
		str2 = tmp & 1 ? "ENABLED" : "DISABLED";
		printf("\tROM:   %08" PRIx32 " [%" PRIu32 "%s] (%s)\n",
			tmp, tmp2, str, str2);
	}

	/* Print Bridge addresses */
	if (maxbars == 2) {
		tmp = 0;
		PCI_CFG_R32(dev, 0x1C, &tmp);
		if (tmp != 0) {
			base = (tmp & 0x00f0) << 8;
			limit = (tmp & 0xf000) | 0xfff;
			PCI_CFG_R32(dev, 0x30, &tmp);
			base |= (tmp & 0xffff) << 16;
			limit |= (tmp & 0xffff0000);
			str = "ENABLED";
			if (limit < base)
				str = "DISABLED";
			printf("\tI/O:   BASE: 0x%08" PRIx32 ", LIMIT: 0x%08"
				PRIx32 " (%s)\n", base, limit, str);
		}

		PCI_CFG_R32(dev, 0x20, &tmp);
		if (tmp != 0) {
			base = (tmp & 0xfff0) << 16;
			limit = (tmp & 0xfff00000) | 0xfffff;
			str = "ENABLED";
			if (limit < base)
				str = "DISABLED";
			printf("\tMEMIO: BASE: 0x%08" PRIx32 ", LIMIT: 0x%08"
				PRIx32 " (%s)\n", base, limit, str);
		}

		PCI_CFG_R32(dev, 0x24, &tmp);
		if (tmp != 0) {
			base = (tmp & 0xfff0) << 16;
			limit = (tmp & 0xfff00000) | 0xfffff;
			str = "ENABLED";
			if (limit < base)
				str = "DISABLED";
			printf("\tMEM:   BASE: 0x%08" PRIx32 ", LIMIT: 0x%08"
				PRIx32 " (%s)\n", base, limit, str);
		}
	}
	printf("\n");
}

void pci_print_device(int bus, int slot, int function)
{
	pci_print_dev(PCI_DEV(bus, slot, function));
}

void pci_print(void)
{
    int fail, bus, slot, func;
    pci_dev_t dev;
    uint8_t header;
    uint32_t id;

    printf("\nPCI devices found and configured:\n");
    for (bus = 0; bus < pci_bus_count(); bus++) {
        for (slot = 0; slot <= PCI_SLOTMAX; slot++) {
            for (func=0; func <= PCI_FUNCMAX; func++) {

                dev = PCI_DEV(bus, slot, func);
                fail = PCI_CFG_R32(dev, PCIR_VENDOR, &id);

                if (!fail && id != PCI_INVALID_VENDORDEVICEID && id != 0) {
	               	pci_print_dev(dev);

        	        /* Stop if not a multi-function device */
                	if (func == 0) {
	                    PCI_CFG_R8(dev, PCIR_HDRTYPE, &header);
        	            if ((header & PCIM_MFDEV) == 0)
                	        break;
	                }
		} else if (func == 0)
			break;
            }
        }
    }
    printf("\n");
}
