/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "pmacros.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <rtems/jffs2.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#include "fstest.h"
#include "fstest_support.h"

#define BLOCK_SIZE (16UL * 1024UL)

#define FLASH_SIZE (8UL * BLOCK_SIZE)

typedef struct {
  rtems_jffs2_flash_control super;
  unsigned char area[FLASH_SIZE];
} flash_control;

static flash_control *get_flash_control(rtems_jffs2_flash_control *super)
{
  return (flash_control *) super;
}

static int flash_read(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  unsigned char *buffer,
  size_t size_of_buffer
)
{
  flash_control *self = get_flash_control(super);
  unsigned char *chunk = &self->area[offset];

  memcpy(buffer, chunk, size_of_buffer);

  return 0;
}

static int flash_write(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  const unsigned char *buffer,
  size_t size_of_buffer
)
{
  flash_control *self = get_flash_control(super);
  unsigned char *chunk = &self->area[offset];
  size_t i;

  for (i = 0; i < size_of_buffer; ++i) {
    chunk[i] &= buffer[i];
  }

  return 0;
}

static int flash_erase(
  rtems_jffs2_flash_control *super,
  uint32_t offset
)
{
  flash_control *self = get_flash_control(super);
  unsigned char *chunk = &self->area[offset];

  memset(chunk, 0xff, BLOCK_SIZE);

  return 0;
}

static flash_control flash_instance = {
  .super = {
    .block_size = BLOCK_SIZE,
    .flash_size = FLASH_SIZE,
    .read = flash_read,
    .write = flash_write,
    .erase = flash_erase
  }
};

static rtems_jffs2_compressor_control compressor_instance = {
  .compress = rtems_jffs2_compressor_rtime_compress,
  .decompress = rtems_jffs2_compressor_rtime_decompress
};

static const rtems_jffs2_mount_data mount_data = {
  .flash_control = &flash_instance.super,
  .compressor_control = &compressor_instance
};

static void erase_all(void)
{
  memset(&flash_instance.area[0], 0xff, FLASH_SIZE);
}

static rtems_resource_snapshot before_mount;

void test_initialize_filesystem(void)
{
  int rv;

  erase_all();

  rv = mkdir(BASE_FOR_TEST, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(rv == 0);

  rtems_resource_snapshot_take(&before_mount);

  rv = mount(
    NULL,
    BASE_FOR_TEST,
    RTEMS_FILESYSTEM_TYPE_JFFS2,
    RTEMS_FILESYSTEM_READ_WRITE,
    &mount_data
  );
  rtems_test_assert(rv == 0);
}

void test_shutdown_filesystem(void)
{
  int rv = unmount(BASE_FOR_TEST);
  rtems_test_assert(rv == 0);
  rtems_test_assert(rtems_resource_snapshot_check(&before_mount));
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_JFFS2

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 40

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
