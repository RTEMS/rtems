/*
 * This file will have to be incorparated into the RTEMS source
 * tree (probably in the existing <machine/types.h> so that these
 * contents are included when an application source file includes
 * <sys/types.h>.
 *
 *  $Id$
 */

#ifndef _MACHINE_TYPES_H_
#define _MACHINE_TYPES_H_

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <machine/endian.h>

typedef signed64	int64_t;
typedef signed32	int32_t;
typedef signed16	int16_t;
typedef signed8		int8_t;

typedef unsigned64	u_int64_t;
typedef unsigned32	u_int32_t;
typedef unsigned16	u_int16_t;
typedef unsigned8	u_int8_t;

#define	_CLOCK_T_	unsigned long
#define	_TIME_T_	long
#define _CLOCKID_T_     unsigned long
#define _TIMER_T_       unsigned long

#ifdef _COMPILING_BSD_KERNEL_
#include <rtems/rtems_bsdnet_internal.h>
#include <rtems/rtems_bsdnet.h>
#endif

#endif /* _MACHINE_TYPES_H_ */
