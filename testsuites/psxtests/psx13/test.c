/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief This tests various file system functions.
 *
 *  This test exercises the following routines:
 *
 *     - lseek()
 *     - dup()
 *     - dup2()
 *     - fdatasync()
 *     - fsync()
 *     - pathconf()
 *     - fpathconf()
 *     - umask()
 *     - utime()
 *     - utimes()
 *     - utimensat()
 *     - futimens()
 *     - sync()
 */

/*
 * COPYRIGHT (C) 1989, 2021 On-Line Applications Research Corporation (OAR).
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
 * * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/score/timespec.h>
#include <rtems/score/todimpl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <utime.h>
#include <tmacros.h>

#include <stdio.h>
#include <unistd.h>

#include <pmacros.h>

const char rtems_test_name[] = "PSX 13";

/**
 * @brief Initializes the three files to be used for the test.
 */
static void InitFiles( void )
{
  int count;
  int rv;
  FILE *fp1, *fp2, *fp3;
  char letter;
  int number;

  fp1 = fopen( "testfile1.tst", "wt" );
  rtems_test_assert( fp1 != NULL );

  fp2 = fopen( "testfile2.tst", "wt" );
  rtems_test_assert( fp2 != NULL );

  fp3 = fopen( "testfile4.tst", "wb" );
  rtems_test_assert( fp3 != NULL );

  letter = 'a';

  for( count = 0 ; count < (26*4); ++count) {
    fprintf( fp1, "%c", letter );
    fprintf( fp2, "%c", letter );

    ++letter;
    if( letter > 'z' )
      letter = 'a';
  }

  number = 0;

  for( count = 0; count < 40; ++count ) {
    fwrite( &number, 1, sizeof(int), fp3 );

    ++number;
    if( number > 9 )
      number = 0;
  }

  rv = fclose( fp1 );
  rtems_test_assert( rv != EOF );

  rv = fclose( fp2 );
  rtems_test_assert( rv != EOF );

  rv = fclose( fp3 );
  rtems_test_assert( rv != EOF );
}

/**
 * @brief Exercises lseek() by lseeking on the console.
 */
static void DeviceLSeekTest( void )
{
  int rv;
  int fd;

  fd = open( "/dev/console", O_RDONLY );
  rtems_test_assert( fd != -1 );

  rv = lseek( fd, 5, SEEK_SET );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == ESPIPE );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

/**
 * @brief Exercises dup().
 */
static void DupTest( void )
{
  int fd1, fd2;
  int flags;
  int rv;

  fd1 = open( "testfile1.tst", O_RDONLY );
  rtems_test_assert( fd1 != -1 );

  fd2 = dup( fd1 );
  rtems_test_assert( fd2 != -1 );

  rv = fcntl( fd1, F_SETFL, O_APPEND );
  rtems_test_assert( rv != -1 );

  flags = fcntl( fd2, F_GETFL ) & O_APPEND;
  rtems_test_assert( flags == 0 );

  rv = close( fd1 );
  rtems_test_assert( rv == 0 );

  rv = close( fd2 );
  rtems_test_assert( rv == 0 );
}

/**
 * @brief Exercises dup2().
 */
static void Dup2Test( void )
{
  int fd1, fd2;
  int flags;
  int rv;

  fd1 = open( "testfile1.tst", O_RDONLY );
  rtems_test_assert( fd1 != -1 );

  fd2 = open( "testfile2.tst", O_RDONLY );
  rtems_test_assert( fd2 != -1 );

  /* make sure dup2 works if both fd1 and fd2 are valid file descriptors. */
  rv = dup2( fd1, fd2 );
  rtems_test_assert( rv != -1 );

  rv = fcntl( fd1, F_SETFL, O_APPEND );
  rtems_test_assert( rv != -1 );

  flags = fcntl( fd1, F_GETFL ) & O_APPEND;
  rtems_test_assert( flags == O_APPEND );

  /* make sure dup2 fails correctly if one or the other arguments are invalid. */
  /* this assumes -1 is an invalid value for a file descriptor!!! (POSIX book, p.135) */
  rv = close( fd1 );
  rtems_test_assert( rv == 0 );

  fd1 = -1;

  rv = dup2( fd1, fd2 );
  rtems_test_assert( rv == -1 );

  fd1 = dup( fd2 );
  fd2 = -1;

  rv = dup2( fd1, fd2 );
  rtems_test_assert( rv == -1 );

  rv = close( fd1 );
  rtems_test_assert( rv == 0 );
}

/**
 * @brief Exercises fdatasync().
 */
