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
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#undef __assert
void __assert (const char *file, int line, const char *msg);

#include <bsp.h>
#include <irq.h>
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

int BSPConsolePort = BSP_CONSOLE_PORT_CONSOLE;
int BSPPrintkPort  = BSP_CONSOLE_PORT_CONSOLE;

/* int BSPConsolePort = BSP_UART_COM2;  */
int BSPBaseBaud    = 115200;

extern BSP_polling_getchar_function_type BSP_poll_char;
extern int getch( void );
extern void kbd_init( void );

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void keyboard_interrupt(void);
extern char BSP_wait_polled_input(void);
extern void _IBMPC_initVideo(void);

static int  conSetAttr(int minor, const struct termios *);
static void isr_on(const rtems_irq_connect_data *);
static void isr_off(const rtems_irq_connect_data *);
static int  isr_is_on(const rtems_irq_connect_data *);

extern int rtems_kbpoll( void );

static rtems_irq_connect_data console_isr_data = {BSP_KEYBOARD,
                     keyboard_interrupt,
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

extern char _IBMPC_inch(void);
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

/*
static void*	     termios_ttyp_console         = NULL;
void enq_key( char key )
{
  if( termios_ttyp_console )
  {
	  rtems_termios_enqueue_raw_characters(termios_ttyp_console, &key,1 );
  }
}
*/

void __assert (const char *file, int line, const char *msg)
{
    static   char exit_msg[] = "EXECUTIVE SHUTDOWN! Any key to reboot...";
  unsigned char  ch;
   
  /*
   * Note we cannot call exit or printf from here, 
   * assert can fail inside ISR too
   */

   /*
   * Close console
   */
  close(2);
  close(1);
  close(0);

  printk("\nassert failed: %s: ", file);
  printk("%d: ", line);
  printk("%s\n\n", msg);
  printk(exit_msg);
  ch = BSP_poll_char();
  printk("\n\n");
  rtemsReboot();

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

  status = rtems_termios_open (major, minor, arg, &cb);

  if(status != RTEMS_SUCCESSFUL)
    {
      printk("Error openning console device\n");
      return status;
    }

  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set(BSPConsolePort, 
			 ((rtems_libio_open_close_args_t *)arg)->iop->data1);
  
  /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl(BSPConsolePort, BSP_UART_INTR_CTRL_TERMIOS);

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

  BSP_uart_set_attributes(BSPConsolePort, baud, databits, parity, stopbits);

  return 0;
}

/*
 * BSP initialization
 */

BSP_output_char_function_type BSP_output_char = 
                       (BSP_output_char_function_type) _IBMPC_outch;

BSP_polling_getchar_function_type BSP_poll_char = BSP_wait_polled_input;


