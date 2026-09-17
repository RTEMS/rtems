/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup powerpc_vmeconfig
 *
 *  @brief mvme3100 BSP specific address space configuration parameters
 */

/*
 * Copyright (C) 2002 - 2007 Till Straumann <strauman@slac.stanford.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H

/*
 * NOTE: the BSP (startup/bspstart.c) uses
 * hardcoded window lengths that match this
 * layout:
 */

#define _VME_A32_WIN0_ON_PCI	0xc0000000
#define _VME_CSR_ON_PCI			0xce000000
#define _VME_A24_ON_PCI			0xcf000000
#define _VME_A16_ON_PCI			0xcfff0000

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a run-time configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map the board RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0xc0000000

/* If your BSP requires a non-standard way to configure
 * the VME interrupt manager then define the symbol
 *
 * BSP_VME_INSTALL_IRQ_MGR
 *
 * to a proper instruction sequence that installs the
 * universe interrupt manager. This requires knowledge
 * of the wiring between the universe and the PIC (main
 * interrupt controller), i.e., which IRQ 'pins' of the
 * universe are wired to which 'lines'/inputs at the PIC.
 * (consult vmeUniverse.h for more information).
 *
 * When installing the universe IRQ manager it is also
 * possible to specify whether it should try to share
 * PIC interrupts with other sources. This might not
 * be supported by all BSPs (but the unverse driver
 * recognizes that).
 *
 * If BSP_VME_INSTALL_IRQ_MGR is undefined then
 * the default algorithm is used (vme_universe.c):
 *
 * This default setup uses only a single wire. It reads
 * the PIC 'line' from PCI configuration space and assumes
 * this to be wired to the first (LIRQ0) IRQ input at the
 * universe. The default setup tries to use interrupt
 * sharing.
 */

extern int BSP_VMEInit(void);
extern int BSP_VMEIrqMgrInstall(void);

/**
 *  @defgroup powerpc_vme BSP_VME_INSTALL_IRQ_MGR Support
 *
 *  @ingroup RTEMSBSPsPowerPCMVME3100
 *
 *  @brief BSP_VME_INSTALL_IRQ_MGR Support Package
 */
#define BSP_VME_INSTALL_IRQ_MGR(err)    \
	do {                                \
		err = vmeTsi148InstallIrqMgrAlt(\
			VMETSI148_IRQ_MGR_FLAG_SHARED, /* use shared IRQs */ \
			0, BSP_VME0_IRQ,            \
			1, BSP_VME1_IRQ,            \
			2, BSP_VME2_IRQ,            \
			3, BSP_VME3_IRQ,            \
			-1 /* terminate list  */    \
		);                              \
	} while (0)

/* This BSP uses the Tsi148 Driver */
#define _VME_DRIVER_TSI148

#endif
