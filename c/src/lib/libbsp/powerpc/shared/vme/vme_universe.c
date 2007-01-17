/*$Id$*/

#include <rtems.h>
#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEConfig.h>
#include <bsp/irq.h>
#define __INSIDE_RTEMS_BSP__
#include <bsp/vmeUniverse.h>

/* Wrap BSP VME calls around driver calls - we do this so EPICS doesn't have to
 * include bridge-specific headers. This file provides the necessary glue
 * to make VME.h and vmeconfig.c independent of the bridge chip.
 */

/* 
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 9/2005,
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

int
BSP_vme2local_adrs(unsigned long am, unsigned long vmeaddr, unsigned long *plocaladdr)
{
int rval=vmeUniverseXlateAddr(1,0,am,vmeaddr,plocaladdr);
	*plocaladdr+=PCI_MEM_BASE;
	return rval;
}

int
BSP_local2vme_adrs(unsigned long am, unsigned long localaddr, unsigned long *pvmeaddr)
{
	return vmeUniverseXlateAddr(0, 0, am,localaddr+PCI_DRAM_OFFSET,pvmeaddr);
}


int
BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg)
{
	return vmeUniverseInstallISR(vector, handler, arg);
}

int
BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg)
{
	return vmeUniverseRemoveISR(vector, handler, arg);
}

/* retrieve the currently installed ISR for a given vector */
BSP_VME_ISR_t
BSP_getVME_isr(unsigned long vector, void **parg)
{
	return vmeUniverseISRGet(vector, parg);
}

int
BSP_enableVME_int_lvl(unsigned int level)
{
	return vmeUniverseIntEnable(level);
}

int
BSP_disableVME_int_lvl(unsigned int level)
{
	return vmeUniverseIntDisable(level);
}

int
BSP_VMEOutboundPortCfg(
	unsigned long port,
	unsigned long address_space,
	unsigned long vme_address,
	unsigned long pci_address,
	unsigned long size)
{
	return vmeUniverseMasterPortCfg(port, address_space, vme_address, pci_address, size);
}

int
BSP_VMEInboundPortCfg(
	unsigned long port,
	unsigned long address_space,
	unsigned long vme_address,
	unsigned long pci_address,
	unsigned long size)
{
	return vmeUniverseSlavePortCfg(port, address_space, vme_address, pci_address, size);
}

void
BSP_VMEOutboundPortsShow(FILE *f)
{
	vmeUniverseMasterPortsShow(f);
}

void
BSP_VMEInboundPortsShow(FILE *f)
{
	vmeUniverseSlavePortsShow(f);
}


int BSP_VMEInit()
{
  if ( vmeUniverseInit() ) {
	/*  maybe no VME at all - or no universe ... */
	return -1;
  }
  vmeUniverseReset();
  return 0;
}

int BSP_VMEIrqMgrInstall()
{
#ifndef BSP_VME_UNIVERSE_INSTALL_IRQ_MGR
  /* No map; use first line only and obtain PIC wire from PCI config */
  vmeUniverseInstallIrqMgrAlt(
	VMEUNIVERSE_IRQ_MGR_FLAG_SHARED, /* use shared IRQs */
	0, -1,	/* Universe pin0 -> PIC line from config space */
	-1      /* terminate list  */
  );

#else
  BSP_VME_UNIVERSE_INSTALL_IRQ_MGR;
#endif
#if defined(BSP_PCI_VME_DRIVER_DOES_EOI) && defined(BSP_PIC_DO_EOI)
  if (vmeUniverse0PciIrqLine<0)
	BSP_panic("Unable to get universe interrupt line info from PCI config");
  _BSP_vme_bridge_irq = vmeUniverse0PciIrqLine;
#endif
  return 0;
}
