/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedStartup
 */

/*
 * Copyright (C) 2008, 2014 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_SHARED_BOOTCARD_H
#define LIBBSP_SHARED_BOOTCARD_H

#include <rtems/config.h>
#include <rtems/bspIo.h>
#include <rtems/malloc.h>
#include <rtems/score/memory.h>

#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsSharedStartup Bootcard
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief Standard system startup.
 *
 * @{
 */

/**
 * @brief Global pointer to the command line of boot_card().
 */
extern const char *bsp_boot_cmdline;

void bsp_start(void);

void bsp_reset(void);

/**
 * @brief Standard system initialization procedure.
 *
 * You may pass a command line in @a cmdline.  It is later available via the
 * global @ref bsp_boot_cmdline variable.
 *
 * This is the C entry point for ALL RTEMS BSPs.  It is invoked from the
 * assembly language initialization file usually called @c start.S which does
 * the basic CPU setup (stack, C runtime environment, zero BSS, load other
 * sections) and calls afterwards boot_card().  The boot card function provides
 * the framework for the BSP initialization sequence.  For the basic flow of
 * initialization see RTEMS C User's Guide, Initialization Manager.
 *
 * This style of initialization ensures that the C++ global constructors are
 * executed after RTEMS is initialized.
 */
RTEMS_NO_RETURN void boot_card(const char *cmdline);

struct Per_CPU_Control;

/**
 * @brief Standard start routine for secondary processors.
 *
 * This function is usually called by low-level startup code of secondary
 * processors or boot loaders starting a secondary processor.  The final step
 * of this function is a call to
 * _SMP_Start_multitasking_on_secondary_processor().
 */
void bsp_start_on_secondary_processor(struct Per_CPU_Control *cpu_self);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_BOOTCARD_H */
