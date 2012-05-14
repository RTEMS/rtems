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
 *
 * If you do define BSP_VME_BAT_IDX you must
 * make sure the corresponding BAT is really
 * available and unused!
 */

#if defined(mvme2100)
#define _VME_A32_WIN0_ON_PCI	0x90000000
#define _VME_A24_ON_PCI			0x9f000000
#define _VME_A16_ON_PCI			0x9fff0000
#define BSP_VME_BAT_IDX			1
#else
#define _VME_A32_WIN0_ON_PCI	0x10000000
#define _VME_A24_ON_PCI			0x1f000000
#define _VME_A16_ON_PCI			0x1fff0000
#define BSP_VME_BAT_IDX			0
#endif

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a run-time configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map the board RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0xc0000000

/* Define BSP_PCI_VME_DRIVER_DOES_EOI to let the vmeUniverse
 * driver (Tsi148 driver doesn't implement this) implement
 * VME IRQ priorities in software.
 *
 * Here's how this works:
 *
 *    1) VME IRQ happens
 *    2) universe propagates IRQ to PCI/PPC/main interrupt
 *       controller ('PIC' - programmable interrupt controller).
 *    3) PIC driver dispatches universe driver's ISR
 *    4) universe driver ISR acknowledges IRQ on VME,
 *       determines VME vector.
 * ++++++++++++ stuff between ++ signs is related to SW priorities +++++++++
 *    5) universe driver *masks* all VME IRQ levels <= interrupting
 *       level.
 *    6) universe driver calls PIC driver's 'EOI' routine.
 *       This effectively re-enables PCI and hence higher
 *       level VME interrupts.
 *    7) universe driver dispatches user VME ISR.
 *
 * ++>> HIGHER PRIORITY VME IRQ COULD HAPPEN HERE and would be handled <<++
 *
 *    8) user ISR returns, universe driver re-enables lower
 *       level VME interrupts, returns.
 *    9) universe driver ISR returns control to PIC driver
 *   10) PIC driver *omits* regular EOI sequence since this
 *       was already done by universe driver (step 6).
 * ++++++++++++ end of special handling (SW priorities) ++++++++++++++++++++
 *   11) PIC driver ISR dispatcher returns.
 *
 * Note that the BSP *MUST* provide the following hooks
 * in order for this to work:
 *   a) bsp.h must define the symbol BSP_PIC_DO_EOI to
 *      a sequence of instructions that terminates an
 *      interrupt at the interrupt controller.
 *   b) The interrupt controller driver must check the
 *      interrupt source and *must omit* running the EOI
 *      sequence if the interrupt source is the vmeUniverse
 *      (because the universe driver already ran BSP_PIC_DO_EOI)
 *      The interrupt controller must define the variable
 *
 *          int _BSP_vme_bridge_irq = -1;
 *
 *      which is assigned the universe's interrupt line information
 *      by vme_universe.c:BSP_VMEIrqMgrInstall(). The interrupt
 *      controller driver may use this variable to determine
 *      if an IRQ was caused by the universe.
 *
 *   c) define BSP_PCI_VME_DRIVER_DOES_EOI
 *
 *  NOTE: If a) and b) are not implemented by the BSP
 *        BSP_PCI_VME_DRIVER_DOES_EOI must be *undefined*.
 */
#define BSP_PCI_VME_DRIVER_DOES_EOI

#ifdef BSP_PCI_VME_DRIVER_DOES_EOI
/* don't reference vmeUniverse0PciIrqLine directly from the irq
 * controller driver - leave it up to BSP_VMEIrqMgrInstall() to
 * set _BSP_vme_bridge_irq. That way, we can avoid linking
 * the universe driver if VME is unused...
 */
extern int _BSP_vme_bridge_irq;
#endif

/* If your BSP requires a non-standard way to configure
 * the VME interrupt manager then define the symbol
 *
 * BSP_VME_UNIVERSE_INSTALL_IRQ_MGR
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
 * If BSP_VME_UNIVERSE_INSTALL_IRQ_MGR is undefined then
 * the default algorithm is used (vme_universe.c):
 *
 * This default setup uses only a single wire. It reads
 * the PIC 'line' from PCI configuration space and assumes
 * this to be wired to the first (LIRQ0) IRQ input at the
 * universe. The default setup tries to use interrupt
 * sharing.
 */

#include <bsp/motorola.h>
#include <bsp/pci.h>

#define BSP_VME_UNIVERSE_INSTALL_IRQ_MGR(err)					\
do {															\
int              bus, dev, i = 0, j;							\
const struct _int_map  *bspmap;									\
  /* install the VME interrupt manager;							\
   * if there's a bsp route map, use it to						\
   * configure additional lines...								\
   */															\
  err = -1;														\
  if (0 == pci_find_device(0x10e3, 0x0000, 0, &bus, &dev, &i)){	\
	if ( (bspmap = motorolaIntMap(currentBoard)) ) {			\
	for ( i=0; bspmap[i].bus >= 0; i++ ) {						\
	  if ( bspmap[i].bus == bus && bspmap[i].slot == dev ) {	\
		int pins[5], names[4];									\
		/* found it; use info here...                   */		\
		/* copy up to 4 entries; terminated with -1 pin */		\
		for ( j=0;												\
		      j<5 && (pins[j]=bspmap[i].pin_route[j].pin-1)>=0;	\
		      j++) {											\
			names[j] = bspmap[i].pin_route[j].int_name[0];		\
		}														\
		pins[4] = -1;											\
		if ( 0 == vmeUniverseInstallIrqMgrAlt(					\
				VMEUNIVERSE_IRQ_MGR_FLAG_SHARED, /* shared IRQs */\
				pins[0], names[0],								\
				pins[1], names[1],								\
				pins[2], names[2],								\
				pins[3], names[3],								\
				-1) ) {											\
		  i = -1;												\
		  break;												\
		}														\
	  }															\
	}															\
    }															\
	if ( i >= 0 )												\
  	  err = vmeUniverseInstallIrqMgrAlt(						\
	  			VMEUNIVERSE_IRQ_MGR_FLAG_SHARED,				\
				0,-1,											\
				-1);											\
  }																\
} while (0)

#endif
