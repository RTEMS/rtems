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

#ifndef _PRIMODE_H
#define _PRIMODE_H

#include <inttypes.h>

#ifndef SIZEOF_MODE_T
#error "missing SIZEOF_MODE_T"
#endif

#if SIZEOF_MODE_T == 8
#define PRIomode_t PRIo64
#elif SIZEOF_MODE_T == 4
#define PRIomode_t PRIo32
#else
#error "unsupported size of mode_t"
#endif

#endif