static void FDataSyncTest( void )
{
  int fd;
  int rv;

  /* Try it with a RD_ONLY file. */
  fd = open( "testfile1.tst", O_RDONLY );
  rtems_test_assert( fd != -1 );

  rv = fdatasync( fd );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EBADF );

  rv = close(fd);
  rtems_test_assert( rv == 0 );

  /* Try it with a bad file descriptor */
  fd = -1;

  rv = fdatasync( fd );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EBADF );

  /* Okay - now the success case... */
  fd = open( "testfile1.tst", O_RDWR );
  rv = fdatasync( fd );
  rtems_test_assert( rv == 0 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

/**
 * @brief Exercises umask().
 */
static void UMaskTest( void )
{
  mode_t rv;

  (void) umask( 023 );

  rv = umask( 022 );
  rtems_test_assert( rv == 023 );
}

/**
 * @brief Exercises utime().
 */
static void UTimeTest( void )
{
  int rv;
  struct utimbuf time;
  struct timespec current_time;
  struct stat fstat;

  /* ENOENT test case */

  /* Case: Pass an invalid filename. */
  rv = utime( "!This is an =invalid p@thname!!! :)", NULL );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == ENOENT );

  /* EACCES test case */

  /* Case: Change user ID to someone besides root */
  rv = seteuid( 1 );
  rtems_test_assert( rv == 0 );

  rv = utime( "testfile1.tst", NULL );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EACCES );

  rv = seteuid( 0 );
  rtems_test_assert( rv == 0 );

  /* EINVAL test cases */

  /* Case: Invalid access time */
  time.actime  = -1;
  time.modtime = 54321;

  rv = utime( "testfile1.tst", &time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Invalid modified time */
  time.actime  = 12345;
  time.modtime = -1;

  rv = utime( "testfile1.tst", &time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Successful test cases */

  /* Case: Test without times argument */
  clock_gettime( CLOCK_REALTIME, &current_time );

  rv = utime( "testfile1.tst", NULL );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_atim.tv_sec );
  rtems_test_assert( current_time.tv_sec <= fstat.st_mtim.tv_sec );

  /* Case: time is filled with valid values */
  time.actime  = 12345;
  time.modtime = 54321;

  rv = utime( "testfile1.tst", &time );
  rtems_test_assert( rv == 0 );

  /* Check that it actually changed the time */
  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( fstat.st_atime == 12345 );
  rtems_test_assert( fstat.st_mtime == 54321 );
}

/**
 * @brief Exercises utimes().
 */
static void UTimesTest( void )
{
  int rv;
  struct timeval time[2];
  struct timespec current_time;
  struct stat fstat;

  /* ENOENT test case */

  /* Case: First, an invalid filename. */
  rv = utimes( "!This is an =invalid p@thname!!! : )", NULL);
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == ENOENT );

  /* EACCES test case */

  /* Change the user ID of the process to someone besides root */
  rv = seteuid( 1 );
  rtems_test_assert( rv == 0 );

  rv = utimes( "testfile1.tst", NULL );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EACCES );

  rv = seteuid( 0 );
  rtems_test_assert( rv == 0 );

  /* EINVAL test cases */

  /* Case: Negative access time tv_sec value */
  time[0].tv_sec = -1;
  time[0].tv_usec = 12345;
  time[1].tv_sec = 54321;
  time[1].tv_usec = 54321;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative modified time second value */
  time[0].tv_sec = 12345;
  time[1].tv_sec = -1;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative access time microsecond value */
  time[1].tv_sec = 54321;
  time[0].tv_usec = -1;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative modified time microsecond value */
  time[0].tv_usec = 12345;
  time[1].tv_usec = -1;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Access time microsecond value too large */
  time[0].tv_usec = 1000000;
  time[1].tv_usec = 54321;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Modified time microsecond value too large */
  time[1].tv_usec = 1000000;
  time[0].tv_usec = 12345;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Successful test cases */

  /* Case: Test without times argument */
  clock_gettime( CLOCK_REALTIME, &current_time );

  rv = utimes( "testfile1.tst", NULL );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_atim.tv_sec );
  rtems_test_assert( current_time.tv_sec <= fstat.st_mtim.tv_sec );

  /* Case: time is filled with valid values */
  time[0].tv_sec = 12345;
  time[0].tv_usec = 12345;
  time[1].tv_sec = 54321;
  time[1].tv_usec = 54321;

  rv = utimes( "testfile1.tst", time );
  rtems_test_assert( rv == 0 );

  /* Check that it actually changed the time */
  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( fstat.st_atime == 12345 );
  rtems_test_assert( fstat.st_mtime == 54321 );
}

