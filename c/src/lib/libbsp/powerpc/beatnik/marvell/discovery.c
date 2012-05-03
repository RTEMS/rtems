/* 
 * Acknowledgements:
 * Valuable information was obtained from the following drivers
 *   netbsd: (C) Allegro Networks Inc; Wasabi Systems Inc.
 *   linux:  (C) MontaVista, Software, Inc; Chris Zankel, Mark A. Greer.
 *   rtems:  (C) Brookhaven National Laboratory; K. Feng
 * but this implementation is original work by the author.
 */

/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
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

#include <rtems.h>
#include <rtems/bspIo.h>
#include <bsp.h>
#include <bsp/gtreg.h>
#include <bsp/pci.h>

#ifndef PCI_VENDOR_ID_MARVELL
#define PCI_VENDOR_ID_MARVELL 0x11ab
#endif

#ifndef PCI_DEVICE_ID_MARVELL_GT64260
#define PCI_DEVICE_ID_MARVELL_GT64260 0x6430
#endif

#ifndef PCI_DEVICE_ID_MARVELL_MV64360
#define PCI_DEVICE_ID_MARVELL_MV64360 0x6460
#endif

#if 0
#define MV64x60_PCI0_CONFIG_ADDR	(BSP_MV64x60_BASE + 0xcf8)
#define MV64x60_PCI0_CONFIG_DATA	(BSP_MV64x60_BASE + 0xcfc)

/* read from bus/slot/fn 0/0/0 */
static unsigned long
pci_early_config_read(int offset, int width)
{
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(0<<8)|(PCI_DEVFN(0,0)<<16)|((offset&~3)<<24));
	switch (width) {
		default:
		case 1:
			return in_8((unsigned char*)pci.pci_config_data + (offset&3));
		case 2:
			return in_le16((unsigned short*)pci.pci_config_data + (offset&3));
		case 4:
			return in_le32((unsigned long *)pci.pci_config_data + (offset&3));
	}
}
#endif

DiscoveryVersion
BSP_getDiscoveryVersion(int assertion)
{
static DiscoveryVersion rval = unknown;

	if ( unknown ==rval ) {
		unsigned char	dc;
		unsigned short	ds;
		/* this must work before and after the call to BSP_pciInitialize() --
		 * since the host bridge is at 0,0,0 it doesn't matter if the hosed
		 * access methods are installed or not (as a matter of fact this shouldn't
		 * matter for any device on hose 0)
		 */
printk("config addr is 0x%08x\n", BSP_pci_configuration.pci_config_addr);
printk("config data is 0x%08x\n", BSP_pci_configuration.pci_config_data);
		pci_read_config_word(0,0,0,PCI_VENDOR_ID, &ds);
		if ( PCI_VENDOR_ID_MARVELL != ds ) {
			if ( assertion ) {
				printk("Host bridge vendor id: 0x%04x\n",ds);
				BSP_panic("Host bridge vendor @ pci(0,0,0) is not MARVELL");
			}
			else return unknown;
		}
		pci_read_config_word(0,0,0,PCI_DEVICE_ID, &ds);
		pci_read_config_byte(0,0,0,PCI_REVISION_ID, &dc);
		switch (ds) {
			case PCI_DEVICE_ID_MARVELL_MV64360:
				rval = MV_64360;
			break;

			case PCI_DEVICE_ID_MARVELL_GT64260:
				switch (dc) {
					default:
					break;

					case 0x10:
					return (rval = GT_64260_A);

					case 0x20:
					return (rval = GT_64260_B);
				}

			default:
				if ( assertion ) {
					printk("Marvell device id 0x%04x, revision 0x%02x; check %s:%u\n",
							ds, dc,
							__FILE__,__LINE__);
					BSP_panic("Unknown Marvell bridge or revision@ pci(0,0,0) is not MARVELL");
				}
			break;
		}
	}

	return rval;
}
