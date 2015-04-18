/**
 * @file
 *
 * @ingroup generic_or1k_uart
 *
 * @brief UART support.
 */

/*
 * COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#include <libchip/sersupp.h>
#include <bsp/generic_or1k.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/uart.h>
#include <rtems/score/isr.h>

static void uart_initialize(int minor);
static int  uart_first_open(int major, int minor, void *arg);
static int  uart_last_close(int major, int minor, void *arg);
static int  uart_read_polled(int minor);
static ssize_t uart_write(int minor, const char *buf, size_t len);
static void uart_write_polled(int minor, char c);
static int  uart_set_attributes(int minor, const struct termios *t);

#if 0
/*
 *  These will be useful when the driver supports interrupt driven IO.
 */
static rtems_vector_number uart_get_irq_number(const console_tbl *ct)
{
  return ct->ulIntVector;
}

static uint32_t uart_get_baud(const console_tbl *ct)
{
  return ct->ulClock;
}
#endif

static void uart_set_baud(int baud)
{
  uint16_t divisor = (OR1K_BSP_CLOCK_FREQ) / (16 * baud);
  OR1K_REG(OR1K_BSP_UART_REG_LINE_CTRL) =
    OR1K_BSP_UART_REG_LINE_CTRL_DLAB;

  OR1K_REG(OR1K_BSP_UART_REG_DEV_LATCH_LOW) = divisor & 0xff;

  OR1K_REG(OR1K_BSP_UART_REG_DEV_LATCH_HIGH) =
    (divisor >> 8);
}

static void uart_initialize(int minor)
{
  /* Set baud rate */
  uart_set_baud(OR1K_UART_DEFAULT_BAUD);

  /* Set data pattern configuration */
  OR1K_REG(OR1K_BSP_UART_REG_LINE_CTRL) =
    OR1K_BSP_UART_REG_LINE_CTRL_WLEN8;

  /* Reset receiver and transmitter */
  OR1K_REG(OR1K_BSP_UART_REG_FIFO_CTRL) =
    OR1K_BSP_UART_REG_FIFO_CTRL_ENABLE_FIFO |
    OR1K_BSP_UART_REG_FIFO_CTRL_CLEAR_RCVR  |
    OR1K_BSP_UART_REG_FIFO_CTRL_TRIGGER_14;

  /* Disable all interrupts */
  OR1K_REG(OR1K_BSP_UART_REG_INT_ENABLE) = 0x00;

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
  unsigned char lsr;

 /* Get a character when avaiable */
  do {
       lsr = OR1K_REG(OR1K_BSP_UART_REG_LINE_STATUS);
  } while ((lsr & OR1K_BSP_UART_REG_LINE_STATUS_DR)
           != OR1K_BSP_UART_REG_LINE_STATUS_DR);

  return OR1K_REG(OR1K_BSP_UART_REG_RX);
}

static void uart_write_polled(int minor, char c)
{
  unsigned char lsr;

  /* Wait until there is no pending data in the transmitter FIFO (empty) */
  do {
      lsr = OR1K_REG(OR1K_BSP_UART_REG_LINE_STATUS);
  } while (!(lsr & OR1K_BSP_UART_REG_LINE_STATUS_THRE));

  OR1K_REG(OR1K_BSP_UART_REG_TX) = c;
}

static ssize_t uart_write(
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

const console_fns generic_or1k_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = uart_first_open,
  .deviceLastClose = uart_last_close,
  .deviceRead = uart_read_polled,
  .deviceWrite = uart_write,
  .deviceInitialize = uart_initialize,
  .deviceWritePolled = uart_write_polled,
  .deviceSetAttributes = uart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
