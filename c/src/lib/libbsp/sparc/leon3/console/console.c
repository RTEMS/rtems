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
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <amba.h>

/*
 *  Should we use a polled or interrupt drived console?
 *  
 *  NOTE: This is defined in the custom/leon.cfg file.
 *
 *  WARNING:  In sis 1.6, it did not appear that the UART interrupts
 *            worked in a desirable fashion.  Immediately upon writing
 *            a character into the TX buffer, an interrupt was generated.
 *            This did not allow enough time for the program to put more
 *            characters in the buffer.  So every character resulted in
 *            "priming" the transmitter.   This effectively results in 
 *            in a polled console with a useless interrupt per character
 *            on output.  It is reasonable to assume that input does not
 *            share this problem although it was not investigated.
 *
 */

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
);

/* body is in debugputs.c */

/*
 *  console_inbyte_nonblocking 
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking( int port );

/* body is in debugputs.c */


/*
 *  Console Termios Support Entry Points
 *
 */

int console_write_support (int minor, const char *buf, int len)
{
  int nwrite = 0;

  while (nwrite < len) {
    console_outbyte_polled( minor, *buf++ );
    nwrite++;
  }
  return nwrite;
}


/*
 *  Console Device Driver Entry Points
 *
 */
int uarts = 0;
static int isinit = 0;
volatile LEON3_UART_Regs_Map *LEON3_Console_Uart[LEON3_APBUARTS];

int scan_uarts() {	
  unsigned int iobar, conf;
  int i;
	amba_apb_device apbuarts[LEON3_APBUARTS];
	
  if (isinit == 0) {
    i = 0; uarts = 0;
    
    uarts = amba_find_apbslvs(&amba_conf,VENDOR_GAISLER,GAISLER_APBUART,apbuarts,LEON3_APBUARTS);
    for(i=0; i<uarts; i++){
			LEON3_Console_Uart[i] = apbuarts[i].start;
		}
    isinit = 1;
  }
  return uarts;
}

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  int i, uart0;
  char console_name[16];
  extern rtems_configuration_table Configuration;

  rtems_termios_initialize();

  /* Find UARTs */
  scan_uarts();
	
  if (Configuration.User_multiprocessing_table != NULL)
    uart0 =  LEON3_Cpu_Index;
  else
    uart0 = 0;  

  /*  Register Device Names */
  
  if (uarts && (uart0 < uarts)) 
  {  
    status = rtems_io_register_name( "/dev/console", major, 0 );
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);

    strcpy(console_name,"/dev/console_a");
    for (i = uart0+1; i < uarts; i++)
    {
      console_name[13]++;
      status = rtems_io_register_name( console_name, major, i);
    }
  }


  /*
   *  Initialize Hardware
   */
  if ((Configuration.User_multiprocessing_table == NULL) ||
      ((Configuration.User_multiprocessing_table)->node == 1))
  {
    for (i = uart0; i < uarts; i++)
    {
      LEON3_Console_Uart[i]->ctrl |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE;
      LEON3_Console_Uart[i]->status = 0;  
    }
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

  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    console_inbyte_nonblocking,  /* pollRead */
    console_write_support,       /* write */
    NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };


  assert( minor <= LEON3_APBUARTS );
  if ( minor > LEON3_APBUARTS )
    return RTEMS_INVALID_NUMBER;
 
  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);


  return RTEMS_SUCCESSFUL;
}
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}
 
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}
 
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}
 
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}

/* putchar/getchar for printk */

static void bsp_out_char(char c)
{
  console_outbyte_polled(0, c);
}

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static char bsp_in_char(void)
{
  int tmp;

  while ((tmp = console_inbyte_nonblocking(0)) < 0);
  return (char) tmp;
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;
