/* SPDX-License-Identifier: BSD-2-Clause */

/*  GR-TMTC-1553 PCI Target driver.
 * 
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-TMTC-1553 interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr_tmtc_1553_set_resources().
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

#ifndef __GR_TMTC_1553_H__
#define __GR_TMTC_1553_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-TMTC-1553 resources. The resources will be
 * used by the drivers controlling the cores on the GR-TMTC-1553 target AMBA bus.
 *
 * The gr_rasta_io_resources is declared weak so that the user can override the
 * default configuration. The array must be terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr_tmtc_1553_resources[];

/* Options to gr_rasta_io_print function */
#define TMTC_1553_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */
#define TMTC_1553_OPTIONS_IRQ    0x02 /* Print current IRQ setup */

/* Print information about GR-RASTA-IO PCI board */
void gr_tmtc_1553_print(int options);

/* Register GR-RASTA-IO driver */
void gr_tmtc_1553_register_drv(void);

/* Print information about all GR-TMTC-1553 devices registered to this driver */
void gr_tmtc_1553_print(int options);

/* Print information about one GR-TMTC-1553 device */
void gr_tmtc_1553_print_dev(struct drvmgr_dev *dev, int options);

#ifdef __cplusplus
}
#endif

#endif
