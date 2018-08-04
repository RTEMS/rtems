/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <fcntl.h>
#include <rtems/blkdev.h>
#include "rtems/sparse-disk.h"

#include "tmacros.h"

const char rtems_test_name[] = "SPARSEDISK 1";

/* Number of bytes for test pattern within a sparse disk container */
#define STATIC_PATTERN_SIZE 4096

/* Block size used for the sparse disk in a sparse disk container */
#define STATIC_BLOCK_SIZE 4096

/* Number of block allocated for the sparse disk in a sparse disk container */
#define STATIC_ALLOCATED_BLOCK_COUNT 1

/* Blocks simulated by the sparse disk in a disk container */
#define STATIC_SIMULATED_BLOCK_COUNT 4096

/*
 * Container which cotains a sparse disk + memory for key table and data as would get
 * allocated by rtems_sparse_disk_create() + memory for a memory test pattern
 * By using this container white box testing of a sparse disk becomes possible
 */
typedef struct {
  rtems_sparse_disk sparse_disk;
  rtems_sparse_disk_key keytable[STATIC_ALLOCATED_BLOCK_COUNT];
  uint8_t data[STATIC_BLOCK_SIZE * STATIC_ALLOCATED_BLOCK_COUNT];
  uint8_t pattern[STATIC_PATTERN_SIZE];
} sparse_disk_container;

/*
 * Black box test the disk parameters of a sparse disk
 */
static void test_disk_params(
  const int               file_descriptor,
  const uint32_t          block_size,
  const uint32_t          media_block_size,
  const rtems_blkdev_bnum block_number )
{
  int                rv;
  uint32_t           value       = 0;
  rtems_disk_device *fd_dd       = NULL;
  rtems_blkdev_bnum  block_count = 0;


  rv = rtems_disk_fd_get_media_block_size( file_descriptor, &value );
  rtems_test_assert( 0 == rv );
  rtems_test_assert( media_block_size == value );

  value = 0;
  rv    = rtems_disk_fd_get_block_size( file_descriptor, &value );
  rtems_test_assert( 0 == rv );
  rtems_test_assert( block_size == value );

  block_count = 0;
  rv          = rtems_disk_fd_get_block_count( file_descriptor, &block_count );
  rtems_test_assert( 0 == rv );
  rtems_test_assert( block_number == block_count );

  rv = rtems_disk_fd_get_disk_device( file_descriptor, &fd_dd );
  rtems_test_assert( 0 == rv );
  rtems_test_assert( NULL != fd_dd );
}

/*
 * Verify that writing to a sparse disk delivers expected results
 */
static void test_writing(
  const int               file_descriptor,
  const uint32_t          block_size,
  const rtems_blkdev_bnum blocks_allocated )
{
  int               rv;
  rtems_blkdev_bnum block_count = 0;
  unsigned int      byte_count;
  off_t             file_pos;
  uint8_t           buff[block_size];


  /* Write a pattern to all allocated blocks */
  for ( block_count = 0; block_count < blocks_allocated; block_count++ ) {
    file_pos = (off_t) block_count * block_size;
    rv       = lseek( file_descriptor, file_pos, SEEK_SET );
    rtems_test_assert( file_pos == rv );

    rv = read( file_descriptor, buff, block_size );
    rtems_test_assert( block_size == rv );

    for ( byte_count = 0;
          byte_count < ( block_size / sizeof( byte_count ) );
          byte_count++ ) {
      memcpy( buff + ( byte_count * sizeof( byte_count ) ), &byte_count,
              sizeof( byte_count ) );
    }

    rv = lseek( file_descriptor, file_pos, SEEK_SET );
    rtems_test_assert( file_pos == rv );

    rv = write( file_descriptor, buff, block_size );
    rtems_test_assert( block_size == rv );
  }
}

/*
 * Verify that black box reading for a sparse disk delivers expected results
 */
