/* vmeconfig.c,v 1.1.2.2 2003/03/25 16:46:01 joel Exp */

/* Standard VME bridge configuration for PPC boards */

/* Copyright Author: Till Straumann <strauman@slac.stanford.edu>, 3/2002 */

/* Copyright 2004, Brookhaven National Lab. and S. Kate Feng <feng1@bnl.gov> 
 * Modified to support the MVME5500, 3/2004
 */

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEConfig.h>
#include <bsp/vmeUniverse.h>
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

/* translate through host bridge and vme master window of vme bridge */
int
BSP_vme2local_adrs(unsigned long am, unsigned long vmeaddr, unsigned long *plocaladdr)
{
    int rval=vmeUniverseXlateAddr(1,0,am,vmeaddr,plocaladdr);
    *plocaladdr+=PCI_MEM_BASE;
    return rval;
}

/* how a CPU address is mapped to the VME bus (if at all) */
int
BSP_local2vme_adrs(unsigned long am, unsigned long localaddr, unsigned long *pvmeaddr)
{
  return vmeUniverseXlateAddr(0, 0, am,localaddr+PCI_DRAM_OFFSET,pvmeaddr);
}

int BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg)
{
  return(vmeUniverseInstallISR(vector, handler, arg));
}

int
BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg)
{
  return(vmeUniverseRemoveISR(vector, handler, arg));
}

/* retrieve the currently installed ISR for a given vector */
BSP_VME_ISR_t BSP_getVME_isr(unsigned long vector, void **parg)
{
  return(vmeUniverseISRGet(vector, parg));
}

int BSP_enableVME_int_lvl(unsigned int level)
{
  return(vmeUniverseIntEnable(level));
}

int BSP_disableVME_int_lvl(unsigned int level)
{
  return(vmeUniverseIntDisable(level));
}


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
	VME_AM_EXT_SUP_DATA| VME_AM_IS_MEMORY,
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
  vmeUniverseInstallIrqMgrAlt(1,
			0, BSP_GPP_IRQ_LOWEST_OFFSET + 12,
			1, BSP_GPP_IRQ_LOWEST_OFFSET + 13,
			2, BSP_GPP_IRQ_LOWEST_OFFSET + 14,
			3, BSP_GPP_IRQ_LOWEST_OFFSET + 15,
			-1); 
  
}
