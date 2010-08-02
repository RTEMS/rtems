/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief rtems_mkdir() implementation.
 *
 * The implementation is based on FreeBSD 'bin/mkdir/mkdir.c' revision 163213.
 */

/*-
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c) 1983, 1992, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems/libio.h>

/*
 * Returns 1 if a directory has been created,
 * 2 if it already existed, and 0 on failure.
 */
static int
build(char *path, mode_t omode)
{
  struct stat sb;
  mode_t numask, oumask;
  int first, last, retval;
  char *p;

  p = path;
  oumask = 0;
  retval = 1;
  if (p[0] == '/')    /* Skip leading '/'. */
    ++p;
  for (first = 1, last = 0; !last ; ++p) {
    if (p[0] == '\0')
      last = 1;
    else if (p[0] != '/')
      continue;
    *p = '\0';
    if (!last && p[1] == '\0')
      last = 1;
    if (first) {
      /*
       * POSIX 1003.2:
       * For each dir operand that does not name an existing
       * directory, effects equivalent to those caused by the
       * following command shall occcur:
       *
       * mkdir -p -m $(umask -S),u+wx $(dirname dir) &&
       *    mkdir [-m mode] dir
       *
       * We change the user's umask and then restore it,
       * instead of doing chmod's.
       */
      oumask = umask(0);
      numask = oumask & ~(S_IWUSR | S_IXUSR);
      (void)umask(numask);
      first = 0;
    }
    if (last)
      (void)umask(oumask);
    if (mkdir(path, last ? omode : S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
      if (errno == EEXIST || errno == EISDIR) {
        if (stat(path, &sb) < 0) {
          retval = 0;
          break;
        } else if (!S_ISDIR(sb.st_mode)) {
          if (last)
            errno = EEXIST;
          else
            errno = ENOTDIR;
          retval = 0;
          break;
        }
        if (last)
          retval = 2;
      } else {
        retval = 0;
        break;
      }
    }
    if (!last)
        *p = '/';
  }
  if (!first && !last)
    (void)umask(oumask);
  return (retval);
}

int
rtems_mkdir(const char *path, mode_t mode)
{
  int success = 0;
  char *dup_path = strdup(path);

  if (dup_path != NULL) {
    success = build(dup_path, mode);
    free(dup_path);
  }

  return success != 0 ? 0 : -1;
}
