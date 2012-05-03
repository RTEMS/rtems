#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H

/* BSP specific address space configuration parameters */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002,
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
 * The BSP maps VME address ranges into
 * one BAT.
 * NOTE: the BSP (startup/bspstart.c) uses
 * hardcoded window lengths that match this
 * layout:
 *
 * BSP_VME_BAT_IDX defines
 * which BAT to use for mapping the VME bus.
 * If this is undefined, no extra BAT will be
 * configured and VME has to share the available
 * PCI address space with PCI devices.
 */
#undef   BSP_VME_BAT_IDX

#define _VME_A32_WIN0_ON_PCI	0x90000000
#define _VME_A24_ON_PCI		0x9f000000
#define _VME_A16_ON_PCI		0x9fff0000

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map our RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#undef  _VME_DRAM_OFFSET
#define _VME_DRAM_OFFSET                0xc0000000
#define _VME_DRAM_32_OFFSET1		0x20000000
#define _VME_DRAM_32_OFFSET2            0x20b00000
#define _VME_DRAM_24_OFFSET1            0x00000000
#define _VME_DRAM_24_OFFSET2            0x00100000
#define _VME_DRAM_16_OFFSET1            0x00000000
#define _VME_DRAM_16_OFFSET2            0x00008000

#define _VME_A24_SIZE                   0x00100000
#define _VME_A16_SIZE                   0x00008000

#undef _VME_CSR_ON_PCI

/* Tell the interrupt manager that the universe driver
 * already called openpic_eoi() and that this step hence
 * must be omitted.
 */

#define BSP_PCI_VME_DRIVER_DOES_EOI

/* don't reference vmeUniverse0PciIrqLine directly here - leave it up to
 * bspstart() to set BSP_vme_bridge_irq. That way, we can generate variants
 * of the BSP with / without the universe driver...
 */
extern int _BSP_vme_bridge_irq;

extern int BSP_VMEInit();
extern int BSP_VMEIrqMgrInstall();

#define BSP_VME_UNIVERSE_INSTALL_IRQ_MGR(err)					\
	do {														\
  		err = vmeUniverseInstallIrqMgr(0,5,1,6);				\
	} while (0)

#endif
