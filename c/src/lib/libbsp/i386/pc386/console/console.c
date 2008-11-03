/*-------------------------------------------------------------------------+
| console.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the PC386 console I/O package.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   console.c,v 1.4 1995/12/19 20:07:23 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <uart.h>
#include <libcpu/cpuModel.h>

#include <rtems/mw_uid.h>
#include "mouse_parser.h"

/*
 * Possible value for console input/output :
 *	BSP_CONSOLE_PORT_CONSOLE
 *	BSP_UART_COM1
 *	BSP_UART_COM2
 *
 * Note:
 *   1. Currently BSPPrintkPort, cannot be assigned to COM2,
 *      it will be fixed soon.
 *
 *   2. If both BSPConsolePort and BSPPrintkport are assigned
 *      to same serial device it does not work that great
 */

#if (USE_COM1_AS_CONSOLE == 1)
int BSPConsolePort = BSP_UART_COM1;
int BSPPrintkPort  = BSP_UART_COM1;
#else
int BSPConsolePort = BSP_CONSOLE_PORT_CONSOLE;
int BSPPrintkPort  = BSP_CONSOLE_PORT_CONSOLE;
#endif

int BSPBaseBaud    = 115200;

extern BSP_polling_getchar_function_type BSP_poll_char;
extern int getch( void );
extern void kbd_init( void );

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void keyboard_interrupt(void );
extern void keyboard_interrupt_wrapper(void *);
extern char BSP_wait_polled_input(void);
extern void _IBMPC_initVideo(void);

static int  conSetAttr(int minor, const struct termios *);
static void isr_on(const rtems_irq_connect_data *);
static void isr_off(const rtems_irq_connect_data *);
static int  isr_is_on(const rtems_irq_connect_data *);

extern int rtems_kbpoll( void );

static rtems_irq_connect_data console_isr_data = {BSP_KEYBOARD,
                                                  keyboard_interrupt_wrapper,
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

extern int  rtems_kbpoll( void );

static int
ibmpc_console_write(int minor, const char *buf, int len)
{
  int count;
  for (count = 0; count < len; count++)
  {
    _IBMPC_outch( buf[ count ] );
    if( buf[ count ] == '\n')
      _IBMPC_outch( '\r' );            /* LF = LF + CR */
  }
  return 0;
}

int kbd_poll_read( int minor )
{
  if( rtems_kbpoll() )
  {
     int c = getch();
     return c;
  }
  return -1;
}

/*-------------------------------------------------------------------------+
| Console device driver INITIALIZE entry point.
+--------------------------------------------------------------------------+
| Initilizes the I/O console (keyboard + VGA display) driver.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
  rtems_status_code status;

  /* Initialize the KBD interface */
  kbd_init();

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();

#ifdef RTEMS_RUNTIME_CONSOLE_SELECT
  /*
   * If no video card, fall back to serial port console
   */
#include <crt.h>
  if((BSPConsolePort == BSP_CONSOLE_PORT_CONSOLE)
   && (*(unsigned char*) NB_MAX_ROW_ADDR == 0)
   && (*(unsigned short*)NB_MAX_COL_ADDR == 0)) {
    BSPConsolePort = BSP_UART_COM2;
    BSPPrintkPort  = BSP_UART_COM1;
  }
#endif

  /*
   *  The video was initialized in the start.s code and does not need
   *  to be reinitialized.
   */

  if(BSPConsolePort == BSP_CONSOLE_PORT_CONSOLE)
    {
      /* Install keyboard interrupt handler */
      status = BSP_install_rtems_irq_handler(&console_isr_data);

    if (!status)
	{
	  printk("Error installing keyboard interrupt handler!\n");
	  rtems_fatal_error_occurred(status);
	}

      status = rtems_io_register_name("/dev/console", major, 0);
      if (status != RTEMS_SUCCESSFUL)
	{
	  printk("Error registering console device!\n");
	  rtems_fatal_error_occurred(status);
	}
      printk("Initialized console on port CONSOLE\n\n");
    }
  else
    {
      /*
       * Do device-specific initialization
       */
      /* 9600-8-N-1 */
      BSP_uart_init(BSPConsolePort, 9600, CHR_8_BITS, 0, 0, 0);

      /* Set interrupt handler */
      if(BSPConsolePort == BSP_UART_COM1)
   	{
	     console_isr_data.name = BSP_UART_COM1_IRQ;
        console_isr_data.hdl  = BSP_uart_termios_isr_com1;

   	}
      else
	   {
          assert(BSPConsolePort == BSP_UART_COM2);
          console_isr_data.name = BSP_UART_COM2_IRQ;
          console_isr_data.hdl  = BSP_uart_termios_isr_com2;
    	}
      status = BSP_install_rtems_irq_handler(&console_isr_data);

      if (!status){
	  printk("Error installing serial console interrupt handler!\n");
	  rtems_fatal_error_occurred(status);
      }
      /*
       * Register the device
       */
      status = rtems_io_register_name ("/dev/console", major, 0);
      if (status != RTEMS_SUCCESSFUL)
	{
	  printk("Error registering console device!\n");
	  rtems_fatal_error_occurred (status);
	}

      if(BSPConsolePort == BSP_UART_COM1)
	{
	  printk("Initialized console on port COM1 9600-8-N-1\n\n");
	}
      else
	{
	  printk("Initialized console on port COM2 9600-8-N-1\n\n");
	}

      if(BSPPrintkPort == BSP_UART_COM1)
        {
          printk("Warning : This will be the last message on console\n");

          /*
           * FIXME: cast below defeats the very idea of having
           * function pointer types defined
           */
          BSP_output_char = (BSP_output_char_function_type)
                              BSP_output_char_via_serial;
          BSP_poll_char   = (BSP_polling_getchar_function_type)
                              BSP_poll_char_via_serial;
        }
      else if(BSPPrintkPort != BSP_CONSOLE_PORT_CONSOLE)
        {
           printk("illegal assignement of printk channel");
         rtems_fatal_error_occurred (status);
        }

    }
  return RTEMS_SUCCESSFUL;
} /* console_initialize */

