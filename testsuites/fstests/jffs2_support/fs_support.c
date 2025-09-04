/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dev/flash/flash_sim_flashdev.h>
#include <dev/flash/jffs2_flashdev.h>
#include <rtems/jffs2.h>
#include <rtems/libio.h>

#include "fstest.h"
#include "fstest_support.h"

#define FLASH_PAGE_SIZE ( 1024UL )
#define PAGES_PER_SECTOR ( 16UL )
#define SECTORS_PER_DEVICE ( 8UL )

#define FLASHDEV_PATH "dev/flashdev0"

static rtems_jffs2_compressor_control compressor_instance = {
  .compress = rtems_jffs2_compressor_rtime_compress,
  .decompress = rtems_jffs2_compressor_rtime_decompress
};

static rtems_flashdev *flash = NULL;

void test_initialize_filesystem( void )
{
  int rv;
  int status;
  rtems_status_code sc;
  rtems_flashdev_region region;
  struct flash_sim_flashdev_attributes attr;

  attr.read_delay_ns = 0;
  attr.write_delay_ns = 0;
  attr.erase_delay_ns = 0;
  attr.page_size_bytes = FLASH_PAGE_SIZE;
  attr.pages_per_sector = PAGES_PER_SECTOR;
  attr.total_sectors = SECTORS_PER_DEVICE;
  attr.type = RTEMS_FLASHDEV_NOR;
  attr.alloc = NULL;
  attr.free = NULL;

  flash = flash_sim_flashdev_init( &attr );
  rtems_test_assert( flash != NULL );

  /* Register the flashdev as a device */
  status = rtems_flashdev_register( flash, FLASHDEV_PATH );
  rtems_test_assert( !status );

  rv = mkdir( BASE_FOR_TEST, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( rv == 0 );

  region.offset = 0;
  region.size = FLASH_PAGE_SIZE * PAGES_PER_SECTOR * SECTORS_PER_DEVICE;

  sc = jffs2_flashdev_mount(
    FLASHDEV_PATH,
    BASE_FOR_TEST,
    &region,
    &compressor_instance,
    false
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
}

void test_shutdown_filesystem( void )
{
  int rv = unmount( BASE_FOR_TEST );
  rtems_test_assert( rv == 0 );

  rv = rtems_flashdev_unregister( FLASHDEV_PATH );
  rtems_test_assert( rv == 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_FILESYSTEM_JFFS2

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 40

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
