/* Implementation of the VME.h and VMEDMA.h APIs for the BSP using the
 * vmeUniverse/vmeTsi148 drivers
 *
 * This file is named vme_universe.c for historic reasons.
 */


#include <rtems.h>
#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEDMA.h>
#include <bsp/VMEConfig.h>
#include <bsp/irq.h>
#include <stdio.h>

#define __INSIDE_RTEMS_BSP__

#if !defined(_VME_DRIVER_TSI148) && !defined(_VME_DRIVER_UNIVERSE)
#define _VME_DRIVER_UNIVERSE
#endif

#if defined(_VME_DRIVER_TSI148)
#define _VME_TSI148_DECLARE_SHOW_ROUTINES
#include <bsp/vmeTsi148.h>
#include <bsp/vmeTsi148DMA.h>
#endif

#if defined(_VME_DRIVER_UNIVERSE)
#define _VME_UNIVERSE_DECLARE_SHOW_ROUTINES
#include <bsp/vmeUniverse.h>
#include <bsp/vmeUniverseDMA.h>
#if     !defined(BSP_VME_INSTALL_IRQ_MGR) && defined(BSP_VME_UNIVERSE_INSTALL_IRQ_MGR)
#define BSP_VME_INSTALL_IRQ_MGR BSP_VME_UNIVERSE_INSTALL_IRQ_MGR
#endif
#endif

#include <bsp/bspVmeDmaList.h>

/* Wrap BSP VME calls around driver calls - we do this so EPICS doesn't have to
 * include bridge-specific headers. This file provides the necessary glue
 * to make VME.h and vmeconfig.c independent of the bridge chip.
 *
 * This file is named 'vme_universe.c' for historical reasons.
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 9/2005,
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
typedef struct {
	int				(*xlate_adrs)(int, int, unsigned long, unsigned long, unsigned long *);
	int				(*install_isr)(unsigned long, BSP_VME_ISR_t, void *);
	int				(*remove_isr)(unsigned long, BSP_VME_ISR_t, void *);
	BSP_VME_ISR_t	(*get_isr)(unsigned long vector, void **);
	int				(*enable_int_lvl)(unsigned int);
	int				(*disable_int_lvl)(unsigned int);
	int				(*outbound_p_cfg)(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
	int				(*inbound_p_cfg) (unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
	void			(*outbound_p_show)(FILE*);
	void			(*inbound_p_show) (FILE*);
	void            (*reset_bus)(void);
	int				(*install_irq_mgr)(int, int, int, ...);
	int             irq_mgr_flags;
} VMEOpsRec, *VMEOps;

/* two separate 'ops' structs for historic reasons */
typedef struct DmaOpsRec_ {
	int				(*setup)(int, uint32_t, uint32_t, void *);
	int				(*start)(int, uint32_t, uint32_t, uint32_t);
	uint32_t		(*status)(int);
	VMEDmaListClass	listClass;
	int				nChannels;
	int				*vectors;
} DmaOpsRec, *DmaOps;

#ifdef _VME_DRIVER_UNIVERSE
static VMEOpsRec uniOpsRec = {
	xlate_adrs: 		vmeUniverseXlateAddr,
	install_isr: 		vmeUniverseInstallISR,
	remove_isr: 		vmeUniverseRemoveISR,
	get_isr:	 		vmeUniverseISRGet,
	enable_int_lvl:		vmeUniverseIntEnable,
	disable_int_lvl:	vmeUniverseIntDisable,
	outbound_p_cfg:		vmeUniverseMasterPortCfg,
	inbound_p_cfg:		vmeUniverseSlavePortCfg,
	outbound_p_show:	vmeUniverseMasterPortsShow,
	inbound_p_show:		vmeUniverseSlavePortsShow,
	reset_bus:          vmeUniverseResetBus,
	install_irq_mgr:	vmeUniverseInstallIrqMgrAlt,
	irq_mgr_flags:      VMEUNIVERSE_IRQ_MGR_FLAG_SHARED |
	                    VMEUNIVERSE_IRQ_MGR_FLAG_PW_WORKAROUND,
};

static int uniVecs[] = { UNIV_DMA_INT_VEC };

static DmaOpsRec uniDmaOpsRec = {
	setup:				vmeUniverseDmaSetup,
	start:				vmeUniverseDmaStart,
	status:				vmeUniverseDmaStatus,
	listClass:			&vmeUniverseDmaListClass,
	nChannels:			1,
	vectors:			uniVecs,
};
#endif

#ifdef _VME_DRIVER_TSI148
static VMEOpsRec tsiOpsRec = {
	xlate_adrs: 		vmeTsi148XlateAddr,
	install_isr: 		vmeTsi148InstallISR,
	remove_isr: 		vmeTsi148RemoveISR,
	get_isr:	 		vmeTsi148ISRGet,
	enable_int_lvl:		vmeTsi148IntEnable,
	disable_int_lvl:	vmeTsi148IntDisable,
	outbound_p_cfg:		vmeTsi148OutboundPortCfg,
	inbound_p_cfg:		vmeTsi148InboundPortCfg,
	outbound_p_show:	vmeTsi148OutboundPortsShow,
	inbound_p_show:		vmeTsi148InboundPortsShow,
	reset_bus:          vmeTsi148ResetBus,
	install_irq_mgr:	vmeTsi148InstallIrqMgrAlt,
	irq_mgr_flags:      VMETSI148_IRQ_MGR_FLAG_SHARED,
};

static int tsiVecs[] = {
	TSI_DMA_INT_VEC,
	TSI_DMA1_INT_VEC,
};

static DmaOpsRec tsiDmaOpsRec = {
	setup:				vmeTsi148DmaSetup,
	start:				vmeTsi148DmaStart,
	status:				vmeTsi148DmaStatus,
	listClass:			&vmeTsi148DmaListClass,
	nChannels:			2,
	vectors:			tsiVecs,
};
#endif

