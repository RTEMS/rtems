/*
 *  COPYRIGHT (c) 2015.
 *  On-Line Applications Research Corporation (OAR).
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

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

const char rtems_test_name[] = "FSSCANDIR " FILESYSTEM;

/*
 * This code is from the scandir() man page.
 */
static void test_scandir(void)
{
  struct dirent **namelist;
  int n;

  n = scandir(".", &namelist, 0, NULL);
  if (n < 0) {
    perror("scandir");
  } else {
    while(n--) {
      printf("%s\n", namelist[n]->d_name);
      free(namelist[n]);
    }
    free(namelist);
  }

  rtems_test_assert(MAXNAMLEN == NAME_MAX);
}


void test (void)
{
  test_scandir();
}
