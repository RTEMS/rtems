/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>

#include "fstest.h"
#include "pmacros.h"

const char *databuf =
  "Happy days are here again.  Happy days are here again.1Happy "
  "days are here again.2Happy days are here again.3Happy days are here again."
  "4Happy days are here again.5Happy days are here again.6Happy days are here "
  "again.7Happy days are here again.";

void
read_write_test (void)
{

  int fd;
  int status;
  char *name01 = "name01";
  char *name02 = "name02";
  struct stat statbuf;
  char *readbuf;
  size_t len = strlen (databuf);
  off_t pos = 0;

  int n;
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
   * Create an empty file
   */
  fd = open (name01, O_CREAT | O_WRONLY, mode);
  status = close (fd);
  rtems_test_assert (status == 0);
  /*
   * Verify the empty file
   */
  status = stat (name01, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_size == 0);

  /*
   * Write data to the empty file
   */
  fd = open (name01, O_WRONLY);
  rtems_test_assert (fd != -1);

  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = stat (name01, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_size == len);

  /*
   * Read the data from the file
   */
  readbuf = (char *) malloc (len + 1);
  rtems_test_assert (readbuf);

  fd = open (name01, O_RDONLY);
  rtems_test_assert (fd != -1);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Open the file using O_APPEND and write the data
   */
  memset (readbuf, 0, len + 1);
  fd = open (name01, O_WRONLY | O_APPEND);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == 2 * len);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Read the data and verify it
   */
  fd = open (name01, O_RDONLY);
  rtems_test_assert (fd != -1);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Open the file using O_RDWR
   */
  memset (readbuf, 0, len + 1);

  fd = open (name01, O_RDWR);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));
  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == 2 * len);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Open the file using O_TRUNC
   */

  fd = open (name01, O_WRONLY | O_TRUNC);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Verify if the length is zero
   */
  status = stat (name01, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_size == 0);

  /*
   * Open a directory
   */
  status = mkdir (name02, mode);
  rtems_test_assert (status == 0);
  fd = open (name02, O_RDONLY);
  rtems_test_assert (fd != -1);

  status = close (fd);
  rtems_test_assert (status == 0);

  free (readbuf);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);
}

void
truncate_test03 (void)
{

  int fd;
  int status;
  char *name01 = "name01";
  struct stat statbuf;

  char data;
  int n;
  int i;

  size_t len = strlen (databuf);

  char *readbuf;
  off_t good_size = 100;
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
   * Create an empty file
   */
  fd = creat (name01, mode);
  status = close (fd);
  rtems_test_assert (status == 0);


  /*
   * Truncate it to a valid size
   */
  status = truncate (name01, good_size);
  rtems_test_assert (status == 0);
  /*
   * Verify the size and the data
   */
  status = stat (name01, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (good_size == statbuf.st_size);

  fd = open (name01, O_RDONLY);
  while ((n = read (fd, &data, 1)) > 0) {
    rtems_test_assert (data == 0);
  }

  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Fill a file with data
   */
  fd = open (name01, O_WRONLY);
  rtems_test_assert (fd != -1);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);

  /*
   * Truncate it to the half size
   */

  status = truncate (name01, len / 2);
  status = truncate (name01, len);

  /*
   * verify the data
   */
  readbuf = (char *) malloc (len / 2);
  rtems_test_assert (readbuf);
  fd = open (name01, O_RDONLY);
  rtems_test_assert (fd != -1);
  n = read (fd, readbuf, len / 2);
  rtems_test_assert (n == len / 2);
  rtems_test_assert (!strncmp (databuf, readbuf, len / 2));
  n = read (fd, readbuf, len / 2);
  rtems_test_assert (n == len / 2);
  for (i = 0; i < len / 2; i++) {
    rtems_test_assert (readbuf[i] == 0);
  }
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);
}

