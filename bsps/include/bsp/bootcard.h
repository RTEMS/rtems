/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedStartup
 */

/*
 * Copyright (c) 2008-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_BOOTCARD_H
#define LIBBSP_SHARED_BOOTCARD_H

#include <rtems/config.h>
#include <rtems/bspIo.h>
#include <rtems/malloc.h>
#include <rtems/score/memory.h>
#include <rtems/score/wkspace.h>

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
void boot_card(const char *cmdline) RTEMS_NO_RETURN;

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
