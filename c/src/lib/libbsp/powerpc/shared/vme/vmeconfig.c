/* Default VME bridge configuration - note that this file
 * is independent of the bridge driver/chip
 */

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

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEConfig.h>
#ifdef BSP_VME_BAT_IDX
#include <libcpu/bat.h>
#endif
#include <rtems/bspIo.h>

extern int BSP_VMEInit(void);
extern int BSP_VMEIrqMgrInstall(void);

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
	0x0e000000);
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

#ifdef _VME_CSR_ON_PCI
  /* Map VME64 CSR */
  BSP_VMEOutboundPortCfg(
		  7,
		  VME_AM_CSR,
		  0,
		  _VME_CSR_ON_PCI,
		  0x01000000);
#endif


#ifdef _VME_DRAM_OFFSET
  /* map our memory to VME giving the driver a hint that it's ordinary memory
   * so they can enable decoupled cycles which should give better performance...
   */
  BSP_VMEInboundPortCfg(
	0,
	VME_AM_EXT_SUP_DATA | VME_AM_IS_MEMORY,
	_VME_DRAM_OFFSET,
	PCI_DRAM_OFFSET,
	BSP_mem_size);
#endif

  /* stdio is not yet initialized; the driver will revert to printk */
  BSP_VMEOutboundPortsShow(0);
  BSP_VMEInboundPortsShow(0);

  BSP_VMEIrqMgrInstall();
}
