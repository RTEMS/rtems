/*
 * This file was copied from newlib-1.8.2/newlib/unix/ttyname.c
 * and transformed into ttyname_r().
 *
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *
 *  $Id$
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)ttyname.c	5.10 (Berkeley) 5/6/91";
#endif /* LIBC_SCCS and not lint */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <termios.h>
#include <unistd.h>
#include <paths.h>
#include <_syslist.h>
#include <errno.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 *  ttyname_r() - POSIX 1003.1b 4.7.2 - Demetermine Terminal Device Name
 */

int ttyname_r(
  int   fd,
  char *name,
  int   namesize
)
{
  struct stat sb;
  struct termios tty;
  struct dirent *dirp;
  DIR *dp;
  struct stat dsb;
  char *rval;

  /* Must be a terminal. */
  if (tcgetattr (fd, &tty) < 0)
    rtems_set_errno_and_return_minus_one(EBADF);

  /* Must be a character device. */
  if (_fstat (fd, &sb) || !S_ISCHR (sb.st_mode))
    rtems_set_errno_and_return_minus_one(EBADF);

  if ((dp = opendir (_PATH_DEV)) == NULL)
    rtems_set_errno_and_return_minus_one(EBADF);

  for (rval = NULL; (dirp = readdir (dp)) != NULL ;)
    {
      if (dirp->d_ino != sb.st_ino)
	continue;
      strcpy (name + sizeof (_PATH_DEV) - 1, dirp->d_name);
      if (stat (name, &dsb) || sb.st_dev != dsb.st_dev ||
	  sb.st_ino != dsb.st_ino)
	continue;
      (void) closedir (dp);
      rval = name;
      break;
    }
  (void) closedir (dp);
  return 0;
}

static char buf[sizeof (_PATH_DEV) + MAXNAMLEN] = _PATH_DEV;

/*
 *  ttyname() - POSIX 1003.1b 4.7.2 - Demetermine Terminal Device Name
 */

char *ttyname(
  int fd
)
{
  if ( !ttyname_r( fd, buf, sizeof(buf) ) )
    return buf;
  return NULL;
}

