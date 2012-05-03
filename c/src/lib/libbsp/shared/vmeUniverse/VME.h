#ifndef RTEMS_BSP_VME_API_H
#define RTEMS_BSP_VME_API_H

/* SVGM et al. BSP's VME support */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002,
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

#include <stdio.h>

/* address modifiers & friends */
#include <bsp/vme_am_defs.h>

/* VME related declarations */

/*
 * BSP-specific configuration routine; sets up
 * VME windows and installs the VME interrupt manager.
 */
void BSP_vme_config();

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

/* Assert VME SYSRESET */
void
BSP_VMEResetBus(void);

#endif
