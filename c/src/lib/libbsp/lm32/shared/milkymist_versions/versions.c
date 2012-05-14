/*  versions.c
 *
 *  Milkymist versioning driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <rtems.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "milkymist_versions.h"

#define SOC_DEVICE_NAME "/dev/soc"
#define PCB_DEVICE_NAME "/dev/pcb"
#define PCBREV_DEVICE_NAME "/dev/pcb_rev"

rtems_device_driver versions_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;

  sc = rtems_io_register_name(SOC_DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create SoC version device");
  sc = rtems_io_register_name(PCB_DEVICE_NAME, major, 1);
  RTEMS_CHECK_SC(sc, "create PCB type device");
  sc = rtems_io_register_name(PCBREV_DEVICE_NAME, major, 2);
  RTEMS_CHECK_SC(sc, "create PCB revision device");

  return RTEMS_SUCCESSFUL;
}

static int get_soc_version(char *buffer)
{
  char fmt[13];
  char *version;
  int len;
  unsigned int id;
  unsigned int major, minor, subminor, rc;

  id = MM_READ(MM_SYSTEM_ID);
	major = (id & 0xf0000000) >> 28;
	minor = (id & 0x0f000000) >> 24;
	subminor = (id & 0x00f00000) >> 20;
	rc = (id & 0x000f0000) >> 16;

  version = fmt;
  version += sprintf(version, "%u.%u", major, minor);
	if (subminor != 0)
		version += sprintf(version, ".%u", subminor);
	if (rc != 0)
		version += sprintf(version, "RC%u", rc);

  len = version - fmt;
  memcpy(buffer, fmt, len);
  return len;
}

static int get_pcb_type(char *buffer)
{
  unsigned int id;

  id = MM_READ(MM_SYSTEM_ID);
  buffer[0] = (id & 0x0000ff00) >> 8;
  buffer[1] = id & 0x000000ff;
  return 2;
}

static int get_pcb_revision(char *buffer)
{
  unsigned int v;

  v = MM_READ(MM_GPIO_IN);
  v = (v & 0x78) >> 3;
  buffer[0] = '0' + v;
  return 1;
}

rtems_device_driver versions_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;

  if(rw_args->offset != 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  switch (minor) {
    case 0:
      if (rw_args->count < 12) {
        rw_args->bytes_moved = 0;
        return RTEMS_UNSATISFIED;
      }
      rw_args->bytes_moved = get_soc_version((char *)rw_args->buffer);
      return RTEMS_SUCCESSFUL;
    case 1:
      if (rw_args->count < 2) {
        rw_args->bytes_moved = 0;
        return RTEMS_UNSATISFIED;
      }
      rw_args->bytes_moved = get_pcb_type((char *)rw_args->buffer);
      return RTEMS_SUCCESSFUL;
    case 2:
      if (rw_args->count < 1) {
        rw_args->bytes_moved = 0;
        return RTEMS_UNSATISFIED;
      }
      rw_args->bytes_moved = get_pcb_revision((char *)rw_args->buffer);
      return RTEMS_SUCCESSFUL;
  }

  rw_args->bytes_moved = 0;
  return RTEMS_UNSATISFIED;
}
