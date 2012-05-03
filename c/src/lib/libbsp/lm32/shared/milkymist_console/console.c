/*
 *  Console driver for Milkymist
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#include <unistd.h>
#include <termios.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <bsp/irq-generic.h>

#include "../include/system_conf.h"
#include "uart.h"

BSP_output_char_function_type BSP_output_char = BSP_uart_polled_write;
BSP_polling_getchar_function_type BSP_poll_char = BSP_uart_polled_read;

static struct rtems_termios_tty *tty;

static int mmconsole_first_open(int major, int minor, void *arg)
{
  tty = ((rtems_libio_open_close_args_t *) arg)->iop->data1;
  return rtems_termios_set_initial_baud(tty, UART_BAUD_RATE);
}

static int mmconsole_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int mmconsole_set_attributes(int minor, const struct termios *t)
{
  int baud;

  switch (t->c_cflag & CBAUD) {
    case B0:
      baud = 0;
      break;
    case B50:
      baud = 50;
      break;
    case B75:
      baud = 75;
      break;
    case B110:
      baud = 110;
      break;
    case B134:
      baud = 134;
      break;
    case B150:
      baud = 150;
      break;
    case B200:
      baud = 200;
      break;
    case B300:
      baud = 300;
      break;
    case B600:
      baud = 600;
      break;
    case B1200:
      baud = 1200;
      break;
    case B1800:
      baud = 1800;
      break;
    case B2400:
      baud = 2400;
      break;
    case B4800:
      baud = 4800;
      break;
    case B9600:
      baud = 9600;
      break;
    case B19200:
      baud = 19200;
      break;
    case B38400:
      baud = 38400;
      break;
    case B57600:
      baud = 57600;
      break;
    case B115200:
      baud = 115200;
      break;
    case B230400:
      baud = 230400;
      break;
    case B460800:
      baud = 460800;
      break;
    default:
      baud = -1;
      break;
  }

  if (baud > 0)
    MM_WRITE(MM_UART_DIV, MM_READ(MM_FREQUENCY)/baud/16);

  return 0;
}

static ssize_t mmconsole_write(int minor, const char *buf, size_t n)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  MM_WRITE(MM_UART_RXTX, *buf);
  rtems_interrupt_enable(level);
  return 0;
}

static rtems_isr mmconsole_interrupt(rtems_vector_number n)
{
  char c;
  while (MM_READ(MM_UART_STAT) & UART_STAT_RX_EVT) {
    c = MM_READ(MM_UART_RXTX);
    MM_WRITE(MM_UART_STAT, UART_STAT_RX_EVT);
    rtems_termios_enqueue_raw_characters(tty, &c, 1);
  }
  if (MM_READ(MM_UART_STAT) & UART_STAT_TX_EVT) {
    MM_WRITE(MM_UART_STAT, UART_STAT_TX_EVT);
    rtems_termios_dequeue_characters(tty, 1);
  }
  lm32_interrupt_ack(1 << MM_IRQ_UART);
}

static const rtems_termios_callbacks mmconsole_callbacks = {
  .firstOpen = mmconsole_first_open,
  .lastClose = mmconsole_last_close,
  .pollRead = NULL,
  .write = mmconsole_write,
  .setAttributes = mmconsole_set_attributes,
  .stopRemoteTx = NULL,
  .startRemoteTx = NULL,
  .outputUsesInterrupts = TERMIOS_IRQ_DRIVEN
};

rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code status;
  rtems_isr_entry dummy;

  rtems_termios_initialize();

  status = rtems_io_register_name("/dev/console", major, 0);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  rtems_interrupt_catch(mmconsole_interrupt, MM_IRQ_UART, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_UART);
  MM_WRITE(MM_UART_CTRL, UART_CTRL_RX_INT|UART_CTRL_TX_INT);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void  *arg
)
{
  return rtems_termios_open(major, minor, arg, &mmconsole_callbacks);
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_close(arg);
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_read(arg);
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_write(arg);
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return rtems_termios_ioctl(arg);
}
