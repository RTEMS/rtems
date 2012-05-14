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

#include "fstest.h"
#include "pmacros.h"

static void open_mkdir_error (void)
{
  int fd;
  int status;
  char *name01 = "name01";
  char *name02 = "name02";
  char *name03 = "name03";

  char name[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to  this
   */
  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);


  /*
   * Create a file and a directory for test.
   */
  fd = creat (name01, mode);
  status = close (fd);
  rtems_test_assert (status == 0);
  status = mkdir (name02, mode);
  rtems_test_assert (status == 0);

  /*
   * O_CREAT and O_EXCL are set, and the named file exists.
   */
  EXPECT_ERROR (EEXIST, open, name01, O_CREAT | O_EXCL);

  EXPECT_ERROR (EEXIST, mkdir, name01, mode);
  /*
   * The named file is a directory
   * and oflag includes O_WRONLY or O_RDWR.
   */
  EXPECT_ERROR (EISDIR, open, name02, O_WRONLY);
  EXPECT_ERROR (EISDIR, open, name02, O_RDWR);

  /*
   * O_CREAT is not set and the named file does not exist
   * or O_CREAT is set and either the path prefix does not exist or
   * the path argument points to an empty string.
   */

  sprintf (name, "%s/%s", name03, name02);
  EXPECT_ERROR (ENOENT, open, name, O_WRONLY);
  EXPECT_ERROR (ENOENT, open, "", O_WRONLY);
  EXPECT_ERROR (ENOENT, open, name03, O_WRONLY);

  EXPECT_ERROR (ENOENT, mkdir, name, mode);
  EXPECT_ERROR (ENOENT, mkdir, "", mode);

  /*
   * A component of the path prefix is not a directory.
   */

  sprintf (name, "%s/%s", name01, name02);
  EXPECT_ERROR (ENOTDIR, open, name, O_WRONLY);

  EXPECT_ERROR (ENOTDIR, mkdir, name, mode);
  /*
   * The fildes argument is not a valid file descriptor.
   */
  EXPECT_ERROR (EBADF, close, -1);
  EXPECT_ERROR (EBADF, close, 100);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);

}

static void rename_error (void)
{

  int fd;
  int status;
  char *name01 = "name01";
  char *name02 = "name02";
  char *name03 = "name03";
  char *nonexistence = "name04";

  char name[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   *Create a new directory and change the current directory to  this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);


  /*
   * Create a new directory and a new directory in it
   */

  status = mkdir (name01, mode);
  rtems_test_assert (status == 0);
  status = mkdir (name02, mode);
  rtems_test_assert (status == 0);
  sprintf (name, "%s/%s", name01, name03);
  status = mkdir (name, mode);
  rtems_test_assert (status == 0);

  /*
   * The link named by new is a directory that is
   *  not an empty directory.
   */
  status = rename (name02, name01);
  rtems_test_assert (status != 0);
  rtems_test_assert (errno == EEXIST || errno == ENOTEMPTY);
  /*
   * The new directory pathname contains a path prefix
   *  that names the old directory.
   */
  EXPECT_ERROR (EINVAL, rename, name01, name);
  /*
   * The new argument points to a directory and
   *  the old argument points to a file that is not a directory.
   */
  fd = creat (name03, mode);
  status = close (fd);
  rtems_test_assert (status == 0);
  EXPECT_ERROR (EISDIR, rename, name03, name02);

  /*
   * The link named by old does not name an existing file,
   *    or either old or new points to an empty string.
   */

  EXPECT_ERROR (ENOENT, rename, nonexistence, name01);
  EXPECT_ERROR (ENOENT, rename, "", name01);
  EXPECT_ERROR (ENOENT, rename, name01, "");

  /*
   * A component of either path prefix is not a directory;
   * or the old argument names a directory and new argument names
   *  a non-directory file.
   */

  sprintf (name, "%s/%s", name03, name01);
  EXPECT_ERROR (ENOTDIR, rename, name, name03);
  EXPECT_ERROR (ENOTDIR, rename, name03, name);
  EXPECT_ERROR (ENOTDIR, rename, name02, name03);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);
}