static VMEOps theOps    = 0;
static DmaOps theDmaOps = 0;

int
BSP_vme2local_adrs(unsigned long am, unsigned long vmeaddr, unsigned long *plocaladdr)
{
int rval=theOps->xlate_adrs(1,0,am,vmeaddr,plocaladdr);
	*plocaladdr+=PCI_MEM_BASE;
	return rval;
}

int
BSP_local2vme_adrs(unsigned long am, unsigned long localaddr, unsigned long *pvmeaddr)
{
	return theOps->xlate_adrs(0, 0, am,localaddr+PCI_DRAM_OFFSET,pvmeaddr);
}

int
BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg)
{
	return theOps->install_isr(vector, handler, arg);
}

int
BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg)
{
	return theOps->remove_isr(vector, handler, arg);
}

/* retrieve the currently installed ISR for a given vector */
BSP_VME_ISR_t
BSP_getVME_isr(unsigned long vector, void **parg)
{
	return theOps->get_isr(vector, parg);
}

int
BSP_enableVME_int_lvl(unsigned int level)
{
	return theOps->enable_int_lvl(level);
}

int
BSP_disableVME_int_lvl(unsigned int level)
{
	return theOps->disable_int_lvl(level);
}

int
BSP_VMEOutboundPortCfg(
	unsigned long port,
	unsigned long address_space,
	unsigned long vme_address,
	unsigned long pci_address,
	unsigned long size)
{
	return theOps->outbound_p_cfg(port, address_space, vme_address, pci_address, size);
}

int
BSP_VMEInboundPortCfg(
	unsigned long port,
	unsigned long address_space,
	unsigned long vme_address,
	unsigned long pci_address,
	unsigned long size)
{
	return theOps->inbound_p_cfg(port, address_space, vme_address, pci_address, size);
}

void
BSP_VMEOutboundPortsShow(FILE *f)
{
	theOps->outbound_p_show(f);
}

void
BSP_VMEInboundPortsShow(FILE *f)
{
	theOps->inbound_p_show(f);
}

void
BSP_VMEResetBus(void)
{
	theOps->reset_bus();
}

int
BSP_VMEDmaSetup(int channel, uint32_t bus_mode, uint32_t xfer_mode, void *custom_setup)
{
	return theDmaOps->setup(channel, bus_mode, xfer_mode, custom_setup);
}

int
BSP_VMEDmaStart(int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes)
{
	return theDmaOps->start(channel, pci_addr, vme_addr, n_bytes);
}

uint32_t
BSP_VMEDmaStatus(int channel)
{
	return theDmaOps->status(channel);
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

		pc = theDmaOps->listClass;

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

	if ( channel < 0 || channel >= theDmaOps->nChannels )
		return -1;

	vec = theDmaOps->vectors[channel];

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

#if defined(_VME_DRIVER_TSI148) && !defined(VME_CLEAR_BRIDGE_ERRORS)
static unsigned short
tsi_clear_errors(int quiet)
{
unsigned long	v;
unsigned short	rval;
	v = vmeTsi148ClearVMEBusErrors(0);

	/* return bits 8..23 of VEAT; set bit 15 to make sure rval is nonzero on error */
	rval =  v ? ((v>>8) & 0xffff) | (1<<15) : 0;
	return rval;
}

#define VME_CLEAR_BRIDGE_ERRORS tsi_clear_errors
#endif

extern unsigned short (*_BSP_clear_vmebridge_errors)(int);

int BSP_VMEInit(void)
{
#if defined(_VME_DRIVER_UNIVERSE)
  if ( 0 == vmeUniverseInit() ) {
  	theOps    = &uniOpsRec;
	theDmaOps = &uniDmaOpsRec;
	vmeUniverseReset();
  }
#endif
#if defined(_VME_DRIVER_UNIVERSE) && defined(_VME_DRIVER_TSI148)
	else
#endif
#if defined(_VME_DRIVER_TSI148)
  if ( 0 == vmeTsi148Init() ) {
 	theOps    = &tsiOpsRec;
	theDmaOps = &tsiDmaOpsRec;
	vmeTsi148Reset();
#ifdef VME_CLEAR_BRIDGE_ERRORS
	{

	  _BSP_clear_vmebridge_errors = VME_CLEAR_BRIDGE_ERRORS;

	}
#endif
  }
#endif
    else
	  /*  maybe no VME at all - or no universe/tsi148 ... */
	  return -1;

  return 0;
}

int BSP_VMEIrqMgrInstall(void)
{
int err;
#ifndef BSP_VME_INSTALL_IRQ_MGR
  /* No map; use first line only and obtain PIC wire from PCI config */
  err = theOps->install_irq_mgr(
	theOps->irq_mgr_flags, /* use shared IRQs */
	0, -1,	/* Universe/Tsi148 pin0 -> PIC line from config space */
	-1      /* terminate list  */
  );
#else
  BSP_VME_INSTALL_IRQ_MGR(err);
#endif

  if ( err )
  	return err;

/* This feature is only supported by the Universe driver (not Tsi148) */
#if defined(BSP_PCI_VME_DRIVER_DOES_EOI) && defined(BSP_PIC_DO_EOI)
#ifdef 	_VME_DRIVER_TSI148
#error  "BSP_PCI_VME_DRIVER_DOES_EOI/BSP_PIC_DO_EOI feature can only be used with vmeUniverse"
#endif
  if ( vmeUniverse0PciIrqLine < 0 )
	BSP_panic("Unable to get universe interrupt line info from PCI config");
  _BSP_vme_bridge_irq = vmeUniverse0PciIrqLine;
#endif
  return 0;
}
