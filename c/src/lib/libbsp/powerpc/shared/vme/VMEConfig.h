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
 */
#define _VME_A32_WIN0_ON_PCI	0x10000000
#define _VME_A24_ON_PCI			0x1f000000
#define _VME_A16_ON_PCI			0x1fff0000

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map our RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0xc0000000

#endif
