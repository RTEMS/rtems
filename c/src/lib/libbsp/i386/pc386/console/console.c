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
| *  COPYRIGHT (c) 1989-1997.
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


#include <stdlib.h>

#include <bsp.h>
#include <irq.h>
#include <rtems/libio.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define KEYBOARD_IRQ  0x01  /* Keyboard IRQ. */


/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern rtems_isr _IBMPC_keyboard_isr(rtems_vector_number);
       /* keyboard (IRQ 0x01) Interrupt Service Routine (defined in 'inch.c') */


/*-------------------------------------------------------------------------+
| Functions
+--------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------+
|         Function: console_cleanup
|      Description: This routine is called at exit to clean up the console
|                   hardware. 
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
console_cleanup(void)
{
  /* nothing */
} /* console_cleanup */


/*-------------------------------------------------------------------------+
|         Function: is_character_ready
|      Description: Check if a character is available for input, and if so
|                   return it.
| Global Variables: None.
|        Arguments: c - character read if available, otherwise unchanged.
|          Returns: TRUE if there was a character available for input,
|                   FALSE otherwise. 
+--------------------------------------------------------------------------*/
rtems_boolean
is_character_ready(char *c)
{
  return (_IBMPC_chrdy(c) ? TRUE : FALSE);
} /* is_character_ready */


/*-------------------------------------------------------------------------+
|         Function: inbyte
|      Description: Read a character from the console (keyboard).
| Global Variables: None.
|        Arguments: None.
|          Returns: Caracter read from the console. 
+--------------------------------------------------------------------------*/
unsigned char
inbyte(void)
{
  char c = _IBMPC_inch();

  /* Echo character to screen */
  _IBMPC_outch(c);
  if (c == '\r')
    _IBMPC_outch('\n');  /* CR = CR + LF */

  return c;
} /* inbyte */


/*-------------------------------------------------------------------------+
|         Function: outbyte
|      Description: Write a character to the console (display).
| Global Variables: None.
|        Arguments: Character to be written.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
outbyte(char c)
{
  _IBMPC_outch(c);
} /* outbyte */


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

  /* Install keyboard interrupt handler */
  status = PC386_installRtemsIrqHandler(KEYBOARD_IRQ, _IBMPC_keyboard_isr);

  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error installing keyboard interrupt handler!\n");
    rtems_fatal_error_occurred(status);
  }

  status =
    rtems_io_register_name("/dev/console", major, (rtems_device_minor_number)0);
 
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering console device!\n");
    rtems_fatal_error_occurred(status);
  }
 
  atexit(console_cleanup);

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
  return RTEMS_SUCCESSFUL;
} /* console_open */


/*-------------------------------------------------------------------------+
| Console device driver CLOSE entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{
  return RTEMS_SUCCESSFUL;
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
 
  for (count = 0; count < maximum; count++)
  {
    buffer[count] = inbyte();
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
 
  for (count = 0; count < maximum; count++)
  {
    outbyte(buffer[count]);
    if (buffer[count] == '\n')
      outbyte('\r');            /* LF = LF + CR */
  }

  rw_args->bytes_moved = maximum;
  return RTEMS_SUCCESSFUL;
} /* console_write */


/*-------------------------------------------------------------------------+
| Console device driver CONTROL entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_control(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg)
{
  return RTEMS_SUCCESSFUL;
} /* console_control */
