/*-------------------------------------------------------------------------+
| console.c - ARM BSP 
+--------------------------------------------------------------------------+
| This file contains the ARM console I/O package.
+--------------------------------------------------------------------------+
|   COPYRIGHT (c) 2000 Canon Research France SA.
|   Emmanuel Raguet, mailto:raguet@crf.canon.fr
| 
|   The license and distribution terms for this file may be
|   found in found in the file LICENSE in this distribution or at
|   http://www.OARcorp.com/rtems/license.html.
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
#include <rtems/bspIo.h>
#include <irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <registers.h>
#include <uart.h>

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

int BSPConsolePort = BSP_UART_COM1;
int BSPPrintkPort  = BSP_UART_COM1;

int BSPBaseBaud    = 115200;


/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern char BSP_wait_polled_input(void);
extern BSP_polling_getchar_function_type BSP_poll_char;
extern void rtemsReboot(void);

static int  conSetAttr(int minor, const struct termios *);
static void isr_on(const rtems_irq_connect_data *);
static void isr_off(const rtems_irq_connect_data *);
static int  isr_is_on(const rtems_irq_connect_data *);

/*
 * BSP initialization
 */

/* for printk support */
BSP_output_char_function_type BSP_output_char = 
   (BSP_output_char_function_type) BSP_output_char_via_serial;
BSP_polling_getchar_function_type BSP_poll_char = 
   (BSP_polling_getchar_function_type) BSP_poll_char_via_serial;


static rtems_irq_connect_data console_isr_data = {BSP_UART,
						   BSP_uart_termios_isr_com1,
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
  if (Regs[INTMASK] & 0x4)
    return 0;
  else
    return 1;
}

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

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();
  
  /*
   * Do device-specific initialization
   */
      
  /* 38400-8-N-1 */
  BSP_uart_init(BSPConsolePort, 38400, 0);
      
      
  /* Set interrupt handler */
  console_isr_data.name = BSP_UART;
  console_isr_data.hdl  = BSP_uart_termios_isr_com1;
  console_isr_data.irqLevel = 3;
  console_isr_data.irqTrigger = 0;
    
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

  printk("Initialized console on port COM1 38400-8-N-1\n\n");

  return RTEMS_SUCCESSFUL;
} /* console_initialize */


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
    NULL,	              /* pollRead */
    BSP_uart_termios_write_com1, /* write */
    conSetAttr,	              /* setAttributes */
    NULL,	              /* stopRemoteTx */
    NULL,	              /* startRemoteTx */
    1		              /* outputUsesInterrupts */
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

  BSP_uart_set_baud(BSPConsolePort, baud);

  return 0;
}


