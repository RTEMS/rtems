/*
 * This file will have to be incorparated into the RTEMS source
 * tree (probably in the existing <machine/types.h> so that these
 * contents are included when an application source file includes
 * <sys/types.h>.
 */

#ifndef _MACHINE_TYPES_H_
#define _MACHINE_TYPES_H_

#include <rtems.h>
#include <machine/endian.h>

typedef rtems_signed64		int64_t;
typedef rtems_signed32		int32_t;
typedef rtems_signed16		int16_t;
typedef rtems_signed8		int8_t;

typedef rtems_unsigned64	u_int64_t;
typedef rtems_unsigned32	u_int32_t;
typedef rtems_unsigned16	u_int16_t;
typedef rtems_unsigned8		u_int8_t;

#define	_CLOCK_T_	unsigned long
#define	_TIME_T_	long

#ifdef _COMPILING_BSD_KERNEL_
#include <rtems/rtems_bsdnet_internal.h>
#include <rtems/rtems_bsdnet.h>
#endif

#endif /* _MACHINE_TYPES_H_ */
