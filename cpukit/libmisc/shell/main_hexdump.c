/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1989, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)hexdump.c	8.1 (Berkeley) 6/6/93";
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/usr.bin/hexdump/hexdump.c,v 1.7 2002/09/04 23:29:01 dwmalone Exp $");
#endif
#endif /* not lint */

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>

#include <sys/types.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define rindex(s,c)	strrchr(s,c)

#include "hexdump.h"

#if RTEMS_REMOVED
FS *fshead;				/* head of format strings */
int blocksize;				/* data block size */
int exitval;				/* final exit value */
int length = -1;			/* max bytes to read */
#endif

void
rtems_shell_hexdump_exit (rtems_shell_hexdump_globals* globals, int code)
{
  globals->exit_code = code;
  longjmp (globals->exit_jmp, 1);
}

static int main_hexdump(rtems_shell_hexdump_globals* globals, int argc, char *argv[]);

static int
rtems_shell_main_hexdump(int argc, char *argv[])
{
  rtems_shell_hexdump_globals  hexdump_globals;
  rtems_shell_hexdump_globals* globals = &hexdump_globals;
  memset (globals, 0, sizeof (hexdump_globals));
  vflag = FIRST;
  ateof = 1;
  hexdump_globals.exit_code = 1;
  if (setjmp (hexdump_globals.exit_jmp) == 0)
    hexdump_globals.exit_code = main_hexdump (globals, argc, argv);
  if (curp)
    free (curp);
  if (savp)
    free (savp);
  while (fshead)
  {
    FS* nextfs = fshead->nextfs;
    while (fshead->nextfu)
    {
      FU* nextfu = fshead->nextfu->nextfu;
      if (fshead->nextfu->fmt)
        free(fshead->nextfu->fmt);
      while (fshead->nextfu->nextpr)
      {
        PR* nextpr = fshead->nextfu->nextpr->nextpr;
        if (((fshead->nextfu->nextpr->flags & F_TEXT) == 0) &&
            fshead->nextfu->nextpr->fmt)
          free(fshead->nextfu->nextpr->fmt);
        free(fshead->nextfu->nextpr);
        fshead->nextfu->nextpr = nextpr;
      }
      free(fshead->nextfu);
      fshead->nextfu = nextfu;
    }
    free(fshead);
    fshead = nextfs;
  }
  if (hdstdin)
  {
    fclose (hdstdin);
    free (hdstdin);
  }
  return hexdump_globals.exit_code;
}

int
main_hexdump(rtems_shell_hexdump_globals* globals, int argc, char *argv[])
{
	FS *tfs;
	char *p;

#if RTEMS_REMOVED
	(void)setlocale(LC_ALL, "");
#endif

	if (!(p = rindex(argv[0], 'o')) || strcmp(p, "od"))
		newsyntax(globals, argc, &argv);
	else
		oldsyntax(globals, argc, &argv);

	/* figure out the data block size */
	for (blocksize = 0, tfs = fshead; tfs; tfs = tfs->nextfs) {
		tfs->bcnt = size(globals, tfs);
		if (blocksize < tfs->bcnt)
			blocksize = tfs->bcnt;
	}
	/* rewrite the rules, do syntax checking */
	for (tfs = fshead; tfs; tfs = tfs->nextfs)
		rewrite(globals, tfs);

	(void)next(globals, argv);
	display(globals);
  exit(exitval);
  return exitval;
}

rtems_shell_cmd_t rtems_shell_HEXDUMP_Command = {
  "hexdump",                                                /* name */
  "hexdump [-bcCdovx] [-e fmt] [-f fmt_file] [-n length]\n" /* usage */
  "        [-s skip] [file ...]",
  "files",                                                  /* topic */
  rtems_shell_main_hexdump,                                 /* command */
  NULL,                                                     /* alias */
  NULL                                                      /* next */
};

