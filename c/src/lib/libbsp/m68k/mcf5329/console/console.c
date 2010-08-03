 /*
  *  Multi UART console serial I/O.
  *
  * TO DO: Add DMA input/output
  */

#include <stdio.h>
#include <fcntl.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <bsp.h>
#include <malloc.h>
#include <rtems/mw_uid.h>

#include <rtems/bspIo.h>

#define UART_INTC0_IRQ_VECTOR(x) (64+26+(x))

#define MCF_UART_USR_ERROR ( MCF_UART_USR_RB | \
                             MCF_UART_USR_FE | \
                             MCF_UART_USR_PE | \
                             MCF_UART_USR_OE )

static ssize_t IntUartPollWrite(int minor, const char *buf, size_t len);
static ssize_t IntUartInterruptWrite(int minor, const char *buf, size_t len);

static void _BSP_null_char(char c)
{
  int level;

  if (c == '\n')
    _BSP_null_char('\r');
  rtems_interrupt_disable(level);
  while ((MCF_UART_USR(CONSOLE_PORT) & MCF_UART_USR_TXRDY) == 0)
    continue;
  MCF_UART_UTB(CONSOLE_PORT) = c;
  while ((MCF_UART_USR(CONSOLE_PORT) & MCF_UART_USR_TXRDY) == 0)
    continue;
  rtems_interrupt_enable(level);
}

BSP_output_char_function_type     BSP_output_char = _BSP_null_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

#define MAX_UART_INFO     3
#define RX_BUFFER_SIZE    512

struct IntUartInfoStruct
{
  int iomode;
  volatile int uimr;
  int baud;
  int databits;
  int parity;
  int stopbits;
  int hwflow;
  int rx_in;
  int rx_out;
  char rx_buffer[RX_BUFFER_SIZE];
  void *ttyp;
};

struct IntUartInfoStruct IntUartInfo[MAX_UART_INFO];

/***************************************************************************
   Function : IntUartSet

   Description : This updates the hardware UART settings.
 ***************************************************************************/
static void
IntUartSet(int minor, int baud, int databits, int parity, int stopbits,
           int hwflow)
{
  int divisor;
  uint32_t clock_speed;
  uint8_t umr1 = 0;
  uint8_t umr2 = 0;
  struct IntUartInfoStruct *info = &IntUartInfo[minor];
  int level;

  rtems_interrupt_disable(level);

  /* disable interrupts, clear RTS line, and disable the UARTS */
  MCF_UART_UIMR(minor) = 0;
  MCF_UART_UOP0(minor) = 1;
  MCF_UART_UCR(minor) = (MCF_UART_UCR_TX_DISABLED | MCF_UART_UCR_RX_DISABLED);

  /* save the current values */
  info->uimr = 0;
  info->baud = baud;
  info->databits = databits;
  info->parity = parity;
  info->stopbits = stopbits;
  info->hwflow = hwflow;

  clock_speed = bsp_get_BUS_clock_speed();
  /* determine the baud divisor value */
  divisor = ((clock_speed) / (32 * baud));
  if (divisor < 2)
    divisor = 2;

  /* check to see if doing hardware flow control */
  if (hwflow) {
    /* set hardware flow options */
    umr1 |= MCF_UART_UMR_RXRTS;
    umr2 |= MCF_UART_UMR_TXCTS;
  }

  /* determine the new umr values */
  umr1 |= (parity | databits);
  umr2 |= (stopbits);

  /* reset the uart */
  MCF_UART_UCR(minor) = MCF_UART_UCR_RESET_ERROR;
  MCF_UART_UCR(minor) = MCF_UART_UCR_RESET_RX;
  MCF_UART_UCR(minor) = MCF_UART_UCR_RESET_TX;

  /* reset the uart mode register and update values */
  MCF_UART_UCR(minor) = MCF_UART_UCR_RESET_MR;
  MCF_UART_UMR(minor) = umr1;
  MCF_UART_UMR(minor) = umr2;

  /* set the baud rate values */
  MCF_UART_UCSR(minor) =
    (MCF_UART_UCSR_RCS_SYS_CLK | MCF_UART_UCSR_TCS_SYS_CLK);
  MCF_UART_UBG1(minor) = (divisor & 0xff00) >> 8;
  MCF_UART_UBG2(minor) = (divisor & 0x00ff);

  /* enable the uart */
  MCF_UART_UCR(minor) = (MCF_UART_UCR_TX_ENABLED | MCF_UART_UCR_RX_ENABLED);

  /* check to see if interrupts need to be enabled */
  if (info->iomode != TERMIOS_POLLED) {
    /* enable rx interrupts */
    info->uimr |= MCF_UART_UIMR_RXRDY_FU;
    MCF_UART_UIMR(minor) = info->uimr;
  }

  /* check to see if doing hardware flow control */
  if (hwflow) {
    /* assert the RTS line */
    MCF_UART_UOP1(minor) = 1;
  }

  rtems_interrupt_enable(level);

}

