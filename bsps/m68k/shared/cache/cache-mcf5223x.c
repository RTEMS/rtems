/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <mcf5223x/mcf5223x.h>
#include "cache.h"

/*
 * Cannot be frozen
 */
static void _CPU_cache_freeze_data(void) {}
static void _CPU_cache_unfreeze_data(void) {}
static void _CPU_cache_freeze_instruction(void) {}
static void _CPU_cache_unfreeze_instruction(void) {}

/*
 * Write-through data cache -- flushes are unnecessary
 */
static void _CPU_cache_flush_1_data_line(const void *d_addr) {}
static void _CPU_cache_flush_entire_data(void) {}

static void _CPU_cache_enable_instruction(void) {}
static void _CPU_cache_disable_instruction(void) {}
static void _CPU_cache_invalidate_entire_instruction(void) {}
static void _CPU_cache_invalidate_1_instruction_line(const void *addr) {}

static void _CPU_cache_enable_data(void) {}
static void _CPU_cache_disable_data(void) {}
static void _CPU_cache_invalidate_entire_data(void) {}
static void _CPU_cache_invalidate_1_data_line(const void *addr) {}

#include "../../../shared/cache/cacheimpl.h"
