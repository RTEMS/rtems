/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief MD5 Shell Command Implementation
 */

/*
 * COPYRIGHT (c) 2014.
 * Chris Johns (chrisj@rtems.org)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  .name = "md5",
  .usage = "md5 [file ...]",
  .topic = "files",
  .command = rtems_shell_main_md5,
  .alias = NULL,
  .next = NULL
};
