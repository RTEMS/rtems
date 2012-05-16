/*
 *  This file contains the TTY driver for the serial ports on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* Define CONSOLE_USE_INTERRUPTS to enable APBUART interrupt handling instead
 * of polling mode.
 *
 * Note that it is not possible to use the interrupt mode of the driver
 * together with the "old" APBUART and -u to GRMON. However the new
 * APBUART core (from GRLIB 1.0.17-b2710) has the GRMON debug bit and can 
 * handle interrupts.
 *
 * NOTE: This can be defined in the make/custom/leon3.cfg file.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <amba.h>

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
int syscon_uart_index __attribute__((weak)) = 0;

/*
 *  apbuart_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

extern void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent
  );


/* body is in printk_support.c */

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

extern int apbuart_inbyte_nonblocking(struct apbuart_regs *regs);

/* body is in debugputs.c */

struct apbuart_priv {
  struct apbuart_regs *regs;
  unsigned int freq_hz;
#if CONSOLE_USE_INTERRUPTS
  int irq;
  void *cookie;
  volatile int sending;
  char *buf;
#endif
};
static struct apbuart_priv apbuarts[BSP_NUMBER_OF_TERMIOS_PORTS];
static int uarts = 0;

#if CONSOLE_USE_INTERRUPTS

/* Handle UART interrupts */
void console_isr(void *arg)
{
  struct apbuart_priv *uart = arg;
  unsigned int status;
  char data;

  /* Get all received characters */
  while ((status=uart->regs->status) & LEON_REG_UART_STATUS_DR) {
    /* Data has arrived, get new data */
    data = uart->regs->data;

    /* Tell termios layer about new character */
    rtems_termios_enqueue_raw_characters(uart->cookie, &data, 1);
  }

  if (status & LEON_REG_UART_STATUS_THE) {
    /* Sent the one char, we disable TX interrupts */
    uart->regs->ctrl &= ~LEON_REG_UART_CTRL_TI;

    /* Tell close that we sent everything */
    uart->sending = 0;

    /* write_interrupt will get called from this function */
    rtems_termios_dequeue_characters(uart->cookie, 1);
  }
}

/*
 *  Console Termios Write-Buffer Support Entry Point
 *
 */
int console_write_interrupt(int minor, const char *buf, int len)
{
  struct apbuart_priv *uart;
  unsigned int oldLevel;

  if (minor == 0)
    uart = &apbuarts[syscon_uart_index];
  else
    uart = &apbuarts[minor - 1];

  /* Remember what position in buffer */

  rtems_interrupt_disable(oldLevel);

  /* Enable TX interrupt */
  uart->regs->ctrl |= LEON_REG_UART_CTRL_TI;

  /* start UART TX, this will result in an interrupt when done */
  uart->regs->data = *buf;

  uart->sending = 1;

  rtems_interrupt_enable(oldLevel);

  return 0;
}

#else

/*
 *  Console Termios Support Entry Points
 *
 */

ssize_t console_write_polled(int minor, const char *buf, size_t len)
{
  int nwrite = 0, port;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  while (nwrite < len) {
    apbuart_outbyte_polled(apbuarts[port].regs, *buf++, 1, 0);
    nwrite++;
  }
  return nwrite;
}

int console_pollRead(int minor)
{
  int port;

  if (minor == 0)
    port = syscon_uart_index;
  else
    port = minor - 1;

  return apbuart_inbyte_nonblocking(apbuarts[port].regs);
}

#endif

int console_set_attributes(int minor, const struct termios *t)
{
  unsigned int scaler;
  unsigned int ctrl;
  int baud;
  struct apbuart_priv *uart;

  switch (t->c_cflag & CSIZE) {
    default:
    case CS5:
    case CS6:
    case CS7:
      /* Hardware doesn't support other than CS8 */
      return -1;
    case CS8:
      break;
  }

  if (minor == 0)
    uart = &apbuarts[syscon_uart_index];
  else
    uart = &apbuarts[minor - 1];

  /* Read out current value */
  ctrl = uart->regs->ctrl;

  switch (t->c_cflag & (PARENB|PARODD)) {
    case (PARENB|PARODD):
      /* Odd parity */
      ctrl |= LEON_REG_UART_CTRL_PE|LEON_REG_UART_CTRL_PS;
      break;

    case PARENB:
      /* Even parity */
      ctrl &= ~LEON_REG_UART_CTRL_PS;
      ctrl |= LEON_REG_UART_CTRL_PE;
      break;

    default:
    case 0:
    case PARODD:
      /* No Parity */
      ctrl &= ~(LEON_REG_UART_CTRL_PS|LEON_REG_UART_CTRL_PE);
  }

  if (!(t->c_cflag & CLOCAL)) {
    ctrl |= LEON_REG_UART_CTRL_FL;
  } else {
    ctrl &= ~LEON_REG_UART_CTRL_FL;
  }

  /* Update new settings */
  uart->regs->ctrl = ctrl;

  /* Baud rate */
  baud = rtems_termios_baud_to_number(t->c_cflag);
  if (baud > 0) {
    /* Calculate Baud rate generator "scaler" number */
    scaler = (((uart->freq_hz * 10) / (baud * 8)) - 5) / 10;

    /* Set new baud rate by setting scaler */
    uart->regs->scaler = scaler;
  }

  return 0;
}

