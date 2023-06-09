/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
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

#include "tmacros.h"

#include "test_flashdev.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define TEST_NAME_LENGTH 10

#define TEST_DATA_SIZE (PAGE_SIZE * PAGE_COUNT)
#define PAGE_COUNT 16
#define PAGE_SIZE 128
#define WB_SIZE 1

const char rtems_test_name[] = "FLASHDEV 1";

static void run_test(void);

static void run_test(void) {

  char buff[TEST_DATA_SIZE] = {0};
  FILE *file;
  int fd;
  rtems_flashdev* flash;
  int status;
  char* read_data;
  rtems_flashdev_region e_args;
  rtems_flashdev_ioctl_page_info pg_info;
  rtems_flashdev_region region;
  uint32_t jedec;
  int page_count;
  int type;
  size_t wb_size;

  /* Initalize the flash device driver and flashdev */
  flash = test_flashdev_init();
  rtems_test_assert(flash != NULL);

  /* Register the flashdev as a device */
  status = rtems_flashdev_register(flash, "dev/flashdev0");
  rtems_test_assert(!status);

  /* Open the flashdev */
  file = fopen("dev/flashdev0", "r+");
  rtems_test_assert(file != NULL);
  fd = fileno(file);

  /* Read data from flash */
  read_data = fgets(buff, TEST_DATA_SIZE, file);
  rtems_test_assert(read_data != NULL);

  /* Fseek to start of flash */
  status = fseek(file, 0x0, SEEK_SET);
  rtems_test_assert(!status);

  /* Write the test name to the flash */
  status = fwrite(rtems_test_name, TEST_NAME_LENGTH, 1, file);
  rtems_test_assert(status == 1);

  /* Fseek to start of flash and read again */
  status = fseek(file, 0x0, SEEK_SET);
  rtems_test_assert(!status);
  fgets(buff, TEST_DATA_SIZE, file);
  rtems_test_assert(!strncmp(buff, rtems_test_name, TEST_NAME_LENGTH));

  /* Test Erasing */
  e_args.offset = 0x0;
  e_args.size = PAGE_SIZE;
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_ERASE, &e_args);
  rtems_test_assert(!status);

  fseek(file, 0x0, SEEK_SET);
  fgets(buff, TEST_DATA_SIZE, file);
  rtems_test_assert(buff[0] == 0);

  /* Test getting JEDEC ID */
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_JEDEC_ID, &jedec);
  rtems_test_assert(!status);
  rtems_test_assert(jedec == 0x00ABCDEF);

  /* Test getting flash type */
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_TYPE, &type);
  rtems_test_assert(!status);
  rtems_test_assert(type == RTEMS_FLASHDEV_NOR);

  /* Test getting page info from offset */
  pg_info.location = PAGE_SIZE + PAGE_SIZE/2;

  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_OFFSET, &pg_info);
  rtems_test_assert(!status);
  rtems_test_assert(pg_info.page_info.offset == PAGE_SIZE);
  rtems_test_assert(pg_info.page_info.size == PAGE_SIZE);

  /* Test getting page info from index */
  pg_info.location = 2;
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_INDEX, &pg_info);
  rtems_test_assert(!status);
  rtems_test_assert(pg_info.page_info.offset == 2*PAGE_SIZE);
  rtems_test_assert(pg_info.page_info.size == PAGE_SIZE);

  /* Test getting page count */
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_PAGE_COUNT, &page_count);
  rtems_test_assert(!status);
  rtems_test_assert(page_count == PAGE_COUNT);

  /* Test getting write block size */
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_WRITE_BLOCK_SIZE, &wb_size);
  rtems_test_assert(!status);
  rtems_test_assert(wb_size == WB_SIZE);

  /* Test Regions */
  region.offset = 0x400;
  region.size = 0x200;
  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_REGION_SET, &region);
  rtems_test_assert(!status);

  /* Test read to larger then region */
  fseek(file, 0x0, SEEK_SET);
  read_data = fgets(buff, 2048, file);
  rtems_test_assert(read_data == NULL);

  /* Test fseek outside of region */
  status = fseek(file, 0x201, SEEK_SET);
  rtems_test_assert(status);

  /* Write to base unset region and check the writes location */
  fseek(file, 0x0, SEEK_SET);
  fwrite("HELLO WORLD", 11, 1, file);
  ioctl(fd, RTEMS_FLASHDEV_IOCTL_REGION_UNSET, NULL);
  fseek(file, 0x400, SEEK_SET);
  fgets(buff, 11, file);
  rtems_test_assert(strncmp(buff, "HELLO WORLD", 11));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  run_test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 2000

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