static void test_reading(
  const int               file_descriptor,
  const uint32_t          block_size,
  const rtems_blkdev_bnum blocks_allocated,
  const uint8_t           fill_pattern )
{
  int               rv;
  rtems_blkdev_bnum block_count = 0;
  unsigned int      byte_count;
  off_t             file_pos;
  uint8_t           buff[block_size];
  uint32_t          value = 0;


  rv = fsync( file_descriptor );
  rtems_test_assert( 0 == rv );

  /* Read back the patterns */
  for ( block_count = 0; block_count < blocks_allocated; block_count++ ) {
    file_pos = (off_t) block_count * block_size;
    value    = lseek( file_descriptor, file_pos, SEEK_SET );
    rtems_test_assert( file_pos == value );

    rv = read( file_descriptor, &buff, block_size );
    rtems_test_assert( block_size <= rv );

    for ( byte_count = 0;
          byte_count < ( block_size / sizeof( byte_count ) );
          byte_count++ ) {
      rv = memcmp( buff + ( byte_count * sizeof( byte_count ) ),
                   &byte_count,
                   sizeof( byte_count ) );
      rtems_test_assert( 0 == rv );
    }
  }

  /* Try to read from unallocated block */
  file_pos = (off_t) block_count * block_size;
  rv       = lseek( file_descriptor, file_pos, SEEK_SET );
  rtems_test_assert( file_pos == rv );

  rv = read( file_descriptor, buff, block_size );
  rtems_test_assert( block_size == rv );

  for ( byte_count = 0; byte_count < block_size; ++byte_count )
    rtems_test_assert( fill_pattern == buff[byte_count] );
}

/*
 * Do black box io testing on a sparse disk
 */
static void test_device_io( const char *device_name,
  const uint32_t                        block_size,
  const uint32_t                        media_block_size,
  const rtems_blkdev_bnum               block_number,
  const rtems_blkdev_bnum               blocks_allocated,
  const uint8_t                         fill_pattern )
{
  int rv;
  int file_descriptor;


  file_descriptor = open( device_name, O_RDWR );
  rtems_test_assert( 0 <= file_descriptor );

  test_disk_params(
    file_descriptor,
    block_size,
    media_block_size,
    block_number
    );

  test_writing(
    file_descriptor,
    block_size,
    blocks_allocated
    );

  test_reading(
    file_descriptor,
    block_size,
    blocks_allocated,
    fill_pattern
    );

  rv = close( file_descriptor );
  rtems_test_assert( 0 == rv );
}

/*
 * In white box testing verify the key table of the sparse disk is correct
 */
static void test_static_key_table(
  const sparse_disk_container *disk_container,
  const rtems_blkdev_bnum      blocks_allocated,
  const uint32_t               block_size )
{
  unsigned int i;


  for ( i = 0; i < blocks_allocated; ++i ) {
    rtems_test_assert( i == disk_container->keytable[i].block );
    rtems_test_assert(
      &disk_container->data[i * block_size]
      == disk_container->keytable[i].data );
  }
}

/*
 * Verify the test pattern used in white box testing is as expected
 */
static void test_static_pattern(
  const unsigned int pattern_size,
  const uint8_t     *pattern )
{
  unsigned int i;


  for ( i = 0; i < pattern_size; ++i )
    rtems_test_assert( ( (uint8_t) ( pattern_size - 1 - i ) ) == pattern[i] );
}

/*
 * Read write testing with a statically allocated disk. Thus white box testing can be done
 */
