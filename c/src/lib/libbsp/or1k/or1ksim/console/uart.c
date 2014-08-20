/**
 * @file
 *
 * @ingroup or1ksim_uart
 *
 * @brief UART support.
 */

/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#include <libchip/sersupp.h>
#include <bsp/or1ksim.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/uart.h>
#include <rtems/score/isr.h>

static rtems_vector_number uart_get_irq_number(const console_tbl *ct)
{
   return ct->ulIntVector;
}

static uint32_t uart_get_baud(const console_tbl *ct)
{
   return ct->ulClock;
}

static void uart_set_baud(int baud)
{
  int divisor = (OR1KSIM_BSP_CLOCK_FREQ) / (16 * baud);
  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_LINE_CTRL) |=
    OR1KSIM_BSP_UART_REG_LINE_CTRL_DLAB;

  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_DEV_LATCH_LOW) = divisor & 0xff;

  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_DEV_LATCH_HIGH) =
    (divisor >> 8) & 0xff;

  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_LINE_CTRL) &=
    ~(OR1KSIM_BSP_UART_REG_LINE_CTRL_DLAB);
}

static void uart_initialize(int minor)
{
  /* Disable all interrupts */
  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_INT_ENABLE) = 0x00;

  /* Reset receiver and transmitter */
   OR1KSIM_REG(OR1KSIM_BSP_UART_REG_FIFO_CTRL) =
     OR1KSIM_BSP_UART_REG_FIFO_CTRL_ENABLE_FIFO |
     OR1KSIM_BSP_UART_REG_FIFO_CTRL_CLEAR_RCVR  |
     OR1KSIM_BSP_UART_REG_FIFO_CTRL_CLEAR_XMIT  |
     OR1KSIM_BSP_UART_REG_FIFO_CTRL_TRIGGER_14;

  /* Set data pattern configuration */
  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_LINE_CTRL) =
    OR1KSIM_BSP_UART_REG_LINE_CTRL_WLEN8 &
      (OR1KSIM_BSP_UART_REG_LINE_CTRL_STOP |
       OR1KSIM_BSP_UART_REG_LINE_CTRL_PARITY);

  /* Set baud rate */
  uart_set_baud(OR1KSIM_UART_DEFAULT_BAUD);
}

static int uart_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, ct->ulClock);

  return 0;
}

static int uart_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int uart_read_polled(int minor)
{
   return -1;
}

static void uart_write_polled(int minor, char c)
{
  unsigned char lsr;
  const uint32_t transmit_finished =
    (OR1KSIM_BSP_UART_REG_LINE_STATUS_TEMT |
     OR1KSIM_BSP_UART_REG_LINE_STATUS_THRE);

  /* Wait until there is no pending data in the transmitter FIFO (empty) */
  do {
      lsr = OR1KSIM_REG(OR1KSIM_BSP_UART_REG_LINE_STATUS);
  } while (!(lsr & OR1KSIM_BSP_UART_REG_LINE_STATUS_THRE));

  OR1KSIM_REG(OR1KSIM_BSP_UART_REG_TX) = c;

  /* Wait until trasmit data is finished */
  do {
      lsr = OR1KSIM_REG(OR1KSIM_BSP_UART_REG_LINE_STATUS);
  } while ( (lsr & transmit_finished) != transmit_finished );
}

static ssize_t uart_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i){
    uart_write_polled(minor, s [i]);
  }

  return n;
}

static int uart_set_attributes(int minor, const struct termios *term)
{
  return -1;
}

const console_fns or1ksim_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = uart_first_open,
  .deviceLastClose = uart_last_close,
  .deviceRead = uart_read_polled,
  .deviceWrite = uart_write_support_polled,
  .deviceInitialize = uart_initialize,
  .deviceWritePolled = uart_write_polled,
  .deviceSetAttributes = uart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
