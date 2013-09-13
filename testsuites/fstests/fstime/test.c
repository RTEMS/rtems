/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

#ifdef __rtems__
#include "fstest.h"
#include "pmacros.h"
#else
#include <assert.h>
#define rtems_test_assert(x) assert(x)
#define TIME_PRECISION  (2)
#define TIME_EQUAL(x,y) (abs((x)-(y))<TIME_PRECISION)
#endif

static int do_create(const char *path, int oflag, mode_t mode)
{
  int fd = open (path, O_CREAT | oflag, mode);
  rtems_test_assert (fd >= 0);

  return fd;
}

static int do_open(const char *path, int oflag)
{
  int fd = open (path, O_CREAT | oflag);
  rtems_test_assert (fd >= 0);

  return fd;
}

static void time_test01 (void)
{
  struct stat st;
  struct utimbuf timbuf;
  int status;
  int fd;
  time_t creation_time;
  time_t truncation_time;
  time_t dir01_creation_time;
  char databuf[] = "TEST";
  char readbuf[sizeof(databuf)];
  const char *file01 = "test01";
  const char *file02 = "test02";
  const char *file03 = "test03";
  const char *dir01 = "dir01";

  int n;
  int len = strlen (databuf);

  const char *wd = __func__;
  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  /*
   * Create a new directory and change to this
   */
  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * Sleep a few seconds
   */
  puts ("Sleep a few seconds");
  sleep (3 * TIME_PRECISION);

  /*
   * Create the test files
   */
  fd = do_create (file01, O_WRONLY, mode);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = do_create (file02, O_WRONLY, mode);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = close (fd);
  rtems_test_assert (status == 0);

  /* A simple C version of touch */
  fd = do_create (file03, O_WRONLY, mode);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * If O_CREAT is set and the file did not previously exist, upon
   * successful completion, open() shall mark for update the st_atime,
   * st_ctime, and st_mtime fields of the file and the st_ctime and
   * st_mtime fields of the parent directory.
   */
  status = stat (file01, &st);
  rtems_test_assert (status == 0);

  /*
   * Make sure they are the same
   */
  rtems_test_assert (st.st_ctime == st.st_mtime);

  creation_time = st.st_ctime;

  status = stat (".", &st);
  rtems_test_assert (status == 0);

  /*
   * Make sure they are the same
   */
  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_mtime));
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_ctime));

  status = stat (file02, &st);
  rtems_test_assert (status == 0);

  /*
   * Make sure they are the same
   */
  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_mtime));
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_ctime));

  status = stat (file03, &st);
  rtems_test_assert (status == 0);

  /*
   * Make sure they are the same
   */
  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_mtime));
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_ctime));

  /*
   * Sleep a few seconds
   */
  puts ("Sleep a few seconds");
  sleep (3 * TIME_PRECISION);

  /*
   * Create an empty directory
   */
  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  /*
   * truncate file01 to len, so it does not changes the file size
   */
  status = truncate (file01, len);
  rtems_test_assert (status == 0);

  /*
   *truncate file02 to len+1, it changes the file size
   */
  status = truncate (file02, len + 1);
  rtems_test_assert (status == 0);

  /*
   * Truncate an empty file which does not change the length.
   */
  fd = do_open (file03, O_TRUNC | O_WRONLY);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * ftruncate() and open() with O_TRUNC shall upon successful completion mark
   * for update the st_ctime and st_mtime fields of the file.
   *
   * truncate() shall upon successful completion, if the file size is changed,
   * mark for update the st_ctime and st_mtime fields of the file.
   *
   * The POSIX standard "The Open Group Base Specifications Issue 7", IEEE Std
   * 1003.1, 2013 Edition says nothing about the behaviour of truncate() if the
   * file size remains unchanged.
   *
   * Future directions of the standard may mandate the behaviour specified in
   * ftruncate():
   *
   * http://austingroupbugs.net/view.php?id=489
   */

  /*
   * file01 is currently unspecified
   */
  status = stat (file01, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_ctime == st.st_mtime);
  if (TIME_EQUAL (creation_time, st.st_ctime)) {
    puts ("WARNING: truncate() behaviour may violate future POSIX standard");
  }

  truncation_time = st.st_ctime;

  /*
   * file02 shall update
   */
  status = stat (file02, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (!TIME_EQUAL (creation_time, st.st_ctime));

  /*
   * file03 shall update
   */
  status = stat (file03, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (!TIME_EQUAL (creation_time, st.st_ctime));

  /*
   *  Upon successful completion, mkdir() shall mark for update the
   *  st_atime, st_ctime, and st_mtime fields of the directory.
   *  Also, the st_ctime and st_mtime fields of the directory that
   *  contains the new entry shall be marked for update.
   */
  status = stat (dir01, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_ctime == st.st_mtime);

  dir01_creation_time = st.st_ctime;

  status = stat (".", &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (dir01_creation_time, st.st_mtime));

  /*
   * Sleep a few seconds
   */
  puts ("Sleep a few seconds");
  sleep (3 * TIME_PRECISION);

  /*
   * Upon successful completion, where nbyte is greater than 0,
   * write() shall mark for update the st_ctime and st_mtime fields of the file
   */

  /*
   * read file01, and this should not uptate st_mtime and st_ctime
   */
  fd = do_open (file01, O_RDONLY);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  status = fstat (fd, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (truncation_time, st.st_mtime));

  status = close (fd);
  rtems_test_assert (status == 0);
  /*
   * write file01, and this should uptate st_mtime st_ctime
   */
  fd = do_open (file01, O_WRONLY);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = fstat (fd, &st);

  rtems_test_assert (st.st_ctime == st.st_mtime);
  rtems_test_assert (!TIME_EQUAL (truncation_time, st.st_mtime));
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * The utime() function shall set the access and modification times
   *  of the file named by the path argument.
   */
  timbuf.actime = creation_time;
  timbuf.modtime = creation_time;

  status = utime (file01, &timbuf);
  rtems_test_assert (status == 0);

  status = stat (file01, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_atime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_atime));
  rtems_test_assert (!TIME_EQUAL (creation_time, st.st_ctime));

  status = utime (dir01, &timbuf);
  rtems_test_assert (status == 0);

  status = stat (dir01, &st);
  rtems_test_assert (status == 0);

  rtems_test_assert (st.st_atime == st.st_mtime);
  rtems_test_assert (TIME_EQUAL (creation_time, st.st_atime));
  rtems_test_assert (!TIME_EQUAL (creation_time, st.st_ctime));
}

/*
 * These tests only get time_t value, and test
 * if they are changed. Thest tests don't check atime
 */
#ifdef __rtems__
void test (void)
#else
int main(int argc, char **argv)
#endif
{

  puts( "\n\n*** TIME TEST ***" );
  time_test01();
  puts( "*** END OF TIME TEST ***" );

#ifndef __rtems__
  return 0;
#endif
}
