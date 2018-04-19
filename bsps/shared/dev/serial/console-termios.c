/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/console.h>
#include <rtems/termiostypes.h>

rtems_device_driver console_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_INTERNAL_ERROR;
}

rtems_device_driver console_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_INTERNAL_ERROR;
}

rtems_device_driver console_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_INTERNAL_ERROR;
}

rtems_device_driver console_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_INTERNAL_ERROR;
}

rtems_device_driver console_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_INTERNAL_ERROR;
}
