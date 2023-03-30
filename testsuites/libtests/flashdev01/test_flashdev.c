/*
 * Copyright (C) 2023 Aaron Nyholm
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

#include "test_flashdev.h"

#include <stdlib.h>
#include <string.h>

#include <rtems/seterr.h>

#define TEST_DATA_SIZE (PAGE_SIZE * PAGE_COUNT)
#define PAGE_COUNT 16
#define PAGE_SIZE 128
#define WB_SIZE 1
#define MAX_NUM_REGIONS 48
#define BITALLOC_SIZE 32
#define NUM_BITALLOC ((MAX_NUM_REGIONS + BITALLOC_SIZE - 1) / BITALLOC_SIZE)

/**
 * This flash device driver is for testing flashdev
 * API calls.
 */
typedef struct test_flashdev {
  char* data;
  uint32_t jedec_id;
  uint32_t bit_allocator[NUM_BITALLOC];
  rtems_flashdev_region regions[MAX_NUM_REGIONS];
} test_flashdev;

int test_flashdev_page_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *page_offset,
  size_t *page_size
);

int test_flashdev_page_by_index(
  rtems_flashdev *flash,
  off_t search_index,
  off_t *page_offset,
  size_t *page_size
);

int test_flashdev_page_count(
  rtems_flashdev *flash,
  int *page_count
);

int test_flashdev_wb_size(
  rtems_flashdev *flash,
  size_t *write_block_size
);

uint32_t test_flashdev_jedec_id(
  rtems_flashdev* flash
);

int test_flashdev_type(
  rtems_flashdev* flash,
  rtems_flashdev_flash_type* type
);

int test_flashdev_read(
  rtems_flashdev* flash,
  uintptr_t offset,
  size_t count,
  void* buffer
);

int test_flashdev_write(
  rtems_flashdev* flash,
  uintptr_t offset,
  size_t count,
  const void* buffer
);

int test_flashdev_erase(
  rtems_flashdev* flash,
  uintptr_t offset,
  size_t count
);

/* Find page info by offset handler */
int test_flashdev_page_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *page_offset,
  size_t *page_size
)
{
  *page_offset = search_offset - (search_offset%PAGE_SIZE);
  *page_size = PAGE_SIZE;
  return 0;
}

/* Find page by index handler */
int test_flashdev_page_by_index(
  rtems_flashdev *flash,
  off_t search_index,
  off_t *page_offset,
  size_t *page_size
)
{
  *page_offset = search_index * PAGE_SIZE;
  *page_size = PAGE_SIZE;
  return 0;
}

/* Page count handler */
int test_flashdev_page_count(
  rtems_flashdev *flash,
  int *page_count
)
{
  *page_count = PAGE_COUNT;
  return 0;
}

/* Write block size handler */
int test_flashdev_wb_size(
  rtems_flashdev *flash,
  size_t *write_block_size
)
{
  *write_block_size = WB_SIZE;
  return 0;
}

/* JEDEC ID handler, this would normally require a READID
 * call to the physical flash device.
 */
uint32_t test_flashdev_jedec_id(
  rtems_flashdev* flash
)
{
  test_flashdev* driver = flash->driver;
  return driver->jedec_id;
}

/* Function to identify what kind of flash is attached. */
int test_flashdev_type(
  rtems_flashdev *flash,
  rtems_flashdev_flash_type *type
)
{
  *type = RTEMS_FLASHDEV_NOR;
  return 0;
}

/* Read flash call. Any offset or count protections are
 * required to be done in the driver function. */
int test_flashdev_read(
  rtems_flashdev* flash,
  uintptr_t offset,
  size_t count,
  void* buffer
)
{
  test_flashdev* driver = flash->driver;

  if (offset + count > TEST_DATA_SIZE) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  memcpy(buffer, &driver->data[offset], count);
  return 0;
}

/* Write Flash call. Any offset or count protections are
 * required to be done in the driver function. */
int test_flashdev_write(
  rtems_flashdev* flash,
  uintptr_t offset,
  size_t count,
  const void* buffer
)
{
  test_flashdev* driver = flash->driver;

  if (offset + count > TEST_DATA_SIZE) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  memcpy(&driver->data[offset], buffer, count);
  return 0;
}

/* Erase Flash call. Any offset or count protections are
 * required to be done in the driver function. */
int test_flashdev_erase(
  rtems_flashdev* flash,
  uintptr_t offset,
  size_t count
)
{
  test_flashdev* driver = flash->driver;

  if (offset + count > TEST_DATA_SIZE) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if (offset%PAGE_SIZE || count%PAGE_SIZE) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  memset(&driver->data[offset], 0, count);
  return 0;
}

/* Initialize Flashdev and underlying driver. */
rtems_flashdev* test_flashdev_init(void)
{
  rtems_flashdev *flash = rtems_flashdev_alloc_and_init(sizeof(rtems_flashdev));

  if (flash == NULL) {
    return NULL;
  }

  test_flashdev* flash_driver = calloc(1, sizeof(test_flashdev));

  if (flash_driver == NULL) {
    rtems_flashdev_destroy_and_free(flash);
    return NULL;
  }

  flash_driver->data = calloc(1, TEST_DATA_SIZE);
  if (flash_driver->data == NULL) {
    free(flash_driver);
    rtems_flashdev_destroy_and_free(flash);
    return NULL;
  }

  flash_driver->jedec_id = 0x00ABCDEF;

  rtems_flashdev_region_table *ftable = calloc(1, sizeof(rtems_flashdev_region_table));
  ftable->max_regions = MAX_NUM_REGIONS;
  ftable->regions = flash_driver->regions;
  ftable->bit_allocator = flash_driver->bit_allocator;

  flash->driver = flash_driver;
  flash->read = &test_flashdev_read;
  flash->write = &test_flashdev_write;
  flash->erase = &test_flashdev_erase;
  flash->jedec_id = &test_flashdev_jedec_id;
  flash->flash_type = &test_flashdev_type;
  flash->page_info_by_offset = &test_flashdev_page_by_off;
  flash->page_info_by_index = &test_flashdev_page_by_index;
  flash->page_count = &test_flashdev_page_count;
  flash->write_block_size = &test_flashdev_wb_size;
  flash->region_table = ftable;

  return flash;
}
