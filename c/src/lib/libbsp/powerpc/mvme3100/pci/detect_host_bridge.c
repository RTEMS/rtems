/* PCI Initialization */

/*
 * Authorship
 * ----------
 * This software ('mvme3100' RTEMS BSP) was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mvme3100' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
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

    printk("%3d:0x%02x:%d    0x%04x-0x%04x:  0x%04x 0x%04x 0x%08x 0x%08x       %d -> %3d (=0x%02x)\n",
		bus, dev, fun, vi, di, cd, st, b1, b2, ip, il, il);
	return 0;
}

void
BSP_pciConfigDump_early(void)
{
	printk("BUS:SLOT:FUN  VENDOR-DEV_ID: COMMAND STATUS BASE_ADDR0 BASE_ADDR1 IRQ_PIN -> IRQ_LINE\n");
	BSP_pciScan(0, dump_dev_cb, 0);
}
