/*  milkymist_versions.h
 *
 *  Milkymist versioning driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_VERSIONS_H_
#define __MILKYMIST_VERSIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver versions_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver versions_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define VERSIONS_DRIVER_TABLE_ENTRY {versions_initialize, \
NULL, NULL, versions_read, NULL, NULL}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_VERSIONS_H_ */
