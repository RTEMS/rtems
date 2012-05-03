/*  milkymist_pfpu.h
 *
 *  Milkymist PFPU driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_PFPU_H_
#define __MILKYMIST_PFPU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Ioctls */
#define PFPU_EXECUTE       0x4600

#define PFPU_PROGSIZE		   (2048)
#define PFPU_REG_COUNT		 (128)

struct pfpu_td {
	unsigned int *output;
	unsigned int hmeshlast;
	unsigned int vmeshlast;
	unsigned int *program;
	unsigned int progsize;
	float *registers;
	bool update; /* < shall we update the "registers" array after completion */
	bool invalidate; /* < shall we invalidate L1 data cache after completion */
};

rtems_device_driver pfpu_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver pfpu_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define PFPU_DRIVER_TABLE_ENTRY {pfpu_initialize, \
NULL, NULL, NULL, NULL, pfpu_control}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_PFPU_H_ */
