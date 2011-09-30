/*
 * Copyright (c) 2011 by
 * Ralf Corsépius, Ulm, Germany. All rights reserved.
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

#ifndef SIZEOF_TIME_T
#error "missing SIZEOF_TIME_T"
#endif

#if SIZEOF_TIME_T == 8
#define PRIdtime_t PRId64
#elif SIZEOF_TIME_T == 4
#define PRIdtime_t PRId32
#else
#error "unsupported size of time_t"
#endif

#endif
