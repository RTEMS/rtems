/*  extisrdrv.h
 *
 *  This file describes the external interrupt driver
 *
 */

#ifndef __EXT_ISR_DRIVER_H
#define __EXT_ISR_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* variables */

extern rtems_device_major_number rtems_externalISR_major;
extern rtems_device_minor_number rtems_externalISR_minor;

/* default external ISR driver entry */

#define EXTISR_DRIVER_TABLE_ENTRY \
  { ExternalISR_initialize, NULL, NULL, NULL, NULL, ExternalISR_control }
 
rtems_device_driver ExternalISR_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver ExternalISR_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
