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

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#include "fstest.h"
#include "pmacros.h"

/*
 * Test if the successful call works as expect
 */
static void link_test01 (void)
{
  char *name0 = "t0";
  char *name1 = "t1";
  char *name2 = "t2";


  int status;
  int fd;

  mode_t mode = 0644;
  struct stat statbuf;


  puts ("link creates hardlinks");
  fd = creat (name0, mode);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = stat (name0, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (statbuf.st_nlink == 1);

  puts ("test if the stat is the same");
  status = link (name0, name1);
  rtems_test_assert (status == 0);

  status = stat (name0, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 2);

  status = stat (name1, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 2);

  /*
   * link the file and check the nlink
   */
  status = link (name1, name2);
  rtems_test_assert (status == 0);

  status = stat (name0, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 3);

  status = stat (name1, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 3);

  status = stat (name2, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 3);

  /*
   *  call chmod and chown and test.
   */
  puts ("chmod and chown");

  chown (name1, 65534, 65533);

  status = stat (name0, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 3);
  rtems_test_assert (statbuf.st_uid = 65534);
  rtems_test_assert (statbuf.st_gid = 65533);

  status = stat (name1, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 3);
  rtems_test_assert (statbuf.st_uid = 65534);
  rtems_test_assert (statbuf.st_gid = 65533);

  status = stat (name2, &statbuf);
  rtems_test_assert (status == 0);

  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 3);
  rtems_test_assert (statbuf.st_uid = 65534);
  rtems_test_assert (statbuf.st_gid = 65533);

  /*
   *
   *  unlink then test if the nlink changes
   */
  puts ("unlink then stat the file ");

  status = unlink (name0);
  rtems_test_assert (status == 0);

  status = stat (name0, &statbuf);
  rtems_test_assert (status == -1);
  rtems_test_assert (errno = ENOENT);

  status = stat (name1, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 2);

  status = stat (name2, &statbuf);
  rtems_test_assert (status == 0);
  rtems_test_assert (S_ISREG (statbuf.st_mode));
  rtems_test_assert (statbuf.st_nlink == 2);

  status = unlink (name1);
  rtems_test_assert (status == 0);

  status = unlink (name2);
  rtems_test_assert (status == 0);

}

void test (void)
{
  puts ("\n\n*** LINK TEST ***");
  link_test01 ();
  puts ("*** END OF LINK TEST ***");
}
