#ifndef RTEMS_BSP_VME_UNIVERSE_H
#define RTEMS_BSP_VME_UNIVERSE_H
/* $Id$ */

/* SVGM et al. BSP's VME support */
/* Author: Till Straumann, <strauman@slac.stanford.edu> */

/* pull in bsp.h */
#include <bsp.h>
/* our VME bridge */
#include <bsp/vmeUniverse.h>
/* our address space configuration */
#include <bsp/VMEConfig.h>

/* VME related declarations */
/* how to map a VME address to the CPU local bus.
 * Note that this traverses two bridges:
 * the grackle and the universe. For the
 * Universe, there is a lookup procedure while
 * we assume a 1:1 mapping for the grackle...
 */

/* NOTE about the fast mapping macros:
 * using these macros is only safe if the user app
 * does _NOT_ change the universe mappings!
 * While changing the PCI windows probably doesn't
 * make much sense (involves changing the MMU/DBATs as well),
 * The user might wish to change the VME address
 * layout, i.e. by remapping _VME_A32_WIN0_ON_VME
 * and _VME_DRAM_OFFSET...
 * Hence, using the A24 and A16 macros is probably safe.
 */

#define BSP_vme_init() \
	vmeUniverseInit

/* translate through host bridge and vme master window of vme bridge */
static inline int
BSP_vme2local_adrs(unsigned am, unsigned long vmeaddr, unsigned long *plocaladdr)
{
int rval=vmeUniverseXlateAddr(1,0,am,vmeaddr,plocaladdr);
	*plocaladdr+=PCI_MEM_BASE;
	return rval;
}

/* when using this macro, the universe setup MUST NOT BE
 * CHANGED by the application...
 */
#define BSP_vme2local_A32_fast(vmeaddr) \
	((vmeaddr)-_VME_A32_WIN0_ON_VME + _VME_A32_WIN0_ON_PCI + PCI_MEM_BASE)
#define BSP_vme2local_A24_fast(vmeaddr) \
	(((vmeaddr)&0x7ffffff)+_VME_A24_ON_PCI + PCI_MEM_BASE)
#define BSP_vme2local_A16_fast(vmeaddr) \
	(((vmeaddr)&0xffff)+_VME_A16_ON_PCI + PCI_MEM_BASE)

/* how a CPU address is mapped to the VME bus (if at all)
 */
static inline int
BSP_local2vme_adrs(unsigned am, unsigned long localaddr, unsigned long *pvmeaddr)
{
return vmeUniverseXlateAddr(0, 0, am,localaddr+PCI_DRAM_OFFSET,pvmeaddr);
}

#define BSP_localdram2vme_fast(localaddr) \
	((localaddr)+_VME_DRAM_OFFSET)

/* interrupt handlers and levels */
typedef void (*BSP_VME_ISR_t)(void *usrArg, unsigned long vector);

#define BSP_installVME_isr(vector, handler, arg) \
	vmeUniverseInstallISR(vector, handler, arg)

#define BSP_removeVME_isr(vector, handler, arg) \
	vmeUniverseRemoveISR(vector, handler, arg)

/* retrieve the currently installed ISR for a given vector */
#define BSP_getVME_isr(vector, parg) \
    vmeUniverseISRGet(vector, parg)

#define BSP_enableVME_int_lvl(level) \
	vmeUniverseIntEnable(level)

#define BSP_disableVME_int_lvl(level) \
	vmeUniverseIntDisable(level)

/* Tell the interrupt manager that the universe driver
 * already called openpic_eoi() and that this step hence
 * must be omitted.
 */
#define BSP_PCI_VME_DRIVER_DOES_EOI
extern int _BSP_vme_bridge_irq;
/* don't reference vmeUniverse0PciIrqLine directly here - leave it up to
 * bspstart() to set BSP_vme_bridge_irq. That way, we can generate variants
 * of the BSP with / without the universe driver...
 */
#define BSP_PCI_VME_BRIDGE_IRQ _BSP_vme_bridge_irq

#endif