/***************************************************************************
   Function : IntUartSetAttributes

   Description : This provides the hardware-dependent portion of tcsetattr().
   value and sets it. At the moment this just sets the baud rate.

   Note: The highest baudrate is 115200 as this stays within
   an error of +/- 5% at 25MHz processor clock
 ***************************************************************************/
static int IntUartSetAttributes(int minor, const struct termios *t)
{
  /* set default index values */
  int baud = (int) 19200;
  int databits = (int) MCF_UART_UMR_BC_8;
  int parity = (int) MCF_UART_UMR_PM_NONE;
  int stopbits = (int) MCF_UART_UMR_SB_STOP_BITS_1;
  int hwflow = (int) 0;
  struct IntUartInfoStruct *info = &IntUartInfo[minor];

  /* check to see if input is valid */
  if (t != (const struct termios *) 0) {
    /* determine baud rate index */
    baud = rtems_termios_baud_to_number(t->c_cflag & CBAUD);

    /* determine data bits */
    switch (t->c_cflag & CSIZE) {
      case CS5:
        databits = (int) MCF_UART_UMR_BC_5;
        break;
      case CS6:
        databits = (int) MCF_UART_UMR_BC_6;
        break;
      case CS7:
        databits = (int) MCF_UART_UMR_BC_7;
        break;
      case CS8:
        databits = (int) MCF_UART_UMR_BC_8;
        break;
    }

    /* determine if parity is enabled */
    if (t->c_cflag & PARENB) {
      if (t->c_cflag & PARODD) {
        /* odd parity */
        parity = (int) MCF_UART_UMR_PM_ODD;
      } else {
        /* even parity */
        parity = (int) MCF_UART_UMR_PM_EVEN;
      }
    }

    /* determine stop bits */
    if (t->c_cflag & CSTOPB) {
      /* two stop bits */
      stopbits = (int) MCF_UART_UMR_SB_STOP_BITS_2;
    }

    /* check to see if hardware flow control */
    if (t->c_cflag & CRTSCTS) {
      hwflow = 1;
    }
  }

  /* check to see if values have changed */
  if ((baud != info->baud) ||
      (databits != info->databits) ||
      (parity != info->parity) ||
      (stopbits != info->stopbits) || (hwflow != info->hwflow)) {

    /* call function to set values */
    IntUartSet(minor, baud, databits, parity, stopbits, hwflow);
  }

  return (RTEMS_SUCCESSFUL);

}

/***************************************************************************
   Function : IntUartInterruptHandler

   Description : This is the interrupt handler for the internal uart. It
   determines which channel caused the interrupt before queueing any received
   chars and dequeueing chars waiting for transmission.
 ***************************************************************************/
static rtems_isr IntUartInterruptHandler(rtems_vector_number v)
{
  unsigned int chan = v - UART_INTC0_IRQ_VECTOR(0);
  struct IntUartInfoStruct *info = &IntUartInfo[chan];

  /* check to see if received data */
  if (MCF_UART_UISR(chan) & MCF_UART_UISR_RXRDY_FU) {
    /* read data and put into the receive buffer */
    while (MCF_UART_USR(chan) & MCF_UART_USR_RXRDY) {

      if (MCF_UART_USR(chan) & MCF_UART_USR_ERROR) {
        /* clear the error */
        MCF_UART_UCR(chan) = MCF_UART_UCR_RESET_ERROR;
      }
      /* put data in rx buffer and check for errors */
      info->rx_buffer[info->rx_in] = MCF_UART_URB(chan);

      /* update buffer values */
      info->rx_in++;

      if (info->rx_in >= RX_BUFFER_SIZE) {
        info->rx_in = 0;
      }
    }
    /* Make sure the port has been opened */
    if (info->ttyp) {

      /* check to see if task driven */
      if (info->iomode == TERMIOS_TASK_DRIVEN) {
        /* notify rx task that rx buffer has data */
        rtems_termios_rxirq_occured(info->ttyp);
      } else {
        /* Push up the received data */
        rtems_termios_enqueue_raw_characters(info->ttyp, info->rx_buffer,
                                             info->rx_in);
        info->rx_in = 0;
      }
    }
  }

  /* check to see if data needs to be transmitted */
  if ((info->uimr & MCF_UART_UIMR_TXRDY) &&
      (MCF_UART_UISR(chan) & MCF_UART_UISR_TXRDY)) {

    /* disable tx interrupts */
    info->uimr &= ~MCF_UART_UIMR_TXRDY;
    MCF_UART_UIMR(chan) = info->uimr;

    /* tell upper level that character has been sent */
    if (info->ttyp)
      rtems_termios_dequeue_characters(info->ttyp, 1);
  }
}

