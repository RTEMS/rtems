/**
 * @file  rtems/stdint.h
 *
 * Wrapper to <stdint.h>, switching to <inttypes.h> on systems 
 * only having <inttypes.h> (e.g. Solaris-5.7).
 */
 
/*
 * $Id$
 */

#ifndef _RTEMS_STDINT_H
#define _RTEMS_STDINT_H

#include <rtems/score/cpuopts.h>

#if RTEMS_USES_STDINT_H
#include <stdint.h>

#else
#include <inttypes.h>
#endif

typedef uint8_t		unsigned8;	/* unsigned 8-bit  integer */
typedef uint16_t	unsigned16;	/* unsigned 16-bit integer */
typedef uint32_t	unsigned32;	/* unsigned 32-bit integer */
#if 0
typedef uint64_t	unsigned64;	/* unsigned 64-bit integer */
#endif

typedef int8_t		signed8;	/* signed 8-bit  integer */
typedef int16_t		signed16;	/* signed 16-bit integer */
typedef int32_t		signed32;	/* signed 32-bit integer */
#if 0
typedef int64_t		signed64;	/* signed 64-bit integer */
#endif

#endif
