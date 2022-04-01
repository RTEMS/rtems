/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <tmacros.h>

#include "fs_config.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <rtems/libio.h>
#include <rtems/dosfs.h>
#include <rtems/libcsupport.h>

#include "ramdisk_support.h"
#include "fstest.h"
#include "fstest_support.h"

#define BLOCK_SIZE 512

static const msdos_format_request_param_t rqdata = {
  .OEMName             = "RTEMS",
  .VolLabel            = "RTEMSDisk",
  .sectors_per_cluster = 2,
  .fat_num             = 0,
  .files_per_root_dir  = 0,
  .media               = 0,
  .quick_format        = true,
  .skip_alignment      = 0,
  .info_level          = 0
};

static rtems_resource_snapshot before_mount;

void test_initialize_filesystem(void)
{
  int rc=0;
  rc = mkdir (BASE_FOR_TEST,S_IRWXU|S_IRWXG|S_IRWXO);
  rtems_test_assert(rc==0);

  init_ramdisk();

  rc=msdos_format(RAMDISK_PATH,&rqdata);
  rtems_test_assert(rc==0);

  rtems_resource_snapshot_take(&before_mount);

  rc=mount(RAMDISK_PATH,
      BASE_FOR_TEST,
      "dosfs",
      RTEMS_FILESYSTEM_READ_WRITE,
      NULL);
  rtems_test_assert(rc==0);
}


void test_shutdown_filesystem(void)
{
  int rc=0;
  rc=unmount(BASE_FOR_TEST) ;
  rtems_test_assert(rc==0);
  rtems_test_assert(rtems_resource_snapshot_check(&before_mount));
  del_ramdisk();
}

/* configuration information */

/* drivers */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

/**
 * Configure base RTEMS resources.
 */

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES RTEMS_DOSFS_SEMAPHORES_PER_INSTANCE
#define CONFIGURE_MAXIMUM_TASKS                     10
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS    40
#define CONFIGURE_INIT_TASK_STACK_SIZE (16 * 1024)
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