static void truncate_error (void)
{

  int fd;
  int status;
  char *file = "name";

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

  const char *wd = __func__;

  /*
   *Create a new directory and change the current directory to  this
   */
  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);
  /*
   * Create a file
   */
  fd = creat (file, mode);
  status = close (fd);
  rtems_test_assert (status == 0);


  /*
   * The length argument was less than 0.
   */
  EXPECT_ERROR (EINVAL, truncate, file, -1);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);
}


static void rmdir_unlink_error (void)
{
  int status;
  int fd;
  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  char *nonexistence = "name04";

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to  this
   */
  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * Create a new directory and a file in it for test
   */
  status = mkdir ("tmp", mode);
  rtems_test_assert (status == 0);
  fd = creat ("tmp/file", mode);
  status = close (fd);
  rtems_test_assert (status == 0);



  /*
   * The path argument names a directory that is not an empty directory,
   * or there are hard links to the directory other than
   * dot or a single entry in dot-dot.
   */

  EXPECT_ERROR (ENOTEMPTY, rmdir, "..");
  EXPECT_ERROR (ENOTEMPTY, rmdir, "tmp");


  /*
   * The path argument contains a last component that is dot.
   */


  EXPECT_ERROR (EINVAL, rmdir, ".");
  EXPECT_ERROR (EINVAL, rmdir, "tmp/.");

  /*
   * A component of path does not name an existing file,
   * or the path argument names a nonexistent directory or
   * points to an empty string
   */
  EXPECT_ERROR (ENOENT, rmdir, "");
  EXPECT_ERROR (ENOENT, rmdir, nonexistence);

  EXPECT_ERROR (ENOENT, unlink, "");
  EXPECT_ERROR (ENOENT, unlink, nonexistence);

  /*
   * component of path is not a directory.
   */

  EXPECT_ERROR (ENOTDIR, rmdir, "tmp/file");

  EXPECT_ERROR (ENOTDIR, unlink, "tmp/file/dir");
  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);


}

static void rdwr_error (void)
{

  int status;
  int fd;
  char *file01 = "name01";
  char *databuf = "test";
  char *readbuf[10];
  int shift = sizeof(off_t) * 8 - 1;
  off_t one = 1;
  off_t tiny = one << shift;
  off_t huge = tiny - 1;
  off_t off;

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to  this
   */
  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  fd = open (file01, O_WRONLY | O_CREAT, mode);

  /*
   * The fildes argument is not a valid file descriptor open for reading.
   */

  EXPECT_ERROR (EBADF, read, fd, readbuf, 10);
  EXPECT_ERROR (EBADF, read, 100, readbuf, 10);

  status = close (fd);
  rtems_test_assert (status == 0);
  /*
   * The fildes argument is not a valid file descriptor open for writing.
   */
  fd = open (file01, O_RDONLY, mode);

  EXPECT_ERROR (EBADF, write, fd, databuf, 10);

  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EBADF, write, fd, readbuf, 10);

  /*
   * The whence argument is not a proper value,
   * or the resulting file offset would be negative for a regular file,
   * block special file, or directory.
   */

  fd = open (file01, O_RDWR, mode);

  EXPECT_ERROR (EINVAL, lseek, fd, -100, SEEK_END);
  EXPECT_ERROR (EINVAL, lseek, fd, -100, SEEK_CUR);
  EXPECT_ERROR (EINVAL, lseek, fd, -100, SEEK_SET);

  status = ftruncate (fd, 1);
  rtems_test_assert (status == 0);
  EXPECT_ERROR (EOVERFLOW, lseek, fd, huge, SEEK_END);

  off = lseek (fd, 1, SEEK_SET);
  rtems_test_assert (off == 1);
  EXPECT_ERROR (EOVERFLOW, lseek, fd, huge, SEEK_CUR);

  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EBADF, lseek, fd, 0, SEEK_SET);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);

}

void test (void)
{

  puts ("\n\n*** ERROR TEST ***");
  open_mkdir_error ();
  rename_error ();
  truncate_error ();
  rmdir_unlink_error ();
  rdwr_error ();
  puts ("*** END OF ERROR TEST ***");
}
