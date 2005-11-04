#ifndef RTEMS_BSP_VME_API_H
#define RTEMS_BSP_VME_API_H
/* $Id$ */

/* SVGM et al. BSP's VME support */
/* Author: Till Straumann, <strauman@slac.stanford.edu> */

#include <stdio.h>

/* address modifiers & friends */
#include <bsp/vme_am_defs.h>

/* VME related declarations */

/*
 * BSP-specific configuration routine; sets up
 * VME windows and installs the VME interrupt manager.
 */
void BSP_vme_config() __attribute__((weak));

/* translate through host bridge and vme master window of vme bridge */
int
BSP_vme2local_adrs(unsigned long am, unsigned long vmeaddr, unsigned long *plocaladdr);

/* how a CPU address is mapped to the VME bus (if at all) */
int
BSP_local2vme_adrs(unsigned long am, unsigned long localaddr, unsigned long *pvmeaddr);

/* interrupt handlers and levels */
typedef void (*BSP_VME_ISR_t)(void *usrArg, unsigned long vector);

int
BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg);
int
BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *arg);

/* retrieve the currently installed ISR for a given vector */
BSP_VME_ISR_t
BSP_getVME_isr(unsigned long vector, void **parg);

int
BSP_enableVME_int_lvl(unsigned int level);

int
BSP_disableVME_int_lvl(unsigned int level);

int
BSP_VMEOutboundPortCfg(
	unsigned long port,
	unsigned long address_space,
	unsigned long vme_address,
	unsigned long pci_address,
	unsigned long size);

int
BSP_VMEInboundPortCfg(
	unsigned long port,
	unsigned long address_space,
	unsigned long vme_address,
	unsigned long pci_address,
	unsigned long size);

void
BSP_VMEOutboundPortsShow(FILE *f);

void
BSP_VMEInboundPortsShow(FILE *f);

#endif
