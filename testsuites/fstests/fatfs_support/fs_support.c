/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
 *  On-Line Applications Research Corporation (OAR).
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
#include <sys/stat.h>
#include <sys/types.h>
#include <tmacros.h>

#include <rtems/fatfs.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>

#include "fs_config.h"
#include "fstest.h"
#include "fstest_support.h"
#include "ramdisk_support.h"

/* Include FatFS headers - these are internal to the FatFS implementation */
extern int fatfs_diskio_register_device(
  unsigned char pdrv,
  const char   *device_path
);
extern void fatfs_diskio_unregister_device( unsigned char pdrv );

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
} mkfs_parm;

typedef unsigned char FRESULT;

/* FatFS function declaration */
extern FRESULT f_mkfs(
  const char      *path,
  const mkfs_parm *opt,
  void            *work,
  unsigned int     len
);

#define BLOCK_SIZE 512

static const mkfs_parm fatfs_format_options = {
  .fmt               = FM_FAT, /* Format as FAT12/16 (auto-detect) */
  .num_fat           = 2,      /* Number of FAT copies */
  .align             = 0,      /* Auto data area alignment */
  .n_root            = 512, /* Number of root directory entries for FAT12/16 */
  .auto_cluster_size = 0    /* Auto cluster size */
};

static rtems_resource_snapshot before_mount;
static unsigned char fatfs_work_buffer[ 4096 ]; /* Work buffer for f_mkfs() */

static int fatfs_format_disk( const char *device_path )
{
  FRESULT fr;
  int     rc;

  rc = fatfs_diskio_register_device( 0, device_path );
  if ( rc != 0 ) {
    printf( "Device registration failed: %d\n", rc );
    return -1;
  }

  fr = f_mkfs(
    "0:",
    &fatfs_format_options,
    fatfs_work_buffer,
    sizeof( fatfs_work_buffer )
  );
  if ( fr != FR_OK ) {
    printf( "FatFS formatting failed: %d\n", fr );
    fatfs_diskio_unregister_device( 0 );
    return -1;
  }

  fatfs_diskio_unregister_device( 0 );

  return 0;
}

void test_initialize_filesystem( void )
{
  int rc = 0;

  rc = mkdir( BASE_FOR_TEST, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert( rc == 0 );

  init_ramdisk();

  rc = fatfs_format_disk( RAMDISK_PATH );
  rtems_test_assert( rc == 0 );

  rtems_resource_snapshot_take( &before_mount );

  rc = mount(
    RAMDISK_PATH,
    BASE_FOR_TEST,
    "fatfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  if ( rc != 0 ) {
    printf( "Mount failed with errno: %s\n", strerror( errno ) );
  }
  rtems_test_assert( rc == 0 );
}

void test_shutdown_filesystem( void )
{
  int rc = 0;
  rc     = unmount( BASE_FOR_TEST );

  if ( rc != 0 ) {
    printf( "Unmount failed with errno: %d (%s)\n", errno, strerror( errno ) );
  } else {
    printf( "Unmount successful!\n" );
  }
  rtems_test_assert( rc == 0 );
  rtems_test_assert( rtems_resource_snapshot_check( &before_mount ) );
  del_ramdisk();
}

/* configuration information */

/* drivers */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

/**
 * Configure base RTEMS resources.
 */

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES 10
#define CONFIGURE_MAXIMUM_TASKS 10
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 40
#define CONFIGURE_INIT_TASK_STACK_SIZE ( 16 * 1024 )
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_FILESYSTEM_FATFS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
