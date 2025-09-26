/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include <sys/statvfs.h>
#include <fcntl.h>
#include <errno.h>
#include <rtems/libcsupport.h>

#define BLOCK_SIZE 16
const char rtems_test_name[] = "PSXIMFS 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void open_it(bool readOnly, bool create);
void write_helper(void);
void read_helper(void);
void truncate_helper(void);
void extend_helper(int eno);
void close_it(void);
void unlink_it(void);
void statvfs_helper(void);

int TestFd;
uint8_t Buffer[256];
ssize_t TotalWritten;

#define FILE_NAME "biggie"

void open_it(bool readOnly, bool create)
{
  int flag = 0;

  if ( readOnly )
    flag |= O_RDONLY;
  else {
    if ( create )
      flag |= O_CREAT;
    flag |= O_RDWR;
  }

  /* open the file */
  puts( "open(" FILE_NAME ") - OK " );
  TestFd = open( FILE_NAME, flag, 0777 );
  rtems_test_assert( TestFd != -1 );
}

void write_helper(void)
{
  ssize_t written;

  TotalWritten = 0;
  puts( "write(" FILE_NAME ") - OK " );
  do {
    written = write( TestFd, Buffer, sizeof(Buffer) );
    if ( written == -1 ) {
      if ( errno == EFBIG ) {
        printf( "Total written = %zd\n", TotalWritten );
        return;
      }
      printf(
        "Unable to create largest IMFS file (error=%s)\n",
        strerror(errno)
      );
      rtems_test_exit(0);
    }
    TotalWritten += written;
  } while (1);

}

void read_helper(void)
{
  uint8_t ch;
  ssize_t sc;
  int     i=0;

  puts( "read(" FILE_NAME ") - OK " );
  do {
    sc = read( TestFd, &ch, sizeof(ch) );
    if ( sc == 1 ) {
      if ( ch != (i%256) ) {
        printf(
          "MISMATCH 0x%02x != 0x%02x at offset %d\n",
          ch,
          i % 256,
          i
        );
        rtems_test_exit(0);
      }
      i++;
    } else if ( sc != 0 ) {
      printf(
        "ERROR - at offset %d - returned %zd and error=%s\n",
        i,
        sc,
        strerror( errno )
      );
      rtems_test_exit(0);
    }
  } while ( sc > 0 );

  if ( i == TotalWritten ) {
    puts( "File correctly read until EOF returned\n" );
  }
}

void truncate_helper(void)
{
  off_t position;
  off_t new;
  off_t sc;
  int   rc;

  position = lseek( TestFd, 0, SEEK_END );
  printf( "Seek to end .. returned %d\n", (int) position );
  rtems_test_assert( position == TotalWritten );

  puts( "lseek/ftruncate loop.." );
  new = position;
  do {
    sc = lseek( TestFd, new, SEEK_SET );
    rtems_test_assert( sc == new );

    rc = ftruncate( TestFd, new );
    if ( rc != 0 ) {
      printf(
        "ERROR - at offset %d - returned %d and error=%s\n",
        (int) new,
        rc,
        strerror( errno )
      );
    }
    rtems_test_assert( rc == 0 );
    --new;
  } while (new > 0);
}

void extend_helper(int eno)
{
  off_t position;
  off_t new;
  off_t sc;
  int   rc;

  position = lseek( TestFd, 0, SEEK_END );
  printf( "Seek to end .. returned %d\n", (int) position );

  /*
   * test case to ftruncate a file to a length > its size
   */

  rc = ftruncate( TestFd, 2 );
  rtems_test_assert( rc == 0 );

  puts( "lseek/ftruncate loop.." );
  new = position;
  do {
    sc = lseek( TestFd, new, SEEK_SET );
    rtems_test_assert( sc == new );

    rc = ftruncate( TestFd, new );
    if ( rc != 0 ) {
      if( errno != eno ) {
        printf(
          "ERROR - at offset %d - returned %d and error=%s\n",
          (int) new,
          rc,
          strerror( errno )
        );
	break;
      }
      else {
	break;
      }
    }
    rtems_test_assert( rc == 0 );
    ++new;
  } while ( 1 );
}

void close_it(void)
{
  int rc;

  puts( "close(" FILE_NAME ") - OK " );
  rc = close( TestFd );
  rtems_test_assert( rc == 0 );
}

void unlink_it(void)
{
  int rc;

  puts( "unlink(" FILE_NAME ") - OK" );
  rc = unlink( FILE_NAME );
  rtems_test_assert( rc == 0 );
}

void statvfs_helper(void)
{
  struct statvfs imfs_statvfs;
  puts( "statvfs(" FILE_NAME ") - OK " );
  int rc = statvfs(FILE_NAME, &imfs_statvfs);
  rtems_test_assert(rc == 0);
  rtems_test_assert(imfs_statvfs.f_bsize == BLOCK_SIZE);
  rtems_test_assert(imfs_statvfs.f_files == 2);
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  int i;
  void *alloc_ptr = (void *)0;
  off_t position;
  off_t new_position;
  char buf [1];
  ssize_t n;

  TEST_BEGIN();

  for (i=0 ; i<sizeof(Buffer) ; i++ )
    Buffer[i] = (uint8_t) i;

  open_it(false, true);

  statvfs_helper();

  write_helper();
  close_it();

  puts( "" );

  open_it(true, false);
  read_helper();
  close_it();

  open_it(false, false);
  truncate_helper();

  /*
   * Allocate the heap, so that extend cannot be successful
   */
  alloc_ptr = malloc( malloc_free_space() - 4 );

  extend_helper(ENOSPC);

  /*
   * free the allocated heap memory
   */
  free(alloc_ptr);

  extend_helper(EFBIG);
  position = lseek( TestFd , 0, SEEK_END );
  new_position = lseek( TestFd, position + 2, SEEK_SET );
  rtems_test_assert( new_position == position + 2 );

  n = write( TestFd, buf, sizeof(buf) );
  rtems_test_assert( n == -1 );
  rtems_test_assert( errno == EFBIG );

  close_it();
  unlink_it();

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK BLOCK_SIZE
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
