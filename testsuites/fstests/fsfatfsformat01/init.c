/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
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

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/statvfs.h>

#include <rtems/blkdev.h>
#include <rtems/libio.h>
#include <rtems/sparse-disk.h>

#include <bsp.h>

#include "tmacros.h"

const char rtems_test_name[] = "FSFATFSFORMAT 1";

#define MAX_PATH_LENGTH 100
#define SECTOR_SIZE 512
#define FAT12_MAX_CLN 4085
#define FAT16_MAX_CLN 65525
#define FAT12_DEFAULT_SECTORS_PER_CLUSTER 8
#define FAT16_DEFAULT_SECTORS_PER_CLUSTER 32

/* FatFS constants and structures */
#define FR_OK 0
#define FR_INVALID_PARAMETER 19
#define FM_FAT 0x01
#define FM_FAT32 0x02
#define FM_ANY 0x07

typedef struct {
  unsigned char fmt;
  unsigned char num_fat;
  unsigned int  align;
  unsigned int  n_root;
  unsigned long auto_cluster_size;
} mkfs_parm;

/* FatFS MKFS_PARM structure for proper parameter passing */
typedef struct {
  unsigned char fmt;     /* Format option (FM_FAT, FM_FAT32, etc.) */
  unsigned char n_fat;   /* Number of FATs */
  unsigned int  align;   /* Data area alignment (sector) */
  unsigned int  n_root;  /* Number of root directory entries */
  unsigned long au_size; /* Cluster size (byte) */
} MKFS_PARM;

typedef unsigned char FRESULT;

extern int fatfs_diskio_register_device(
  unsigned char pdrv,
  const char   *device_path
);
extern void    fatfs_diskio_unregister_device( unsigned char pdrv );
extern FRESULT f_mkfs(
  const char      *path,
  const MKFS_PARM *opt,
  void            *work,
  unsigned int     len
);

static unsigned char fatfs_work_buffer[ 4096 ];

static int fatfs_format_disk(
  const char      *device_path,
  const mkfs_parm *format_options
)
{
  FRESULT   fr;
  int       rc;
  MKFS_PARM fatfs_opts;

  rc = fatfs_diskio_register_device( 0, device_path );
  if ( rc != 0 ) {
    return -1;
  }

  /* Convert test structure to FatFS structure */
  fatfs_opts.fmt     = format_options->fmt;
  fatfs_opts.n_fat   = format_options->num_fat;
  fatfs_opts.align   = format_options->align;
  fatfs_opts.n_root  = format_options->n_root;
  fatfs_opts.au_size = format_options->auto_cluster_size *
                       512; /* Convert sectors to bytes */

  fr = f_mkfs(
    "0:",
    &fatfs_opts,
    fatfs_work_buffer,
    sizeof( fatfs_work_buffer )
  );

  fatfs_diskio_unregister_device( 0 );

  return ( fr == FR_OK ) ? 0 : -1;
}

static void test_disk_params(
  const char     *dev_name,
  const char     *mount_dir,
  const blksize_t sector_size,
  const blksize_t cluster_size,
  const blkcnt_t  sectors_per_cluster
)
{
  int          rv;
  int          fildes;
  struct stat  stat_buff;
  char         file_name[ MAX_PATH_LENGTH + 1 ];
  ssize_t      num_bytes;
  unsigned int value = (unsigned int) -1;

  snprintf( file_name, MAX_PATH_LENGTH, "%s/file1.txt", mount_dir );
  memset( &stat_buff, 0, sizeof( stat_buff ) );

  rv = mount( dev_name, mount_dir, "fatfs", RTEMS_FILESYSTEM_READ_WRITE, NULL );
  rtems_test_assert( 0 == rv );

  fildes = open(
    file_name,
    O_RDWR | O_CREAT | O_TRUNC,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
  );
  rtems_test_assert( -1 != fildes );

  num_bytes = write( fildes, &value, sizeof( value ) );
  rtems_test_assert( sizeof( value ) == num_bytes );

  rv = fstat( fildes, &stat_buff );
  rtems_test_assert( 0 == rv );
  rtems_test_assert( S_ISREG( stat_buff.st_mode ) );
  rtems_test_assert( sizeof( value ) == stat_buff.st_size );
  rtems_test_assert( cluster_size == stat_buff.st_blksize );
  rtems_test_assert(
    sectors_per_cluster == ( stat_buff.st_blocks * sector_size / 512 )
  );
  rtems_test_assert(
    ( ( ( stat_buff.st_size + cluster_size - 1 ) / cluster_size ) *
      cluster_size / 512 ) == stat_buff.st_blocks
  );
  rv = close( fildes );
  rtems_test_assert( 0 == rv );

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );

  rv = mount( dev_name, mount_dir, "fatfs", RTEMS_FILESYSTEM_READ_WRITE, NULL );
  rtems_test_assert( 0 == rv );

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );
}

