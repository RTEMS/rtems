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
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
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

#include <bsp.h>
#include <irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <pc386uart.h>

int PC386ConsolePort = PC386_CONSOLE_PORT_CONSOLE;

static int conSetAttr(int minor, const struct termios *);

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void 	_IBMPC_keyboard_isr(void);
extern void 	_IBMPC_keyboard_isr_on(const rtems_irq_connect_data*);
extern void 	_IBMPC_keyboard_isr_off(const rtems_irq_connect_data*);
extern int 	_IBMPC_keyboard_isr_is_on(const rtems_irq_connect_data*);

static rtems_irq_connect_data console_isr_data = {PC_386_KEYBOARD,
						   _IBMPC_keyboard_isr,
						   _IBMPC_keyboard_isr_on,
						   _IBMPC_keyboard_isr_off,
						   _IBMPC_keyboard_isr_is_on};
						   

extern rtems_boolean _IBMPC_scankey(char *);  /* defined in 'inch.c' */

void console_reserve_resources(rtems_configuration_table *conf)
{
  if(PC386ConsolePort != PC386_CONSOLE_PORT_CONSOLE)
    {
      rtems_termios_reserve_resources(conf, 1);
    }
  return;
}

void __assert(const char *file, int line, const char *msg)
{
  static   char buf[20];
  static   char exit_msg[] = "EXECUTIVE SHUTDOWN! Any key to reboot...";
  static   char assert_msg[] = "assert failed: ";
  unsigned char  ch;
  const    unsigned char *cp;
	
 
  /*
   * Note we cannot call exit or printf from here, 
   * assert can fail inside ISR too
   */
  if(PC386ConsolePort == PC386_CONSOLE_PORT_CONSOLE)
    {
      printk("\nassert failed: %s: ", file);
      printk("%d: ", line);
      printk("%s\n\n", msg);
      printk(exit_msg);
      while(!_IBMPC_scankey(&ch));
      printk("\n\n");
    }
  else
    {
      PC386_uart_intr_ctrl(PC386ConsolePort, PC386_UART_INTR_CTRL_DISABLE);
      
      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
      
      for(cp=assert_msg; *cp!=0; cp++)
	{
	  PC386_uart_polled_write(PC386ConsolePort, *cp);
	}

      for(cp=file; *cp!=0; cp++)
	{
	  PC386_uart_polled_write(PC386ConsolePort, *cp);
	}
      
      PC386_uart_polled_write(PC386ConsolePort, ':');
      PC386_uart_polled_write(PC386ConsolePort, ' ');

      sprintf(buf, "%d: ", line);

      for(cp=buf; *cp!=0; cp++)
	{
	  PC386_uart_polled_write(PC386ConsolePort, *cp);
	}

      for(cp=msg; *cp!=0; cp++)
	{
	  PC386_uart_polled_write(PC386ConsolePort, *cp);
	}

      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
	  
      for(cp=exit_msg; *cp != 0; cp++)
	{
	  PC386_uart_polled_write(PC386ConsolePort, *cp);
	}

      PC386_uart_polled_read(PC386ConsolePort);

      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');
      PC386_uart_polled_write(PC386ConsolePort, '\r');
      PC386_uart_polled_write(PC386ConsolePort, '\n');

    }

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

  /* Initialize video */
  _IBMPC_initVideo();

  if(PC386ConsolePort == PC386_CONSOLE_PORT_CONSOLE)
    {

      /* Install keyboard interrupt handler */
  status = pc386_install_rtems_irq_handler(&console_isr_data);
  
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
       * Set up TERMIOS
       */
      rtems_termios_initialize ();
      
      /*
       * Do device-specific initialization
       */
      
      /* 9600-8-N-1 */
      PC386_uart_init(PC386ConsolePort, 9600, 0);
      
      
      /* Set interrupt handler */
      if(PC386ConsolePort == PC386_UART_COM1)
	{
	  console_isr_data.name = PC386_UART_COM1_IRQ;
	  console_isr_data.hdl  = PC386_uart_termios_isr_com1;
	  
	}
      else
	{
          assert(PC386ConsolePort == PC386_UART_COM2);
	  console_isr_data.name = PC386_UART_COM2_IRQ;
	  console_isr_data.hdl  = PC386_uart_termios_isr_com2;
	}

      status =pc386_install_rtems_irq_handler(&console_isr_data);
      /*
       * Register the device
       */
      status = rtems_io_register_name ("/dev/console", major, 0);
      if (status != RTEMS_SUCCESSFUL)
	{
	  printk("Error registering console device!\n");
	  rtems_fatal_error_occurred (status);
	}

      if(PC386ConsolePort == PC386_UART_COM1)
	{
	  printk("Initialized console on port COM1 9600-8-N-1\n\n");
	}
      else
	{
	  printk("Initialized console on port COM2 9600-8-N-1\n\n");
	}
    }
 
  return RTEMS_SUCCESSFUL;
} /* console_initialize */


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
    NULL,	              /* lastClose */
    NULL,	              /* pollRead */
    PC386_uart_termios_write_com1, /* write */
    conSetAttr,	              /* setAttributes */
    NULL,	              /* stopRemoteTx */
    NULL,	              /* startRemoteTx */
    1		              /* outputUsesInterrupts */
  };

  if(PC386ConsolePort == PC386_CONSOLE_PORT_CONSOLE)
    {
      return RTEMS_SUCCESSFUL;
    }

  if(PC386ConsolePort == PC386_UART_COM2)
    {
      cb.write = PC386_uart_termios_write_com2;
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
  PC386_uart_termios_set(PC386ConsolePort, 
			 ((rtems_libio_open_close_args_t *)arg)->iop->data1);
  
  /* Enable interrupts  on channel */
  PC386_uart_intr_ctrl(PC386ConsolePort, PC386_UART_INTR_CTRL_TERMIOS);

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
  rtems_device_driver res = RTEMS_SUCCESSFUL;
  
  if(PC386ConsolePort != PC386_CONSOLE_PORT_CONSOLE)
    {
      res =  rtems_termios_close (arg);
    }
  pc386_remove_rtems_irq_handler (&console_isr_data);
  
  return res;
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
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  char                  *buffer  = rw_args->buffer;
  int            count, maximum  = rw_args->count;

  if(PC386ConsolePort != PC386_CONSOLE_PORT_CONSOLE)
    {
      return rtems_termios_read (arg);
    }
 
  for (count = 0; count < maximum; count++)
  {
    /* Get character */
    buffer[count] = _IBMPC_inch_sleep();

    /* Echo character to screen */
    _IBMPC_outch(buffer[count]);
    if (buffer[count] == '\r')
      {
	_IBMPC_outch('\n');  /* CR = CR + LF */
      }

    if (buffer[count] == '\n' || buffer[count] == '\r')
    {
      /* What if this goes past the end of the buffer?  We're hosed. [bhc] */
      buffer[count++]  = '\n';
      buffer[count]    = '\0';
      break;
    }
  }
 
  rw_args->bytes_moved = count;
  return ((count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED);
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
  int            count, maximum  = rw_args->count;

  if(PC386ConsolePort != PC386_CONSOLE_PORT_CONSOLE)
    {
      return rtems_termios_write (arg);
    }
 
  for (count = 0; count < maximum; count++)
  {
    _IBMPC_outch(buffer[count]);
    if (buffer[count] == '\n')
      _IBMPC_outch('\r');            /* LF = LF + CR */
  }

  rw_args->bytes_moved = maximum;
  return RTEMS_SUCCESSFUL;
} /* console_write */


 
/*
 * Handle ioctl request.
 */
rtems_device_driver 
console_control(rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                      * arg
)
{ 
  if(PC386ConsolePort != PC386_CONSOLE_PORT_CONSOLE)
    {
      return rtems_termios_ioctl (arg);
    }

  return RTEMS_SUCCESSFUL;
}

static int
conSetAttr(int minor, const struct termios *t)
{
  int baud;

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
      baud = 0;
      rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);
      return 0;
    }

  PC386_uart_set_baud(PC386ConsolePort, baud);

  return 0;
}









