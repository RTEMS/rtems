/* SPDX-License-Identifier: BSD-2-Clause */

/*  GR-RASTA-SPW-ROUTER PCI Peripheral driver
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
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
 */

#ifndef __GR_RASTA_SPW_ROUTER_H__
#define __GR_RASTA_SPW_ROUTER_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-RASTA-SPW-ROUTER resources. The resources will be
 * used by the drivers controlling the cores on the GR-RASTA-SPW-ROUTER target
 * AMBA bus.
 *
 * The gr_rasta_spw_router_resources is declared weak so that the user can
 * override the default configuration. The array must be terminated with a
 * NULL resource.
 */
extern struct drvmgr_bus_res *gr_rasta_spw_router_resources[];

/* Options to gr_rasta_spw_router_print function */
#define RASTA_SPW_ROUTER_OPTIONS_AMBA  0x01 /* Print AMBA bus devices */
#define RASTA_SPW_ROUTER_OPTIONS_IRQ   0x02 /* Print current IRQ setup */

/* Print information about all GR-CPCI-LEON4-N2X PCI boards */
void gr_rasta_spw_router_print(int options);

/* Print information about one GR-CPCI-LEON4-N2X PCI board */
void gr_rasta_spw_router_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-RASTA-SPW-ROUTER driver to Driver Manager */
void gr_rasta_spw_router_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
