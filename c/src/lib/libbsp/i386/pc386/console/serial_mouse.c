/***************************************************************************
 *
 * $Header$
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
 * MODIFICATION/HISTORY:
 *
 * $Log$
 * Revision 1.4  2000/10/23 14:10:25  joel
 * 2000-10-23	Joel Sherrill <joel@OARcorp.com>
 *
 * 	* console/serial_mouse.c: Fixed typos introduced by removal of CR/LF.
 *
 * Revision 1.3  2000/10/20 16:01:13  joel
 * 2000-10-20	Rosimildo da Silva <rdasilva@connecttel.com>
 *
 * 	* console/serial_mouse.c: Added support for changing serial parameters.
 *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <bsp.h>
#include <irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <uart.h>
#include <libcpu/cpuModel.h>

int BSP_poll_read(int);

#include <rtems/mw_uid.h>
#include "serial_mouse.h"
#include "mouse_parser.h"

/* Internal routines */
static int serial_mouse_conSetAttr( int minor, const struct termios *t);
static void isr_on(const rtems_irq_connect_data *);
static void isr_off(const rtems_irq_connect_data *);
static int  isr_is_on(const rtems_irq_connect_data *);


extern BSP_polling_getchar_function_type BSP_poll_char;
extern int BSPConsolePort;

/* Select Default to be COM1  */
#if !defined( SERIAL_MOUSE_COM1 ) && !defined( SERIAL_MOUSE_COM2 )
#define SERIAL_MOUSE_COM1  1
#endif

/* select which serial port the mouse is connected to */
#ifdef   SERIAL_MOUSE_COM1
#define  BSP_UART_PORT    BSP_UART_COM1
#define  BSP_UART_IRQ     BSP_UART_COM1_IRQ
#define  BSP_ISR_FUNC     BSP_uart_termios_isr_com1
#define  BSP_WRITE_FUNC   BSP_uart_termios_write_com1
#endif

#ifdef   SERIAL_MOUSE_COM2
#define  BSP_UART_PORT    BSP_UART_COM2
#define  BSP_UART_IRQ     BSP_UART_COM2_IRQ
#define  BSP_ISR_FUNC     BSP_uart_termios_isr_com2
#define  BSP_WRITE_FUNC   BSP_uart_termios_write_com2
#endif

/*
 * Interrupt structure for serial_mouse
 */
static rtems_irq_connect_data serial_mouse_isr_data = 
{ 
  BSP_UART_IRQ,
  BSP_ISR_FUNC,
  isr_on,
  isr_off,
  isr_is_on};

static void isr_on(const rtems_irq_connect_data *unused)
{
  return;
}
						   
static void isr_off(const rtems_irq_connect_data *unused)
{
  return;
}

static int isr_is_on(const rtems_irq_connect_data *irq)
{
  return BSP_irq_enabled_at_i8259s(irq->name);
}

void serial_mouse_reserve_resources(rtems_configuration_table *conf)
{
  rtems_termios_reserve_resources(conf, 1);
  return;
}

/*
 *  Serial Mouse - device driver INITIALIZE entry point.
 */
rtems_device_driver
serial_mouse_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
  rtems_status_code status;

  /* Check if this port is not been used as console */
  if( BSPConsolePort == BSP_UART_PORT )
  {
    status = -1;
    printk("SERIAL MOUSE: port selected as console.( %d )\n", BSP_UART_PORT  );
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
  BSP_uart_init( BSP_UART_PORT, 1200, CHR_8_BITS, 0, 0, 0 );
  status = BSP_install_rtems_irq_handler( &serial_mouse_isr_data );
  if( !status )
  {
    printk("Error installing serial mouse interrupt handler!\n");
    rtems_fatal_error_occurred(status);
  }
  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/mouse", major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
      printk("Error registering /dev/mouse device!\n");
      rtems_fatal_error_occurred (status);
  }
  printk("Device: /dev/mouse on COM%d -- ok \n", BSP_UART_PORT );
  return RTEMS_SUCCESSFUL;
} /* tty_initialize */


static int serial_mouse_last_close(int major, int minor, void *arg)
{
  BSP_remove_rtems_irq_handler( &serial_mouse_isr_data );
  return 0;
}

/*
 * serial_mouse - device driver OPEN entry point
 */
rtems_device_driver
serial_mouse_open(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg)
{
  rtems_status_code  status;
  static rtems_termios_callbacks cb = 
  {
    NULL,	              /* firstOpen */
    serial_mouse_last_close,       /* lastClose */
    NULL,                          /* poll read  */
    BSP_WRITE_FUNC,        /* write */
    serial_mouse_conSetAttr,	     /* setAttributes */
    NULL,	              /* stopRemoteTx */
    NULL,	              /* startRemoteTx */
    1		              /* outputUsesInterrupts */
  };

  status = rtems_termios_open( major, minor, arg, &cb );
  if(status != RTEMS_SUCCESSFUL)
  {
     printk("Error openning serial_mouse device\n");
     return status;
  }

  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set( BSP_UART_PORT, 
                       ((rtems_libio_open_close_args_t *)arg)->iop->data1 );
  /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl( BSP_UART_PORT, BSP_UART_INTR_CTRL_TERMIOS);
  return RTEMS_SUCCESSFUL;
}

/*
 * TTY - device driver CLOSE entry point
 */
rtems_device_driver
serial_mouse_close(rtems_device_major_number major,
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
serial_mouse_read(rtems_device_major_number major,
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
serial_mouse_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                    * arg)
{
    return rtems_termios_write (arg);
 
} /* tty_write */

/*
 * Handle ioctl request. This is a generic internal
 * routine to handle both devices.
 */
static rtems_device_driver serial_mouse_control_internal( int port, void  *arg )
{ 
	rtems_libio_ioctl_args_t *args = arg;
	switch( args->command ) 
	{
	   default:
      return rtems_termios_ioctl (arg);
		break;

      case MW_UID_REGISTER_DEVICE:
      printk( "SerialMouse: reg=%s\n", args->buffer );
      register_mou_msg_queue( args->buffer, BSP_UART_PORT );
		break;

      case MW_UID_UNREGISTER_DEVICE:
      unregister_mou_msg_queue( BSP_UART_PORT );
		break;

   }
 	args->ioctl_return = 0;
   return RTEMS_SUCCESSFUL;
}

/*
 * Handle ioctl request for ttyS1.
 */
rtems_device_driver 
serial_mouse_control(rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                      * arg
)
{ 
  return serial_mouse_control_internal( BSP_UART_PORT, arg );
}



static int
conSetAttr(int port, int minor, const struct termios *t)
{
  unsigned long baud, databits, parity, stopbits;

  switch (t->c_cflag & CBAUD) 
    {
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
    default:
      rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);
      return 0;
    }

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
    default: /* just to avoid warnings -- all cases are covered. */
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
  printk("Mouse baud, port=%X, baud=%d\n", port, baud );
  BSP_uart_set_attributes(port, baud, databits, parity, stopbits);

  return 0;
}

/*
 * Handle ioctl request for ttyS2.
 */
static int
serial_mouse_conSetAttr( int minor, const struct termios *t)
{
  return conSetAttr( BSP_UART_PORT, minor, t );
}
