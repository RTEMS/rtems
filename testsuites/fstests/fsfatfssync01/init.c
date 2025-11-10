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

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <rtems/blkdev.h>
#include <rtems/fatfs.h>
#include <rtems/libio.h>
#include <rtems/ramdisk.h>

#include "tmacros.h"

const char rtems_test_name[] = "FSFATFSSYNC 1";

/* FatFS constants and structures */
#define FR_OK 0
#define FM_FAT 0x01
#define FM_FAT32 0x02

typedef struct {
  unsigned char fmt;
  unsigned char num_fat;
  unsigned int  align;
  unsigned int  n_root;
  unsigned long auto_cluster_size;
} MKFS_PARM;

typedef unsigned char FRESULT;

/* External FatFS functions */
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

static void create_file( const char *file )
{
  int fd;
  int rv;

  fd = creat( file, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( fd >= 0 );

  rv = fsync( fd );
  rtems_test_assert( rv == 0 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

static void write_to_file( const char *file, bool sync )
{
  int     fd;
  char    buf[ 1 ] = { 'A' };
  ssize_t n;
  off_t   pos_before, pos_after;
  int     rv;

  fd = open( file, O_RDWR );
  rtems_test_assert( fd >= 0 );

  pos_before = lseek( fd, 0, SEEK_END );

  n = write( fd, buf, sizeof( buf ) );
  rtems_test_assert( n == (ssize_t) sizeof( buf ) );

  pos_after = lseek( fd, 0, SEEK_END );
  rtems_test_assert( pos_after == pos_before + sizeof( buf ) );

  if ( sync ) {
    rv = fsync( fd );
    rtems_test_assert( rv == 0 );
  }

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

static void check_file_size( const char *file, off_t size )
{
  int   fd;
  off_t pos;
  int   rv;

  fd = open( file, O_RDWR );
  rtems_test_assert( fd >= 0 );

  pos = lseek( fd, 0, SEEK_END );
  rtems_test_assert( pos == size );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

static void test( const char *rda, const char *mnt, const char *file )
{
  static const MKFS_PARM fatfs_format_options = {
    .fmt               = FM_FAT,
    .num_fat           = 2,
    .align             = 0,
    .n_root            = 512,
    .auto_cluster_size = 0
  };

  int     disk_fd;
  int     rv;
  FRESULT fr;

  disk_fd = open( rda, O_RDWR );
  rtems_test_assert( disk_fd >= 0 );

  rv = fatfs_diskio_register_device( 0, rda );
  rtems_test_assert( rv == 0 );

  /* Format using FatFS */
  fr = f_mkfs(
    "0:",
    &fatfs_format_options,
    fatfs_work_buffer,
    sizeof( fatfs_work_buffer )
  );
  rtems_test_assert( fr == FR_OK );

  fatfs_diskio_unregister_device( 0 );

  rv = mount_and_make_target_path(
    rda,
    mnt,
    "fatfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  rtems_test_assert( rv == 0 );

  create_file( file );
  rv = rtems_disk_fd_purge( disk_fd );
  rtems_test_assert( rv == 0 );

  write_to_file( file, false );
  rv = rtems_disk_fd_purge( disk_fd );
  rtems_test_assert( rv == 0 );
  check_file_size( file, 1 );

  write_to_file( file, true );
  rv = rtems_disk_fd_purge( disk_fd );
  rtems_test_assert( rv == 0 );
  check_file_size( file, 2 );

  rv = unmount( mnt );
  rtems_test_assert( rv == 0 );

  rv = close( disk_fd );
  rtems_test_assert( rv == 0 );
}

static void Init( rtems_task_argument arg )
{
  (void) arg;

  TEST_BEGIN();

  test( "/dev/rda", "/mnt", "/mnt/file" );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_ramdisk_config rtems_ramdisk_configuration[] = {
  { .block_size = 512, .block_num = 1024 }
};

size_t rtems_ramdisk_configuration_size = 1;

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS RAMDISK_DRIVER_TABLE_ENTRY
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_FILESYSTEM_FATFS

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_EXTRA_TASK_STACKS ( 8 * 1024 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
