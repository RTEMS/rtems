/*
 *  console.c  -- console I/O package
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 * This code is based on the pc386 BSP console.c so the following
 * copyright also applies :
 *
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * Till Straumann, <strauman@slac.stanford.edu>, 12/20/2001
 * separate BSP specific stuff from generics...
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */
  
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>

extern int close(int fd);

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <termios.h>
#include <bsp/uart.h>
#include <bsp/consoleIo.h>
#include <rtems/bspIo.h>	/* printk */

/* Definitions for BSPConsolePort */
/*
 * Possible value for console input/output :
 *	BSP_CONSOLE_PORT_CONSOLE
 *	BSP_UART_COM1
 *	BSP_UART_COM2
 */
int BSPConsolePort = BSP_CONSOLE_PORT;

int BSPBaseBaud    = BSP_UART_BAUD_BASE;

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/

static int  conSetAttr(int minor, const struct termios *);

typedef struct TtySTblRec_ {
		char	*name;
		void	(*isr)(void); /* STUPID API doesn't pass a parameter :-( */
} TtySTblRec, *TtySTbl;

static TtySTblRec ttyS[]={
		{ "/dev/ttyS0",
#ifdef BSP_UART_IOBASE_COM1
		  BSP_uart_termios_isr_com1
#else
		  0
#endif
		},
		{ "/dev/ttyS1",
#ifdef BSP_UART_IOBASE_COM2
		  BSP_uart_termios_isr_com2
#else
		  0
#endif
		},
};


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

  /*
   *  The video was initialized in the start.s code and does not need
   *  to be reinitialized.
   */

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();
      
  /*
   * Do device-specific initialization
   */

  /* RTEMS calls this routine once with 'minor'==0; loop through
   * all known instances...
   */
     
  for (minor=0; minor < sizeof(ttyS)/sizeof(ttyS[0]); minor++) {
	char *nm;
	  /*
	   * Skip ports (possibly not supported by BSP...) we have no ISR for
	   */
	  if ( ! ttyS[minor].isr )
		continue;
	  /*
	   * Register the device
	   */
	  status = rtems_io_register_name ((nm=ttyS[minor].name), major, minor);
	  if ( RTEMS_SUCCESSFUL==status && BSPConsolePort == minor)
		{
		  printk("Registering /dev/console as minor %i (==%s)\n",
							minor,
							ttyS[minor].name);
		  /* also register an alias */
		  status = rtems_io_register_name (
							(nm="/dev/console"),
							major,
							minor);
		}
	  if (status != RTEMS_SUCCESSFUL)
		{
		  printk("Error registering %s!\n",nm);
		  rtems_fatal_error_occurred (status);
		}

  }
  return RTEMS_SUCCESSFUL;
} /* console_initialize */

static int console_first_open(int major, int minor, void *arg)
{
  rtems_status_code status;

	  /* must not open a minor device we have no ISR for */
	  assert( minor>=0 && minor < sizeof(ttyS)/sizeof(ttyS[0]) && ttyS[minor].isr );

	  /* 9600-8-N-1 */
	  BSP_uart_init(minor, 9600, 0);
	  status = BSP_uart_install_isr(minor, ttyS[minor].isr);
	  if (!status)
	  	{
		  printk("Error installing serial console interrupt handler for '%s'!\n",
				ttyS[minor].name);
		  rtems_fatal_error_occurred(status);
		}
	  return 0;
}


static int console_last_close(int major, int minor, void *arg)
{
  BSP_uart_remove_isr(minor, ttyS[minor].isr);
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
    console_first_open,			/* firstOpen */
    console_last_close,			/* lastClose */
    NULL,						/* pollRead */
    BSP_uart_termios_write_com,	/* write */
    conSetAttr,					/* setAttributes */
    NULL,						/* stopRemoteTx */
    NULL,						/* startRemoteTx */
    1							/* outputUsesInterrupts */
  };

  status = rtems_termios_open (major, minor, arg, &cb);

  if(status != RTEMS_SUCCESSFUL)
    {
      printk("Error openning console device\n");
      return status;
    }

  /*
   * Pass data area info down to driver
   */
  BSP_uart_termios_set(minor, 
			 ((rtems_libio_open_close_args_t *)arg)->iop->data1);
  /* Enable interrupts  on channel */
  BSP_uart_intr_ctrl(minor, BSP_UART_INTR_CTRL_TERMIOS);

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

  res =  rtems_termios_close (arg);
  
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

  return rtems_termios_read (arg);
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

  return rtems_termios_write (arg);
 
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
  return rtems_termios_ioctl (arg);
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

  BSP_uart_set_baud(minor, baud);

  return 0;
}
