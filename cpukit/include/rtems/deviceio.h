/**
 * @file
 *
 * @brief Operations on IMFS Device Nodes 
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_DEVICEIO_H
#define _RTEMS_DEVICEIO_H

#include <rtems/libio.h>

/**
 *  @defgroup IMFSDevices IMFS Device IO Handler
 * 
 *  @ingroup IMFS
 *
 *  This contains the interface to device drivers using the RTEMS Classic API.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief IMFS device node handlers.
 *
 * IMFS Device Node Handlers
 *
 * This file contains the set of handlers used to map operations on
 * IMFS device nodes onto calls to the RTEMS Classic API IO Manager.
 */
int rtems_deviceio_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

int rtems_deviceio_close(
  rtems_libio_t *iop,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

ssize_t rtems_deviceio_read(
  rtems_libio_t *iop,
  void *buf,
  size_t nbyte,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

ssize_t rtems_deviceio_write(
  rtems_libio_t *iop,
  const void *buf,
  size_t nbyte,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

int rtems_deviceio_control(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *buffer,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

#ifdef __cplusplus
}
#endif
/* __cplusplus */

/**@}*/

#endif /* _RTEMS_DEVICEIO_H */
