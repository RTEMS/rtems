/* vmeconfig.c,v 1.1.2.2 2003/03/25 16:46:01 joel Exp */

/* Standard VME bridge configuration for VGM type boards */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 3/2002 */

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/vmeUniverse.h>
#include <bsp/VMEConfig.h>
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

extern uint32_t VME_Slot1;

void
__BSP_default_vme_config(void)
{
union {
	struct _BATU	bat;
	unsigned long	batbits;
} dbat0u;
  
  vmeUniverseInit();
  vmeUniverseReset();

  /* setup a PCI area to map the VME bus */

  dbat0u.batbits = _read_DBAT0U();

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
#if 0
  if (VME_Slot1){ 
    /* map our memory to VME */
printk("vmeUniverseSlavePortCfg length of 0x%x\n", BSP_mem_size);
    vmeUniverseSlavePortCfg(
	0,
	VME_AM_EXT_SUP_DATA,
	_VME_DRAM_32_OFFSET1,
	PCI_DRAM_OFFSET,
	BSP_mem_size);
printk("vmeUniverseSlavePortCfg length of 0x%x\n", _VME_A24_SIZE);
    vmeUniverseSlavePortCfg(
        1,
        VME_AM_STD_SUP_DATA,
        _VME_DRAM_24_OFFSET1,
        PCI_DRAM_OFFSET,
        _VME_A24_SIZE);
printk("vmeUniverseSlavePortCfg length of 0x%x\n", _VME_A16_SIZE);
    vmeUniverseSlavePortCfg(
        2,
        VME_AM_SUP_SHORT_IO,
        _VME_DRAM_16_OFFSET1,
        PCI_DRAM_OFFSET,
        _VME_A16_SIZE);
  }
  else {
printk("vmeUniverseSlavePortCfg length of 0x%x\n", BSP_mem_size);
     vmeUniverseSlavePortCfg(
        0,
        VME_AM_EXT_SUP_DATA,
        _VME_DRAM_32_OFFSET2,
        PCI_DRAM_OFFSET,
        BSP_mem_size);
printk("vmeUniverseSlavePortCfg length of 0x%x\n", _VME_A24_SIZE);
    vmeUniverseSlavePortCfg(
        1,
        VME_AM_STD_SUP_DATA,
        _VME_DRAM_24_OFFSET2,
        PCI_DRAM_OFFSET,
        _VME_A24_SIZE);
printk("vmeUniverseSlavePortCfg length of 0x%x\n", _VME_A16_SIZE);
    vmeUniverseSlavePortCfg(
        2,
        VME_AM_SUP_SHORT_IO,
        _VME_DRAM_16_OFFSET2,
        PCI_DRAM_OFFSET,
        _VME_A16_SIZE);
  }
#endif

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