/***************************************************************************
   Function : IntUartInitialize

   Description : This initialises the internal uart hardware for all
   internal uarts. If the internal uart is to be interrupt driven then the
   interrupt vectors are hooked.
 ***************************************************************************/
static void IntUartInitialize(void)
{
  unsigned int chan;
  struct IntUartInfoStruct *info;
  rtems_isr_entry old_handler;
  int level;

  for (chan = 0; chan < MAX_UART_INFO; chan++) {
    info = &IntUartInfo[chan];

    info->ttyp = NULL;
    info->rx_in = 0;
    info->rx_out = 0;
    info->baud = -1;
    info->databits = -1;
    info->parity = -1;
    info->stopbits = -1;
    info->hwflow = -1;
    info->iomode = TERMIOS_POLLED;                /*polled console io */

    MCF_UART_UACR(chan) = 0;
    MCF_UART_UIMR(chan) = 0;
    if (info->iomode != TERMIOS_POLLED) {
      rtems_interrupt_catch(IntUartInterruptHandler,
                            UART_INTC0_IRQ_VECTOR(chan), &old_handler);
    }

    /* set uart default values */
    IntUartSetAttributes(chan, NULL);

    /* unmask interrupt */
    rtems_interrupt_disable(level);
    switch (chan) {
      case 0:
        MCF_INTC0_ICR26 = MCF_INTC_ICR_IL(UART0_IRQ_LEVEL);
        MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK26);
        break;

      case 1:
        MCF_INTC0_ICR27 = MCF_INTC_ICR_IL(UART1_IRQ_LEVEL);
        MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK27);
        break;

      case 2:
        MCF_INTC0_ICR28 = MCF_INTC_ICR_IL(UART2_IRQ_LEVEL);
        MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK28);
        break;
    }
    rtems_interrupt_enable(level);

  }                                               /* of chan loop */

}                                                 /* IntUartInitialise */

/***************************************************************************
   Function : IntUartInterruptWrite

   Description : This writes a single character to the appropriate uart
   channel. This is either called during an interrupt or in the user's task
   to initiate a transmit sequence. Calling this routine enables Tx
   interrupts.
 ***************************************************************************/
static ssize_t IntUartInterruptWrite(int minor, const char *buf, size_t len)
{
  int level;

  rtems_interrupt_disable(level);

  /* write out character */
  MCF_UART_UTB(minor) = *buf;

  /* enable tx interrupt */
  IntUartInfo[minor].uimr |= MCF_UART_UIMR_TXRDY;
  MCF_UART_UIMR(minor) = IntUartInfo[minor].uimr;

  rtems_interrupt_enable(level);
  return (0);
}

/***************************************************************************
   Function : IntUartInterruptOpen

   Description : This enables interrupts when the tty is opened.
 ***************************************************************************/
static int IntUartInterruptOpen(int major, int minor, void *arg)
{
  struct IntUartInfoStruct *info = &IntUartInfo[minor];

  /* enable the uart */
  MCF_UART_UCR(minor) = (MCF_UART_UCR_TX_ENABLED | MCF_UART_UCR_RX_ENABLED);

  /* check to see if interrupts need to be enabled */
  if (info->iomode != TERMIOS_POLLED) {
    /* enable rx interrupts */
    info->uimr |= MCF_UART_UIMR_RXRDY_FU;
    MCF_UART_UIMR(minor) = info->uimr;
  }

  /* check to see if doing hardware flow control */
  if (info->hwflow) {
    /* assert the RTS line */
    MCF_UART_UOP1(minor) = 1;
  }

  return (0);
}

