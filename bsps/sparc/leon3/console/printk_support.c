/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <leon.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <rtems/score/thread.h>
#include <grlib/apbuart.h>

int leon3_debug_uart_index __attribute__((weak)) = 0;
struct apbuart_regs *leon3_debug_uart = NULL;

/*
 * Before UART driver is available, use the idle stack to buffer early uses of
 * printk().
 */
static size_t bsp_debug_uart_pre_init_buf_index;

static void bsp_debug_uart_pre_init_out(char c)
{
  size_t i;

  i = bsp_debug_uart_pre_init_buf_index;

  if (i < _Thread_Idle_stack_size) {
    bsp_debug_uart_pre_init_buf_index = i + 1;
    _Thread_Idle_stacks[i] = c;
  }
}

static void bsp_debug_uart_discard(char c)
{
  (void) c;
}

static void bsp_debug_uart_output_char(char c)
{
  apbuart_outbyte_polled(leon3_debug_uart, c, 1, 1);
}

static int bsp_debug_uart_poll_char(void)
{
  return apbuart_inbyte_nonblocking(leon3_debug_uart);
}

/* Initialize the BSP system debug console layer. It will scan AMBA Plu&Play
 * for a debug APBUART and enable RX/TX for that UART.
 */
static void bsp_debug_uart_init(void)
{
  int i;
  struct ambapp_dev *adev;
  struct ambapp_apb_info *apb;

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
  adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                 VENDOR_GAISLER, GAISLER_APBUART,
                                 ambapp_find_by_idx, (void *)&i);
  if (adev != NULL) {
    size_t i;
    size_t n;

    /*
     * Found a matching debug console, initialize debug UART if present for
     * printk().
     */
    apb = (struct ambapp_apb_info *)adev->devinfo;
    leon3_debug_uart = (struct apbuart_regs *)apb->start;
    leon3_debug_uart->ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
    leon3_debug_uart->status = 0;

    BSP_poll_char = bsp_debug_uart_poll_char;
    BSP_output_char = bsp_debug_uart_output_char;

    n = bsp_debug_uart_pre_init_buf_index;

    for (i = 0; i < n; ++i) {
      rtems_putc(_Thread_Idle_stacks[i]);
    }
  } else {
    BSP_output_char = bsp_debug_uart_discard;
  }
}

RTEMS_SYSINIT_ITEM(
  bsp_debug_uart_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FOURTH
);

BSP_output_char_function_type BSP_output_char = bsp_debug_uart_pre_init_out;

BSP_polling_getchar_function_type BSP_poll_char;
