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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static int isinit = 0;

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
int debug_uart_index __attribute__((weak)) = 0;
ambapp_apb_uart *dbg_uart = NULL;

/* Initialize the BSP system debug console layer. It will scan AMBA Plu&Play
 * for a debug APBUART and enable RX/TX for that UART.
 */
int bsp_debug_uart_init(void)
{
  int i;
  struct ambapp_dev *adev;
  struct ambapp_apb_info *apb;

  if (isinit == 0) {
    /* Update debug_uart_index to index used as debug console.
     * Let user select Debug console by setting debug_uart_index. If the
     * BSP is to provide the default UART (debug_uart_index==0):
     *   non-MP: APBUART[0] is debug console
     *   MP: LEON CPU index select UART
     */
    if (debug_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
      debug_uart_index = LEON3_Cpu_Index;
#else
      debug_uart_index = 0;
#endif
    } else {
      debug_uart_index = debug_uart_index - 1; /* User selected dbg-console */
    }

    /* Find APBUART core for System Debug Console */
    i = debug_uart_index;
    adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),
                                   VENDOR_GAISLER, GAISLER_APBUART,
                                   ambapp_find_by_idx, (void *)&i);
    if (adev) {
      /* Found a matching debug console, initialize debug uart if present
       * for printk
       */
      apb = (struct ambapp_apb_info *)adev->devinfo;
      dbg_uart = (ambapp_apb_uart *)apb->start;
      dbg_uart->ctrl |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE;
      dbg_uart->status = 0;
    }
    isinit = 1;
  }

  if (dbg_uart == NULL)
    return 0;
  else
    return 1;
}

/*
 *  apbuart_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void apbuart_outbyte_polled(
  ambapp_apb_uart *regs,
  unsigned char ch
)
{
  while ( (regs->status & LEON_REG_UART_STATUS_THE) == 0 );
  regs->data = (unsigned int) ch;
}

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */
int apbuart_inbyte_nonblocking(ambapp_apb_uart *regs)
{
  /* Clear errors */
  if (regs->status & LEON_REG_UART_STATUS_ERR)
    regs->status = ~LEON_REG_UART_STATUS_ERR;

  if ((regs->status & LEON_REG_UART_STATUS_DR) == 0)
    return EOF;
  else
    return (int) regs->data;
}

/* putchar/getchar for printk */
static void bsp_out_char(char c)
{
  if (dbg_uart == NULL)
    return;

  apbuart_outbyte_polled(dbg_uart, c);
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static int bsp_in_char(void)
{
  int tmp;

  if (dbg_uart == NULL)
    return EOF;

  while ((tmp = apbuart_inbyte_nonblocking(dbg_uart)) < 0)
    ;
  return tmp;
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;
