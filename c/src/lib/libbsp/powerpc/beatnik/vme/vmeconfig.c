/* Standard VME bridge configuration for MVME5500, MVME6100 */

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

#include <rtems.h>
#include <rtems/bspIo.h>
#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEConfig.h>
#include <bsp/irq.h>
#include <bsp/vmeUniverse.h>
#define _VME_TSI148_DECLARE_SHOW_ROUTINES
#include <bsp/vmeTsi148.h>
#include <libcpu/bat.h>

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
} VMEOpsRec, *VMEOps;

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
};

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
};

static VMEOps theOps = 0;

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

void
__BSP_default_vme_config(void)
{
int err = 1;
	if ( 0 == vmeUniverseInit() ) {
		theOps = &uniOpsRec;
		vmeUniverseReset();
	} else if ( 0 == vmeTsi148Init() ) {
		theOps = &tsiOpsRec;
		vmeTsi148Reset();
		_BSP_clear_vmebridge_errors = tsi_clear_errors;
	} else 
		return; /* no VME bridge found chip */

  /* map VME address ranges */
  BSP_VMEOutboundPortCfg(
	0,
	VME_AM_EXT_SUP_DATA,
	_VME_A32_WIN0_ON_VME,
	_VME_A32_WIN0_ON_PCI,
	0x0e000000
	);
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
  /* map our memory to VME */
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

  switch (BSP_getBoardType()) {
	case MVME6100:
		err = theOps->install_irq_mgr(
					VMETSI148_IRQ_MGR_FLAG_SHARED,
					0, BSP_IRQ_GPP_0 + 20,
					1, BSP_IRQ_GPP_0 + 21,
					2, BSP_IRQ_GPP_0 + 22,
					3, BSP_IRQ_GPP_0 + 23,
					-1);
	break;

	case MVME5500:
		err = theOps->install_irq_mgr(
					VMEUNIVERSE_IRQ_MGR_FLAG_SHARED |
					VMEUNIVERSE_IRQ_MGR_FLAG_PW_WORKAROUND,
					0, BSP_IRQ_GPP_0 + 12,
					1, BSP_IRQ_GPP_0 + 13,
					2, BSP_IRQ_GPP_0 + 14,
					3, BSP_IRQ_GPP_0 + 15,
					-1);
	break;

	default:
		printk("WARNING: unknown board; ");
	break;
  }
  if ( err )
	printk("VME interrupt manager NOT INSTALLED (error: %i)\n", err);
}
