/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This header file provides interfaces of the
 *   GR-CPCI-LEON4-N2X (NGFP) PCI Peripheral driver.
 */

/*
 *  COPYRIGHT (c) 2013.
 *  Cobham Gaisler AB.
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
 *
 *  Configures the GR-CPIC-LEON4-N2X interface PCI board in peripheral
 *  mode. This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *
 *  Driver resource options:
 *   NAME          DEFAULT VALUE
 *   ahbmst2pci    _RAM_START            AMBA->PCI translation PCI base address
 *   ambaFreq      200000000 (200MHz)    AMBA system frequency of LEON4-N2X
 *   cgEnMask      0x1f (all)            Clock gating enable mask
 *
 * TODO/UNTESTED
 *   Interrupt testing
 *   bar0 RESOURCE 0x00000000            L2-Cache SDRAM memory
 *   bar1 RESOURCE 0xf0000000            L2-Cache registers
 */

#ifndef __GR_CPCI_LEON4_N2X_H__
#define __GR_CPCI_LEON4_N2X_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-CPCI-LEON4-N2X resources. The resources will be
 * used by the drivers controlling the cores on the GR-CPCI-LEON4-N2X target
 * AMBA bus.
 *
 * The gr_leon4_n2x_resources is declared weak so that the user can override the
 * default configuration. The array must be terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr_leon4_n2x_resources[];

/* Options to gr_cpci_leon4_n2x_print function */
#define GR_LEON4_N2X_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */

/* Print information about all GR-CPCI-LEON4-N2X PCI boards */
void gr_leon4_n2x_print(int options);

/* Print information about one GR-CPCI-LEON4-N2X PCI board */
void gr_cpci_leon4_n2x_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-CPCI-LEON4-N2X driver */
void gr_cpci_leon4_n2x_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
