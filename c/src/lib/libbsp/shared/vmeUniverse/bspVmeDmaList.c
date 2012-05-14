/* bspVmeDmaList.c:
 * implementation of generic parts of the 'linked-list VME DMA' API.
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2006, 2007,
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bsp/bspVmeDmaList.h>
#include "bspVmeDmaListP.h"

#define DEBUG

typedef struct VMEDmaListNodeRec_ {
	VMEDmaListNode	p, n;		/* linkage         */
	DmaDescriptor	d;			/* real descriptor */
	void            *usrData;
	VMEDmaListClass	class;		/* pointer to 'class' record */
} VMEDmaListNodeRec;

#define LCHUNK	10

#ifdef DEBUG
static void
lprint(VMEDmaListNode d)
{
	printf("n 0x%08lx, p: 0x%08lx, n: 0x%08lx d: 0x%08lx\n",
		(uint32_t)d, (uint32_t)d->p, (uint32_t)d->n, (uint32_t)d->d);
}
#endif

static VMEDmaListNode
lalloc(VMEDmaListClass pc)
{
VMEDmaListNode rval;
int i;

	if ( !pc->freeList ) {
		/* alloc block of 10 descriptors */
		pc->freeList = calloc( (LCHUNK),  sizeof(*pc->freeList));

		if ( ! (pc->freeList) ) {
			return 0;
		}

		/* link together and set 'class' pointer */
		for (i=0; i<(LCHUNK)-1; i++) {
			pc->freeList[i].n     = &pc->freeList[i+1];
			pc->freeList[i].class = pc;
		}
		pc->freeList[i].n     = 0;
		pc->freeList[i].class = pc;

		/* Allocate 'real' descriptor memory */
		if ( pc->desc_alloc ) {
			for (i=0; i<(LCHUNK); i++) {
				if ( ! (pc->freeList[i].d = pc->desc_alloc()) ) {
					int j;
					if ( pc->desc_free ) {
						for (j=0; j<i; j++)
							pc->desc_free(pc->freeList[i].d);
					}
					free(pc->freeList);
					pc->freeList = 0;
					return 0;
				}
			}
		} else {
			int      blksize;
			uint32_t algnmsk = pc->desc_align - 1;
			char     *memptr;

			/* ignore their 'free' method */
			pc->desc_free = 0;

			blksize  = (pc->desc_size + algnmsk) & ~algnmsk;

			if ( ! (memptr = malloc(blksize*(LCHUNK) + pc->desc_align - 1)) ) {
				free(pc->freeList);
				pc->freeList = 0;
				return 0;
			}

			/* align memory ptr; must not be freed() anymore */
			memptr = (char*)( ((uint32_t)memptr + algnmsk) & ~ algnmsk );

			for ( i = 0; i<(LCHUNK); i++, memptr+=blksize ) {
				memset(memptr, 0, blksize);
				pc->freeList[i].d = (DmaDescriptor)memptr;
			}
		}
	}
	rval         = pc->freeList;
	pc->freeList = pc->freeList->n;
	rval->n      = rval->p = 0;
	return rval;
}

static int
lfree(VMEDmaListNode d)
{
	if ( d->p || d->n )
		return -1;
	d->n = d->class->freeList;
	d->class->freeList = d;
	return 0;
}

static int
lenq(VMEDmaListNode a, VMEDmaListNode d)
{
	if ( a ) {
		/* enqueue */
		if ( d->n || d->p )
			return -1;
		if ( (d->n = a->n) )
			a->n->p = d;
		d->p = a;
		a->n = d;
	} else {
		/* dequeue */
		if ( d->n )
			d->n->p = d->p;
		if ( d->p )
			d->p->n = d->n;
		d->n = d->p = 0;
	}
	return 0;
}


int
BSP_VMEDmaListDescriptorStartTool(volatile void *controller, int channel, VMEDmaListNode n)
{
	if ( !n )
		return -1;
	return n->class->desc_start(controller, channel, n->d);
}

