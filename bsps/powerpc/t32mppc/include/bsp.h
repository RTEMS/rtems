/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCT32MPPC
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_POWERPC_T32MPPC_BSP_H
#define LIBBSP_POWERPC_T32MPPC_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCT32MPPC t32mppc
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief Lauterbach Trace32 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

extern uint32_t bsp_time_base_frequency;

void t32mppc_decrementer_dispatch(void);

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* LIBBSP_POWERPC_T32MPPC_BSP_H */
