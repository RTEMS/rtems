/*
 * Copyright (c) 2011 by
 * Ralf Corsepius, Ulm, Germany. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

/*
 * Helper macro to print "time_t"
 */

#ifndef _PRITIME_H
#define _PRITIME_H

#include <inttypes.h>
#include <rtems/score/cpuopts.h>

#if __RTEMS_SIZEOF_TIME_T__ == 8
#define PRIdtime_t PRId64
#elif __RTEMS_SIZEOF_TIME_T__ == 4
#define PRIdtime_t PRId32
#else
#error "unsupported size of time_t"
#endif

#endif
