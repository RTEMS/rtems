#ifndef VME_TSI148_DMA_H
#define VME_TSI148_DMA_H

#include <bsp/vmeTsi148.h>

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

/* DMA List access functions for use by bspVmeDmaList */
extern struct VMEDmaListClassRec_ vmeTsi148DmaListClass;

typedef struct VmeTsi148DmaListDescriptorRec_ *VmeTsi148DmaListDescriptor;

int
vmeTsi148DmaSetupXX(BERegister *base, int channel, uint32_t mode, uint32_t xfer_mode, void *custom);

int
vmeTsi148DmaSetup(int channel, uint32_t mode, uint32_t xfer_mode, void *custom);

int
vmeTsi148DmaListStartXX(BERegister *base, int channel, VmeTsi148DmaListDescriptor d);

int
vmeTsi148DmaListStart(int channel, VmeTsi148DmaListDescriptor d);

int
vmeTsi148DmaStartXX(BERegister *base, int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes);

int
vmeTsi148DmaStart(int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes);

uint32_t
vmeTsi148DmaStatusXX(BERegister *base, int channel);

uint32_t
vmeTsi148DmaStatus(int channel);

#ifdef __cplusplus
}
#endif

#endif
