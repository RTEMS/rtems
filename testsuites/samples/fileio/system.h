/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include "tmacros.h"

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* global variables */


/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define FILEIO_BUILD 1
#if BSP_SMALL_MEMORY
#undef FILEIO_BUILD
#endif

#if defined(RTEMS_BSP_HAS_IDE_DRIVER) && !BSP_SMALL_MEMORY
#include <libchip/ata.h> /* for ata driver prototype */
#include <libchip/ide_ctrl.h> /* for general ide driver prototype */
#endif

#define CONFIGURE_MAXIMUM_DRIVERS           4
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#ifdef RTEMS_BSP_HAS_IDE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
#define CONFIGURE_ATA_DRIVER_TASK_PRIORITY  14
#endif
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS  2
#define CONFIGURE_BDBUF_MAX_WRITE_BLOCKS       8
#define CONFIGURE_SWAPOUT_TASK_PRIORITY        15
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_RFS
#define CONFIGURE_FILESYSTEM_DOSFS

/*
 * XXX: these values are higher than needed...
 */
#define CONFIGURE_MAXIMUM_TASKS             20
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_STACK_CHECKER_ENABLED
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MALLOC_STATISTICS

#define CONFIGURE_UNIFIED_WORK_AREAS
#include <rtems/confdefs.h>

/* end of include file */
