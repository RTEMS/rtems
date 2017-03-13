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

#include "tmacros.h"
#include <fcntl.h>
#include <rtems/dosfs.h>
#include <rtems/sparse-disk.h>
#include <rtems/blkdev.h>
#include <bsp.h>

const char rtems_test_name[] = "FSDOSFSWRITE 1";

#define MAX_PATH_LENGTH 100 /* Maximum number of characters per path */
#define SECTOR_SIZE 512 /* sector size (bytes) */
#define FAT16_MAX_CLN 65525 /* maximum + 1 number of clusters for FAT16 */
#define FAT16_DEFAULT_SECTORS_PER_CLUSTER 32 /* Default number of sectors per cluster for FAT16 */
#define SECTORS_PER_CLUSTER 2

static void format_and_mount( const char *dev_name, const char *mount_dir )
{
  static const msdos_format_request_param_t rqdata = {
    .sectors_per_cluster = SECTORS_PER_CLUSTER,
    .quick_format        = true
  };

  int                                       rv;


  rv = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );

  rv = mount( dev_name,
              mount_dir,
              RTEMS_FILESYSTEM_TYPE_DOSFS,
              RTEMS_FILESYSTEM_READ_WRITE,
              NULL );
  rtems_test_assert( rv == 0 );
}

static void do_fsync( const char *file )
{
  int rv;
  int fd;


  fd = open( file, O_RDONLY );
  rtems_test_assert( fd >= 0 );

  rv = fsync( fd );
  rtems_test_assert( rv == 0 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

static void check_block_stats( const char *dev_name,
  const char                              *mount_dir,
  const rtems_blkdev_stats                *expected_stats )
{
  int                fd;
  int                rv;
  rtems_blkdev_stats actual_stats;


  do_fsync( mount_dir );

  fd = open( dev_name, O_RDONLY );
  rtems_test_assert( fd >= 0 );

  rv = ioctl( fd, RTEMS_BLKIO_GETDEVSTATS, &actual_stats );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( memcmp( &actual_stats, expected_stats,
                             sizeof( actual_stats ) ) == 0 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

static void reset_block_stats( const char *dev_name, const char *mount_dir )
{
  int fd;
  int rv;


  do_fsync( mount_dir );

  fd = open( dev_name, O_RDONLY );
  rtems_test_assert( fd >= 0 );

  rv = ioctl( fd, RTEMS_BLKIO_PURGEDEV );
  rtems_test_assert( rv == 0 );

  rv = ioctl( fd, RTEMS_BLKIO_RESETDEVSTATS );
  rtems_test_assert( rv == 0 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

static int create_file( const char *file_name )
{
  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;


  return creat( file_name, mode );
}

static void test_normal_file_write(
  const char *dev_name,
  const char *mount_dir,
  const char *file_name )
{
  static const rtems_blkdev_stats complete_existing_block_stats = {
    .read_hits            = 0,
    .read_misses          = 0,
    .read_ahead_transfers = 0,
    .read_blocks          = 0,
    .read_errors          = 0,
    .write_transfers      = 1,
    .write_blocks         = 1,
    .write_errors         = 0
  };
  static const rtems_blkdev_stats complete_new_block_stats = {
    .read_hits            = 3,
    .read_misses          = 2,
    .read_ahead_transfers = 0,
    .read_blocks          = 2,
    .read_errors          = 0,
    .write_transfers      = 1,
    .write_blocks         = 3,
    .write_errors         = 0
  };
  static const rtems_blkdev_stats partial_new_block_stats = {
    .read_hits            = 3,
    .read_misses          = 3,
    .read_ahead_transfers = 0,
    .read_blocks          = 3,
    .read_errors          = 0,
    .write_transfers      = 1,
    .write_blocks         = 3,
    .write_errors         = 0
  };

  int                             rv;
  int                             fd;
  ssize_t                         num_bytes;
  uint8_t                         cluster_buf[SECTOR_SIZE
                                              * SECTORS_PER_CLUSTER];
  uint32_t                        cluster_size = sizeof( cluster_buf );
  off_t                           off;


  memset( cluster_buf, 0xFE, cluster_size );

  format_and_mount( dev_name, mount_dir );

  fd = create_file( file_name );
  rtems_test_assert( fd >= 0 );

  num_bytes = write( fd, cluster_buf, cluster_size );
  rtems_test_assert( (ssize_t) cluster_size == num_bytes );

  off = lseek( fd, 0, SEEK_SET );
  rtems_test_assert( off == 0 );

  reset_block_stats( dev_name, mount_dir );

  /* Write a complete cluster into an existing file space */
  num_bytes = write( fd, cluster_buf, cluster_size );
  rtems_test_assert( (ssize_t) cluster_size == num_bytes );

  check_block_stats( dev_name, mount_dir, &complete_existing_block_stats );
  reset_block_stats( dev_name, mount_dir );

  /* Write a complete cluster into a new file space */
  num_bytes = write( fd, cluster_buf, cluster_size );
  rtems_test_assert( (ssize_t) cluster_size == num_bytes );

  check_block_stats( dev_name, mount_dir, &complete_new_block_stats );
  reset_block_stats( dev_name, mount_dir );

  /* Write a new partial cluster into a new file space */
  num_bytes = write( fd, cluster_buf, 1 );
  rtems_test_assert( num_bytes == 1 );

  check_block_stats( dev_name, mount_dir, &partial_new_block_stats );

  rv = close( fd );
  rtems_test_assert( 0 == rv );

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );
}

static void test_fat12_root_directory_write( const char *dev_name,
  const char                                            *mount_dir,
  const char                                            *file_name )
{
  static const rtems_blkdev_stats fat12_root_dir_stats = {
    .read_hits            = 11,
    .read_misses          = 2,
    .read_ahead_transfers = 0,
    .read_blocks          = 2,
    .read_errors          = 0,
    .write_transfers      = 1,
    .write_blocks         = 1,
    .write_errors         = 0
  };

  int                             fd;
  int                             rv;


  format_and_mount( dev_name, mount_dir );

  reset_block_stats( dev_name, mount_dir );

  fd = create_file( file_name );
  rtems_test_assert( fd >= 0 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );

  check_block_stats( dev_name, mount_dir, &fat12_root_dir_stats );

  rv = unmount( mount_dir );
  rtems_test_assert( rv == 0 );
}

static void test( void )
{
  static const char dev_name[]  = "/dev/sda";
  static const char mount_dir[] = "/mnt";
  static const char file_name[] = "/mnt/file.txt";

  rtems_status_code sc;
  int               rv;


  sc = rtems_disk_io_initialize();
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rv = mkdir( mount_dir, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( 0 == rv );

  /* A 1.44 MB disk */
  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    64,
    2880,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  test_fat12_root_directory_write( dev_name, mount_dir, file_name );

  test_normal_file_write( dev_name, mount_dir, file_name );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );
}

static void Init( rtems_task_argument arg )
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_FILESYSTEM_DOSFS

/* 1 device file for blkstats + 1 file for writing + 1 mount_dir + stdin + stdout + stderr + device file when mounted */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 8

#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 32 * 1024 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE ( 32 * 1024 )

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
