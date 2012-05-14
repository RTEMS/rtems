/*
 *  Driver for the sh1 703x on-chip serial devices (sci)
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _sh_sci_h
#define _sh_sci_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Devices are set to 9600 bps, 8 databits, 1 stopbit, no
 * parity and asynchronous mode by default.
 *
 * NOTE:
 *       The onboard serial devices of the SH do not support hardware
 *       handshake.
 */

#define DEVSCI_DRIVER_TABLE_ENTRY \
  { sh_sci_initialize, sh_sci_open, sh_sci_close, sh_sci_read, \
      sh_sci_write, sh_sci_control }

extern rtems_device_driver sh_sci_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_sci_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_sci_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_sci_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_sci_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_sci_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
