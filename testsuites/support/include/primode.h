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
#include <rtems/score/cpuopts.h>

#if __RTEMS_SIZEOF_MODE_T__ == 8
#define PRIomode_t PRIo64
#elif __RTEMS_SIZEOF_MODE_T__ == 4
#define PRIomode_t PRIo32
#else
#error "unsupported size of mode_t"
#endif

#endif
