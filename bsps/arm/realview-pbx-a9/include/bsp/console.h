/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_REALVIEW_PBX_A9_BSP_CONSOLE_H
#define LIBBSP_ARM_REALVIEW_PBX_A9_BSP_CONSOLE_H

#include <dev/serial/arm-pl011.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern arm_pl011_context rvpbx_pl011_context;

bool rvpbx_pl011_probe(rtems_termios_device_context *base);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_REALVIEW_PBX_A9_BSP_CONSOLE_H */
