/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file_name may be
 * found in the file_name LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <fcntl.h>
#include <inttypes.h>
#include <sys/statvfs.h>
#include <rtems/libio.h>
#include <rtems/blkdev.h>
#include <rtems/dosfs.h>
#include <rtems/sparse-disk.h>

#include <bsp.h>

const char rtems_test_name[] = "FSDOSFSFORMAT 1";

#define MAX_PATH_LENGTH 100 /* Maximum number of characters per path */
#define SECTOR_SIZE 512 /* sector size (bytes) */
#define FAT12_MAX_CLN 4085 /* maximum + 1 number of clusters for FAT12 */
#define FAT16_MAX_CLN 65525 /* maximum + 1 number of clusters for FAT16 */
#define FAT12_DEFAULT_SECTORS_PER_CLUSTER 8 /* Default number of sectors per cluster for FAT12 */
#define FAT16_DEFAULT_SECTORS_PER_CLUSTER 32 /* Default number of sectors per cluster for FAT16 */

static void test_disk_params(
  const char     *dev_name,
  const char     *mount_dir,
  const blksize_t sector_size,
  const blksize_t cluster_size,
  const blkcnt_t  sectors_per_cluster )
{
  int          rv;
  int          fildes;
  struct stat  stat_buff;
  char         file_name[MAX_PATH_LENGTH + 1];
  ssize_t      num_bytes;
  unsigned int value = (unsigned int) -1;


  snprintf( file_name, MAX_PATH_LENGTH, "%s/file1.txt", mount_dir );
  memset( &stat_buff, 0, sizeof( stat_buff ) );

  rv = mount( dev_name,
              mount_dir,
              RTEMS_FILESYSTEM_TYPE_DOSFS,
              RTEMS_FILESYSTEM_READ_WRITE,
              NULL );
  rtems_test_assert( 0 == rv );

  fildes = open( file_name,
                 O_RDWR | O_CREAT | O_TRUNC,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
  rtems_test_assert( -1 != fildes );

  num_bytes = write( fildes, &value, sizeof( value ) );
  rtems_test_assert( sizeof( value ) == num_bytes );

  rv = fstat( fildes, &stat_buff );
  rtems_test_assert( 0 == rv );
  rtems_test_assert( S_ISREG( stat_buff.st_mode ) );
  rtems_test_assert( sizeof( value ) == stat_buff.st_size );
  rtems_test_assert( cluster_size == stat_buff.st_blksize );
  rtems_test_assert( sectors_per_cluster
                     == ( stat_buff.st_blocks * sector_size / 512 ) );
  rtems_test_assert( ( ( ( stat_buff.st_size + cluster_size
                           - 1 ) / cluster_size ) * cluster_size / 512 )
                     == stat_buff.st_blocks );
  rv = close( fildes );
  rtems_test_assert( 0 == rv );

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );

  /* See if we can re-mount the file system */
  rv = mount( dev_name,
              mount_dir,
              RTEMS_FILESYSTEM_TYPE_DOSFS,
              RTEMS_FILESYSTEM_READ_WRITE,
              NULL );
  rtems_test_assert( 0 == rv );

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );
}

static void test_create_file(
  const char *mount_dir,
  uint32_t    file_idx,
  bool        expect_ok )
{
  char file_name[MAX_PATH_LENGTH + 1];
  int  fd;


  snprintf( file_name,
            MAX_PATH_LENGTH,
            "%s/file%" PRIu32 ".txt",
            mount_dir,
            file_idx );
  fd = open( file_name,
             O_RDWR | O_CREAT | O_TRUNC,
             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );

  if ( expect_ok ) {
    int rv;

    rtems_test_assert( fd >= 0 );

    rv = close( fd );
    rtems_test_assert( rv == 0 );
  } else {
    rtems_test_assert( fd == -1 );
  }
}

static void test_file_creation(
  const char    *dev_name,
  const char    *mount_dir,
  const uint32_t number_of_files )
{
  int      rv;
  uint32_t file_idx;
  char     file_name[MAX_PATH_LENGTH + 1];


  rv = mount( dev_name,
              mount_dir,
              RTEMS_FILESYSTEM_TYPE_DOSFS,
              RTEMS_FILESYSTEM_READ_WRITE,
              NULL );
  rtems_test_assert( 0 == rv );

  for ( file_idx = 0; file_idx < number_of_files; ++file_idx ) {
    test_create_file( mount_dir, file_idx, true );
  }

  test_create_file( mount_dir, file_idx, false );

  for ( file_idx = 0; file_idx < number_of_files; ++file_idx ) {
    snprintf( file_name,
              MAX_PATH_LENGTH,
              "%s/file%" PRIu32 ".txt",
              mount_dir,
              file_idx );
    rv = unlink( file_name );
    rtems_test_assert( 0 == rv );
  }

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );
}

