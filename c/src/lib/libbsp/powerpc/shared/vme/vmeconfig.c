/* $Id$ */

/* Default VME bridge configuration - note that this file
 * is independent of the bridge driver/chip
 */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 3/2002 */

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEConfig.h>
#include <libcpu/bat.h>
#include <rtems/bspIo.h>

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

  if ( BSP_VMEInit() ) {
	printk("Skipping VME initialization...\n");
	return;
  }

#ifdef BSP_VME_BAT_IDX
  /* setup a PCI area to map the VME bus */
  setdbat(BSP_VME_BAT_IDX,
		  PCI_MEM_BASE + _VME_A32_WIN0_ON_PCI,
		  PCI_MEM_BASE + _VME_A32_WIN0_ON_PCI,
		  0x10000000,
		  IO_PAGE);
#endif

  /* map VME address ranges */
  BSP_VMEOutboundPortCfg(
	0,
	VME_AM_EXT_SUP_DATA,
	_VME_A32_WIN0_ON_VME,
	_VME_A32_WIN0_ON_PCI,
	0x0F000000);
  BSP_VMEOutboundPortCfg(
	1,
	VME_AM_STD_SUP_DATA,
	0x00000000,
	_VME_A24_ON_PCI,
	0x00ff0000);
  BSP_VMEOutboundPortCfg(
	2,
	VME_AM_SUP_SHORT_IO,
	0x00000000,
	_VME_A16_ON_PCI,
	0x00010000);

#ifdef _VME_DRAM_OFFSET
  /* map our memory to VME */
  BSP_VMEInboundPortCfg(
	0,
	VME_AM_EXT_SUP_DATA,
	_VME_DRAM_OFFSET,
	PCI_DRAM_OFFSET,
	BSP_mem_size);
#endif

  /* stdio is not yet initialized; the driver will revert to printk */
  BSP_VMEOutboundPortsShow(0);
  BSP_VMEInboundPortsShow(0);

  BSP_VMEIrqMgrInstall();
}