/* AMBA PP find routine. Extract AMBA PnP information into data structure. */
int find_matching_apbuart(struct ambapp_dev *dev, int index, void *arg)
{
  struct ambapp_apb_info *apb = (struct ambapp_apb_info *)dev->devinfo;

  /* Extract needed information of one APBUART */
  apbuarts[uarts].regs = (struct apbuart_regs *)apb->start;
#if CONSOLE_USE_INTERRUPTS
  apbuarts[uarts].irq = apb->irq;
#endif
  /* Get APBUART core frequency, it is assumed that it is the same
   * as Bus frequency where the UART is situated
   */
  apbuarts[uarts].freq_hz = ambapp_freq_get(&ambapp_plb, dev);
  uarts++;

  if (uarts >= BSP_NUMBER_OF_TERMIOS_PORTS)
    return 1; /* Satisfied number of UARTs, stop search */
  else
    return 0; /* Continue searching for more UARTs */
}

/* Find all UARTs */
int console_scan_uarts(void)
{
  memset(apbuarts, 0, sizeof(apbuarts));

  /* Find APBUART cores */
  ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS), VENDOR_GAISLER,
                  GAISLER_APBUART, find_matching_apbuart, NULL);

  return uarts;
}

/*
 *  Console Device Driver Entry Points
 *
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  int i;
  char console_name[16];

  rtems_termios_initialize();

  /* Find UARTs */
  console_scan_uarts();

  /* Update syscon_uart_index to index used as /dev/console
   * Let user select System console by setting syscon_uart_index. If the
   * BSP is to provide the default UART (syscon_uart_index==0):
   *   non-MP: APBUART[0] is system console
   *   MP: LEON CPU index select UART
   */
  if (syscon_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    syscon_uart_index = LEON3_Cpu_Index;
#else
    syscon_uart_index = 0;
#endif
  } else {
    syscon_uart_index = syscon_uart_index - 1; /* User selected sys-console */
  }

  /*  Register Device Names
   *
   *  0 /dev/console   - APBUART[USER-SELECTED, DEFAULT=APBUART[0]]
   *  1 /dev/console_a - APBUART[0] (by default not present because is console)
   *  2 /dev/console_b - APBUART[1]
   *  ...
   *
   * On a MP system one should not open UARTs that other OS instances use.
   */
  if (syscon_uart_index < uarts) {
    status = rtems_io_register_name("/dev/console", major, 0);
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }
  strcpy(console_name,"/dev/console_a");
  for (i = 0; i < uarts; i++) {
    if (i == syscon_uart_index)
      continue; /* skip UART that is registered as /dev/console */
    console_name[13] = 'a' + i;
    status = rtems_io_register_name( console_name, major, i+1);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  struct apbuart_priv *uart;
#if CONSOLE_USE_INTERRUPTS
  rtems_libio_open_close_args_t *priv = arg;

  /* Interrupt mode routines */
  static const rtems_termios_callbacks Callbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    NULL,                        /* pollRead */
    console_write_interrupt,     /* write */
    console_set_attributes,      /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    1                            /* outputUsesInterrupts */
  };
#else
  /* Polling mode routines */
  static const rtems_termios_callbacks Callbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    console_pollRead,            /* pollRead */
    console_write_polled,        /* write */
    console_set_attributes,      /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };
#endif

  assert(minor <= uarts);
  if (minor > uarts || minor == (syscon_uart_index + 1))
    return RTEMS_INVALID_NUMBER;

  sc = rtems_termios_open(major, minor, arg, &Callbacks);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (minor == 0)
    uart = &apbuarts[syscon_uart_index];
  else
    uart = &apbuarts[minor - 1];

#if CONSOLE_USE_INTERRUPTS
  if (priv && priv->iop)
    uart->cookie = priv->iop->data1;
  else
    uart->cookie = NULL;

  /* Register Interrupt handler */
  sc = rtems_interrupt_handler_install(uart->irq, "console",
                                       RTEMS_INTERRUPT_SHARED, console_isr,
                                       uart);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  uart->sending = 0;
  /* Enable Receiver and transmitter and Turn on RX interrupts */
  uart->regs->ctrl |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE |
                      LEON_REG_UART_CTRL_RI;
#else
  /* Initialize UART on opening */
  uart->regs->ctrl |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE;
#endif
  uart->regs->status = 0;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
#if CONSOLE_USE_INTERRUPTS
  struct apbuart_priv *uart;

  if (minor == 0)
    uart = &apbuarts[syscon_uart_index];
  else
    uart = &apbuarts[minor - 1];

  /* Turn off RX interrupts */
  uart->regs->ctrl &= ~(LEON_REG_UART_CTRL_RI);

  /**** Flush device ****/
  while (uart->sending) {
    /* Wait until all data has been sent */
  }

  /* uninstall ISR */
  rtems_interrupt_handler_remove(uart->irq, console_isr, uart);
#endif
  return rtems_termios_close(arg);
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read(arg);
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write(arg);
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl(arg);
}

