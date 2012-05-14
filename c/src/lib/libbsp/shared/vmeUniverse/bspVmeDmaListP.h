#ifndef BSP_VME_DMA_LIST_P_H
#define BSP_VME_DMA_LIST_P_H

#include <bsp/bspVmeDmaList.h>

/* Private Interface to the bspVmeDmaList facility
 *
 * This is used by chip drivers to implement the
 * 'class' members so that 'bspVmeDmaList' can access
 * the device in an abstract manner.
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
#ifdef __cplusplus
extern "C" {
#endif

typedef void           *DmaDescriptor;

/* Member functions to be implemented by chip drivers */

typedef DmaDescriptor (*DmaDescriptorAlloc)(void);
typedef void          (*DmaDescriptorFree) (DmaDescriptor d);
typedef void          (*DmaDescriptorInit) (DmaDescriptor d);
/* Setup takes the parameters declared in VMEDMA.h */
typedef int           (*DmaDescriptorSetup)(DmaDescriptor d, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
typedef void          (*DmaDescriptorSetNxt)(DmaDescriptor d, DmaDescriptor next);
typedef int           (*DmaDescriptorStart)(volatile void *controller_addr, int channel, DmaDescriptor);
typedef int           (*DmaDescriptorRefr) (DmaDescriptor);
typedef void          (*DmaDescriptorDump) (DmaDescriptor);


typedef struct VMEDmaListClassRec_ {
	int					desc_size;	/* size of a descritor */
	int					desc_align;	/* alignment of a descriptor */
	VMEDmaListNode			freeList;	/* list of free descriptors of this class, MUST be initialized to NULL */
	DmaDescriptorAlloc	desc_alloc;	/* [optional, may be NULL] allocator for one descriptor */
	DmaDescriptorFree	desc_free;	/* [optional, may be NULL] destructor for one descriptor */
	DmaDescriptorInit   desc_init;  /* [optional, may be NULL] set stuff that don't change during lifetime */
	DmaDescriptorSetNxt	desc_setnxt;/* set 'NEXT' pointer in descriptor; mark as LAST if next == 0 */
	DmaDescriptorSetup  desc_setup;	/* setup a descriptor   */
	DmaDescriptorStart  desc_start;	/* start list at a descriptor   */
	DmaDescriptorRefr	desc_refr;	/* refresh a descriptor */
	DmaDescriptorDump	desc_dump;	/* dump a descriptor (for debugging) */
} VMEDmaListClassRec;

#ifdef __cplusplus
}
#endif

#endif
