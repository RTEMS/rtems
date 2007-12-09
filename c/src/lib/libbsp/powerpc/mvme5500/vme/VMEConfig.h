#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H
/* VMEConfig.h, S. Kate Feng modified it for MVME5500 3/04  */
/* BSP specific address space configuration parameters */

/* 
 * The BSP maps VME address ranges into
 * one BAT.
 * NOTE: the BSP (startup/bspstart.c) uses
 * hardcoded window lengths that match this
 * layout:
 */
#define _VME_A32_WIN0_ON_PCI	        0x90000000
#define _VME_A24_ON_PCI			0x9f000000
#define _VME_A16_ON_PCI			0x9fff0000

/* Reuse BAT 0 for VME */
#define BSP_VME_BAT_IDX			0

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map our RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0x90000000

#define BSP_VME_UNIVERSE_INSTALL_IRQ_MGR(err)			\
	do {											\
		err = vmeUniverseInstallIrqMgr(0,64+12,1,64+13);	\
	} while (0)

#endif
