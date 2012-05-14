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

#include "fstest.h"
#include "pmacros.h"

static void time_test01 (void)
{
  struct stat statbuf;
  struct utimbuf timbuf;
  int status = 0;
  int fd;
  time_t ctime1, mtime1;
  time_t ctime2, mtime2;
  char *readbuf;
  char *databuf = "TEST";
  char *file01 = "test01";
  char *file02 = "test02";
  char *dir01 = "dir01";

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
   * Create two files
   */
  fd = open (file01, O_CREAT | O_WRONLY, mode);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = open (file02, O_CREAT | O_WRONLY, mode);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = close (fd);
  rtems_test_assert (status == 0);
  /*
   * If O_CREAT is set and the file did not previously exist, upon
   * successful completion, open() shall mark for update the st_atime,
   * st_ctime, and st_mtime fields of the file and the st_ctime and
   * st_mtime fields of the parent directory.
   */
  status = stat (file01, &statbuf);
  rtems_test_assert (status == 0);
  ctime1 = statbuf.st_ctime;
  mtime1 = statbuf.st_mtime;

  status = stat (".", &statbuf);
  rtems_test_assert (status == 0);
  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  /*
   * Make sure they are the same
   */

  rtems_test_assert (TIME_EQUAL (ctime1, mtime1));
  rtems_test_assert (TIME_EQUAL (ctime1, mtime2));
  rtems_test_assert (TIME_EQUAL (ctime1, ctime2));

  status = stat (file02, &statbuf);
  rtems_test_assert (status == 0);
  ctime1 = statbuf.st_ctime;
  mtime1 = statbuf.st_mtime;

  status = stat (".", &statbuf);
  rtems_test_assert (status == 0);
  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  /*
   * Make sure they are the same
   */
  rtems_test_assert (TIME_EQUAL (ctime1, mtime1));
  rtems_test_assert (TIME_EQUAL (ctime1, mtime2));
  rtems_test_assert (TIME_EQUAL (ctime1, ctime2));

  /*
   * Sleep a few seconds
   */
  puts ("Sleep a few seconds");

  sleep (TIME_PRECISION);

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
   *
   *  truncate shall not modify the file offset for any open file
   *   descriptions associated with the file. Upon successful completion,
   *   if the file size is changed, this function shall mark for update
   *   the st_ctime and st_mtime fields of the file
   */

  /*
   * file01 shall not update
   */
  status = stat (file01, &statbuf);
  rtems_test_assert (status == 0);
  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (TIME_EQUAL (ctime1, mtime2));
  rtems_test_assert (TIME_EQUAL (ctime1, ctime2));

  /*
   * file02 shall update
   */
  status = stat (file02, &statbuf);
  rtems_test_assert (status == 0);
  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (TIME_EQUAL (ctime2, mtime2));
  rtems_test_assert (!TIME_EQUAL (ctime1, mtime2));
  rtems_test_assert (!TIME_EQUAL (ctime1, ctime2));

  /*
   *  Upon successful completion, mkdir() shall mark for update the
   *  5st_atime, st_ctime, and st_mtime fields of the directory.
   *  Also, the st_ctime and st_mtime fields of the directory that
   *  contains the new entry shall be marked for update.
   */
  status = stat (dir01, &statbuf);
  rtems_test_assert (status == 0);
  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;
  rtems_test_assert (TIME_EQUAL (ctime2, mtime2));
  rtems_test_assert (!TIME_EQUAL (ctime1, mtime2));
  rtems_test_assert (!TIME_EQUAL (ctime1, ctime2));

  status = stat (".", &statbuf);
  rtems_test_assert (status == 0);
  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (!TIME_EQUAL (ctime1, ctime2));
  rtems_test_assert (!TIME_EQUAL (ctime1, mtime2));

  /*
   * Upon successful completion, where nbyte is greater than 0,
   * write() shall mark for update the st_ctime and st_mtime fields of the file
   */

  /*
   * read file01, and this should not uptate st_mtime and st_ctime
   */
  readbuf = (char *) malloc (len + 1);
  fd = open (file01, O_RDONLY);
  rtems_test_assert (fd != -1);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  status = fstat (fd, &statbuf);

  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (TIME_EQUAL (ctime1, ctime2));
  rtems_test_assert (TIME_EQUAL (ctime1, mtime2));

  status = close (fd);
  rtems_test_assert (status == 0);
  /*
   * write file01, and this should uptate st_mtime st_ctime
   */
  readbuf = (char *) malloc (len + 1);
  fd = open (file01, O_WRONLY);
  rtems_test_assert (fd != -1);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = fstat (fd, &statbuf);

  ctime2 = statbuf.st_ctime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (!TIME_EQUAL (ctime1, ctime2));
  rtems_test_assert (!TIME_EQUAL (ctime1, mtime2));
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * The utime() function shall set the access and modification times
   *  of the file named by the path argument.
   */
  timbuf.actime = ctime1;
  timbuf.modtime = mtime1;

  status = utime (file01, &timbuf);
  rtems_test_assert (status == 0);

  status = stat (file01, &statbuf);
  ctime2 = statbuf.st_atime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (TIME_EQUAL (ctime1, ctime2));
  rtems_test_assert (TIME_EQUAL (ctime1, mtime2));

  status = utime (dir01, &timbuf);
  rtems_test_assert (status == 0);

  status = stat (dir01, &statbuf);
  ctime2 = statbuf.st_atime;
  mtime2 = statbuf.st_mtime;

  rtems_test_assert (TIME_EQUAL (ctime1, ctime2));
  rtems_test_assert (TIME_EQUAL (ctime1, mtime2));

}

/*
 * These tests only get time_t value, and test
 * if they are changed. Thest tests don't check atime
 */
void test (void)
{

  puts( "\n\n*** TIME TEST ***" );
  time_test01();
  puts( "*** END OF TIME TEST ***" );
}
