/**
 * @file rtems/sptables.h
 *
 *  This include file contains the executive's pre-initialized tables
 *  used when in a single processor configuration.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SPTABLES_H
#define _RTEMS_SPTABLES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/config.h>

#include <rtems/debug.h>
#include <rtems/fatal.h>
#include <rtems/init.h>
#include <rtems/io.h>
#include <rtems/score/sysstate.h>

#include <rtems/rtems/intr.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/mp.h>
#endif
#include <rtems/rtems/part.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/timer.h>

#if defined(RTEMS_MULTIPROCESSING)
/*
 *  This is the default Multiprocessing Configuration Table.
 *  It is used in single processor configurations.
 */
  #if defined(SAPI_INIT)
    const rtems_multiprocessing_table
	   _Initialization_Default_multiprocessing_table = {
      1,                        /* local node number */
      1,                        /* maximum number nodes in system */
      0,                        /* maximum number global objects */
      0,                        /* maximum number proxies */
      STACK_MINIMUM_SIZE,       /* MPCI receive server stack size */
      NULL,                     /* pointer to MPCI address table */
    };
  #else
    extern const rtems_multiprocessing_table
	   _Initialization_Default_multiprocessing_table;
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif
