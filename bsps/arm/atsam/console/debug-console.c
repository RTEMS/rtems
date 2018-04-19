/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bspIo.h>
#include <rtems/sysinit.h>

#include <bsp/atsam-clock-config.h>
#include <chip.h>
#include <include/dbg_console.h>

static void atsam_debug_console_out(char c)
{
  DBG_PutChar((uint8_t) c);
}

static void atsam_debug_console_init(void)
{
  DBG_Configure(115200, BOARD_MCK);
  BSP_output_char = atsam_debug_console_out;
}

static void atsam_debug_console_early_init(char c)
{
  atsam_debug_console_init();
  atsam_debug_console_out(c);
}

static int atsam_debug_console_in(void)
{
  return (int) DBG_GetChar();
}

BSP_output_char_function_type BSP_output_char = atsam_debug_console_early_init;

BSP_polling_getchar_function_type BSP_poll_char = atsam_debug_console_in;

RTEMS_SYSINIT_ITEM(
  atsam_debug_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
