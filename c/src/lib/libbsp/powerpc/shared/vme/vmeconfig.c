/* $Id$ */

/* Standard VME bridge configuration for VGM type boards */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 3/2002 */

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/irq.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>
#include <bsp/motorola.h>

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

SPR_RO(DBAT0U)

void
__BSP_default_vme_config(void)
{
union {
	struct _BATU	bat;
	unsigned long	batbits;
} dbat0u;

  if (currentBoard < MVME_2300 || currentBoard >= MVME_1600) {
		printk("VME bridge for this board is unknown - if it's a Tundra Universe, add the board to 'shared/vme/vmeconfig.c'\n");
		printk("Skipping VME initialization...\n");
		return;
  }

  vmeUniverseInit();
  vmeUniverseReset();

  /* setup a PCI area to map the VME bus */

  dbat0u.batbits = _read_DBAT0U();

  /* if we have page tables, BAT0 is available */
  if (dbat0u.bat.vs || dbat0u.bat.vp) {
	printk("WARNING: BAT0 is taken (no pagetables?); VME bridge must share PCI range for VME access\n");
	printk("Skipping VME initialization...\n");
	return;
  }

  setdbat(0,
		  PCI_MEM_BASE + _VME_A32_WIN0_ON_PCI,
		  PCI_MEM_BASE + _VME_A32_WIN0_ON_PCI,
		  0x10000000,
		  IO_PAGE);

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
  vmeUniverseInstallIrqMgr(0,5,1,6);
  if (vmeUniverse0PciIrqLine<0)
	BSP_panic("Unable to get interrupt line info from PCI config");
  _BSP_vme_bridge_irq=BSP_PCI_IRQ_LOWEST_OFFSET+vmeUniverse0PciIrqLine;
}
