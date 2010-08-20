/*  ac97.c
 *
 *  This file is the sound driver for the Milkymist One board
 *  It does an OSS style character device in /dev/snd
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

#define GPIO_DRIVER_TABLE_ENTRY { ac97_initialize, \
ac97_open, ac97_close, ac97_read, ac97_write, ac97_control}

#define SND_DEVICE_NAME  "/dev/snd"

static struct milkymist_ac97 {
  rtems_device_minor_number minor;
  pthread_mutex_t mutex;
} ac97_driver;

rtems_device_driver gpio_initialize(
rtems_device_major_number  major,
rtems_device_minor_number  minor,
void *arg)
{
  rtems_status_code status;

  printk( "ac97 driver initializing..\n" );

  status = rtems_io_register_name(SND_DEVICE_NAME, major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering /dev/snd ac97 device \n");
    rtems_fatal_error_occurred( status );
  }
  gpio[0].minor = 0;
  gpio[0].mutex = PTHREAD_MUTEX_INITIALIZER;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  if (pthread_mutex_unlock(&ac97_driver.mutex) == 0){
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
  if (pthread_mutex_trylock(&ac97_driver.mutex) == 0){
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
  
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;

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

