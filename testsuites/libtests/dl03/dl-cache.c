/*
 * Copyright (c) 2016, 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems/rtl/rtl.h>
#include <rtems/rtl/rtl-obj-cache.h>
#include <rtems/rtl/rtl-trace.h>

#include "dl-cache.h"

#define CACHE_SIZE         (2048)
#define CACHE_SIZE_TOO_BIG (-1)
#define CACHE_BUFFER_SIZE  (CACHE_SIZE * 4)

static uint8_t* contents;

static const char *const filename = "/dl-test";

static void dl_cache_create_file(void)
{
  uint16_t* p;
  int       fd;
  int       i;
  rtems_test_assert((contents = malloc(CACHE_BUFFER_SIZE)) != NULL);
  memset(contents, 0, CACHE_BUFFER_SIZE);
  p = (uint16_t*) contents;
  for (i = 0; i < (CACHE_BUFFER_SIZE / sizeof(uint16_t)); ++i)
    *p++ = i;
  rtems_test_assert((fd = open(filename,
                               O_WRONLY | O_TRUNC | O_CREAT,
                               S_IRUSR | S_IWUSR)) >= 0);
  rtems_test_assert(write(fd, contents, CACHE_BUFFER_SIZE) == CACHE_BUFFER_SIZE);
  rtems_test_assert(close(fd) >= 0);
}

static bool dl_cache_check(void* buffer, off_t offset, size_t length)
{
  uint16_t* b;
  uint16_t* c;
  int       i;
  b = buffer;
  c = (uint16_t*) (contents + offset);
  printf("cache:   buffer: ");
  for (i = 0; i < 4; ++i)
    printf("%04x/%04x ", b[i], c[i]);
  printf("\n");
  return memcmp(buffer, contents + offset, length) == 0;
}

static off_t dl_cache_buffer_offset(rtems_rtl_obj_cache* cache, void* buffer)
{
  return (off_t) (((uint8_t*) buffer) - ((uint8_t*) cache->buffer));
}

static void dl_init_rtl(void)
{
  /*
   * Check the RTL object is created and can be locked and unlocked.
   */
  rtems_test_assert(rtems_rtl_data_unprotected () == NULL);
  rtems_test_assert(rtems_rtl_lock () != NULL);
  rtems_rtl_unlock ();
  rtems_test_assert(rtems_rtl_data_unprotected () != NULL);
  rtems_rtl_trace_set_mask(RTEMS_RTL_TRACE_ALL | RTEMS_RTL_TRACE_CACHE);
}

int dl_cache_test(void)
{
  rtems_rtl_obj_cache cache;
  int                 fd;
  void*               buffer;
  off_t               offset_in;
  off_t               offset;
  size_t              length_in;
  size_t              length;

  /*
   * Make sure the RTL can initialise.
   */
  dl_init_rtl();

  /*
   * Create the file to test the cache with.
   */
  dl_cache_create_file();

  /*
   * Check the too big error is handled.
   */
  printf ("cache create with large size\n");
  rtems_test_assert(rtems_rtl_obj_cache_open(&cache,
                                             CACHE_SIZE_TOO_BIG) == false);

  /*
   * Create a cache.
   */
  printf ("cache create\n");
  rtems_test_assert(rtems_rtl_obj_cache_open(&cache,
                                             CACHE_SIZE) == true);
  rtems_test_assert(cache.fd == -1);
  rtems_test_assert(cache.file_size == 0);
  rtems_test_assert(cache.size == CACHE_SIZE);
  rtems_test_assert(cache.buffer != NULL);

  /*
   * Open the file to use with the cache tests.
   */
  printf ("cache file open\n");
  rtems_test_assert((fd = open(filename, O_RDONLY)) >= 0);

  buffer = NULL; offset_in = 0; length_in = length = CACHE_SIZE / 2;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == offset_in);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = 0; length_in = length = CACHE_SIZE;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == offset_in);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_SIZE / 2; length_in = length = CACHE_SIZE / 2;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == offset_in);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - CACHE_SIZE; length_in = length = CACHE_SIZE;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == 0);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - (CACHE_SIZE / 2); length_in = length = CACHE_SIZE / 2;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == (CACHE_SIZE / 2));
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = 0; length_in = length = CACHE_SIZE / 4;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == offset_in);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = 0; length_in = length = CACHE_SIZE / 8;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == offset_in);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = 0; length_in = length = CACHE_SIZE;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(offset == offset_in);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - 40; length_in = length = 16;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - 40; length_in = length = 40;
  printf("cache read: in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - 40; length_in = length = 80;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(length == 40);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - CACHE_SIZE + 80;
  length_in = length = 20;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  buffer = NULL; offset_in = CACHE_BUFFER_SIZE - 40;
  length_in = length = 40;
  printf("cache read:  in: offset=%d length=%d\n", (int) offset_in, (int) length);
  rtems_test_assert(rtems_rtl_obj_cache_read(&cache,
                                             fd,
                                             offset_in,
                                             &buffer,
                                             &length) == true);
  offset = dl_cache_buffer_offset(&cache, buffer);
  printf("cache read: out: offset=%d length=%d\n", (int) offset, (int) length);
  rtems_test_assert(length == length_in);
  rtems_test_assert(dl_cache_check(buffer, (int) offset_in, length) == true);

  rtems_rtl_obj_cache_close(&cache);

  return 0;
}