void
lseek_test (void)
{
  int fd;
  int status;
  char *name01 = "test_name01";
  struct stat statbuf;

  int n;
  int i;

  size_t len = strlen (databuf);
  off_t pos;
  int total_written = 0;

  char *readbuf;
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
   * Create a file and fill with the data.
   */
  puts ("Create a new file");
  fd = creat (name01, mode);
  rtems_test_assert (fd != -1);

  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == 0);

  pos = lseek (fd, 0, SEEK_END);
  rtems_test_assert (pos == 0);

  pos = lseek (fd, 0, SEEK_SET);
  rtems_test_assert (pos == 0);


  printf ("Writing %d bytes to file\n", len * 10);
  for (i = 0; i < 10; i++) {
    n = write (fd, databuf, len);
    rtems_test_assert (n != -1);
    total_written += n;
  }
  printf ("Successfully wrote %d\n", total_written);

  /*
   * Check the current position
   */
  puts ("Check the current position");
  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == total_written);

  pos = lseek (fd, 0, SEEK_END);
  rtems_test_assert (pos == total_written);

  /*
   * ftruncate shall not change the posistion
   */
  status = ftruncate (fd, total_written + 1);
  rtems_test_assert (status == 0);

  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == total_written);

  pos = lseek (fd, 0, SEEK_END);
  printf ("%jd\n", (intmax_t) pos);
  rtems_test_assert (pos == total_written + 1);

  status = ftruncate (fd, total_written);
  rtems_test_assert (status == 0);

  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == total_written + 1);


  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Check the file size
   */
  status = stat (name01, &statbuf);
  rtems_test_assert (statbuf.st_size == total_written);

  /*
   * Open the file with O_RDONLY and check the lseek
   */
  readbuf = (char *) malloc (len);
  fd = open (name01, O_RDONLY);
  pos = lseek (fd, len, SEEK_CUR);
  rtems_test_assert (pos == len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  pos = lseek (fd, len, SEEK_CUR);
  rtems_test_assert (pos == 3 * len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  pos = lseek (fd, -len, SEEK_CUR);
  rtems_test_assert (pos == 3 * len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  pos = lseek (fd, 4 * len, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));


  pos = lseek (fd, 10, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (strncmp (databuf, readbuf, len) != 0);

  pos = lseek (fd, -len, SEEK_END);
  n = read (fd, readbuf, 2 * len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Open the file withe O_RDWR and check the lseek
   */

  fd = open (name01, O_RDWR);

  pos = lseek (fd, len, SEEK_CUR);
  rtems_test_assert (pos == len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  pos = lseek (fd, len, SEEK_CUR);
  rtems_test_assert (pos == 3 * len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  pos = lseek (fd, -len, SEEK_CUR);
  rtems_test_assert (pos == 3 * len);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  pos = lseek (fd, 4 * len, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  /*
   * Go to the wrong position, so the data is not the same
   */
  pos = lseek (fd, 10, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (strncmp (databuf, readbuf, len) != 0);

  /*
   * Use SEEK_END
   */
  pos = lseek (fd, -len, SEEK_END);
  n = read (fd, readbuf, 2 * len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  memset (readbuf, 0, len);

  /*
   * Write the zero to the end of file.
   */
  pos = lseek (fd, -len, SEEK_END);
  rtems_test_assert (pos == total_written - len);
  n = write (fd, readbuf, len);
  rtems_test_assert (n == len);
  /*
   * Verify it
   */
  pos = lseek (fd, total_written - len, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  for (i = 0; i < n; i++) {
    rtems_test_assert (readbuf[i] == 0);
  }

  /*
   * Write the zero to the beginning of file.
   */
  pos = lseek (fd, -total_written, SEEK_END);
  rtems_test_assert (pos == 0);
  n = write (fd, readbuf, len);
  rtems_test_assert (n == len);

  /*
   * Verify it
   */

  pos = lseek (fd, 0, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  for (i = 0; i < n; i++) {
    rtems_test_assert (readbuf[i] == 0);
  }

  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (strncmp (databuf, readbuf, len) == 0);
  /*
   * Call ftruncate to decrease the file and the position not change
   */
  status = ftruncate (fd, len);
  rtems_test_assert (status == 0);
  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == len * 2);

  status = close (fd);
  rtems_test_assert (status == 0);
  /*
   * Go back to parent directory
   */
  status = chdir ("..");
  rtems_test_assert (status == 0);

}

void
test (void)
{
  read_write_test ();
  lseek_test ();
}
