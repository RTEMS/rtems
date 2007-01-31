/* $Id$ */

/* Implementation of the VMEDMA.h API for the BSP using the vmeUniverse driver */

/* 
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2006, 2007
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

#include <stdint.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEDMA.h>
#include <bsp/vmeUniverse.h>
#include <bsp/vmeUniverseDMA.h>
#include <bsp/bspVmeDmaList.h>

int
BSP_VMEDmaSetup(int channel, uint32_t bus_mode, uint32_t xfer_mode, void *custom_setup)
{
	return vmeUniverseDmaSetup(channel, bus_mode, xfer_mode, custom_setup);
}

int
BSP_VMEDmaStart(int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes)
{
	return vmeUniverseDmaStart(channel, pci_addr, vme_addr, n_bytes);
}

uint32_t
BSP_VMEDmaStatus(int channel)
{
	return vmeUniverseDmaStatus(channel);
}

BSP_VMEDmaListDescriptor
BSP_VMEDmaListDescriptorSetup(
		BSP_VMEDmaListDescriptor d,
		uint32_t                 attr_mask,
		uint32_t				 xfer_mode,
		uint32_t                 pci_addr,
		uint32_t                 vme_addr,
		uint32_t                 n_bytes)
{
VMEDmaListClass	pc;
	if ( !d ) {
		pc = &vmeUniverseDmaListClass;

		return BSP_VMEDmaListDescriptorNewTool(
					pc,
					attr_mask,
					xfer_mode,
					pci_addr,
					vme_addr,
					n_bytes);
					
	}
	return BSP_VMEDmaListDescriptorSetupTool(d, attr_mask, xfer_mode, pci_addr, vme_addr, n_bytes);
}

int
BSP_VMEDmaListStart(int channel, BSP_VMEDmaListDescriptor list)
{
	return BSP_VMEDmaListDescriptorStartTool(0, channel, list);
}

/* NOT thread safe! */
int
BSP_VMEDmaInstallISR(int channel, BSP_VMEDmaIRQCallback cb, void *usr_arg)
{
int vec;
BSP_VME_ISR_t curr;
void          *carg;

	if ( channel != 0 )
		return -1;

	vec = UNIV_DMA_INT_VEC;

	curr = BSP_getVME_isr(vec, &carg);

	if ( cb && curr ) {
		/* IRQ currently in use */
		return -1;
	}

	if ( !cb && !curr ) {
		/* Allow uninstall if no handler is currently installed;
		 * just make sure IRQ is disabled
		 */
		BSP_disableVME_int_lvl(vec);
		return 0;
	}
		
	if ( cb ) {
		if ( BSP_installVME_isr(vec, (BSP_VME_ISR_t)cb, usr_arg) )
			return -4;
		BSP_enableVME_int_lvl(vec);
	} else {
		BSP_disableVME_int_lvl(vec);
		if ( BSP_removeVME_isr(vec, curr, carg) )
			return -4;
	}
	return 0;
}
