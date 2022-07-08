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
#include <mcf5235/mcf5235.h>
#include "cache.h"

/*
 *  Default value for the cacr is set by the BSP
 */
extern uint32_t cacr_mode;

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

static void _CPU_cache_enable_instruction(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode &= ~MCF5XXX_CACR_DIDI;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

static void _CPU_cache_disable_instruction(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode |= MCF5XXX_CACR_DIDI;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

static void _CPU_cache_invalidate_entire_instruction(void)
{
    m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVI);
}

static void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
    /*
     * Top half of cache is I-space
     */
    addr = (void *)((int)addr | 0x400);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

static void _CPU_cache_enable_data(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode &= ~MCF5XXX_CACR_DISD;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

static void _CPU_cache_disable_data(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    cacr_mode |= MCF5XXX_CACR_DISD;
    m68k_set_cacr(cacr_mode);
    rtems_interrupt_enable(level);
}

static void _CPU_cache_invalidate_entire_data(void)
{
    m68k_set_cacr(cacr_mode | MCF5XXX_CACR_CINV | MCF5XXX_CACR_INVD);
}

static void _CPU_cache_invalidate_1_data_line(const void *addr)
{
    /*
     * Bottom half of cache is D-space
     */
    addr = (void *)((int)addr & ~0x400);
    __asm__ volatile ("cpushl %%bc,(%0)" :: "a" (addr));
}

#include "../../../shared/cache/cacheimpl.h"
