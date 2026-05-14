/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2026 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * MOTLoad GEV in NVRAM.
 */

#ifndef _POWERPC_MOTLOAD_GEV_NVRAM_h
#define _POWERPC_MOTLOAD_GEV_NVRAM_h

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Set the NVRAM base. Setting the base will result in the GEV
 * variables being scanned and the environment table being set with
 * any value settings.
 *
 * @param nvram_base Base address if the byte addressable NVRAM
 */
void motload_gev_set_nvbase(uintptr_t nvram_base);

/*
 * Set the RTEMS network unit for environment vairables. You can set
 * up to 4 interfaces, 0 to 3. This call can be made in BSP start or
 * BSP early start.
 *
 * For example if the GEV variable "mot-/dev/enet1-cipa" is set and
 * this call is made with the unit set 1 the environment variable
 * "RTEMS_NET_IF_1_IP_ADDR" will be set to the IP address of the GEV
 * variable.
 *
 * @param unit Network interface unit number, ie `0` for `enet0`
 */
void motload_gev_set_net_unit(const size_t unit);

/*
 * Set the RTEMS network unit label. You can set up to 4 interfaces, 0
 * to 3. This call can be made in BSP start or BSP early start.
 *
 * @param unit Network interface unit number, ie `0` for `enet0`
 * @param label Network interface unit label, ie `0` for `em0`
 */
void motload_gev_set_net_label(const size_t unit, const char* label);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
