/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2015.
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

#include <tmacros.h>
#include "test_support.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <rtems/libio.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>

#define MEMFILE_BYTES_PER_BLOCK 16

const char rtems_test_name[] = "PSXIMFS 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  static const char mount_point [] = "dir01";
  static const char fs_type [] = RTEMS_FILESYSTEM_TYPE_IMFS;
  static const char slink_2_name [] = "node-slink-2";
  static const uintptr_t mount_table_entry_size [] = {
    sizeof( rtems_filesystem_mount_table_entry_t )
      + sizeof( fs_type )
      + sizeof( rtems_filesystem_global_location_t )
  };
  static const uintptr_t slink_2_name_size [] = {
    sizeof( slink_2_name )
  };
  static const uintptr_t some_blocks [] = {
    MEMFILE_BYTES_PER_BLOCK * 10
  };
  static const char some_data[MEMFILE_BYTES_PER_BLOCK * 11];

  int status = 0;
  void *opaque;
  char linkname_n[32] = {0};
  char linkname_p[32] = {0};
  int i;
  int fd;
  struct stat stat_buf;

  TEST_BEGIN();

  puts( "Creating directory /dir00" );
  status = mkdir( "/dir00", S_IRWXU );
  rtems_test_assert( status == 0 );

  puts( "Creating directory /dir00/dir01" );
  status = mkdir( "/dir00/dir01", S_IRWXU );
  rtems_test_assert( status == 0 );

  puts( "Changing directory to /dir00" );
  status = chdir( "/dir00" );
  rtems_test_assert( status == 0 );

  puts( "Creating link dir01-link0 for dir01" );
  status = link( "dir01", "dir01-link0" );
  rtems_test_assert( status == 0 );

  for( i = 1 ; ; ++i ) {
    sprintf( linkname_p, "dir01-link%04d", i-1 );
    sprintf( linkname_n, "dir01-link%04d", i );
    printf( "\nCreating link %s for %s\n", linkname_n, linkname_p );
    status = link( linkname_p, linkname_n );
    if( status != 0 ) {
      puts("Link creation failed" );
      break;
    }
  }

  puts( "Creating a regular node /node, RDONLY" );
  status = mknod( "/node", S_IFREG | S_IRUSR, 0LL );
  rtems_test_assert( status == 0 );

  puts( "Creating link /node-link for /node" );
  status = link( "/node" , "/node-link" );
  rtems_test_assert( status == 0 );

  puts( "Opening /node-link in WRONLY mode -- expect EACCES" );
  status = open( "/node-link", O_WRONLY );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EACCES );

  puts( "Creating a symlink /node-slink for /node" );
  status = symlink( "/node" , "/node-slink" );
  rtems_test_assert( status == 0 );

  puts( "Opening /node-slink in WRONLY mode -- expect EACCES" );  
  status = open( "/node-slink", O_WRONLY );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EACCES );

  puts( "Allocate most of heap with a little bit left" );
  opaque = rtems_heap_greedy_allocate( some_blocks, 1 );

  puts( "Create an empty file.");
  status = mknod( "/foo", S_IFREG | S_IRWXU, 0LL );
  rtems_test_assert( status == 0 );

  puts( "Then increase it's size to more than remaining space" );
  fd = open( "/foo", O_WRONLY | O_TRUNC);
  rtems_test_assert( fd >= 0 );
  status = write(fd, some_data, sizeof(some_data));
  rtems_test_assert( status == -1);
  rtems_test_assert( errno == ENOSPC );

  puts( "Clean up again" );
  status = close(fd);
  rtems_test_assert( status == 0);
  status = remove( "/foo" );
  rtems_test_assert( status == 0);
  rtems_heap_greedy_free( opaque );

  puts( "Allocate most of heap" );
  opaque = rtems_heap_greedy_allocate( mount_table_entry_size, 1 );

  printf( "Attempt to mount a fs at %s -- expect ENOMEM", mount_point );
  status = mount( NULL,
		  mount_point,
		  fs_type,
		  RTEMS_FILESYSTEM_READ_WRITE,
		  NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Freeing allocated memory" );
  rtems_heap_greedy_free( opaque );

  puts( "Changing directory to /" );
  status = chdir( "/" );
  rtems_test_assert( status == 0 );

  puts( "Allocate most of heap" );
  opaque = rtems_heap_greedy_allocate( NULL, 0 );

  puts( "Attempt to create /node-link-2 for /node -- expect ENOMEM" );
  status = link( "/node", "/node-link-2" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Attempt to create /node-slink-2 for /node -- expect ENOMEM" );
  status = symlink( "/node", "node-slink-2" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Freeing allocated memory" );
  rtems_heap_greedy_free( opaque );

  puts( "Allocate most of heap" );
  opaque = rtems_heap_greedy_allocate( slink_2_name_size, 1 );

  printf( "Attempt to create %s for /node -- expect ENOMEM", slink_2_name );
  status = symlink( "/node", slink_2_name );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Freeing allocated memory" );
  rtems_heap_greedy_free( opaque );

  puts( "Attempt to stat a hardlink" );
  status = lstat( "/node-link", &stat_buf );
  rtems_test_assert( status == 0 );

  puts( "Changing euid to 10" );
  status = seteuid( 10 );
  rtems_test_assert( status == 0 );

  puts( "Attempt chmod on /node -- expect EPERM" );
  status = chmod( "/node", S_IRUSR );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EPERM );

  puts( "Attempt chown on /node -- expect EPERM" );
  status = chown( "/node", 10, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EPERM );

  puts( "Changing euid back to 0 [root]" );
  status = seteuid( 0 );
  rtems_test_assert( status == 0 );

  puts( "Creating a fifo -- OK" );
  status = mkfifo( "/fifo", S_IRWXU );
  rtems_test_assert( status == 0 );
  
  puts( "chown /fifo to 10 -- OK" );
  status = chown( "/fifo", 10, 10 );
  rtems_test_assert( status == 0 );

  puts( "Changing euid to 10" );
  status = seteuid( 10 );
  rtems_test_assert( status == 0 );

  puts( "chmod /fifo -- OK" );
  status = chmod( "/fifo", S_IRWXU );
  rtems_test_assert( status == 0 );

  printf( "chown /fifo to %o -- OK\n", 0 );
  status = chown( "/fifo", 0, 0 );
  rtems_test_assert( status == 0 );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_MAXIMUM_TASKS                  1
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK   MEMFILE_BYTES_PER_BLOCK
#define CONFIGURE_IMFS_ENABLE_MKFIFO
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
