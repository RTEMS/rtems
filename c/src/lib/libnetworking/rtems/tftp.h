/*
 *  $Id$
 */

/*
 * Trivial File Transfer Protocol (TFTP)
 *
 * Transfer file to/from remote host
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

/*
 * Usage:
 * 
 * To open `/bootfiles/image' on `hostname' for reading:
 *         fd = open ("/TFTP/hostname/bootfiles/image", O_RDONLY);
 *
 * The `hostname' must be four dot-separated decimal values.
 *
 * To open a file on the host which supplied the BOOTP
 * information just leave the `hostname' part empty:
 *         fd = open ("/TFTP//bootfiles/image", O_RDONLY);
 *
 */

#ifndef _TFTP_DRIVER_h
#define _TFTP_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Driver entry points
 */
#define TFTP_DRIVER_TABLE_ENTRY \
  { rtems_tftp_initialize, rtems_tftp_open, rtems_tftp_close, \
    rtems_tftp_read, rtems_tftp_write, rtems_tftp_control }

rtems_device_driver rtems_tftp_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver rtems_tftp_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver rtems_tftp_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver rtems_tftp_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver rtems_tftp_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver rtems_tftp_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