VMEDmaListNode
BSP_VMEDmaListDescriptorSetupTool(
		VMEDmaListNode n,
		uint32_t	attr_mask,
		uint32_t	xfer_mode,
		uint32_t	pci_addr,
		uint32_t	vme_addr,
		uint32_t	n_bytes)
{
	if ( !n )
		return 0;

	if ( n->class->desc_setup(n->d, attr_mask, xfer_mode, pci_addr, vme_addr, n_bytes) ) {
		return 0;
	}

	return n;
}

VMEDmaListNode
BSP_VMEDmaListDescriptorNewTool(
		VMEDmaListClass pc,
		uint32_t	attr_mask,
		uint32_t	xfer_mode,
		uint32_t	pci_addr,
		uint32_t	vme_addr,
		uint32_t	n_bytes)
{
VMEDmaListNode n;

	if ( !(n=lalloc(pc)) )
		return 0;	/* no memory */

	if ( n->class->desc_init )
		n->class->desc_init(n->d);

	if ( n->class->desc_setup(n->d, attr_mask, xfer_mode, pci_addr, vme_addr, n_bytes) ) {
		BSP_VMEDmaListDescriptorDestroy(n);
		return 0;
	}
	return n;
}

int
BSP_VMEDmaListDescriptorDestroy(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode d = p;
	return lfree(d);
}

int
BSP_VMEDmaListDestroy(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode d = p;
VMEDmaListNode n;
	while (d) {
		n = d->n;
		if ( BSP_VMEDmaListDescriptorEnq(0, d) ||
		     BSP_VMEDmaListDescriptorDestroy(d) )
			return -1;
		d = n;
	}
	return 0;
}

int
BSP_VMEDmaListDescriptorEnq(BSP_VMEDmaListDescriptor p, BSP_VMEDmaListDescriptor q)
{
VMEDmaListNode      anchor = p;
VMEDmaListNode           d = q;
DmaDescriptorSetNxt setnxt = d->class->desc_setnxt;

	if ( !anchor ) {
		/* dequeue can't fail - we can update dnlal first (need d->p) */
		if ( d->p )
			setnxt(d->p->d, d->n ? d->n->d : 0);
		/* paranoia */
		setnxt(d->d, 0);
	} else {
		if ( d->class != anchor->class )
			return -1;
	}
	if ( lenq(anchor, d) )
		return -1;
	/* update descriptor pointers */
	if ( anchor ) {
		setnxt(d->d,      d->n ? d->n->d : 0);
		setnxt(anchor->d, d->d);
	}
	return 0;
}

BSP_VMEDmaListDescriptor
BSP_VMEDmaListDescriptorNext(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode d = p;
	return d->n;
}

BSP_VMEDmaListDescriptor
BSP_VMEDmaDescriptorPrev(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode d = p;
	return d->p;
}

void
BSP_VMEDmaListDescriptorSetUsr(BSP_VMEDmaListDescriptor p, void *usrData)
{
VMEDmaListNode d = p;
	d->usrData = usrData;
}

void *
BSP_VMEDmaListDescriptorGetUsr(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode d = p;
	return d->usrData;
}

int
BSP_VMEDmaListRefresh(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode    anchor = p;
DmaDescriptorRefr desc_refr;
VMEDmaListNode    n;
	if ( (desc_refr = anchor->class->desc_refr) ) {
		for ( n = anchor; n; n = n->n ) {
			desc_refr(n->d);
		}
	}
	return 0;
}

#ifdef DEBUG
void
BSP_VMEDmaListDump(BSP_VMEDmaListDescriptor p)
{
VMEDmaListNode d = p;
	while (d) {
		printf("----------\n");
		lprint(d);
		if (d->class->desc_dump)
			d->class->desc_dump(d->d);
		d = d->n;
	}
}
#endif
