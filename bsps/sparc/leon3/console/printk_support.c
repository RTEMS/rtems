/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the definition of ::BSP_output_char and
 *   ::BSP_poll_char.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
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
#include <bsp/leon3.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <grlib/apbuart.h>
#include <grlib/io.h>

#if !defined(LEON3_APBUART_BASE)
#include <grlib/ambapp.h>

int leon3_debug_uart_index __attribute__((weak)) = 0;

apbuart *leon3_debug_uart = NULL;
#endif

static void bsp_debug_uart_init(void);

static void apbuart_enable_receive_and_transmit(apbuart *regs)
{
  uint32_t ctrl;

  ctrl = grlib_load_32(&regs->ctrl);
  ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
  grlib_store_32(&regs->ctrl, ctrl);
  grlib_store_32(&regs->status, 0);
}

static void bsp_debug_uart_output_char(char c)
{
  apbuart_outbyte_polled(leon3_debug_uart, c);
  apbuart_outbyte_wait(leon3_debug_uart);
}

static int bsp_debug_uart_poll_char(void)
{
  return apbuart_inbyte_nonblocking(leon3_debug_uart);
}

static void bsp_debug_uart_pre_init_out(char c)
{
  bsp_debug_uart_init();
  (*BSP_output_char)(c);
}

#if defined(LEON3_APBUART_BASE)

static void bsp_debug_uart_init(void)
{
  apbuart_enable_receive_and_transmit(leon3_debug_uart);
  BSP_poll_char = bsp_debug_uart_poll_char;
  BSP_output_char = bsp_debug_uart_output_char;
}

#else /* !LEON3_APBUART_BASE */

static void bsp_debug_uart_discard(char c)
{
  (void) c;
}

/* Initialize the BSP system debug console layer. It will scan AMBA Plu&Play
 * for a debug APBUART and enable RX/TX for that UART.
 */
static void bsp_debug_uart_init(void)
{
  int i;
  struct ambapp_dev *adev;

  if ( BSP_output_char != bsp_debug_uart_pre_init_out ) {
    return;
  }

  BSP_output_char = bsp_debug_uart_discard;

  /* Update leon3_debug_uart_index to index used as debug console. Let user
   * select Debug console by setting leon3_debug_uart_index. If the BSP is to
   * provide the default UART (leon3_debug_uart_index==0):
   *   non-MP: APBUART[0] is debug console
   *   MP: LEON CPU index select UART
   */
  if (leon3_debug_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    leon3_debug_uart_index = LEON3_Cpu_Index;
#else
    leon3_debug_uart_index = 0;
#endif
  } else {
    leon3_debug_uart_index--; /* User selected dbg-console */
  }

  /* Find APBUART core for System Debug Console */
  i = leon3_debug_uart_index;
  adev = (void *)ambapp_for_each(ambapp_plb(), (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_APBUART,
                                 ambapp_find_by_idx, (void *)&i);
  if (adev != NULL) {
    struct ambapp_apb_info *apb;

    /*
     * Found a matching debug console, initialize debug UART if present for
     * printk().
     */
    apb = (struct ambapp_apb_info *)adev->devinfo;
    leon3_debug_uart = (apbuart *)apb->start;
    apbuart_enable_receive_and_transmit(leon3_debug_uart);

    BSP_poll_char = bsp_debug_uart_poll_char;
    BSP_output_char = bsp_debug_uart_output_char;
  }
}

#endif /* LEON3_APBUART_BASE */

RTEMS_SYSINIT_ITEM(
  bsp_debug_uart_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FOURTH
);

BSP_output_char_function_type BSP_output_char = bsp_debug_uart_pre_init_out;

BSP_polling_getchar_function_type BSP_poll_char;
