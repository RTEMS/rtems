/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libchip/serial.h>

#include <rtems/bspIo.h>

static void output_char(char c)
{
  int minor = (int) Console_Port_Minor;
  const console_tbl *ct = Console_Port_Tbl != NULL ?
    Console_Port_Tbl[minor] : &Console_Configuration_Ports[minor];
  const console_fns *cf = ct->pDeviceFns;

  (*cf->deviceWritePolled)(minor, c);
}

static void output_char_init(char c)
{
  if (Console_Port_Tbl == NULL) {
    int minor = (int) Console_Port_Minor;
    const console_fns *cf = Console_Configuration_Ports[minor].pDeviceFns;

    (*cf->deviceInitialize)(minor);
  }

  BSP_output_char = output_char;
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