static int console_open_count = 0;

static int console_last_close(int major, int minor, void *arg)
{
  BSP_remove_rtems_irq_handler (&console_isr_data);

  return 0;
}

static int ser_console_first_open(int major, int minor, void *arg)
{
  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set(BSPConsolePort,
			 ((rtems_libio_open_close_args_t *)arg)->iop->data1);

  /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl(BSPConsolePort, BSP_UART_INTR_CTRL_TERMIOS);

  return 0;
}

/*-------------------------------------------------------------------------+
| Console device driver OPEN entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_open(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg)
{
  rtems_status_code              status;
  static rtems_termios_callbacks cb =
  {
    NULL,	              /* firstOpen */
    console_last_close,       /* lastClose */
    NULL,          /* pollRead */
    BSP_uart_termios_write_com1, /* write */
    conSetAttr,	              /* setAttributes */
    NULL,	              /* stopRemoteTx */
    NULL,	              /* startRemoteTx */
    1		              /* outputUsesInterrupts */
  };

  if(BSPConsolePort == BSP_CONSOLE_PORT_CONSOLE)
    {

      /* Let's set the routines for termios to poll the
       * Kbd queue for data
       */
      cb.pollRead = kbd_poll_read;
      cb.outputUsesInterrupts = 0;
      /* write the "echo" if it is on */
      cb.write = ibmpc_console_write;

      cb.setAttributes = NULL;
      ++console_open_count;
      status = rtems_termios_open (major, minor, arg, &cb);
      if(status != RTEMS_SUCCESSFUL)
      {
         printk("Error openning console device\n");
      }
      return status;
    }

  if(BSPConsolePort == BSP_UART_COM2)
    {
      cb.write = BSP_uart_termios_write_com2;
    }

  cb.firstOpen = ser_console_first_open;

  status = rtems_termios_open (major, minor, arg, &cb);

  if(status != RTEMS_SUCCESSFUL)
    {
      printk("Error openning console device\n");
      return status;
    }

  return RTEMS_SUCCESSFUL;
}

/*-------------------------------------------------------------------------+
| Console device driver CLOSE entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{
   return rtems_termios_close (arg);
} /* console_close */

/*-------------------------------------------------------------------------+
| Console device driver READ entry point.
+--------------------------------------------------------------------------+
| Read characters from the I/O console. We only have stdin.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_read(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{
 return rtems_termios_read( arg );
} /* console_read */

/*-------------------------------------------------------------------------+
| Console device driver WRITE entry point.
+--------------------------------------------------------------------------+
| Write characters to the I/O console. Stderr and stdout are the same.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                    * arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  char                  *buffer  = rw_args->buffer;
  int                    maximum  = rw_args->count;

  if(BSPConsolePort != BSP_CONSOLE_PORT_CONSOLE)
    {
      return rtems_termios_write (arg);
    }

  /* write data to VGA */
  ibmpc_console_write( minor, buffer, maximum );
  rw_args->bytes_moved = maximum;
  return RTEMS_SUCCESSFUL;
} /* console_write */

extern int vt_ioctl( unsigned int cmd, unsigned long arg);

/*
 * Handle ioctl request.
 */
rtems_device_driver
console_control(rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                      * arg
)
{
	rtems_libio_ioctl_args_t *args = arg;
	switch (args->command)
	{
	   default:
      if( vt_ioctl( args->command, (unsigned long)args->buffer ) != 0 )
          return rtems_termios_ioctl (arg);
		break;

      case MW_UID_REGISTER_DEVICE:
      printk( "SerialMouse: reg=%s\n", args->buffer );
      register_kbd_msg_queue( args->buffer, 0 );
		break;

      case MW_UID_UNREGISTER_DEVICE:
      unregister_kbd_msg_queue( 0 );
		break;
   }
 	args->ioctl_return = 0;
   return RTEMS_SUCCESSFUL;
}

static int
conSetAttr(int minor, const struct termios *t)
{
  unsigned long baud, databits, parity, stopbits;

  baud = termios_baud_to_number(t->c_cflag & CBAUD);
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

  BSP_uart_set_attributes(BSPConsolePort, baud, databits, parity, stopbits);

  return 0;
}

void keyboard_interrupt_wrapper(void *unused){
  keyboard_interrupt();
}

/*
 * BSP initialization
 */

BSP_output_char_function_type BSP_output_char =
                       (BSP_output_char_function_type) _IBMPC_outch;

BSP_polling_getchar_function_type BSP_poll_char = BSP_wait_polled_input;
