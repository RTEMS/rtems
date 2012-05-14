#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H

/* BSP specific address space configuration parameters */

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

#define _VME_DRIVER_TSI148
#define _VME_DRIVER_UNIVERSE

/* 
 * NOTE: the BSP (startup/bspstart.c) uses
 * hardcoded window lengths that match this
 * layout when setting BATs:
 */
#define _VME_A32_WIN0_ON_PCI	0x90000000
/* If _VME_CSR_ON_PCI is defined then the A32 window is reduced to accommodate
 * CSR for space.
 */
#define _VME_CSR_ON_PCI			0x9e000000
#define _VME_A24_ON_PCI			0x9f000000
#define _VME_A16_ON_PCI			0x9fff0000

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map our RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0x90000000

#define BSP_VME_INSTALL_IRQ_MGR(err)	\
  do {									\
  err = -1;								\
  switch (BSP_getBoardType()) {			\
	case MVME6100:						\
		err = theOps->install_irq_mgr(	\
					VMETSI148_IRQ_MGR_FLAG_SHARED,	\
					0, BSP_IRQ_GPP_0 + 20,			\
					1, BSP_IRQ_GPP_0 + 21,			\
					2, BSP_IRQ_GPP_0 + 22,			\
					3, BSP_IRQ_GPP_0 + 23,			\
					-1);				\
	break;								\
										\
	case MVME5500:						\
		err = theOps->install_irq_mgr(	\
					VMEUNIVERSE_IRQ_MGR_FLAG_SHARED |			\
					VMEUNIVERSE_IRQ_MGR_FLAG_PW_WORKAROUND,		\
					0, BSP_IRQ_GPP_0 + 12,						\
					1, BSP_IRQ_GPP_0 + 13,						\
					2, BSP_IRQ_GPP_0 + 14,						\
					3, BSP_IRQ_GPP_0 + 15,						\
					-1);										\
	break;								\
										\
	default:							\
		printk("WARNING: unknown board; ");						\
	break;								\
  }										\
  if ( err )							\
	printk("VME interrupt manager NOT INSTALLED (error: %i)\n", err); \
  } while (0)

#endif
