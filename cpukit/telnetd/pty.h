/*
 * /dev/ptyXX  (A first version for pseudo-terminals)
 *
 *  Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _RTEMS_PTY_H
#define _RTEMS_PTY_H

#ifdef __cplusplus
extern "C" {
#endif	

#include <rtems.h>	

/* Number of ptys to setup */
extern size_t rtems_pty_maximum_ptys;

/* Return the devname for a free pty slot.
 * If no slot available (socket>=0)
 * then the socket argument is closed
 */
char * rtems_pty_get(int socket);


/* OBSOLETE */
#define get_pty		rtems_pty_get

rtems_device_driver pty_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg);
rtems_device_driver pty_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);


#define PTY_DRIVER_TABLE_ENTRY \
       { pty_initialize , pty_open , pty_close , \
   pty_read , pty_write , pty_control }

#ifdef __cplusplus
}
#endif	

#endif
