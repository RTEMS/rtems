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
 * MOTLoad RTEMS Network Environment Variables
 *
 * Set the environment variables based on the GEV settings.  The
 * pattern for scanning follows the parsing that was in EPICS Base's
 * `setBootConfigFromNVRAM.c`. The code was moved to RTEMS as part of
 * a clean up of EPICS code.
 */

#ifndef _POWERPC_MOTLOAD_GEV_NET_ENV_h
#define _POWERPC_MOTLOAD_GEV_NET_ENV_h

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Set the RTEMS network environment vairables from GEV variables for
 * the specified network interface
 *
 * @param nvram_base Base address if the byte addressable NVRAM
 * @param unit Network interface unit number, ie `0` for `enet0`
 * @param label Network interface unit label, ie `0` for `em0`
 */
void motload_gev_set_net_envs(uintptr_t nvram_base, const size_t unit,
                              const char* label);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
