/* SPDX-License-Identifier: BSD-2-Clause */

/*  LEON3 GRLIB AMBA Plug & Play bus driver interface.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  This is driver is a wrapper for the general AMBA Plug & Play bus
 *  driver. This is the root bus driver for GRLIB systems.
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

#ifndef __AMBAPP_BUS_GRLIB_H__
#define __AMBAPP_BUS_GRLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

struct grlib_config {
	struct ambapp_bus	*abus;
	struct drvmgr_bus_res	*resources;
};

/* Register GRLIB AMBA PnP Bus as root bus at driver manager */
extern int ambapp_grlib_root_register(struct grlib_config *config);

/* Register bus driver to Driver Manager */
void ambapp_grlib_register(void);

#ifdef __cplusplus
}
#endif

#endif