/**
 * @brief Exercises utimensat().
 */
static void UTimensatTest( void )
{
  int rv;
  struct timespec time[2];
  struct timespec current_time;
  struct stat fstat;

  /* ENOSYS test cases */

  /* Case: Pass an unsupported file descriptor */
  rv = utimensat(
    0,
    "!This is an =invalid p@thname!!! : )",
    NULL,
    0
  );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == ENOSYS );

  /* Case: Pass unsupported flag */
  rv = utimensat(
    AT_FDCWD,
    "!This is an =invalid p@thname!!! : )",
    NULL,
    1
  );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == ENOSYS );

  /* ENOENT test case */

  /* Use an invalid filename. */
  rv = utimensat(
    AT_FDCWD,
    "!This is an =invalid p@thname!!! : )",
    NULL,
    0
  );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == ENOENT );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );

  /* EACCES test Cases */

  /* Case: When times is NULL and the user has insufficient privileges */

  /* Change the user ID of the process to someone besides root */
  rv = seteuid( 1 );
  rtems_test_assert( rv == 0 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", NULL, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EACCES );

  rv = seteuid( 0 );
  rtems_test_assert( rv == 0 );

  /* Case: File is read-only and time's tv_nsec members are UTIME_NOW */

  /* Change file to be read-only */
  rv = chmod( "testfile1.tst", 06444 );
  rtems_test_assert( rv == 0 );

  _Timespec_Set( &time[0], 0, UTIME_NOW );
  _Timespec_Set( &time[1], 0, UTIME_NOW );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EACCES );

  rv = chmod( "testfile1.tst", fstat.st_mode );
  rtems_test_assert( rv == 0 );

  /* EINVAL test cases */

  /* Case: Negative access time second value */
  _Timespec_Set( &time[0], -12345, 12345 );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative modified time second value */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], -54321, 54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative access time nanosecond value */
  _Timespec_Set( &time[0], 12345, -12345 );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative modified time nanosecond value */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, -54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Access time nanosecond value too large */
  _Timespec_Set( &time[0], 12345, TOD_NANOSECONDS_PER_SECOND );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Modified time nanosecond value too large */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, TOD_NANOSECONDS_PER_SECOND );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Successful test cases */

  /* Case: Test without times argument */
  clock_gettime( CLOCK_REALTIME, &current_time );

  rv = utimensat( AT_FDCWD, "testfile1.tst", NULL, 0 );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_atim.tv_sec );
  rtems_test_assert( current_time.tv_sec <= fstat.st_mtim.tv_sec );

  /* Case: Running with access time nanosecond field equal to UTIME_NOW */
  _Timespec_Set( &time[0], 12345, UTIME_NOW );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_atim.tv_sec );
  rtems_test_assert( fstat.st_mtime == 54321 );

  /* Case: Running with modified time nanosecond field equal to UTIME_NOW */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, UTIME_NOW );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( fstat.st_atime == 12345 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_mtim.tv_sec );

  /* Case: Normal run */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = utimensat( AT_FDCWD, "testfile1.tst", time, 0 );
  rtems_test_assert( rv == 0 );

  /* Check that it actually changed the time */
  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( fstat.st_atime == 12345 );
  rtems_test_assert( fstat.st_mtime == 54321 );
}

/**
 * @brief Exercises futimens().
 */
