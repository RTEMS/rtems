/*  gpio.c
 *
 *  This file is the gpio driver for the Milkymist One board
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <rtems.h>
#include <bsp.h>
#include "../include/system_conf.h"
#include <rtems/libio.h>

#define GPIO_DRIVER_TABLE_ENTRY { gpio_initialize, \
gpio_open, gpio_close, gpio_read, gpio_write, gpio_control}

#define LED1_DEVICE_NAME  "/dev/led1"
#define LED2_DEVICE_NAME  "/dev/led2"
#define BTN1_DEVICE_NAME  "/dev/btn1"
#define BTN2_DEVICE_NAME  "/dev/btn2"
#define BTN3_DEVICE_NAME  "/dev/btn3"

static struct milkymist_gpio {
  rtems_device_minor_number minor;
  unsigned int *address;
  unsigned int offset;
  uint8_t readonly;
  pthread_mutex_t mutex;
} gpio[5];

rtems_device_driver gpio_initialize(
rtems_device_major_number  major,
rtems_device_minor_number  minor,
void *arg)
{
  rtems_status_code status;

  printk( "gpio driver initializing..\n" );

  status = rtems_io_register_name(LED1_DEVICE_NAME, major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering gpio device led1\n");
    rtems_fatal_error_occurred( status );
  }
  gpio[0].minor = 0;
  gpio[0].readonly = 0;
  gpio[0].offset = 0x00000001;
  gpio[0].address = (unsigned int *)0xe0001004;
  gpio[0].mutex = PTHREAD_MUTEX_INITIALIZER;

  status = rtems_io_register_name(LED2_DEVICE_NAME, major, 1);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering gpio device led2\n");
    rtems_fatal_error_occurred( status );
  }
  gpio[1].minor = 1;
  gpio[1].readonly = 0;
  gpio[1].offset = 0x00000002;
  gpio[1].address = (unsigned int *)0xe0001004;
  gpio[1].mutex = PTHREAD_MUTEX_INITIALIZER;

  status = rtems_io_register_name(BTN1_DEVICE_NAME, major, 2);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering gpio device btn1\n");
    rtems_fatal_error_occurred( status );
  }
  gpio[2].minor = 2;
  gpio[2].readonly = 1;
  gpio[2].offset = 0x00000001;
  gpio[2].address = (unsigned int *)0xe0001000;
  gpio[2].mutex = PTHREAD_MUTEX_INITIALIZER;
  status = rtems_io_register_name(BTN2_DEVICE_NAME, major, 3);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering gpio device btn2\n");
    rtems_fatal_error_occurred( status );
  }
  gpio[3].minor = 3;
  gpio[3].readonly = 1;
  gpio[3].address = (unsigned int *)0xe0001000;
  gpio[3].offset = 0x00000002;
  gpio[3].mutex = PTHREAD_MUTEX_INITIALIZER;

  status = rtems_io_register_name(BTN3_DEVICE_NAME, major, 4);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering gpio device btn3\n");
    rtems_fatal_error_occurred( status );
  }

  gpio[4].minor = 4;
  gpio[4].readonly = 1;
  gpio[4].offset = 0x00000004;
  gpio[4].address = (unsigned int *)0xe0001000;
  gpio[4].mutex = PTHREAD_MUTEX_INITIALIZER;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  if (pthread_mutex_unlock(&gpio[minor].mutex) == 0){
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_UNSATISFIED;
}

rtems_device_driver gpio_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  if (pthread_mutex_trylock(&gpio[minor].mutex) == 0){
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_UNSATISFIED;
}


rtems_device_driver gpio_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = 1;
  
  if ( *gpio[minor].address & gpio[minor].offset )
    *(uint8_t *)rw_args->buffer = 1;
  else
    *(uint8_t *)rw_args->buffer = 0;
  
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = 1;

  if (gpio[minor].readonly)
    return RTEMS_UNSATISFIED;

  if ( *(uint8_t *)rw_args->buffer )
    *gpio[minor].address |= gpio[minor].offset;
  else
    *gpio[minor].address &= ~gpio[minor].offset;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch( args->command ) {
    default:
     args->ioctl_return = 0;
     break;
  }
  return RTEMS_SUCCESSFUL;
}

