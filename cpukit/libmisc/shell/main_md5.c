/*
 *  MD5 Shell Command Implmentation
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/shell.h>

#include <md5.h>

#define BUFFER_SIZE (4 * 1024)

static int rtems_shell_main_md5(
  int   argc,
  char *argv[])
{
  uint8_t* buffer;
  uint8_t  hash[16];
  size_t   h;

  buffer = malloc(BUFFER_SIZE);
  if (!buffer)
  {
    printf("error: no memory\n");
    return 1;
  }

  --argc;
  ++argv;

  while (argc)
  {
    struct stat sb;
    MD5_CTX     md5;
    int         in;

    if (stat(*argv, &sb) < 0)
    {
        free(buffer);
        printf("error: stat of %s: %s\n", *argv, strerror(errno));
        return 1;
    }

    in = open(*argv, O_RDONLY);
    if (in < 0)
    {
        free(buffer);
        printf("error: opening %s: %s\n", *argv, strerror(errno));
        return 1;
    }

    MD5Init(&md5);

    while (sb.st_size)
    {
      ssize_t size = sb.st_size > BUFFER_SIZE ? BUFFER_SIZE : sb.st_size;

      if (read(in, buffer, size) != size)
      {
        close(in);
        free(buffer);
        printf("error: reading %s: %s\n", *argv, strerror(errno));
        return 1;
      }

      MD5Update(&md5, buffer, size);

      sb.st_size -= size;
    }

    MD5Final(hash, &md5);

    close(in);

    printf("MD5 (%s) = ", *argv);
    for (h = 0; h < sizeof(hash); ++h)
      printf("%02x", (int) hash[h]);
    printf("\n");

    --argc;
    ++argv;
  }

  free(buffer);

  return 0;
}

rtems_shell_cmd_t rtems_shell_MD5_Command = {
  "md5",                 /* name */
  "md5 [file ...]",      /* usage */
  "files",               /* topic */
  rtems_shell_main_md5,  /* command */
  NULL,                  /* alias */
  NULL                   /* next */
};
