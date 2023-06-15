/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the TTY driver for the GRLIb APBUART
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for GRLIB BSP.
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler.
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

#include <bsp.h>
#include <amba.h>
#include <rtems/libio.h>
#include <rtems/sysinit.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <grlib/apbuart.h>
#include <grlib/ambapp.h>
#include <grlib/io.h>

int grlib_debug_uart_index __attribute__((weak)) = 0;
apbuart *grlib_debug_uart = NULL;

/* Before UART driver has registered (or when no UART is available), calls to
 * printk that gets to bsp_out_char() will be filling data into the
 * pre_printk_dbgbuf[] buffer, hopefully the buffer can help debugging the
 * early BSP boot.. At least the last printk() will be caught.
 *
static char pre_printk_dbgbuf[32] = {0};
static int pre_printk_pos = 0;
 */

/* Initialize the BSP system debug console layer. It will scan AMBA Plu&Play
 * for a debug APBUART and enable RX/TX for that UART.
 */
static void bsp_debug_uart_init(void)
{
  int i;
  struct ambapp_dev *adev;
  struct ambapp_apb_info *apb;

  /* Update grlib_debug_uart_index to index used as debug console. Let user
   * select Debug console by setting grlib_debug_uart_index. If the BSP is to
   * provide the default UART (grlib_debug_uart_index==0):
   *   non-MP: APBUART[0] is debug console
   *   MP: LEON CPU index select UART
   */
  if (grlib_debug_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    grlib_debug_uart_index = GRLIB_Cpu_Index;
#else
    grlib_debug_uart_index = 0;
#endif
  } else {
    grlib_debug_uart_index--; /* User selected dbg-console */
  }

  /* Find APBUART core for System Debug Console */
  i = grlib_debug_uart_index;
  adev = (void *)ambapp_for_each(ambapp_plb(), (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_APBUART,
                                 ambapp_find_by_idx, (void *)&i);
  if (adev) {
    uint32_t ctrl;

    /* Found a matching debug console, initialize debug uart if present
     * for printk
     */
    apb = (struct ambapp_apb_info *)adev->devinfo;
    grlib_debug_uart = (apbuart *)apb->start;
    ctrl = grlib_load_32(&grlib_debug_uart->ctrl);
    ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
    grlib_store_32(&grlib_debug_uart->ctrl, ctrl);
    grlib_store_32(&grlib_debug_uart->status, 0);
  }
}

RTEMS_SYSINIT_ITEM(
  bsp_debug_uart_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FOURTH
);

/* putchar/getchar for printk */
static void bsp_out_char(char c)
{
  if (grlib_debug_uart == NULL) {
    uint32_t ctrl;

    /* Try to assign standard UART address to debug driver to pass some tests */
    grlib_debug_uart = (apbuart *) 0x80000100;
    ctrl = grlib_load_32(&grlib_debug_uart->ctrl);
    ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
    grlib_store_32(&grlib_debug_uart->ctrl, ctrl);
    grlib_store_32(&grlib_debug_uart->status, 0);
    /* Local debug buffer when UART driver has not registered */
    /*
    pre_printk_dbgbuf[pre_printk_pos++] = c;
    pre_printk_pos = pre_printk_pos & (sizeof(pre_printk_dbgbuf)-1);
    return;
    */
  }

  apbuart_outbyte_polled(grlib_debug_uart, c);
  apbuart_outbyte_wait(grlib_debug_uart);
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static int bsp_in_char(void)
{
  int tmp;

  if (grlib_debug_uart == NULL)
    return EOF;

  while ((tmp = apbuart_inbyte_nonblocking(grlib_debug_uart)) < 0)
    ;
  return tmp;
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;
