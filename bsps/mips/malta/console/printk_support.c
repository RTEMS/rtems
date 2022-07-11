/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This file contains a stub for the required printk support.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include <rtems/bspIo.h>
#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>

rtems_device_minor_number         BSPPrintkPort = 0;

void BSP_com_outch(char ch);
int BSP_com_inch( void );

/*
 *  Following assume all are ns16650
 */
void BSP_com_outch(char ch)
{
  console_tbl                   *cptr;

  cptr = &Console_Configuration_Ports[BSPPrintkPort];

  return ns16550_outch_polled( cptr, ch );
}

int BSP_com_inch( void )
{
  int           result;
  console_tbl   *cptr;

  cptr = &Console_Configuration_Ports[BSPPrintkPort];

  do {
    result = ns16550_inch_polled( cptr );
  } while (result == -1);

  return result;
}

BSP_output_char_function_type     BSP_output_char = BSP_com_outch;
BSP_polling_getchar_function_type BSP_poll_char = BSP_com_inch;
