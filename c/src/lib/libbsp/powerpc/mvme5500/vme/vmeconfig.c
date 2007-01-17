/* vmeconfig.c,v 1.1.2.2 2003/03/25 16:46:01 joel Exp */

/* Standard VME bridge configuration for PPC boards */

/* 
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 3/2002,
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

/* Copyright 2004, Brookhaven National Lab. and S. Kate Feng <feng1@bnl.gov> 
 * Modified to support the MVME5500, 3/2004
 */

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/irq.h>
#include <libcpu/bat.h>

/* Use a weak alias for the VME configuration.
 * This permits individual applications to override
 * this routine.
 * They may even create an 'empty'
 *
 *    void BSP_vme_config(void) {}
 *
 * which will avoid linking in the Universe driver
 * at all :-).
 */

void BSP_vme_config(void) __attribute__ (( weak, alias("__BSP_default_vme_config") ));

void
__BSP_default_vme_config(void)
{

  vmeUniverseInit();
  vmeUniverseReset();

  /* setup a PCI0 area to map the VME bus */
  setdbat(0,_VME_A32_WIN0_ON_PCI, _VME_A32_WIN0_ON_PCI, 0x10000000, IO_PAGE);

  /* map VME address ranges */
  vmeUniverseMasterPortCfg(
	0,
	VME_AM_EXT_SUP_DATA,
	_VME_A32_WIN0_ON_VME,
	_VME_A32_WIN0_ON_PCI,
	0x0F000000);
  vmeUniverseMasterPortCfg(
	1,
	VME_AM_STD_SUP_DATA,
	0x00000000,
	_VME_A24_ON_PCI,
	0x00ff0000);
  vmeUniverseMasterPortCfg(
	2,
	VME_AM_SUP_SHORT_IO,
	0x00000000,
	_VME_A16_ON_PCI,
	0x00010000);

#ifdef _VME_DRAM_OFFSET
  /* map our memory to VME */
  vmeUniverseSlavePortCfg(
	0,
	VME_AM_EXT_SUP_DATA,
	_VME_DRAM_OFFSET,
	PCI_DRAM_OFFSET,
	BSP_mem_size);

  /* make sure the host bridge PCI master is enabled */
  vmeUniverseWriteReg(
	vmeUniverseReadReg(UNIV_REGOFF_PCI_CSR) | UNIV_PCI_CSR_BM,
	UNIV_REGOFF_PCI_CSR);
#endif

  /* stdio is not yet initialized; the driver will revert to printk */
  vmeUniverseMasterPortsShow(0);
  vmeUniverseSlavePortsShow(0);

  /* install the VME insterrupt manager */
  vmeUniverseInstallIrqMgr(0,64+12,1,64+13);    
  if (vmeUniverse0PciIrqLine<0)
	BSP_panic("Unable to get interrupt line info from PCI config");
  _BSP_vme_bridge_irq= BSP_GPP_IRQ_LOWEST_OFFSET+vmeUniverse0PciIrqLine;
  /* install alternate resetter  TODO 
  __BSP_alternate_reset = vmeUniverseResetBus;*/
}
