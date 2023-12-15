/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 On-Line Applications Research (OAR)
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

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <rtems/jffs2.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#include "fstest.h"
#include "fstest_support.h"

#define FLASH_PAGE_SIZE (1024UL)
/* Out of Band/Spare area size is per-page */
#define FLASH_PAGE_OOB_SIZE (32UL)

#define PAGES_PER_BLOCK (16UL)
#define BLOCKS_PER_DEVICE (8UL)

#define FLASH_OOB_SIZE (BLOCKS_PER_DEVICE * PAGES_PER_BLOCK * FLASH_PAGE_OOB_SIZE)
#define FLASH_BLOCK_SIZE (PAGES_PER_BLOCK * FLASH_PAGE_SIZE)
#define FLASH_SIZE (BLOCKS_PER_DEVICE * FLASH_BLOCK_SIZE)

typedef struct {
  rtems_jffs2_flash_control super;
  unsigned char area[FLASH_SIZE];
  unsigned char oob[FLASH_OOB_SIZE];
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
  uint32_t page_index = offset / FLASH_PAGE_SIZE;
  uint32_t oob_offset = page_index * FLASH_PAGE_OOB_SIZE;
  unsigned char *chunk = &self->area[offset];
  unsigned char *oobchunk = &self->oob[oob_offset];

  memset(chunk, 0xff, FLASH_BLOCK_SIZE);
  memset(oobchunk, 0xff, PAGES_PER_BLOCK * FLASH_PAGE_OOB_SIZE);

  return 0;
}

static int flash_read_oob(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  uint8_t *buffer,
  uint32_t size_of_buffer
)
{
  flash_control *self = get_flash_control(super);
  uint32_t page_index = offset / FLASH_PAGE_SIZE;
  uint32_t oob_offset = page_index * FLASH_PAGE_OOB_SIZE;
  unsigned char *chunk = &self->oob[oob_offset];

  memcpy(buffer, chunk, size_of_buffer);

  return 0;
}

static int flash_write_oob(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  uint8_t *buffer,
  uint32_t size_of_buffer
)
{
  flash_control *self = get_flash_control(super);
  uint32_t page_index = offset / FLASH_PAGE_SIZE;
  uint32_t oob_offset = page_index * FLASH_PAGE_OOB_SIZE;
  unsigned char *chunk = &self->oob[oob_offset];
  size_t i;

  for (i = 0; i < size_of_buffer; ++i) {
    chunk[i] &= buffer[i];
  }

  return 0;
}

static int flash_block_is_bad(
  rtems_jffs2_flash_control *super,
  uint32_t orig_offset,
  bool *bad
)
{
  *bad = false;
  return 0;
}

static int flash_block_mark_bad(
  rtems_jffs2_flash_control *super,
  uint32_t orig_offset
)
{
  return 0;
}

static uint32_t flash_get_oob_size(
  rtems_jffs2_flash_control *super
)
{
  return FLASH_PAGE_OOB_SIZE;
}

static flash_control flash_instance = {
  .super = {
    .block_size = FLASH_BLOCK_SIZE,
    .flash_size = FLASH_SIZE,
    .read = flash_read,
    .write = flash_write,
    .erase = flash_erase,
    .block_is_bad = flash_block_is_bad,
    .block_mark_bad = flash_block_mark_bad,
    .oob_read = flash_read_oob,
    .oob_write = flash_write_oob,
    .get_oob_size = flash_get_oob_size,
    .write_size = FLASH_PAGE_SIZE
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
  memset(&flash_instance.oob[0], 0xff, FLASH_OOB_SIZE);
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