/***************************************************************************
   Function : IntUartInterruptClose

   Description : This disables interrupts when the tty is closed.
 ***************************************************************************/
static int IntUartInterruptClose(int major, int minor, void *arg)
{
  struct IntUartInfoStruct *info = &IntUartInfo[minor];

  /* disable the interrupts and the uart */
  MCF_UART_UIMR(minor) = 0;
  MCF_UART_UCR(minor) = (MCF_UART_UCR_TX_DISABLED | MCF_UART_UCR_RX_DISABLED);

  /* reset values */
  info->ttyp = NULL;
  info->uimr = 0;
  info->rx_in = 0;
  info->rx_out = 0;

  return (0);
}

/***************************************************************************
   Function : IntUartTaskRead

   Description : This reads all available characters from the internal uart
   and places them into the termios buffer.  The rx interrupts will be
   re-enabled after all data has been read.
 ***************************************************************************/
static int IntUartTaskRead(int minor)
{
  char buffer[RX_BUFFER_SIZE];
  int count;
  int rx_in;
  int index = 0;
  struct IntUartInfoStruct *info = &IntUartInfo[minor];

  /* determine number of values to copy out */
  rx_in = info->rx_in;
  if (info->rx_out <= rx_in) {
    count = rx_in - info->rx_out;
  } else {
    count = (RX_BUFFER_SIZE - info->rx_out) + rx_in;
  }

  /* copy data into local buffer from rx buffer */
  while ((index < count) && (index < RX_BUFFER_SIZE)) {
    /* copy data byte */
    buffer[index] = info->rx_buffer[info->rx_out];
    index++;

    /* increment rx buffer values */
    info->rx_out++;
    if (info->rx_out >= RX_BUFFER_SIZE) {
      info->rx_out = 0;
    }
  }

  /* check to see if buffer is not empty */
  if (count > 0) {
    /* set characters into termios buffer  */
    rtems_termios_enqueue_raw_characters(info->ttyp, buffer, count);
  }

  return (EOF);
}

/***************************************************************************
   Function : IntUartPollRead

   Description : This reads a character from the internal uart. It returns
   to the caller without blocking if not character is waiting.
 ***************************************************************************/
static int IntUartPollRead(int minor)
{
  if ((MCF_UART_USR(minor) & MCF_UART_USR_RXRDY) == 0)
    return (-1);

  return (MCF_UART_URB(minor));
}

/***************************************************************************
   Function : IntUartPollWrite

   Description : This writes out each character in the buffer to the
   appropriate internal uart channel waiting till each one is sucessfully
   transmitted.
 ***************************************************************************/
static ssize_t IntUartPollWrite(int minor, const char *buf, size_t len)
{
  size_t retval = len;
  /* loop over buffer */
  while (len--) {
    /* block until we can transmit */
    while ((MCF_UART_USR(minor) & MCF_UART_USR_TXRDY) == 0)
      continue;
    /* transmit data byte */
    MCF_UART_UTB(minor) = *buf++;
  }
  return retval;
}

/***************************************************************************
   Function : console_initialize

   Description : This initialises termios, both sets of uart hardware before
   registering /dev/tty devices for each channel and the system /dev/console.
 ***************************************************************************/
rtems_device_driver console_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void *arg)
{
  rtems_status_code status;

  /* Set up TERMIOS */
  rtems_termios_initialize();

  /* set io modes for the different channels and initialize device */
  IntUartInfo[minor].iomode = TERMIOS_IRQ_DRIVEN;
  IntUartInitialize();

  /* Register the console port */
  status = rtems_io_register_name("/dev/console", major, CONSOLE_PORT);
  if (status != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(status);
  }

  /* Register the other port */
  if (CONSOLE_PORT != 0) {
    status = rtems_io_register_name("/dev/tty00", major, 0);
    if (status != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(status);
    }
  }
  if (CONSOLE_PORT != 1) {
    status = rtems_io_register_name("/dev/tty01", major, 1);
    if (status != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(status);
    }
  }

  return (RTEMS_SUCCESSFUL);
}

/***************************************************************************
   Function : console_open

   Description : This actually opens the device depending on the minor
   number set during initialisation. The device specific access routines are
   passed to termios when the devices is opened depending on whether it is
   polled or not.
 ***************************************************************************/
