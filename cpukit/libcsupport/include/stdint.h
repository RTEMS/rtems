/*
 * stdint.h
 *
 * ISO C99 integer types
 *
 * $Id$
 */

#ifndef __STDINT_H
#define __STDINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>

/*
 * map RTEMS internal types onto C99 types
 */
typedef signed8        int8_t;
typedef signed16       int16_t;
typedef signed32       int32_t;
typedef signed64       int64_t;

typedef unsigned8      uint8_t;
typedef unsigned16     uint16_t;
typedef unsigned32     uint32_t;
typedef unsigned64     uint64_t;

typedef signed long	intptr_t;
typedef unsigned long 	uintptr_t;

#ifdef __cplusplus
}
#endif

#endif
