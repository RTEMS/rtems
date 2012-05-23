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

static const mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

static const char databuf [] =
  "Happy days are here again.  Happy days are here again.1Happy "
  "days are here again.2Happy days are here again.3Happy days are here again."
  "4Happy days are here again.5Happy days are here again.6Happy days are here "
  "again.7Happy days are here again.";

static const size_t len = sizeof (databuf) - 1;

static void
test_case_enter (const char *wd)
{
  int status;

  printf ("test case: %s\n", wd);

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);

  status = chdir (wd);
  rtems_test_assert (status == 0);
}

static void
test_case_leave (void)
{
  int status;

  status = chdir ("..");
  rtems_test_assert (status == 0);
}

static void
read_write_test (void)
{

  int fd;
  int status;
  char *name01 = "name01";
  char *name02 = "name02";
  struct stat statbuf;
  char *readbuf;
  off_t pos = 0;

  int n;

  test_case_enter (__func__);

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
  rtems_test_assert (fd >= 0);

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
  rtems_test_assert (fd >= 0);
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
  pos = lseek (fd, 0, SEEK_SET);
  rtems_test_assert (pos == 0);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == 3 * len);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Read the data and verify it
   */
  fd = open (name01, O_RDONLY);
  rtems_test_assert (fd >= 0);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));
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
  rtems_test_assert (fd >= 0);

  status = close (fd);
  rtems_test_assert (status == 0);

  free (readbuf);

  test_case_leave ();
}

