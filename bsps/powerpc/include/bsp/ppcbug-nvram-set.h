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
 * PPCBug GEV in NVRAM.
 */

#ifndef _POWERPC_NVRAM_SET_h
#define _POWERPC_NVRAM_SET_h

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Set the NVRAM base. Setting the base will result in the NVRAM
 * variables being scanned and the environment table being set with
 * any value settings. The access can be direct or indirect.
 *
 * @param nvram_base Base address of the byte addressable NVRAM
 * @param nvram_indorect The NVRAM access indirect
 */
void ppcbug_nvram_set_nvbase(uintptr_t nvram_base, bool nvram_indirect);

/*
 * Set the RTEMS network unit for environment variables. You can set
 * up to 4 interfaces, 1 to 4. This call can be made in BSP start or
 * BSP early start.
 *
 * @param unit Network interface unit number, ie `1` for `enet1`
 */
void ppcbug_nvram_set_net_unit(const size_t unit);

/*
 * Set the RTEMS network unit label. You can set up to 4 interfaces, 1
 * to 4. This call can be made in BSP start or BSP early start.
 *
 * @param unit Network interface unit number, ie `1` for `enet1`
 * @param label Network interface unit label, ie `1` for `em0`
 */
void ppcbug_nvram_set_net_label(const size_t unit, const char* label);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
