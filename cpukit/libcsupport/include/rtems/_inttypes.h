/*
 *  COPYRIGHT (c) 2004.
 *  Ralf Corsepius, Ulm/Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __rtems__inttypes_h
#define __rtems__inttypes_h

#include <rtems/_stdint.h>

#define __STRINGIFY(a) #a

/* 8-bit types */
#define __PRI8(x) __STRINGIFY(x)

#define PRId8	__PRI8(d)
#define PRIi8	__PRI8(i)
#define PRIo8	__PRI8(o)
#define PRIu8	__PRI8(u)
#define PRIx8	__PRI8(x)
#define PRIX8	__PRI8(X)


/* 16-bit types */
#define __PRI16(x) __STRINGIFY(x)

#define PRId16	__PRI16(d)
#define PRIi16	__PRI16(i)
#define PRIo16	__PRI16(o)
#define PRIu16	__PRI16(u)
#define PRIx16	__PRI16(x)
#define PRIX16	__PRI16(X)

/* 32-bit types */
#if defined(__rtems_long32)
#define __PRI32(x) __STRINGIFY(l##x)
#else
#define __PRI32(x) __STRINGIFY(x)
#endif

#define PRId32	__PRI32(d)
#define PRIi32	__PRI32(i)
#define PRIo32	__PRI32(o)
#define PRIu32	__PRI32(u)
#define PRIx32	__PRI32(x)
#define PRIX32	__PRI32(X)


/* 64-bit types */
#if defined(__rtems_longlong64)
#define __PRI64(x) __STRINGIFY(ll##x)
#elif defined(__rtems_longlong64)
#define __PRI64(x) __STRINGIFY(l##x)
#else
#define __PRI64(x) __STRINGIFY(x)
#endif

#define PRId64	__PRI64(d)
#define PRIi64	__PRI64(i)
#define PRIo64	__PRI64(o)
#define PRIu64	__PRI64(u)
#define PRIx64	__PRI64(x)
#define PRIX64	__PRI64(X)

#endif /* __rtems__inttypes_h */