static void test_with_whitebox( const char *device_name )
{
  rtems_status_code     sc;
  int                   rv;
  unsigned int          i;
  sparse_disk_container disk_container;
  int                   file_descriptor;
  rtems_blkdev_bnum     block_count  = 0;
  unsigned int          byte_count;
  uint8_t               fill_pattern = 0;


  memset( disk_container.data, 0, sizeof( disk_container.data ) );
  memset( disk_container.keytable, 0, sizeof( disk_container.keytable ) );

  for ( i = 0; i < STATIC_PATTERN_SIZE; ++i )
    disk_container.pattern[i] = (uint8_t) ( STATIC_PATTERN_SIZE - 1 - i );

  sc = rtems_sparse_disk_register(
    "/dev/sda1",
    &disk_container.sparse_disk,
    STATIC_BLOCK_SIZE,
    STATIC_ALLOCATED_BLOCK_COUNT,
    STATIC_SIMULATED_BLOCK_COUNT,
    fill_pattern,
    NULL
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  test_static_key_table(
    &disk_container,
    STATIC_ALLOCATED_BLOCK_COUNT,
    STATIC_BLOCK_SIZE
    );

  for ( i = 0; i < ( STATIC_BLOCK_SIZE * STATIC_ALLOCATED_BLOCK_COUNT ); ++i )
    rtems_test_assert( 0 == disk_container.data[i] );

  test_static_pattern(
    STATIC_PATTERN_SIZE,
    &disk_container.pattern[0]
    );

  file_descriptor = open( device_name, O_RDWR );
  rtems_test_assert( 0 <= file_descriptor );

  test_disk_params(
    file_descriptor,
    STATIC_BLOCK_SIZE,
    STATIC_BLOCK_SIZE,
    STATIC_SIMULATED_BLOCK_COUNT
    );

  test_writing(
    file_descriptor,
    STATIC_BLOCK_SIZE,
    STATIC_ALLOCATED_BLOCK_COUNT
    );

  test_reading(
    file_descriptor,
    STATIC_BLOCK_SIZE,
    STATIC_ALLOCATED_BLOCK_COUNT,
    fill_pattern
    );

  rv = close( file_descriptor );
  rtems_test_assert( 0 == rv );

  test_static_key_table(
    &disk_container,
    STATIC_ALLOCATED_BLOCK_COUNT,
    STATIC_BLOCK_SIZE
    );

  for ( block_count = 0;
        block_count < STATIC_ALLOCATED_BLOCK_COUNT;
        block_count++ ) {
    for ( byte_count = 0;
          byte_count < ( STATIC_BLOCK_SIZE / sizeof( byte_count ) );
          byte_count++ ) {
      rv = memcmp( &disk_container.data[byte_count * sizeof( byte_count )],
                   &byte_count,
                   sizeof( byte_count ) );
      rtems_test_assert( 0 == rv );
    }
  }

  test_static_pattern(
    STATIC_PATTERN_SIZE,
    &disk_container.pattern[0]
    );
}

/*
 * The test sequence
 */
static
void test( void )
{
  rtems_status_code sc;
  int               rv;
  char              device_name[] = "/dev/sda1";
  uint32_t          block_size;
  rtems_blkdev_bnum block_number;
  rtems_blkdev_bnum blocks_allocated;
  int               file_descriptor;
  uint8_t           fill_pattern = 0;

  block_size       = 512;
  block_number     = 4 * 2 * 1024;
  blocks_allocated = 8;
  sc               = rtems_sparse_disk_create_and_register(
    "/dev/sda1",
    block_size,
    blocks_allocated,
    block_number,
    fill_pattern
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Test reading and writing with sector size 512 and 8 such sectors
   * allocated. Block size will default to 512 */
  test_device_io(
    device_name,
    block_size,
    block_size,
    block_number,
    blocks_allocated,
    fill_pattern
    );

  file_descriptor = open( device_name, O_RDWR );
  rtems_test_assert( 0 <= file_descriptor );

  rv = rtems_disk_fd_set_block_size( file_descriptor,
                                     blocks_allocated * block_size );
  rtems_test_assert( 0 == rv );

  rv = close( file_descriptor );
  rtems_test_assert( 0 == rv );

  /* Block size was increased to 4k. Thus all to allocated disk space
   * corresponds to one block. Repeat the read write tests */
  test_device_io(
    device_name,
    block_size * blocks_allocated,
    block_size,
    block_number,
    1,
    fill_pattern
    );

  rv = unlink( device_name );
  rtems_test_assert( 0 == rv );

  /* Do testing with a statically allocated disk. This permits white box
   * testing */
  test_with_whitebox( device_name );
}

static void Init( rtems_task_argument arg )
{
  (void) arg;
  TEST_BEGIN();

  test();

  TEST_END();

  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 16 * 1024 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