rtems_device_driver console_open(rtems_device_major_number major,
                                 rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status = RTEMS_INVALID_NUMBER;
  rtems_libio_open_close_args_t *args = (rtems_libio_open_close_args_t *) arg;
  struct IntUartInfoStruct *info;

  static const rtems_termios_callbacks IntUartPollCallbacks = {
    NULL,                                         /* firstOpen */
    NULL,                                         /* lastClose */
    IntUartPollRead,                              /* pollRead */
    IntUartPollWrite,                             /* write */
    IntUartSetAttributes,                         /* setAttributes */
    NULL,                                         /* stopRemoteTx */
    NULL,                                         /* startRemoteTx */
    TERMIOS_POLLED                                /* mode */
  };
  static const rtems_termios_callbacks IntUartIntrCallbacks = {
    IntUartInterruptOpen,                         /* firstOpen */
    IntUartInterruptClose,                        /* lastClose */
    NULL,                                         /* pollRead */
    IntUartInterruptWrite,                        /* write */
    IntUartSetAttributes,                         /* setAttributes */
    NULL,                                         /* stopRemoteTx */
    NULL,                                         /* startRemoteTx */
    TERMIOS_IRQ_DRIVEN                            /* mode */
  };

  static const rtems_termios_callbacks IntUartTaskCallbacks = {
    IntUartInterruptOpen,                         /* firstOpen */
    IntUartInterruptClose,                        /* lastClose */
    IntUartTaskRead,                              /* pollRead */
    IntUartInterruptWrite,                        /* write */
    IntUartSetAttributes,                         /* setAttributes */
    NULL,                                         /* stopRemoteTx */
    NULL,                                         /* startRemoteTx */
    TERMIOS_TASK_DRIVEN                           /* mode */
  };

  /* open the port depending on the minor device number */
  if ((minor >= 0) && (minor < MAX_UART_INFO)) {
    info = &IntUartInfo[minor];
    switch (info->iomode) {
      case TERMIOS_POLLED:
        status = rtems_termios_open(major, minor, arg, &IntUartPollCallbacks);
        break;
      case TERMIOS_IRQ_DRIVEN:
        status = rtems_termios_open(major, minor, arg, &IntUartIntrCallbacks);
        info->ttyp = args->iop->data1;
        break;
      case TERMIOS_TASK_DRIVEN:
        status = rtems_termios_open(major, minor, arg, &IntUartTaskCallbacks);
        info->ttyp = args->iop->data1;
        break;
    }
  }

  if (status == RTEMS_SUCCESSFUL) {
    /*
     * Reset the default baudrate.
     */
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) >= 0) {
      term.c_cflag &= ~(CBAUD | CSIZE);
      term.c_cflag |= CS8 | B19200;
      tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }
  }

  return (status);
}

/***************************************************************************
   Function : console_close

   Description : This closes the device via termios
 ***************************************************************************/
rtems_device_driver console_close(rtems_device_major_number major,
                                  rtems_device_minor_number minor, void *arg)
{
  return (rtems_termios_close(arg));
}

/***************************************************************************
   Function : console_read

   Description : Read from the device via termios
 ***************************************************************************/
rtems_device_driver console_read(rtems_device_major_number major,
                                 rtems_device_minor_number minor, void *arg)
{
  return (rtems_termios_read(arg));
}

/***************************************************************************
   Function : console_write

   Description : Write to the device via termios
 ***************************************************************************/
rtems_device_driver console_write(rtems_device_major_number major,
                                  rtems_device_minor_number minor, void *arg)
{
  return (rtems_termios_write(arg));
}

/***************************************************************************
   Function : console_ioctl

   Description : Pass the IOCtl call to termios
 ***************************************************************************/
rtems_device_driver console_control(rtems_device_major_number major,
                                    rtems_device_minor_number minor,
                                    void *arg)
{
  return (rtems_termios_ioctl(arg));
}
int DEBUG_OUTCHAR(int c)
{
  if (c == '\n')
    DEBUG_OUTCHAR('\r');
  _BSP_null_char(c);
  return c;
}
void DEBUG_OUTSTR(const char *msg)
{
  while (*msg)
    DEBUG_OUTCHAR(*msg++);
}
void DEBUG_OUTNUM(int i)
{
  int n;
  static const char map[] = "0123456789ABCDEF";

  DEBUG_OUTCHAR(' ');
  for (n = 28; n >= 0; n -= 4)
    DEBUG_OUTCHAR(map[(i >> n) & 0xF]);
}
