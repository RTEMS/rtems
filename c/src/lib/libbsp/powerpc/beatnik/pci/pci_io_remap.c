/* Adjust a PCI bus range's I/O address space by adding an offset */

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
#include <bsp/pci.h>
#include <stdint.h>

#ifndef PCI_MULTI_FUN
#define PCI_MULTI_FUN 0x80
#endif

#ifndef PCI_HEADER_TYPE_MSK
#define PCI_HEADER_TYPE_MSK 0x7f
#endif

/* Reconfigure all I/O base address registers for a range of PCI busses
 * (from and including 'bus_from' up to and not including 'bus_to').
 * adding an offset. This involves adjusting the base and limit registers
 * of PCI-PCI bridges, too.
 *
 * RESTRICTIONS: 'offset' must be 4k aligned (PCI req.); no argument check
 * 			on the bus numbers is done.
 *
 * RETURNS: 0 on success and a number > 0 indicating the number of
 *          non-32bit bridges found where the offset couldn't be added.
 *          Devices behind such a bridge are not accessible through I/O
 *          and should probably be switched off (not done by this code).
 */

int
rtems_pci_io_remap(int bus_from, int bus_to, uint32_t offset)
{
int rval = 0;

int bus, dev, fun, maxf;
int bar, numBars = 0;

uint8_t			b;
uint16_t		s;
uint32_t        d;
unsigned int	bas, lim;

	if ( offset & ((1<<12)-1) ) {
		BSP_panic("rtems_pci_io_remap(): offset must be 4k aligned");
		return -1;
	}


	for ( bus=bus_from; bus < bus_to; bus++ ) {
		for ( dev = 0; dev<PCI_MAX_DEVICES; dev++ ) {

			maxf = 1;

			for ( fun = 0; fun < maxf; fun++ ) {
				pci_read_config_word( bus, dev, fun, PCI_VENDOR_ID, &s );
				if ( 0xffff == s )
					continue;

				pci_read_config_byte( bus, dev, fun, PCI_HEADER_TYPE, &b );

				/* readjust the max. function number to scan if this is a multi-function
				 * device.
				 */
				if ( 0 == fun && (PCI_MULTI_FUN & b) )
					maxf = PCI_MAX_FUNCTIONS;

				/* Check the header type; panic if unknown.
				 * header type 0 has 6 bars, header type 1 (PCI-PCI bridge) has 2
				 */
				b &= PCI_HEADER_TYPE_MSK;
				switch ( b ) {
					default:
						printk("PCI header type %i (@%i/%i/%i)\n", b, bus, dev, fun);
						BSP_panic("rtems_pci_io_remap(): unknown PCI header type");
					return -1; /* keep compiler happy */

					case PCI_HEADER_TYPE_CARDBUS:
						printk("PCI header type %i (@%i/%i/%i)\n", b, bus, dev, fun);
						BSP_panic("rtems_pci_io_remap():  don't know how to deal with Cardbus bridge");
					return -1;

					case PCI_HEADER_TYPE_NORMAL:
						numBars = 6*4;	/* loop below counts reg. offset in bytes */
					break;

					case PCI_HEADER_TYPE_BRIDGE:
						numBars = 2*4;	/* loop below counts reg. offset in bytes */
					break;

				}

				for ( bar = 0; bar < numBars; bar+=4 ) {
					pci_read_config_dword( bus, dev, fun, PCI_BASE_ADDRESS_0 + bar, &d );
					if ( PCI_BASE_ADDRESS_SPACE_IO & d ) {
						/* It's an I/O BAR; remap */
						d &= PCI_BASE_ADDRESS_IO_MASK;
						if ( d ) {
							/* IO bar was configured; add offset */
							d += offset;
							pci_write_config_dword( bus, dev, fun, PCI_BASE_ADDRESS_0 + bar, d );
						}
					} else {
						/* skip upper half of 64-bit window */
						d &= PCI_BASE_ADDRESS_MEM_TYPE_MASK;
						if ( PCI_BASE_ADDRESS_MEM_TYPE_64 == d )
							bar+=4;
					}
				}

				/* Now it's time to deal with bridges */
				if ( PCI_HEADER_TYPE_BRIDGE == b ) {
					/* must adjust the limit registers */
					pci_read_config_byte( bus, dev, fun, PCI_IO_LIMIT, &b );
					pci_read_config_word( bus, dev, fun, PCI_IO_LIMIT_UPPER16, &s );
					lim  = (s<<16) + (( b & PCI_IO_RANGE_MASK ) << 8);
					lim += offset;

					pci_read_config_byte( bus, dev, fun, PCI_IO_BASE, &b );
					pci_read_config_word( bus, dev, fun, PCI_IO_BASE_UPPER16, &s );
					bas  = (s<<16) + (( b & PCI_IO_RANGE_MASK ) << 8);
					bas += offset;

					b &= PCI_IO_RANGE_TYPE_MASK;
					switch ( b ) {
						default:
							printk("Unknown IO range type 0x%x (@%i/%i/%i)\n", b, bus, dev, fun);
							BSP_panic("rtems_pci_io_remap(): unknown IO range type");
						return -1;

						case PCI_IO_RANGE_TYPE_16:
							if ( bas > 0xffff || lim > 0xffff ) {
								printk("PCI I/O range type 1 (16bit) bridge (@%i/%i/%i) found:\n", bus, dev, fun);
								printk("WARNING: base (0x%08x) or limit (0x%08x) exceed 16-bit;\n", bas, lim);
								printk("         devices behind this bridge are NOT accessible!\n");

								/* FIXME: should we disable devices behind this bridge ? */
								bas = lim = 0;
							}
						break;

						case PCI_IO_RANGE_TYPE_32:
						break;
					}

					b = (uint8_t)((bas>>8) & PCI_IO_RANGE_MASK);
					pci_write_config_byte( bus, dev, fun, PCI_IO_BASE, b );

					s = (uint16_t)((bas>>16)&0xffff);
					pci_write_config_word( bus, dev, fun, PCI_IO_BASE_UPPER16, s);

					b = (uint8_t)((lim>>8) & PCI_IO_RANGE_MASK);
					pci_write_config_byte( bus, dev, fun, PCI_IO_LIMIT, b );
					s = (uint16_t)((lim>>16)&0xffff);
					pci_write_config_word( bus, dev, fun, PCI_IO_LIMIT_UPPER16, s );
				}
			}
		}
	}
	return rval;
}
