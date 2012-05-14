/**
 *  @file console.c
 *
 *  This file contains the GBA console I/O package.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <bsp/irq.h>
#include <gba.h>
#include <conio.h>

/**
 *  @brief gba_pollRead function read char
 *
 *  @param  minor unused
 *  @return character code
 */
static int gba_pollRead(int minor)
{
    return(gba_getch());
}

/**
 *  @brief gba_write function writes chars
 *
 *  Input parameters:   minor code, buffer pointer and lenght
 *  @param  minor unused
 *  @param  *buf buffer pointer
 *  @param  len lenght of buffer
 *  @return character code
 *
 */
static ssize_t gba_write(int minor, const char *buf, size_t len)
{
  int i;

  for (i=0;i<len;i++) {
     gba_putch((unsigned short)buf[i]);
  }
  return len;
}

/**
 *  @brief gba_setAttributes function is empty
 *
 *  @param  minor unused
 *  @param  *t unused
 *  @return constant 0
 */
static int gba_setAttributes(int minor, const struct termios *t)
{
  return 0;
}

/** BSP_output_char for printk support */
BSP_output_char_function_type     BSP_output_char = (BSP_output_char_function_type)     gba_putch;
/** BSP_poll_char for printk support */
BSP_polling_getchar_function_type BSP_poll_char   = gba_getch;

/**
 *  @brief Console device driver INITIALIZE entry point
 *
 *  Initilizes the I/O console driver.
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
rtems_device_driver
console_initialize(rtems_device_major_number  major,
                   rtems_device_minor_number  minor,
                   void                      *arg)
{
  rtems_status_code status;

  /* Set up TERMIOS */
  rtems_termios_initialize ();

  /* Do device-specific initialization  */
  /* Allready done in bspstart.c -> gba_textmode(CO60); */

  /* Register the device */
  status = rtems_io_register_name ("/dev/console", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
      printk("Error registering console device!\n");
      rtems_fatal_error_occurred (status);
  }

  printk("Initialized GBA console\n\n");

  return RTEMS_SUCCESSFUL;
}

/**
 *  @brief console_first_open function is empty
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
static int console_first_open(int major, int minor, void *arg)
{
    return 0;
}

/**
 *  @brief console_last_close function is empty
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
static int console_last_close(int major, int minor, void *arg)
{
    return 0;
}


/**
 *  @brief Console device driver OPEN entry point
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
rtems_device_driver
console_open(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{
  rtems_status_code              status;
  static rtems_termios_callbacks cb =
  {
    console_first_open,         /* firstOpen     */
    console_last_close,         /* lastClose     */
    gba_pollRead,               /* pollRead      */
    gba_write,                  /* write         */
    gba_setAttributes,          /* setAttributes */
    NULL,                       /* stopRemoteTx  */
    NULL,                       /* startRemoteTx */
    TERMIOS_POLLED              /* 1 = outputUsesInterrupts */
  };

  status = rtems_termios_open (major, minor, arg, &cb);

  if (status != RTEMS_SUCCESSFUL) {
      printk("Error openning console device\n");
      return status;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 *  @brief Console device driver CLOSE entry point
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
rtems_device_driver
console_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{
  rtems_device_driver res = RTEMS_SUCCESSFUL;

  res =  rtems_termios_close (arg);

  return res;
}

/**
 *  @brief Console device driver READ entry point.
 *
 *  Read characters from the I/O console.
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
rtems_device_driver
console_read(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{

    return rtems_termios_read (arg);

}

/**
 *  @brief Console device driver WRITE entry point.
 *
 *  Write characters to the I/O console.
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
*/
rtems_device_driver
console_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{
    return rtems_termios_write (arg);
}

/**
 *  @brief Handle ioctl request.
 *
 *  @param  major diver major number
 *  @param  minor driver minor mumber
 *  @param  *arg pointer to parameters
 *  @return status code
 */
rtems_device_driver
console_control(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg
)
{
    return rtems_termios_ioctl (arg);
}