static void FutimensTest( void )
{
  int rv;
  int fd;
  struct timespec time[2];
  struct timespec current_time;
  struct stat fstat;

  /* EBADF test case */

  /* Case: Pass an invalid file descriptor */
  _Timespec_Set_to_zero( &time[0] );
  _Timespec_Set_to_zero( &time[1] );
  rv = futimens( -1, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EBADF );

  fd = open( "testfile1.tst", O_RDWR );
  rtems_test_assert( fd != -1 );

  /* EACCES test cases */

  /* Case: When times is NULL and the user has insufficient privileges */

  /* Change the user ID of the process to someone besides root */
  rv = seteuid( 1 );
  rtems_test_assert( rv == 0 );

  rv = futimens( fd, NULL );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EACCES );

  rv = seteuid( 0 );
  rtems_test_assert( rv == 0 );

  /* Case: File is read-only and time's tv_nsec members are UTIME_NOW */

  /* Change file to be read-only */
  rv = chmod( "testfile1.tst", 06444 );
  rtems_test_assert( rv == 0 );

  _Timespec_Set( &time[0], 0, UTIME_NOW );
  _Timespec_Set( &time[1], 0, UTIME_NOW );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EACCES );

  rv = chmod( "testfile1.tst", fstat.st_mode );
  rtems_test_assert( rv == 0 );

  /* EINVAL test cases */

  /* Case: Negative access time second value */
  _Timespec_Set( &time[0], -12345, 12345 );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative modified time second value */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], -54321, 54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative access time nanosecond value */
  _Timespec_Set( &time[0], 12345, -12345 );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Negative modified time nanosecond value */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, -54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Access time nanosecond value too large */
  _Timespec_Set( &time[0], 12345, TOD_NANOSECONDS_PER_SECOND );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Case: Modified time nanosecond value too large */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, TOD_NANOSECONDS_PER_SECOND );

  rv = futimens( fd, time );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EINVAL );

  /* Successful test cases */

  /* Case: Test without times argument */
  clock_gettime( CLOCK_REALTIME, &current_time );

  rv = futimens( fd, NULL );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_atim.tv_sec );
  rtems_test_assert( current_time.tv_sec <= fstat.st_mtim.tv_sec );

/* Case: Running with access time nanosecond field equal to UTIME_NOW */
  _Timespec_Set( &time[0], 12345, UTIME_NOW );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_atim.tv_sec );
  rtems_test_assert( fstat.st_mtime == 54321 );

  /* Case: Running with modified time nanosecond field equal to UTIME_NOW */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, UTIME_NOW );

  rv = futimens( fd, time );
  rtems_test_assert( rv == 0 );

  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( fstat.st_atime == 12345 );
  rtems_test_assert( current_time.tv_sec <= fstat.st_mtim.tv_sec );

  /* Case: Normal run */
  _Timespec_Set( &time[0], 12345, 12345 );
  _Timespec_Set( &time[1], 54321, 54321 );

  rv = futimens( fd, time );
  rtems_test_assert( rv == 0 );

  /* Check that it actually changed the time */
  rv = stat( "testfile1.tst", &fstat );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( fstat.st_atime == 12345 );
  rtems_test_assert( fstat.st_mtime == 54321 );
}

/**
 * @brief Exercises pathconf().
 */
static void PathConfTest( void )
{
  int rv;

  rv = pathconf( "thisfiledoesnotexist", _PC_LINK_MAX );
  rtems_test_assert( rv == -1 );

  rv = pathconf( "testfile1.tst", _PC_LINK_MAX );
  rtems_test_assert( rv != -1 );
}

/**
 * @brief Exercises fpathconf().
 */
static void FPathConfTest( void )
{
  int rv;
  int fd;

  fd = -1;
  rv = fpathconf( fd, _PC_LINK_MAX );
  rtems_test_assert( rv == -1 );

  fd = open( "testfile1.tst", O_RDWR );
  rtems_test_assert( fd != -1 );

  rv = fpathconf( fd, _PC_LINK_MAX );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_MAX_CANON );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_MAX_INPUT );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_NAME_MAX );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_PATH_MAX );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_PIPE_BUF );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_CHOWN_RESTRICTED );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_NO_TRUNC );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_VDISABLE );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_ASYNC_IO );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_PRIO_IO );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_SYNC_IO );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, 255 );
  rtems_test_assert( rv == -1 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );

  fd = open( "testfile1.tst", O_WRONLY );
  rtems_test_assert( rv != -1 );

  rv = fpathconf( fd, _PC_LINK_MAX );
  rtems_test_assert( rv != -1 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

/**
 * @brief Exercises fsync().
 */
static void FSyncTest( void )
{
  int rv;
  int fd;

  fd = open( "testfile1.tst", O_RDWR );
  rtems_test_assert( fd != -1 );

  rv = fsync(fd);
  rtems_test_assert( rv != -1 );

  rv = close( fd );
  rtems_test_assert( rv == 0 );
}

/**
 * @brief Exercises sync().
 */
static void SyncTest( void )
{
  sync();
}

/**
 * @brief The main entry point to the test.
 */
int test_main( void );
int test_main( void )
{
  TEST_BEGIN();

  InitFiles();

  DeviceLSeekTest();
  DupTest();
  Dup2Test();
  FDataSyncTest();
  UMaskTest();
  UTimeTest();
  UTimesTest();
  UTimensatTest();
  FutimensTest();
  FSyncTest();
  PathConfTest();
  FPathConfTest();
  SyncTest();

  TEST_END();

  rtems_test_exit( 0 );
}