static void test( void )
{
  rtems_status_code            sc;
  int                          rv;
  const char                   dev_name[]  = "/dev/rda";
  const char                   mount_dir[] = "/mnt";
  msdos_format_request_param_t rqdata;
  rtems_blkdev_bnum            media_block_count;

  memset( &rqdata, 0, sizeof( rqdata ) );

  rv = mkdir( mount_dir, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( 0 == rv );

  /* FAT12 */
  /* For 1.44 MB disks */
  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    64,
    2880,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Optimized for disk space */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 1;
  rqdata.fat_num             = 1;
  rqdata.files_per_root_dir  = 32;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = true;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE, 1 );
  test_file_creation( dev_name, mount_dir, rqdata.files_per_root_dir );

  /* Try formatting with invalid values */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 1;
  rqdata.fat_num             = 7; /* Invalid number of fats */
  rqdata.files_per_root_dir  = 32;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = true;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv != 0 );

  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 1;
  rqdata.fat_num             = 1;
  rqdata.files_per_root_dir  = 32;
  rqdata.media               = 0x11; /* Invalid media code */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = true;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv != 0 );

  /* Optimized for read/write speed */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 8;
  rqdata.fat_num             = 0;
  rqdata.files_per_root_dir  = 0;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = false;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * rqdata.sectors_per_cluster,
                    rqdata.sectors_per_cluster );

  /* The same disk formatted with FAT16 because sectors per cluster is too high
   * for FAT12 */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 16;
  rqdata.fat_num             = 1;
  rqdata.files_per_root_dir  = 32;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = false;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * rqdata.sectors_per_cluster,
                    rqdata.sectors_per_cluster );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  /* Largest FAT12 disk */
  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    64,
    ( FAT12_MAX_CLN * FAT12_DEFAULT_SECTORS_PER_CLUSTER ) - 1L,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Default parameters (corresponds to optimization for read/write speed) */
  rv = msdos_format( dev_name, NULL );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * FAT12_DEFAULT_SECTORS_PER_CLUSTER,
                    FAT12_DEFAULT_SECTORS_PER_CLUSTER );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  /* FAT16 */
  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    ( FAT12_MAX_CLN * FAT12_DEFAULT_SECTORS_PER_CLUSTER ) + 1L,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Optimized for disk space */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 1;
  rqdata.fat_num             = 1;
  rqdata.files_per_root_dir  = 32;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = true;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    rqdata.sectors_per_cluster * SECTOR_SIZE,
                    rqdata.sectors_per_cluster );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    ( FAT16_MAX_CLN * FAT16_DEFAULT_SECTORS_PER_CLUSTER ) - 1L,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Optimized for read/write speed */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 64;
  rqdata.fat_num             = 0;
  rqdata.files_per_root_dir  = 0;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = false;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * rqdata.sectors_per_cluster,
                    rqdata.sectors_per_cluster );

  /* Default parameters (corresponds to optimization for read/write speed) */
  rv = msdos_format( dev_name, NULL );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * FAT16_DEFAULT_SECTORS_PER_CLUSTER,
                    FAT16_DEFAULT_SECTORS_PER_CLUSTER );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    ( FAT16_MAX_CLN + 10 ) * 64,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 64;
  rqdata.fat_num             = 0;
  rqdata.files_per_root_dir  = 0;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = false;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * rqdata.sectors_per_cluster,
                    rqdata.sectors_per_cluster );
  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  /* Format some disks from 1MB up to 128GB */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 64;
  rqdata.fat_num             = 0;
  rqdata.files_per_root_dir  = 0;
  rqdata.media               = 0;
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = false;
  for (
    media_block_count = 1 * 1024 * ( 1024 / SECTOR_SIZE );
    media_block_count <= 128 * 1024 * 1024 * ( 1024 / SECTOR_SIZE );
    media_block_count *= 2
  ) {
    sc = rtems_sparse_disk_create_and_register(
      dev_name,
      SECTOR_SIZE,
      64,
      media_block_count,
      0
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rv = msdos_format( dev_name, &rqdata );
    rtems_test_assert( rv == 0 );

    test_disk_params(
      dev_name,
      mount_dir,
      SECTOR_SIZE,
      SECTOR_SIZE * rqdata.sectors_per_cluster,
      rqdata.sectors_per_cluster
    );

    rv = unlink( dev_name );
    rtems_test_assert( rv == 0 );
  }

  /* FAT32 */

  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    ( FAT16_MAX_CLN * FAT16_DEFAULT_SECTORS_PER_CLUSTER ) + 41L,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Default parameters */
  rv = msdos_format( dev_name, NULL );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE, 1 );
  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    ( FAT16_MAX_CLN + 20 ) * 64L,
    0
    );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Optimized for read/write speed */
  rqdata.OEMName             = NULL;
  rqdata.VolLabel            = NULL;
  rqdata.sectors_per_cluster = 64;
  rqdata.fat_num             = 0;
  rqdata.files_per_root_dir  = 0;
  rqdata.media               = 0; /* Media code. 0 == Default */
  rqdata.quick_format        = true;
  rqdata.skip_alignment      = false;
  rv                         = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name,
                    mount_dir,
                    SECTOR_SIZE,
                    SECTOR_SIZE * rqdata.sectors_per_cluster,
                    rqdata.sectors_per_cluster );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  /* FAT32 with cluster size of 64KiB */

  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    16777216, /* 8GiB */
    0
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  memset( &rqdata, 0, sizeof( rqdata ) );
  rqdata.sectors_per_cluster = 128;
  rqdata.quick_format = true;
  rv = msdos_format( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );

  test_disk_params(
    dev_name,
    mount_dir,
    SECTOR_SIZE,
    SECTOR_SIZE * rqdata.sectors_per_cluster,
    rqdata.sectors_per_cluster
  );

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
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

/* one active file + stdin + stdout + stderr + device file when mounted */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 32 * 1024 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE ( 32 * 1024 )

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
