#ifndef RTEMS_PCI_IO_REMAP_H
#define RTEMS_PCI_IO_REMAP_H
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
rtems_pci_io_remap(int bus_from, int bus_to, uint32_t offset);

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

#endif

