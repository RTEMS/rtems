#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H
/* $Id$ */

/* BSP specific address space configuration parameters */

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
 * TODO: this should perhaps be a configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map our RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0xc0000000

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

#include <bsp/motorola.h>
#include <bsp/pci.h>

#define BSP_VME_UNIVERSE_INSTALL_IRQ_MGR						\
do {															\
int              bus, dev, i = 0, j;							\
const struct _int_map  *bspmap;									\
  /* install the VME interrupt manager;							\
   * if there's a bsp route map, use it to						\
   * configure additional lines...								\
   */															\
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
				1, /* shared IRQs */							\
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
  	  vmeUniverseInstallIrqMgrAlt(1,0,-1,-1);					\
  }																\
} while (0)

#endif
