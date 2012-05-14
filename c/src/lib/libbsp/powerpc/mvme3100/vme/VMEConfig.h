#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H

/* mvme3100 BSP specific address space configuration parameters */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002..2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
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

/*
 * NOTE: the BSP (startup/bspstart.c) uses
 * hardcoded window lengths that match this
 * layout:
 */

#define _VME_A32_WIN0_ON_PCI	0xc0000000
#define _VME_CSR_ON_PCI			0xce000000
#define _VME_A24_ON_PCI			0xcf000000
#define _VME_A16_ON_PCI			0xcfff0000

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a run-time configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map the board RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0xc0000000

/* If your BSP requires a non-standard way to configure
 * the VME interrupt manager then define the symbol
 *
 * BSP_VME_INSTALL_IRQ_MGR
 *
 * to a proper instruction sequence that installs the
 * universe interrupt manager. This requires knowledge
 * of the wiring between the universe and the PIC (main
 * interrupt controller), i.e., which IRQ 'pins' of the
 * universe are wired to which 'lines'/inputs at the PIC.
 * (consult vmeUniverse.h for more information).
 *
 * When installing the universe IRQ manager it is also
 * possible to specify whether it should try to share
 * PIC interrupts with other sources. This might not
 * be supported by all BSPs (but the unverse driver
 * recognizes that).
 *
 * If BSP_VME_INSTALL_IRQ_MGR is undefined then
 * the default algorithm is used (vme_universe.c):
 *
 * This default setup uses only a single wire. It reads
 * the PIC 'line' from PCI configuration space and assumes
 * this to be wired to the first (LIRQ0) IRQ input at the
 * universe. The default setup tries to use interrupt
 * sharing.
 */
#define BSP_VME_INSTALL_IRQ_MGR(err)    \
	do {                                \
		err = vmeTsi148InstallIrqMgrAlt(\
			VMETSI148_IRQ_MGR_FLAG_SHARED, /* use shared IRQs */ \
			0, BSP_VME0_IRQ,            \
			1, BSP_VME1_IRQ,            \
			2, BSP_VME2_IRQ,            \
			3, BSP_VME3_IRQ,            \
			-1 /* terminate list  */    \
		);                              \
	} while (0)

/* This BSP uses the Tsi148 Driver */
#define _VME_DRIVER_TSI148

#endif
