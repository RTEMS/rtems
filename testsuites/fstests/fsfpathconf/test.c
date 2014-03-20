/*
 *  COPYRIGHT (c) 2012 - .
 *  Krzysztof Miesowicz <krzysztof.miesowicz@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <rtems.h>
#include <fcntl.h>
#include <inttypes.h>
#include <rtems/error.h>
#include <ctype.h>
#include <rtems/libcsupport.h>

#include "fstest.h"
#include "fs_config.h"
#include "tmacros.h"

const char rtems_test_name[] = "FSFPATHCONF " FILESYSTEM;

static void fpathconf_test(void){

  int rv = 0;
  const char *fname = "testfile.km";
  int fd  = -1;

/* attempt to invoke fpathconf on non-existing file */
  rv = fpathconf(fd, _PC_LINK_MAX);
  printf("\nfpathconf of non-existing file give %d , expected -1",rv);

  if (rv == -1) {
    printf("\n... creating file \"%s\"\n",fname);
    fd = open(fname,O_WRONLY | O_CREAT | O_TRUNC,S_IREAD|S_IWRITE);

    if (fd < 0) {
      printf("*** file create failed, errno = %d(%s)\n",errno,strerror(errno));
    }else
      printf("*** file created succesfully\n");

/* invoking fpathconf with request for every possible informations */
    rv = fpathconf(fd, _PC_LINK_MAX);
    printf("\nrequest with _PC_LINK_MAX return : %d",rv);
    rv = fpathconf(fd, _PC_MAX_CANON);
    printf("\nrequest with _PC_MAX_CANON return : %d",rv);
    rv = fpathconf(fd, _PC_MAX_INPUT);
    printf("\nrequest with _PC_MAX_INPUT return : %d",rv);
    rv = fpathconf(fd, _PC_NAME_MAX);
    printf("\nrequest with _PC_NAME_MAX return : %d",rv);
    rv = fpathconf(fd, _PC_PATH_MAX);
    printf("\nrequest with _PC_PATH_MAX return : %d",rv);
    rv = fpathconf(fd, _PC_PIPE_BUF);
    printf("\nrequest with _PC_PIPE_BUF return : %d",rv);
    rv = fpathconf(fd, _PC_CHOWN_RESTRICTED);
    printf("\nrequest with _PC_CHOWN_RESTRICTED return : %d",rv);
    rv = fpathconf(fd, _PC_NO_TRUNC);
    printf("\nrequest with _PC_NO_TRUNC return : %d",rv);
    rv = fpathconf(fd, _PC_VDISABLE);
    printf("\nrequest with _PC_VDISABLE return : %d",rv);
    rv = fpathconf(fd, _PC_ASYNC_IO);
    printf("\nrequest with _PC_ASYNC_IO return : %d",rv);
    rv = fpathconf(fd, _PC_PRIO_IO);
    printf("\nrequest with _PC_PRIO_IO return : %d",rv);
    rv = fpathconf(fd, _PC_SYNC_IO);
    printf("\nrequest with _PC_SYNC_IO return : %d",rv);
/* invoking fpathconf with bad information requested - 255 */
    rv = fpathconf(fd, 255);
    printf("\nrequest with bad argument return : %d",rv);

    close(fd);
    fd = open("testfile.test", O_WRONLY);

    rv = fpathconf(fd, _PC_LINK_MAX);
  }
}

void test(void){

  fpathconf_test();
}

/* end of file */
