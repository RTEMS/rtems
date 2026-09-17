/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME3100
 *
 * @brief PCI initialization.
 */

/*
 * Copyright (C) 2005 - 2007 Till Straumann <strauman@slac.stanford.edu>
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

#include <bsp.h>
#include <bsp/pci.h>
#include <bsp/irq.h>
#include <bsp/openpic.h>
#include <inttypes.h>

/* Motload configures PCI interrupts to start at 16 and up but
 * we'd rather have them starting at 0.
 * Use this callback to fix them up.
 */
static int
fixup_irq_line(int bus, int slot, int fun, void *uarg)
{
	(void) uarg;

unsigned char line;
	pci_read_config_byte( bus, slot, fun, PCI_INTERRUPT_LINE, &line);
	if ( line >= BSP_EXT_IRQ_NUMBER ) {
		pci_write_config_byte( bus, slot, fun, PCI_INTERRUPT_LINE, line - BSP_EXT_IRQ_NUMBER );
	}

	return 0;
}

void BSP_motload_pci_fixup(void)
{
  BSP_pciScan(0, fixup_irq_line, 0);
}

void detect_host_bridge(void)
{
  OpenPIC = (volatile struct OpenPIC *) (BSP_8540_CCSR_BASE + BSP_OPEN_PIC_BASE_OFFSET);
}

static int
dump_dev_cb(
   int bus,
   int dev,
   int fun,
   void *uarg
)
{
  (void) uarg;

  uint16_t vi,di;
  uint16_t cd,st;
  uint32_t b1,b2;
  uint8_t  il,ip;

	pci_read_config_word (bus, dev, fun, PCI_VENDOR_ID,      &vi);
	pci_read_config_word (bus, dev, fun, PCI_DEVICE_ID,      &di);
	pci_read_config_word (bus, dev, fun, PCI_COMMAND,        &cd);
	pci_read_config_word (bus, dev, fun, PCI_STATUS,         &st);
	pci_read_config_dword(bus, dev, fun, PCI_BASE_ADDRESS_0, &b1);
	pci_read_config_dword(bus, dev, fun, PCI_BASE_ADDRESS_1, &b2);
	pci_read_config_byte (bus, dev, fun, PCI_INTERRUPT_LINE, &il);
	pci_read_config_byte (bus, dev, fun, PCI_INTERRUPT_PIN,  &ip);

    printk("%3d:0x%02x:%d    0x%04x-0x%04x:  0x%04x 0x%04x 0x%08" PRIx32 " 0x%08" PRIx32 "       %d -> %3d (=0x%02x)\n",
		bus, dev, fun, vi, di, cd, st, b1, b2, ip, il, il);
	return 0;
}

void
BSP_pciConfigDump_early(void)
{
	printk("BUS:SLOT:FUN  VENDOR-DEV_ID: COMMAND STATUS BASE_ADDR0 BASE_ADDR1 IRQ_PIN -> IRQ_LINE\n");
	BSP_pciScan(0, dump_dev_cb, 0);
}
