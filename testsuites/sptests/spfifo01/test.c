/*  test_main
 *
 *  This routine serves as a test routine.
 *  Exercises the fifo_open
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

/* Includes */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <tmacros.h>
#include <rtems.h>
#include <rtems/libio.h>


void test_main(void)
{

  int status = -1;
  int fd = 0;

  puts("\n\n*** FIFO / PIPE OPEN TEST - 1 ***");
  puts(
"\n\nConfiguration: Pipes not enabled"
       );

  puts("\n\nCreating directory /tmp");
  status = mkdir("/tmp", 0777);
  rtems_test_assert(status == 0);

  puts("\n\nCreating fifo /tmp/fifo");
  status = mkfifo("/tmp/fifo01", 0777);
  rtems_test_assert(status == 0);

  puts("\n\nAttempt to open the fifo file\n");
  puts(
       "Must result in failure since \
pipes are not enabled in the configuration"
       );

  fd = open("/tmp/fifo01", O_RDONLY);
  rtems_test_assert(fd == -1);
  rtems_test_assert(errno == EINTR); // Should this
                                     // be ENOMEM?
  puts("\n\nRemove the entry /tmp/fifo01");
  status = unlink("/tmp/fifo01");
  rtems_test_assert(status == 0);

  puts("\n\nRemove directory /tmp");
  status = rmdir("/tmp");
  rtems_test_assert(status == 0);

  puts("\n\n*** END OF FIFO / PIPE OPEN TEST - 1 ***");
}
  