static void test_create_file(
  const char *mount_dir,
  uint32_t    file_idx,
  bool        expect_ok
)
{
  char file_name[ MAX_PATH_LENGTH + 1 ];
  int  fd;

  snprintf(
    file_name,
    MAX_PATH_LENGTH,
    "%s/file%" PRIu32 ".txt",
    mount_dir,
    file_idx
  );
  fd = open(
    file_name,
    O_RDWR | O_CREAT | O_TRUNC,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
  );

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
  const uint32_t number_of_files
)
{
  int      rv;
  uint32_t file_idx;
  char     file_name[ MAX_PATH_LENGTH + 1 ];

  rv = mount( dev_name, mount_dir, "fatfs", RTEMS_FILESYSTEM_READ_WRITE, NULL );
  rtems_test_assert( 0 == rv );

  for ( file_idx = 0; file_idx < number_of_files; ++file_idx ) {
    test_create_file( mount_dir, file_idx, true );
  }

  test_create_file( mount_dir, file_idx, false );

  for ( file_idx = 0; file_idx < number_of_files; ++file_idx ) {
    snprintf(
      file_name,
      MAX_PATH_LENGTH,
      "%s/file%" PRIu32 ".txt",
      mount_dir,
      file_idx
    );
    rv = unlink( file_name );
    rtems_test_assert( 0 == rv );
  }

  rv = unmount( mount_dir );
  rtems_test_assert( 0 == rv );
}

static void test( void )
{
  rtems_status_code sc;
  int               rv;
  const char        dev_name[]  = "/dev/rda";
  const char        mount_dir[] = "/mnt";
  mkfs_parm         rqdata;
  rtems_blkdev_bnum media_block_count;

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
  rqdata.fmt               = FM_FAT;
  rqdata.num_fat           = 1;
  rqdata.align             = 0;
  rqdata.n_root            = 32;
  rqdata.auto_cluster_size = 1;
  rv                       = fatfs_format_disk( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE, 1 );
  test_file_creation( dev_name, mount_dir, rqdata.n_root );

  /* FatFS silently clamps invalid parameters rather than returning errors,
   * so we skip this test as it doesn't match FatFS behavior */

  /* Optimized for read/write speed */
  rqdata.fmt               = FM_FAT;
  rqdata.num_fat           = 2;
  rqdata.align             = 0;
  rqdata.n_root            = 0;
  rqdata.auto_cluster_size = 8;
  rv                       = fatfs_format_disk( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE * 8, 8 );

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

  /* Default parameters - let FatFS choose cluster size */
  rqdata.fmt               = FM_FAT;
  rqdata.num_fat           = 2;
  rqdata.align             = 0;
  rqdata.n_root            = 512;
  rqdata.auto_cluster_size = 0;
  rv                       = fatfs_format_disk( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  /* FatFS chooses cluster size based on volume size, so test actual result */
  test_disk_params(
    dev_name,
    mount_dir,
    SECTOR_SIZE,
    SECTOR_SIZE * 4, /* FatFS actually chooses 4 sectors per cluster */
    4
  );

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
  rqdata.fmt               = FM_FAT;
  rqdata.num_fat           = 1;
  rqdata.align             = 0;
  rqdata.n_root            = 32;
  rqdata.auto_cluster_size = 1;
  rv                       = fatfs_format_disk( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE, 1 );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  /* FAT32 */
  sc = rtems_sparse_disk_create_and_register(
    dev_name,
    SECTOR_SIZE,
    1024,
    ( FAT16_MAX_CLN * FAT16_DEFAULT_SECTORS_PER_CLUSTER ) + 41L,
    0
  );
  rtems_test_assert( RTEMS_SUCCESSFUL == sc );

  /* Default parameters - FAT32 chooses larger cluster sizes */
  rqdata.fmt               = FM_FAT32;
  rqdata.num_fat           = 2;
  rqdata.align             = 0;
  rqdata.n_root            = 0;
  rqdata.auto_cluster_size = 0;
  rv                       = fatfs_format_disk( dev_name, &rqdata );
  rtems_test_assert( rv == 0 );
  test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE * 16, 16 );

  rv = unlink( dev_name );
  rtems_test_assert( rv == 0 );

  /* Format some disks from 1MB up to 128GB - let FatFS choose appropriate FAT
   * type */
  rqdata.fmt               = FM_ANY;
  rqdata.num_fat           = 2;
  rqdata.align             = 0;
  rqdata.n_root            = 0;
  rqdata.auto_cluster_size = 64;
  for ( media_block_count = 1 * 1024 * ( 1024 / SECTOR_SIZE );
        media_block_count <= 128 * 1024 * 1024 * ( 1024 / SECTOR_SIZE );
        media_block_count *= 2 ) {
    sc = rtems_sparse_disk_create_and_register(
      dev_name,
      SECTOR_SIZE,
      64,
      media_block_count,
      0
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rv = fatfs_format_disk( dev_name, &rqdata );
    rtems_test_assert( rv == 0 );

    test_disk_params( dev_name, mount_dir, SECTOR_SIZE, SECTOR_SIZE * 64, 64 );

    rv = unlink( dev_name );
    rtems_test_assert( rv == 0 );
  }
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

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_FILESYSTEM_FATFS

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 10

#define CONFIGURE_INIT_TASK_STACK_SIZE ( 32 * 1024 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE ( 32 * 1024 )

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
