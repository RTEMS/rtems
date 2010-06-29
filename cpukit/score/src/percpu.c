/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>
#include <string.h>

/*
 * On single core systems, we can efficiently directly access a single
 * statically allocated per cpu structure.  And the fields are initialized
 * as individual elements just like it has always been done.
 */
Per_CPU_Control _Per_CPU_Information;
