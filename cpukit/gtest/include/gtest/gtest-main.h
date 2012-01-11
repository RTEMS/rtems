/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef GTEST_MAIN_H
#define GTEST_MAIN_H

#include <stdlib.h>

#include <gtest/gtest.h>

extern "C" {
  void *POSIX_Init(void *)
  {
    int argc = 0;
    char **argv = NULL;
    testing::InitGoogleTest(&argc, argv);

    int exit_status = RUN_ALL_TESTS();
  
    exit(exit_status);
  
    return NULL;
  }
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_IMFS
#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 64

#define CONFIGURE_MAXIMUM_DRIVERS 32

#define CONFIGURE_MAXIMUM_TASK_VARIABLES 8
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 8

#define CONFIGURE_MAXIMUM_BARRIERS rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_PARTITIONS rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_PERIODS rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_REGIONS rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_SEMAPHORES rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_TIMERS rtems_resource_unlimited(4)

#define CONFIGURE_MESSAGE_BUFFER_MEMORY (64 * 1024)

#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_POSIX_KEYS rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES rtems_resource_unlimited(4)
#define CONFIGURE_MAXIMUM_POSIX_THREADS rtems_resource_unlimited(4)

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 512
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE (32 * 1024)
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (8 * CONFIGURE_BDBUF_BUFFER_MAX_SIZE)
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 0

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#include <rtems.h>
#include <rtems/confdefs.h>

#endif /* GTEST_MAIN_H */
