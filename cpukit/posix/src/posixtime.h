/*
 * Copyright (c) 2011.
 * Ralf Corsépius, Ulm/Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

/* 
 * Adapt RTEMS to newlibs with broken
 * CLOCK_PROCESS_CPUTIME_ID/CLOCK_PROCESS_CPUTIME
 * CLOCK_THREAD_CPUTIME_ID/CLOCK_THREAD_CPUTIME
 */

#ifndef _SRC_POSIX_POSIXTIME_H
#define _SRC_POSIX_POSIXTIME_H

#include <time.h>

#ifdef RTEMS_NEWLIB
/* Older newlibs bogusly used CLOCK_PROCESS_CPUTIME 
   instead of CLOCK_PROCESS_CPUTIME_ID */
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID CLOCK_PROCESS_CPUTIME
#endif
/* Older newlibs bogusly used CLOCK_THREAD_CPUTIME
   instead of CLOCK_PROCESS_CPUTIME_ID */
#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID CLOCK_THREAD_CPUTIME
#endif
#endif

#endif
