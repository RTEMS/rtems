/*
 *  COPYRIGHT (c) 2014
 *  Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "fstest.h"
#include "fs_config.h"
#include "fstest_support.h"
#include "pmacros.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

const char rtems_test_name[] = "FSRENAME " FILESYSTEM;

static void symbolic_link_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *symlink01 = "slink01";
  const char *symlink02 = "slink02";

  char path01[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  struct stat statbuf;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to a symbolic link to another file.
   */

  puts ("\nOld is a simbolic link and rename operates on the simbolic link itself\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = symlink (name01, symlink01);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, symlink01, name02);
  EXPECT_EQUAL (0, lstat, name02, &statbuf);

  puts ("Testing if name02 is now a symlink");

  if(S_ISLNK(statbuf.st_mode) != 0)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, unlink, name02);
  EXPECT_EQUAL (-1, unlink, symlink01);

  /*
   * The new argument points to a symbolic link to another file and
   * the old argument points to a file.
   */

  puts ("\nNew is a simbolic link and rename operates on the simbolic link itself\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = symlink (name01, symlink01);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name02, symlink01);
  EXPECT_EQUAL (0, lstat, symlink01, &statbuf);

  puts ("Testing that symlink01 is not a symlink");

  if(S_ISLNK(statbuf.st_mode) == 0)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (-1, unlink, name02);
  EXPECT_EQUAL (0, unlink, symlink01);

  /*
   * The new argument points to a file inside a directory and
   * the old argument points to a file which filepath contains
   * a symbolic link loop.
   */

  puts ("\nTesting with symbolic link loop's\n");

  status = symlink (symlink01, symlink02);
  rtems_test_assert (status == 0);

  status = symlink (symlink02, symlink01);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/test", symlink01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (ELOOP, rename, path01, name01);

  rv = snprintf (path01, sizeof(path01), "%s/test", symlink02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (ELOOP, rename, path01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, unlink, name01);
  EXPECT_EQUAL (0, unlink, symlink01);
  EXPECT_EQUAL (0, unlink, symlink02);

  /*
   * The new argument points to a file, which filepath contains
   * a symbolic link loop, and
   * the old argument points to a file inside a directory
   */

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = symlink (symlink01, symlink02);
  rtems_test_assert (status == 0);

  status = symlink (symlink02, symlink01);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/test", symlink01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (ELOOP, rename, name01, path01);

  rv = snprintf (path01, sizeof(path01), "%s/test", symlink02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (ELOOP, rename, name01, path01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, unlink, symlink01);
  EXPECT_EQUAL (0, unlink, symlink02);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void same_file_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";

  char path01[30];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to the same file on the same directory.
   */

  puts ("\nRename file with itself\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

  /*
   * The new argument points to a file and
   * the old argument points to the same file from another directory.
   */

  puts ("\nRename file with itself through a hard link in another directory\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path01));
  status = link (name01, path01);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name01, path01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void directory_test (void)
{
  int fd;
  int status;
  int rv;
  int i;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[30];

  char link_name[10];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  struct stat statbuf;

  long LINK_MAX_val;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to a directory.
   */

  puts ("\nRename directory with file\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOTDIR, rename, dir01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a directory and
   * the old argument points to a file.
   */

  puts ("\nRename file with directory\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EISDIR, rename, name01, dir01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to an empty directory and
   * the old argument points to an ancestor directory of new.
   */

  puts ("\nRename directory with ancestor directory\n");

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir02, dir01);
  rtems_test_assert (rv < sizeof(path01));
  status = mkdir (path01, mode);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EINVAL, rename, dir02, path01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, rmdir, path01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * The new argument points to an empty directory and
   * the old argument points to a non empty directory.
   */

  puts ("\nRename directory with non empty directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir02, name02);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, dir01, dir02);

  puts("Testing errno for EEXIST or ENOTEMPTY");

  if(errno == EEXIST || errno == ENOTEMPTY)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * The new argument points to an empty directory and
   * the old argument points to other empty directory.
   */

  puts ("\nRename empty directory with another empty directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, dir01, dir02);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * The new argument points to a non existant directory and
   * the old argument points to an existant directory at LINK_MAX.
   */

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  LINK_MAX_val = pathconf (dir01, _PC_LINK_MAX);
  rtems_test_assert (LINK_MAX_val >= 0);

  status = stat (dir01, &statbuf);
  rtems_test_assert (status == 0);

  for(i = statbuf.st_nlink; i < LINK_MAX_val; i++)
  {
    rv = snprintf (link_name, sizeof(link_name), "%s/%d", dir01, i);
    rtems_test_assert (rv < sizeof(link_name));

    status = mkdir (link_name, mode);
    rtems_test_assert (status == 0);
  }

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, dir01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EMLINK, rename, dir02, path01);

  /*
   * Clear directory
   */

  for(i = statbuf.st_nlink; i < LINK_MAX_val; i++)
  {
    rv = snprintf (link_name, sizeof(link_name), "%s/%d", dir01, i);
    rtems_test_assert (rv < sizeof(link_name));

    status = rmdir (link_name);
    rtems_test_assert (status == 0);
  }

  EXPECT_EQUAL (-1, rmdir, path01);
  EXPECT_EQUAL (0, rmdir, dir02);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a file and
   * the old argument points to another file on a directory with S_ISVTX.
   */

  puts ("\nRename files within directories protected with S_ISVTX\n");

  status = mkdir (dir01, mode | S_ISVTX);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chown (path01, 65534, -1);
  rtems_test_assert (status == 0);

  status = chown (dir01, 65534, -1);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, path01, name02);

  puts("Testing errno for EPERM or EACCES");

  if(errno == EPERM || errno == EACCES)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, unlink, name02);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a file on a directory with S_ISVTX and
   * the old argument points to a file outside that directory.
   */

  status = mkdir (dir01, mode | S_ISVTX);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chown (path01, 65534, -1);
  rtems_test_assert (status == 0);

  status = chown (dir01, 65534, -1);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, name02, path01);

  puts("Testing errno for EPERM or EACCES");

  if(errno == EPERM || errno == EACCES)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, unlink, name02);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void arg_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  char filename[NAME_MAX + 2];
  char path01[20];

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a non existant file and
   * the old argument points to a file.
   */

  puts ("\nRename file with non existant file\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, name01, name02);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, unlink, name01);
  EXPECT_EQUAL (0, unlink, name02);

  /*
   * The new argument points to a file and
   * the old argument points to a non existant file.
   */

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOENT, rename, name02, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (-1, unlink, name02);

  /*
   * The new argument points to a non existant file and
   * the old argument points to a file where a component of the
   * filepath does not exist.
   */

  puts ("\nRename file with non existant filepath\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s/%s", dir01, name01, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (ENOENT, rename, path01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, unlink, name01);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a non existant directory and
   * the old argument points to a directory.
   */

  puts ("\nRename directory with non existant directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rename, dir01, dir02);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (-1, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * The new argument is a name bigger than NAME_MAX and
   * the old argument points to a file.
   */

  puts ("\nRename file with a name size exceeding NAME_MAX\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  /* Generate string with NAME_MAX + 1 length */
  memset(filename, 'a', NAME_MAX + 1);
  filename[NAME_MAX + 1] = '\0';

  EXPECT_ERROR (ENAMETOOLONG, rename, name01, filename);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);
  EXPECT_EQUAL (-1, unlink, filename);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void arg_format_test (void)
{
  int fd;
  int status;
  const char *name01 = "name01";

  const char *dir01 = "dir01";

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a directory and
   * the old argument points to current directory.
   */

  puts ("\nRename directory with current directory\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, "." , dir01);

  puts("Testing errno for EINVAL or EBUSY");

  if(errno == EINVAL || errno == EBUSY)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * The new argument points to current directory and
   * the old argument points to a directory.
   */

  EXPECT_EQUAL (-1, rename, dir01, ".");

  puts("Testing errno for EINVAL or EBUSY");

  if(errno == EINVAL || errno == EBUSY)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * The new argument points to a directory and
   * the old argument points to previous directory.
   */

  puts ("\nRename directory with previous directory\n");

  EXPECT_EQUAL (-1, rename, ".." , dir01);

  puts("Testing errno for EINVAL or EBUSY");

  if(errno == EINVAL || errno == EBUSY)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * The new argument points to previous directory and
   * the old argument points to a directory.
   */

  EXPECT_EQUAL (-1, rename, dir01, "..");

  puts("Testing errno for EINVAL or EBUSY");

  if(errno == EINVAL || errno == EBUSY)
    FS_PASS ();
  else
    FS_FAIL ();

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a file and
   * the old argument is an empty string.
   */

  puts("\nTesting empty filepaths\n");

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOENT, rename, name01, "");

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

  /*
   * The new argument is an empty string and
   * the old argument points to a file.
   */

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (ENOENT, rename, "", name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void write_permission_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  mode_t no_write_access = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP
                         | S_IROTH | S_IXOTH;

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to another file,
   * both inside a directory with no write permission.
   */

  puts ("\nRename two files on a directory with no write permission \n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  status = chdir (dir01);
  rtems_test_assert (status == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chmod (".", no_write_access);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EACCES, rename, name01 , name02);

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file in a directory with no write access and
   * the old argument points to another file on a directory with write access.
   */

  puts ("\nRename file between two directories, with and without write access\n");

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  status = chdir (dir02);
  rtems_test_assert (status == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "../%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EACCES, rename, name01, path01);

  /*
   * The new argument points to a file in a directory with write access and
   * the old argument points to another file on a directory without write access.
   */

  EXPECT_ERROR (EACCES, rename, path01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

  rv = snprintf (path01, sizeof(path01), "../%s", dir01);
  rtems_test_assert (rv < sizeof(path01));
  status = chmod (path01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "../%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_EQUAL (0, unlink, path01);

  rv = snprintf (path01, sizeof(path01), "../%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_EQUAL (0, unlink, path01);

  status = chdir ("..");
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void search_permission_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[20];
  char path02[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  mode_t no_execute_access = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
                           | S_IROTH | S_IWOTH;

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to another file,
   * both inside a directory with no execute permission.
   */

  puts ("\nRename two files on a directory with no execute permission \n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  rv = snprintf (path02, sizeof(path02), "%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path02));
  fd = creat (path02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chmod (dir01, no_execute_access);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EACCES, rename, path01 , path02);

  /*
   * The new argument points to a file in a directory with no execute access and
   * the old argument points to another file on a directory with execute access.
   */

  puts ("\nRename file between two directories, with and without execute access\n");

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  status = chdir (dir02);
  rtems_test_assert (status == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chdir ("..");
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir02, name01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EACCES, rename, path01, path02);

  /*
   * The new argument points to a file in a directory with execute access and
   * the old argument points to another file on a directory without execute access.
   */

  EXPECT_ERROR (EACCES, rename, path02, path01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);

  status = chmod (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, unlink, path02);
  EXPECT_EQUAL (0, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void filesystem_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  char path01[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file on another instance of the filesystem and
   * the old argument points to a file on the base filesystem.
   */

  puts ("\nRename files across diferent filesystems\n");

  rv = chroot ("/");
  rtems_test_assert (rv == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", BASE_FOR_TEST, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EXDEV, rename, name01, path01);

  EXPECT_EQUAL (-1, unlink, path01);
  EXPECT_EQUAL (0, unlink, name01);

  rv = chroot (BASE_FOR_TEST);
  rtems_test_assert (rv == 0);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

void test (void)
{
  symbolic_link_test ();
  same_file_test ();
  directory_test ();
  arg_test ();
  arg_format_test ();
  write_permission_test ();
  search_permission_test ();
  filesystem_test ();
}
