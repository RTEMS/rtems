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

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> /* exit */
#include <sys/stat.h>
#include <sys/types.h>

#include "rtems.h"

#include "fstest_support.h"
#include "fs_config.h"

#include "fstest.h"
#include "pmacros.h"

#define TEMP_DIR "waterbuffalo"


/* Break out of a chroot() environment in C */
static void break_out_of_chroot(void)
{

  int dir_fd;       /* File descriptor to directory */
  struct stat sbuf; /* The stat() buffer */
  chdir("/");

  if (stat(TEMP_DIR,&sbuf)<0) {
    if (errno==ENOENT) {
      if (mkdir(TEMP_DIR,0755)<0) {
        fprintf(stderr,"Failed to create %s - %s\n", TEMP_DIR,
            strerror(errno));
        exit(1);
      }
    } else {
      fprintf(stderr,"Failed to stat %s - %s\n", TEMP_DIR,
          strerror(errno));
      exit(1);
    }
  } else if (!S_ISDIR(sbuf.st_mode)) {
    fprintf(stderr,"Error - %s is not a directory!\n",TEMP_DIR);
    exit(1);
  }

  if ((dir_fd=open(".",O_RDONLY))<0) {
    fprintf(stderr,"Failed to open ""."
        " for reading - %s\n", strerror(errno));
    exit(1);
  }

  if (chroot(TEMP_DIR)<0) {
    fprintf(stderr,"Failed to chroot to %s - %s\n",TEMP_DIR,
        strerror(errno));
    exit(1);
  }

  if (fchdir(dir_fd)<0) {
    fprintf(stderr,"Failed to fchdir - %s\n",
        strerror(errno));
    exit(1);
  }
  close(dir_fd);
  chdir("..");
  chroot(".");

}

/*
 *  Main entry point of every filesystem test
 */

rtems_task Init(
    rtems_task_argument ignored)
{
  int rc=0;
  puts( "\n\n*** FILE SYSTEM TEST ( " FILESYSTEM " ) ***" );

  puts( "Initializing filesystem " FILESYSTEM );
  test_initialize_filesystem();
  rc=chdir(BASE_FOR_TEST);
  rtems_test_assert(rc==0);

  rc=chroot(BASE_FOR_TEST);
  rtems_test_assert(rc==0);

  test();

  break_out_of_chroot();
  chdir("/");

  puts( "\n\nShutting down filesystem " FILESYSTEM );
  test_shutdown_filesystem();

  puts( "*** END OF FILE SYSTEM TEST ( " FILESYSTEM " ) ***" );
  rtems_test_exit(0);
}
