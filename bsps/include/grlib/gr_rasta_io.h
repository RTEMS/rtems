/* SPDX-License-Identifier: BSD-2-Clause */

/*  GR-RASTA-IO PCI Target driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-RASTA-IO interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr_rasta_io_set_resources().
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

#ifndef __GR_RASTA_IO_H__
#define __GR_RASTA_IO_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-RASTA-IO resources. The resources will be
 * used by the drivers controlling the cores on the GR-RASTA-IO target AMBA bus.
 *
 * The gr_rasta_io_resources is declared weak so that the user can override the
 * default configuration. The array must be terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr_rasta_io_resources[];

/* Options to gr_rasta_io_print function */
#define RASTA_IO_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */
#define RASTA_IO_OPTIONS_IRQ    0x02 /* Print current IRQ setup */

/* Print information about GR-RASTA-IO PCI boards */
void gr_rasta_io_print(int options);

/* Print information about a GR-RASTA-IO PCI board */
void gr_rasta_io_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-RASTA-IO driver */
void gr_rasta_io_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
