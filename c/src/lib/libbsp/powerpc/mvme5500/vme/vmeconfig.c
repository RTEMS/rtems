/* vmeconfig.c,v 1.1.2.2 2003/03/25 16:46:01 joel Exp */

/* Standard VME bridge configuration for PPC boards */

/* Copyright Author: Till Straumann <strauman@slac.stanford.edu>, 3/2002 */

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
  vmeUniverseInstallIrqMgr(0,12,1,13);    
  if (vmeUniverse0PciIrqLine<0)
	BSP_panic("Unable to get interrupt line info from PCI config");
  _BSP_vme_bridge_irq= BSP_GPP_IRQ_LOWEST_OFFSET+vmeUniverse0PciIrqLine;
  /* install alternate resetter  TODO 
  __BSP_alternate_reset = vmeUniverseResetBus;*/
}
