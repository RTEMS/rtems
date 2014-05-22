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

#include <bsp/zynq-uart.h>
#include <bsp/zynq-uart-regs.h>

#include <bspopts.h>

#include <libchip/sersupp.h>

static volatile zynq_uart *zynq_uart_get_regs(int minor)
{
  const console_tbl *ct = Console_Port_Tbl != NULL ?
    Console_Port_Tbl[minor] : &Console_Configuration_Ports[minor];

  return (volatile zynq_uart *) ct->ulCtrlPort1;
}

/*
 * Make weak and let the user override.
 */
uint32_t zynq_uart_input_clock(void) __attribute__ ((weak));

uint32_t zynq_uart_input_clock(void)
{
  return BSP_ARM_A9MPCORE_UARTCLK;
}

static int zynq_cal_baud_rate(uint32_t  baudrate,
                              uint32_t* brgr,
                              uint32_t* bauddiv,
                              uint32_t  modereg)
{
  uint32_t brgr_value;    /* Calculated value for baud rate generator */
  uint32_t calcbaudrate;  /* Calculated baud rate */
  uint32_t bauderror;     /* Diff between calculated and requested baud rate */
  uint32_t best_error = 0xFFFFFFFF;
  uint32_t percenterror;
  uint32_t bdiv;
  uint32_t inputclk = zynq_uart_input_clock();

  /*
   * Make sure the baud rate is not impossilby large.
   * Fastest possible baud rate is Input Clock / 2.
   */
  if ((baudrate * 2) > inputclk) {
    return -1;
  }
  /*
   * Check whether the input clock is divided by 8
   */
  if(modereg & ZYNQ_UART_MODE_CLKS) {
    inputclk = inputclk / 8;
  }

  /*
   * Determine the Baud divider. It can be 4to 254.
   * Loop through all possible combinations
   */
  for (bdiv = 4; bdiv < 255; bdiv++) {

    /*
     * Calculate the value for BRGR register
     */
    brgr_value = inputclk / (baudrate * (bdiv + 1));

    /*
     * Calculate the baud rate from the BRGR value
     */
    calcbaudrate = inputclk/ (brgr_value * (bdiv + 1));

    /*
     * Avoid unsigned integer underflow
     */
    if (baudrate > calcbaudrate) {
      bauderror = baudrate - calcbaudrate;
    }
    else {
      bauderror = calcbaudrate - baudrate;
    }

    /*
     * Find the calculated baud rate closest to requested baud rate.
     */
    if (best_error > bauderror) {
      *brgr = brgr_value;
      *bauddiv = bdiv;
      best_error = bauderror;
    }
  }

  /*
   * Make sure the best error is not too large.
   */
  percenterror = (best_error * 100) / baudrate;
#define XUARTPS_MAX_BAUD_ERROR_RATE		 3	/* max % error allowed */
  if (XUARTPS_MAX_BAUD_ERROR_RATE < percenterror) {
    return -1;
  }

  return 0;
}

static void zynq_uart_initialize(int minor)
{
  volatile zynq_uart *regs = zynq_uart_get_regs(minor);
  uint32_t brgr = 0x3e;
  uint32_t bauddiv = 0x6;

  zynq_cal_baud_rate(115200, &brgr, &bauddiv, regs->mode);

  regs->control &= ~(ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN);
  regs->control = ZYNQ_UART_CONTROL_RXDIS
    | ZYNQ_UART_CONTROL_TXDIS
    | ZYNQ_UART_CONTROL_RXRES
    | ZYNQ_UART_CONTROL_TXRES;
  regs->mode = ZYNQ_UART_MODE_CHMODE(ZYNQ_UART_MODE_CHMODE_NORMAL)
    | ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_NONE)
    | ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_8);
  regs->baud_rate_gen = ZYNQ_UART_BAUD_RATE_GEN_CD(brgr);
  regs->baud_rate_div = ZYNQ_UART_BAUD_RATE_DIV_BDIV(bauddiv);
  regs->rx_fifo_trg_lvl = ZYNQ_UART_RX_FIFO_TRG_LVL_RTRIG(0);
  regs->rx_timeout = ZYNQ_UART_RX_TIMEOUT_RTO(0);
  regs->control = ZYNQ_UART_CONTROL_RXEN
    | ZYNQ_UART_CONTROL_TXEN
    | ZYNQ_UART_CONTROL_RSTTO;
}

static int zynq_uart_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_data *cd = &Console_Port_Data[minor];
  const console_tbl *ct = Console_Port_Tbl[minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, (rtems_termios_baud_t) ct->pDeviceParams);

  return 0;
}

static int zynq_uart_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int zynq_uart_read_polled(int minor)
{
  volatile zynq_uart *regs = zynq_uart_get_regs(minor);

  if ((regs->channel_sts & ZYNQ_UART_CHANNEL_STS_REMPTY) != 0) {
    return -1;
  } else {
    return ZYNQ_UART_TX_RX_FIFO_FIFO_GET(regs->tx_rx_fifo);
  }
}

static void zynq_uart_write_polled(int minor, char c)
{
  volatile zynq_uart *regs = zynq_uart_get_regs(minor);

  while ((regs->channel_sts & ZYNQ_UART_CHANNEL_STS_TFUL) != 0) {
    /* Wait */
  }

  regs->tx_rx_fifo = ZYNQ_UART_TX_RX_FIFO_FIFO(c);
}

static ssize_t zynq_uart_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    zynq_uart_write_polled(minor, s[i]);
  }

  return n;
}

static int zynq_uart_set_attribues(int minor, const struct termios *term)
{
#if 0
  volatile zynq_uart *regs = zynq_uart_get_regs(minor);
  uint32_t brgr = 0;
  uint32_t bauddiv = 0;
  int rc;

  rc = zynq_cal_baud_rate(115200, &brgr, &bauddiv, regs->mode);
  if (rc != 0)
    return rc;

  regs->control &= ~(ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN);
  regs->baud_rate_gen = ZYNQ_UART_BAUD_RATE_GEN_CD(brgr);
  regs->baud_rate_div = ZYNQ_UART_BAUD_RATE_DIV_BDIV(bauddiv);
  regs->control |= ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN;

  return 0;
#else
  return -1;
#endif
}

const console_fns zynq_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = zynq_uart_first_open,
  .deviceLastClose = zynq_uart_last_close,
  .deviceRead = zynq_uart_read_polled,
  .deviceWrite = zynq_uart_write_support_polled,
  .deviceInitialize = zynq_uart_initialize,
  .deviceWritePolled = zynq_uart_write_polled,
  .deviceSetAttributes = zynq_uart_set_attribues,
  .deviceOutputUsesInterrupts = false
};