static void
truncate_test03 (void)
{

  int fd;
  int status;
  char *name01 = "name01";
  struct stat statbuf;

  char data;
  int n;
  int i;

  char *readbuf;
  off_t good_size = 100;

  test_case_enter (__func__);

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
  rtems_test_assert (fd >= 0);
  n = write (fd, databuf, len);
  rtems_test_assert (n == len);
  status = close (fd);
  rtems_test_assert (status == 0);

  /*
   * Truncate it to the half size
   */

  status = truncate (name01, len / 2);
  rtems_test_assert (status == 0);
  status = truncate (name01, len);
  rtems_test_assert (status == 0);

  /*
   * verify the data
   */
  readbuf = (char *) malloc (len / 2);
  rtems_test_assert (readbuf);
  fd = open (name01, O_RDONLY);
  rtems_test_assert (fd >= 0);
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

static void
lseek_test (void)
{
  int fd;
  int status;
  const char *name01 = "test_name01";
  struct stat statbuf;

  ssize_t n;
  int i;

  off_t pos;
  ssize_t total_written = 0;

  char *readbuf;

  test_case_enter (__func__);

  /*
   * Create a file and fill with the data.
   */
  puts ("Create a new file");
  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);

  pos = lseek (fd, 0, SEEK_CUR);
  rtems_test_assert (pos == 0);

  pos = lseek (fd, 0, SEEK_END);
  rtems_test_assert (pos == 0);

  pos = lseek (fd, 0, SEEK_SET);
  rtems_test_assert (pos == 0);


  printf ("Writing %zd bytes to file\n", len * 10);
  for (i = 0; i < 10; i++) {
    n = write (fd, databuf, len);
    rtems_test_assert (n == (ssize_t) len);
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

  /*
   * Check the file size
   */
  status = fstat (fd, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (statbuf.st_size == total_written);

  status = ftruncate (fd, total_written);
  rtems_test_assert (status == 0);

  status = close (fd);
  rtems_test_assert (status == 0);

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

  pos = lseek (fd, -(off_t) len, SEEK_CUR);
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

  pos = lseek (fd, -(off_t) len, SEEK_END);
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

  pos = lseek (fd, -(off_t) len, SEEK_CUR);
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
  pos = lseek (fd, -(off_t) len, SEEK_END);
  n = read (fd, readbuf, 2 * len);
  rtems_test_assert (n == len);
  rtems_test_assert (!strncmp (databuf, readbuf, len));

  memset (readbuf, 0, len);

  /*
   * Write the zero to the end of file.
   */
  pos = lseek (fd, -(off_t) len, SEEK_END);
  rtems_test_assert (pos == (off_t) total_written - (off_t) len);
  n = write (fd, readbuf, len);
  rtems_test_assert (n == len);
  /*
   * Verify it
   */
  pos = lseek (fd, (off_t) total_written - (off_t) len, SEEK_SET);
  n = read (fd, readbuf, len);
  rtems_test_assert (n == len);
  for (i = 0; i < n; i++) {
    rtems_test_assert (readbuf[i] == 0);
  }

  /*
   * Write the zero to the beginning of file.
   */
  pos = lseek (fd, -(off_t) total_written, SEEK_END);
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

  test_case_leave ();
}

static void
truncate_to_zero (void)
{
  int fd;
  ssize_t n;
  int status;
  off_t pos;

  test_case_enter (__func__);

  fd = creat ("file", mode);
  rtems_test_assert (fd >= 0);

  n = write (fd, databuf, len);
  rtems_test_assert (n == (ssize_t) len);

  pos = lseek (fd, 0, SEEK_END);
  rtems_test_assert (pos == len);

  status = ftruncate (fd, 0);
  rtems_test_assert (status == 0);

  pos = lseek (fd, 0, SEEK_END);
  rtems_test_assert (pos == 0);

  status = close (fd);
  rtems_test_assert (status == 0);

  test_case_leave ();
}

static void
random_fill (char *dst, size_t n)
{
  static uint32_t u = 0x12345678;
  uint32_t v = u;
  uint32_t w;
  size_t i = 0;
  int j = 0;

  while (i < n) {
    if (j == 0) {
      v *= 1664525;
      v += 1013904223;
      w = v;
    } else {
      w >>= 8;
    }

    dst [i] = (char) w;

    ++i;
    j = (j + 1) % 4;
  }

  u = v;
}

static void
block_rw_lseek (int fd, size_t pos)
{
  off_t actual;

  actual = lseek (fd, pos, SEEK_SET);
  rtems_test_assert (actual == pos);
}

static void
block_rw_write (int fd, char *out, size_t pos, size_t size)
{
  ssize_t n;

  random_fill (out + pos, size);

  block_rw_lseek (fd, pos);

  n = write (fd, out + pos, size);
  rtems_test_assert (n == (ssize_t) size);
}

static void
block_rw_write_cont (int fd, char *out, size_t *pos, size_t size)
{
  ssize_t n;

  random_fill (out + *pos, size);

  n = write (fd, out + *pos, size);
  rtems_test_assert (n == (ssize_t) size);

  *pos += size;
}

static void
block_rw_check (int fd, const char *out, char *in, size_t size)
{
  ssize_t n;
  off_t file_size;

  file_size = lseek (fd, 0, SEEK_END);
  rtems_test_assert (file_size == size);

  block_rw_lseek (fd, 0);

  n = read (fd, in, size);
  rtems_test_assert (n == (ssize_t) size);

  rtems_test_assert (memcmp (out, in, size) == 0);
}

static void
block_rw_prepare (const char *t, int fd, char *out, size_t size)
{
  int status;

  printf ("test case: %s\n", t);

  memset (out, 0, size);

  status = ftruncate (fd, 0);
  rtems_test_assert (status == 0);

  block_rw_lseek (fd, 0);
}

static void
block_rw_case_0 (int fd, size_t block_size, char *out, char *in)
{
  const size_t size = 3 * block_size + 1;

  block_rw_prepare (__func__, fd, out, size);
  block_rw_write (fd, out, 0, size);
  block_rw_check (fd, out, in, size);
}

static void
block_rw_case_1 (int fd, size_t block_size, char *out, char *in)
{
  const size_t size = 2 * block_size;

  block_rw_prepare (__func__, fd, out, size);
  block_rw_write (fd, out, block_size, block_size);
  block_rw_check (fd, out, in, size);
}

static void
block_rw_case_2 (int fd, size_t block_size, char *out, char *in)
{
  const size_t size = (5 * block_size) / 2;

  block_rw_prepare (__func__, fd, out, size);
  block_rw_write (fd, out, (3 * block_size) / 2, block_size);
  block_rw_check (fd, out, in, size);
}

static void
block_rw_case_3 (int fd, size_t block_size, char *out, char *in)
{
  const size_t size = 2 * block_size;

  block_rw_prepare (__func__, fd, out, size);
  block_rw_write (fd, out, block_size, block_size / 3);
  block_rw_write (fd, out, 2 * block_size - block_size / 3, block_size / 3);
  block_rw_check (fd, out, in, size);
}

static void
block_rw_case_4 (int fd, size_t block_size, char *out, char *in)
{
  const size_t size = 3 * block_size + 1;
  size_t pos = 0;

  block_rw_prepare (__func__, fd, out, size);
  block_rw_write_cont (fd, out, &pos, block_size);
  block_rw_write_cont (fd, out, &pos, block_size / 2);
  block_rw_write_cont (fd, out, &pos, block_size);
  block_rw_write_cont (fd, out, &pos, block_size / 2);
  block_rw_write_cont (fd, out, &pos, 1);
  block_rw_check (fd, out, in, size);
}

static void
block_read_and_write (void)
{
  int fd;
  struct stat st;
  int status;
  size_t block_size;
  size_t size;
  char *out;
  char *in;

  test_case_enter (__func__);

  fd = open ("file", O_RDWR | O_CREAT | O_TRUNC, mode);
  rtems_test_assert (fd >= 0);

  status = fstat (fd, &st);
  rtems_test_assert (status == 0);

  block_size = st.st_blksize;
  size = 3 * block_size + 1;

  out = malloc (size);
  rtems_test_assert (out != NULL);

  in = malloc (size);
  rtems_test_assert (in != NULL);

  block_rw_case_0 (fd, block_size, out, in);
  block_rw_case_1 (fd, block_size, out, in);
  block_rw_case_2 (fd, block_size, out, in);
  block_rw_case_3 (fd, block_size, out, in);
  block_rw_case_4 (fd, block_size, out, in);

  status = close (fd);
  rtems_test_assert (status == 0);

  free (out);
  free (in);

  test_case_leave ();
}

void
test (void)
{
  read_write_test ();
  lseek_test ();
  truncate_test03 ();
  truncate_to_zero ();
  block_read_and_write ();
}
