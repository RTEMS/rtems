/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>

void *POSIX_Init(
  void *argument
);

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 10
#define CONFIGURE_MAXIMUM_POSIX_SHMS 1
#define CONFIGURE_MEMORY_OVERHEAD 10

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#include <rtems/posix/shm.h>
const POSIX_Shm_Object_operations _POSIX_Shm_Object_operations = {
  _POSIX_Shm_Object_create_from_heap,
  _POSIX_Shm_Object_resize_from_heap,
  _POSIX_Shm_Object_delete_from_heap,
  _POSIX_Shm_Object_read_from_heap,
  _POSIX_Shm_Object_mmap_from_heap
};
#define CONFIGURE_HAS_OWN_POSIX_SHM_OBJECT_OPERATIONS

#include <rtems/confdefs.h>
