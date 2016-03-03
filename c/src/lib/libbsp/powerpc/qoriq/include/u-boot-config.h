/*
 * Copyright (c) 2010-2015 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_POWERPC_QORIQ_U_BOOT_CONFIG_H
#define LIBBSP_POWERPC_QORIQ_U_BOOT_CONFIG_H

#include <bspopts.h>

#define U_BOOT_BOARD_INFO_DATA_SECTION __attribute__((section(".bsp_start_data")))

#define CONFIG_E500
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2

#endif /* LIBBSP_POWERPC_QORIQ_U_BOOT_CONFIG_H */
