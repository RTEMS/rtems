/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#
BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  bsp_start_clear_bss();
}
