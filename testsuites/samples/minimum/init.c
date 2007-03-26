/*  Minimum Size Application Initialization 
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
}

/* configuration information */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* global variables */
