/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2013. Hesham AL-Matary
 * Copyright (c) 2013 by Alan Cudmore
 * based on work by:
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE
 */

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/raspberrypi.h>
#include <bsp/mm.h>

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
}


void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  bsp_memory_management_initialize();
  bsp_start_clear_bss();
}
