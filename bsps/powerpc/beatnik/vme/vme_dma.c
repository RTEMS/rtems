/* Setup/glue to attach VME DMA driver to the beatnik BSP */

/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
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

#include <stdio.h>
#include <stdint.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/vmeTsi148.h>
#include <bsp/vmeUniverse.h>
#include <bsp/VMEDMA.h>
#include <bsp/vmeTsi148DMA.h>
#include <bsp/vmeUniverseDMA.h>
#include <bsp/bspVmeDmaList.h>

typedef struct DmaOpsRec_ {
	int				(*setup)(int, uint32_t, uint32_t, void *);
	int				(*start)(int, uint32_t, uint32_t, uint32_t);
	uint32_t		(*status)(int);
	VMEDmaListClass	listClass;
} DmaOpsRec, *DmaOps;

static DmaOpsRec universeOps = {
	vmeUniverseDmaSetup,
	vmeUniverseDmaStart,
	vmeUniverseDmaStatus,
	&vmeUniverseDmaListClass,
};

static DmaOpsRec tsiOps = {
	vmeTsi148DmaSetup,
	vmeTsi148DmaStart,
	vmeTsi148DmaStatus,
	&vmeTsi148DmaListClass,
};

static int      setup(int a, uint32_t b, uint32_t c, void *d);
static int      start(int a, uint32_t b, uint32_t c, uint32_t d);
static uint32_t status(int a);

static DmaOpsRec jumpstartOps = {
	setup,
	start,
	status,
	0
};

static DmaOps dmaOps = &jumpstartOps;

static DmaOps selectOps()
{
	return (MVME6100 != BSP_getBoardType()) ?
				&universeOps : &tsiOps;
}

static int
setup(int a, uint32_t b, uint32_t c, void *d)
{
	return (dmaOps=selectOps())->setup(a,b,c,d);
}

static int
start(int a, uint32_t b, uint32_t c, uint32_t d)
{
	return (dmaOps=selectOps())->start(a,b,c,d);
}

static uint32_t
status(int a)
{
	return (dmaOps=selectOps())->status(a);
}


int
BSP_VMEDmaSetup(int channel, uint32_t bus_mode, uint32_t xfer_mode, void *custom_setup)
{
	return dmaOps->setup(channel, bus_mode, xfer_mode, custom_setup);
}

int
BSP_VMEDmaStart(int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes)
{
	return dmaOps->start(channel, pci_addr, vme_addr, n_bytes);
}

uint32_t
BSP_VMEDmaStatus(int channel)
{
	return dmaOps->status(channel);
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
		if ( ! (pc = dmaOps->listClass) ) {
			pc = (dmaOps = selectOps())->listClass;	
		}
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

	if ( MVME6100 != BSP_getBoardType() ) {
		if ( channel != 0 )
			return -1;

		vec = UNIV_DMA_INT_VEC;

	} else {
		if ( channel < 0 || channel > 1 )
			return -1;

		vec  = (channel ? TSI_DMA1_INT_VEC : TSI_DMA_INT_VEC );
	}

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
