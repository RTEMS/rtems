/***************************************************************************
 *
 * MODULE DESCRIPTION:
 * This module implements the RTEMS drivers for the PC serial ports
 * as /dev/ttyS1 for COM1 and /dev/ttyS2 as COM2. If one of the ports
 * is used as the console, this driver would fail to initialize.
 *
 * This code was based on the console driver. It is based on the
 * current termios framework. This is just a shell around the
 * termios support.
 *
 * by: Rosimildo da Silva:
 *     rdasilva@connecttel.com
 *     http://www.connecttel.com
 *
 ****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <uart.h>
#include <libcpu/cpuModel.h>

int BSP_poll_read(int);

/* Internal routines */
static int tty1_conSetAttr( int minor, const struct termios *t);
static int tty2_conSetAttr( int minor, const struct termios *t);
static void isr_on(const rtems_irq_connect_data *);
static void isr_off(const rtems_irq_connect_data *);
static int  isr_is_on(const rtems_irq_connect_data *);

extern BSP_polling_getchar_function_type BSP_poll_char;
extern int BSPConsolePort;
extern void rtems_set_waiting_id_comx( int port,  rtems_id id, rtems_event_set event );

/*
 * Interrupt structure for tty1
 */
static rtems_irq_connect_data tty1_isr_data =
{
  BSP_UART_COM1_IRQ,
  BSP_uart_termios_isr_com1,
  0,
  isr_on,
  isr_off,
  isr_is_on};

/*
 * Interrupt structure for tty2
 */
static rtems_irq_connect_data tty2_isr_data =
{
  BSP_UART_COM2_IRQ,
  BSP_uart_termios_isr_com2,
  0,
  isr_on,
  isr_off,
  isr_is_on};

static void
isr_on(const rtems_irq_connect_data *unused)
{
  return;
}

static void
isr_off(const rtems_irq_connect_data *unused)
{
  return;
}

static int
isr_is_on(const rtems_irq_connect_data *irq)
{
  return BSP_irq_enabled_at_i8259s(irq->name);
}

/*
 *  TTYS1 - device driver INITIALIZE entry point.
 */
rtems_device_driver
tty1_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
  rtems_status_code status;

  /* Check if this port is not been used as console */
  if( BSPConsolePort == BSP_UART_COM1 )
  {
    status = -1;
    printk("TTYS1: port selected as console.\n");
    rtems_fatal_error_occurred( status );
  }

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize();

  /*
   * Do device-specific initialization
   */
  /* 9600-8-N-1, without hardware flow control */
  BSP_uart_init( BSP_UART_COM1, 9600, CHR_8_BITS, 0, 0, 0 );
  status = BSP_install_rtems_irq_handler( &tty1_isr_data );
  if( !status )
  {
    printk("Error installing ttyS1 interrupt handler!\n");
    rtems_fatal_error_occurred(status);
  }
  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/ttyS1", major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
      printk("Error registering ttyS1 device!\n");
      rtems_fatal_error_occurred (status);
  }
  printk("Device: /dev/ttyS1 initialized.\n");
  return RTEMS_SUCCESSFUL;
} /* tty_initialize */

static int tty1_last_close(int major, int minor, void *arg)
{
  BSP_remove_rtems_irq_handler( &tty1_isr_data );
  return 0;
}

/*
 * TTY1 - device driver OPEN entry point
 */
rtems_device_driver
tty1_open(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg)
{
  rtems_status_code  status;
#ifndef USE_TASK_DRIVEN
  static rtems_termios_callbacks cb =
  {
    NULL,                        /* firstOpen */
    tty1_last_close,             /* lastClose */
    NULL,                        /* poll read */
    BSP_uart_termios_write_com1, /* write */
    tty1_conSetAttr,             /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_IRQ_DRIVEN           /* outputUsesInterrupts */
  };
#else
  static rtems_termios_callbacks cb =
  {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    BSP_uart_termios_read_com1,  /* poll read */
    BSP_uart_termios_write_com1, /* write */
    tty1_conSetAttr,             /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_TASK_DRIVEN          /* outputUsesInterrupts */
  };
#endif

  status = rtems_termios_open( major, minor, arg, &cb );
  if(status != RTEMS_SUCCESSFUL)
  {
     printk("Error openning tty1 device\n");
     return status;
  }

  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set( BSP_UART_COM1,
                       ((rtems_libio_open_close_args_t *)arg)->iop->data1 );
  /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl( BSP_UART_COM1, BSP_UART_INTR_CTRL_TERMIOS);
  return RTEMS_SUCCESSFUL;
}

/*
 * TTY - device driver CLOSE entry point
 */
rtems_device_driver
tty_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{

  return (rtems_termios_close (arg));

} /* tty_close */

/*
 * TTY device driver READ entry point.
 * Read characters from the tty device.
 */
rtems_device_driver
tty_read(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{
  return rtems_termios_read (arg);
} /* tty_read */

/*
 * TTY device driver WRITE entry point.
 * Write characters to the tty device.
 */
rtems_device_driver
tty_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                    * arg)
{
    return rtems_termios_write (arg);

} /* tty_write */

