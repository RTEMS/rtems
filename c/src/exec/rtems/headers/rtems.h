/*  rtems.h
 *
 *  This include file contains information about RTEMS executive that
 *  is required by the application and is CPU independent.  It includes
 *  two (2) CPU dependent files to tailor its data structures for a
 *  particular processor.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_RTEMS_GENERIC_h
#define __RTEMS_RTEMS_GENERIC_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <rtems/types.h>

#include <rtems/init.h>
#include <rtems/tasks.h>
#include <rtems/intr.h>
#include <rtems/clock.h>
#include <rtems/extension.h>
#include <rtems/timer.h>
#include <rtems/sem.h>
#include <rtems/message.h>
#include <rtems/event.h>
#include <rtems/signal.h>
#include <rtems/event.h>
#include <rtems/part.h>
#include <rtems/region.h>
#include <rtems/dpmem.h>
#include <rtems/io.h>
#include <rtems/fatal.h>
#include <rtems/ratemon.h>
#include <rtems/mp.h>

#include <rtems/support.h>

#define RTEMS_HAS_HARDWARE_FP CPU_HARDWARE_FP

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
