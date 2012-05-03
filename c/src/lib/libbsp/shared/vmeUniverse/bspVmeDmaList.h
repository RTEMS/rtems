#ifndef BSP_VME_DMA_LIST_H
#define BSP_VME_DMA_LIST_H



/* The bspVmeDmaList utility implements part of the API defined by
 * 'VMEDMA.h' namely all 'VMEDmaListDescriptor' related entry points:
 *
 * BSP_VMEDmaListDescriptorDestroy(),
 * BSP_VMEDmaListDestroy(),
 * BSP_VMEDmaListDescriptorEnq(),
 * BSP_VMEDmaListDescriptorNext(),
 * BSP_VMEDmaListDescriptorPrev(),
 * BSP_VMEDmaListDescriptorSetUsr(),
 * BSP_VMEDmaListDescriptorGetUsr(),
 * BSP_VMEDmaListRefresh()
 *
 * Also, it provides helper routines to assist BSPs with implementing
 *
 * BSP_VMEDmaListDescriptorSetup()
 *
 * and
 *
 * BSP_VMEDmaListStart()
 *
 * by providing
 *
 * BSP_VMEDmaListDescriptorNewTool(),
 * BSP_VMEDmaListDescriptorSetupTool(),
 *
 * and
 *
 * BSP_VMEDmaListDescriptorStartTool()
 *
 * This header should only be used inside a BSP
 * chosing to use bspVmeDmaList for implementing VMEDMA.h
 */

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

#include <bsp/VMEDMA.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VMEDmaListNodeRec_    *VMEDmaListNode;

typedef struct VMEDmaListClassRec_   *VMEDmaListClass;

/* Create and setup a new descriptor.
 * The BSP must supply the appropriate controller class.
 */

VMEDmaListNode
BSP_VMEDmaListDescriptorNewTool(
		VMEDmaListClass pc,
		uint32_t	attr_mask,
		uint32_t	xfer_mode,
		uint32_t	pci_addr,
		uint32_t	vme_addr,
		uint32_t	n_bytes);

/* Setup an existing descriptor */
VMEDmaListNode
BSP_VMEDmaListDescriptorSetupTool(
		VMEDmaListNode n,
		uint32_t	attr_mask,
		uint32_t	xfer_mode,
		uint32_t	pci_addr,
		uint32_t	vme_addr,
		uint32_t	n_bytes);


/* Start a list DMA transfer on 'controller'. The 'controller' information
 * must be provided by the BSP.
 */
int
BSP_VMEDmaListDescriptorStartTool(volatile void *controller, int channel, VMEDmaListNode n);

#ifdef __cplusplus
}
#endif

#endif