/*
 * Handle ioctl request. This is a generic internal
 * routine to handle both devices.
 */
static rtems_device_driver tty_control( int port, void  *arg )
{
	rtems_libio_ioctl_args_t *args = arg;
	switch( args->command )
	{
	   default:
      return rtems_termios_ioctl (arg);
		break;
   }
 	args->ioctl_return = 0;
   return RTEMS_SUCCESSFUL;
}

/*
 * Handle ioctl request for ttyS1.
 */
rtems_device_driver
tty1_control(rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                      * arg
)
{
  return tty_control( BSP_UART_COM1, arg );
}

static int
conSetAttr(int port, int minor, const struct termios *t)
{
  unsigned long baud, databits, parity, stopbits;

  baud = rtems_termios_baud_to_number(t->c_cflag & CBAUD);
  if ( baud > 115200 )
    rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);

  if (t->c_cflag & PARENB) {
    /* Parity is enabled */
    if (t->c_cflag & PARODD) {
      /* Parity is odd */
      parity = PEN;
    }
    else {
      /* Parity is even */
      parity = PEN | EPS;
    }
  }
  else {
    /* No parity */
    parity = 0;
  }

  switch (t->c_cflag & CSIZE) {
    case CS5: databits = CHR_5_BITS; break;
    case CS6: databits = CHR_6_BITS; break;
    case CS7: databits = CHR_7_BITS; break;
    default:  /* just to avoid warnings -- all cases are covered */
    case CS8: databits = CHR_8_BITS; break;
  }

  if (t->c_cflag & CSTOPB) {
    /* 2 stop bits */
    stopbits = STB;
  }
  else {
    /* 1 stop bit */
    stopbits = 0;
  }

  printk("Setting attributes, port=%X, baud=%d, linemode = 0x%02x\n", port, baud, databits | parity | stopbits );
  BSP_uart_set_attributes(port, baud, databits, parity, stopbits);
  return 0;
}

/*
 * Handle ioctl request for ttyS2.
 */
static int
tty1_conSetAttr( int minor, const struct termios *t)
{
  return conSetAttr( BSP_UART_COM1, minor, t );
}

/*
 * TTY2 device driver INITIALIZE entry point.
 */
rtems_device_driver
tty2_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
  rtems_status_code status;

  /* Check if this port is not been used as console */
  if( BSPConsolePort == BSP_UART_COM2 )
  {
    status = -1;
    printk("TTY2: port selected as console.\n");
    rtems_fatal_error_occurred( status );
  }

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize();

  /*
   * Do device-specific initialization
   */
  /* 9600-8-N-1, without hardware flow control */
  BSP_uart_init( BSP_UART_COM2, 9600, CHR_8_BITS, 0, 0, 0);
  status = BSP_install_rtems_irq_handler( &tty2_isr_data );
  if( !status )
  {
    printk("Error installing serial console interrupt handler!\n");
    rtems_fatal_error_occurred(status);
  }
  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/ttyS2", major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
      printk("Error registering tty2 device!\n");
      rtems_fatal_error_occurred (status);
  }
  printk("Device: /dev/ttyS2 initialized.\n");
  return RTEMS_SUCCESSFUL;
} /* tty_initialize */

static int tty2_last_close(int major, int minor, void *arg)
{
  BSP_remove_rtems_irq_handler( &tty2_isr_data );
  return 0;
}

/*
 * TTY2 device driver OPEN entry point
 */
rtems_device_driver
tty2_open(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg)
{
  rtems_status_code              status;
#ifndef USE_TASK_DRIVEN
  static rtems_termios_callbacks cb =
  {
    NULL,                        /* firstOpen */
    tty2_last_close,             /* lastClose */
    NULL,                        /* poll read */
    BSP_uart_termios_write_com2, /* write */
    tty2_conSetAttr,             /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_IRQ_DRIVEN           /* outputUsesInterrupts */
  };
#else
  static rtems_termios_callbacks cb =
  {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    BSP_uart_termios_read_com2,  /* poll read */
    BSP_uart_termios_write_com2, /* write */
    tty2_conSetAttr,             /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_TASK_DRIVEN          /* outputUsesInterrupts */
  };
#endif

  status = rtems_termios_open (major, minor, arg, &cb);
  if(status != RTEMS_SUCCESSFUL)
  {
     printk("Error openning tty1 device\n");
     return status;
  }

  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set( BSP_UART_COM2,
			 ((rtems_libio_open_close_args_t *)arg)->iop->data1 );
   /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl( BSP_UART_COM2, BSP_UART_INTR_CTRL_TERMIOS);
  return RTEMS_SUCCESSFUL;
}

/*
 * Handle ioctl request for TTY2
 */
rtems_device_driver
tty2_control(rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                      * arg
)
{
   return tty_control( BSP_UART_COM2, arg );
}

static int
tty2_conSetAttr( int minor, const struct termios *t)
{
  return conSetAttr( BSP_UART_COM2, minor, t );
}
