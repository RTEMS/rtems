/* remap the zero-based PCI IO spaces of both hoses to a single
 * address space
 *
 * This must be called AFTER to BSP_pci_initialize()
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
#include <bsp.h>
#include <libcpu/io.h>
#include <bsp/pci.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <bsp/gtreg.h>
#include "pci_io_remap.h"

static int
fixup_irq_line(int bus, int slot, int fun, void *uarg)
{
unsigned char line;
	pci_read_config_byte( bus, slot, fun, PCI_INTERRUPT_LINE, &line);
	if ( line < BSP_IRQ_GPP_0 ) {
		pci_write_config_byte( bus, slot, fun, PCI_INTERRUPT_LINE, line + BSP_IRQ_GPP_0 );
	}

	return 0;
}

void BSP_motload_pci_fixup(void)
{
uint32_t	b0,b1,r0,r1,lim,dis;

	/* MotLoad on the mvme5500 and mvme6100 configures the PCI
	 * busses nicely, i.e., the values read from the memory address
	 * space BARs by means of PCI config cycles directly reflect the
	 * CPU memory map. Thus, the presence of two hoses is already hidden.
	 *
	 * Unfortunately, all PCI I/O addresses are 'zero-based' i.e.,
	 * a hose-specific base address would have to be added to
	 * the values read from config space.
	 *
	 * We fix this here so I/O BARs also reflect the CPU memory map.
	 *
	 * Furthermore, the mvme5500 uses
	 *    f000.0000
	 *  ..f07f.ffff  for PCI-0 / hose0
	 *
	 *  and
	 *
	 *    f080.0000
	 *  ..f0ff.0000  for PCI-1 / hose 0
	 *
	 *  whereas the mvme6100 does it the other way round...
	 */

	b0 = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_Low_Decode) );
	b1 = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_Low_Decode) );

	r0 = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_Remap) );
	r1 = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_Remap) );

	switch ( BSP_getDiscoveryVersion(0) ) {
		case MV_64360:
			/* In case of the MV64360 the 'limit' is actually a 'size'!
			 * Disable by setting special bits in the 'BAR disable reg'.
			 */
			dis = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + MV_64360_BASE_ADDR_DISBL) );
			/* disable PCI0 I/O and PCI1 I/O */
			out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + MV_64360_BASE_ADDR_DISBL), dis | (1<<9) | (1<<14) );
			/* remap busses on hose 0; if the remap register was already set, assume
			 * that someone else [such as the bootloader] already performed the fixup
			 */
			if ( (b0 & 0xffff) && 0 == (r0 & 0xffff) ) {
				rtems_pci_io_remap( 0, BSP_pci_hose1_bus_base, (b0 & 0xffff)<<16 );
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_Remap), (b0 & 0xffff) );
			}

			/* remap busses on hose 1 */
			if ( (b1 & 0xffff) && 0 == (r1 & 0xffff) ) {
				rtems_pci_io_remap( BSP_pci_hose1_bus_base, pci_bus_count(), (b1 & 0xffff)<<16 );
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_Remap), (b1 & 0xffff) );
			}

			/* re-enable */
			out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + MV_64360_BASE_ADDR_DISBL), dis );
		break;

		case GT_64260_A:
		case GT_64260_B:
			
			if ( (b0 & 0xfff) && 0 == (r0 & 0xfff) ) { /* base are only 12 bits */
				/* switch window off by setting the limit < base */
				lim = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_High_Decode) );
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_High_Decode), 0 );
				/* remap busses on hose 0 */
				rtems_pci_io_remap( 0, BSP_pci_hose1_bus_base, (b0 & 0xfff)<<20 );

				/* BTW: it seems that writing the base register also copies the
				 * value into the 'remap' register automatically (??)
				 */
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_Remap), (b0 & 0xfff) );

				/* re-enable */
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI0_IO_High_Decode), lim );
			}

			if ( (b1 & 0xfff) && 0 == (r1 & 0xfff) ) { /* base are only 12 bits */
				/* switch window off by setting the limit < base */
				lim = in_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_High_Decode) );
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_High_Decode), 0 );

				/* remap busses on hose 1 */
				rtems_pci_io_remap( BSP_pci_hose1_bus_base, pci_bus_count(), (b1 & 0xfff)<<20 );

				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_Remap), (b1 & 0xfff) );

				/* re-enable */
				out_le32( (volatile unsigned*)(BSP_MV64x60_BASE + GT_PCI1_IO_High_Decode), lim );
			}
		break;

		default:
			BSP_panic("Unknown discovery version; switch in file: "__FILE__" not implemented (yet)");
		break; /* never get here */
	}

	/* Fixup the IRQ lines; the mvme6100 maps them nicely into our scheme, i.e., GPP
	 * interrupts start at 64 upwards
	 *
	 * The mvme5500 is apparently initialized differently :-(. GPP interrupts start at 0
	 * Since all PCI interrupts are wired to GPP we simply check for a value < 64 and
	 * reprogram the interrupt line register.
	 */
	BSP_pciScan(0, fixup_irq_line, 0);
}


