/* $NetBSD: ln.c,v 1.34 2008/07/20 00:52:40 lukem Exp $ */

/*
 * Copyright (c) 1987, 1993, 1994
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
 * 3. Neither the name of the University nor the names of its contributors
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if 0
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1987, 1993, 1994\
 The Regents of the University of California.  All rights reserved.");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)ln.c	8.2 (Berkeley) 3/31/94";
#else
__RCSID("$NetBSD: ln.c,v 1.34 2008/07/20 00:52:40 lukem Exp $");
#endif
#endif /* not lint */
#endif

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#define __need_getopt_newlib
#include <getopt.h>

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <setjmp.h>

typedef struct {
  int	fflag;				/* Unlink existing files. */
  int	hflag;				/* Check new name for symlink first. */
  int	iflag;				/* Interactive mode. */
  int	sflag;				/* Symbolic, not hard, link. */
  int	vflag;                          /* Verbose output */
  char   linkch;

  int exit_code;
  jmp_buf exit_jmp;
} rtems_shell_ln_globals;

#define fflag  globals->fflag
#define hflag  globals->hflag
#define iflag  globals->iflag
#define sflag  globals->sflag
#define vflag  globals->vflag
#define linkch globals->linkch

#define exit_jump &(globals->exit_jmp)

#define exit(ec) rtems_shell_ln_exit(globals, ec)

static int	main_ln(rtems_shell_ln_globals* , int, char *[]);

static void
rtems_shell_ln_exit (rtems_shell_ln_globals* globals, int code)
{
  globals->exit_code = code;
  longjmp (globals->exit_jmp, 1);
}

static int main_ln(rtems_shell_ln_globals* globals, int argc, char *argv[]);

static int rtems_shell_main_ln(int argc, char *argv[])
{
  rtems_shell_ln_globals  ln_globals;
  rtems_shell_ln_globals* globals = &ln_globals;
  memset (globals, 0, sizeof (ln_globals));
  ln_globals.exit_code = 1;
  if (setjmp (ln_globals.exit_jmp) == 0)
    return main_ln (globals, argc, argv);
  return ln_globals.exit_code;
}

#if RTEMS_REMOVED
int	fflag;				/* Unlink existing files. */
int	hflag;				/* Check new name for symlink first. */
int	iflag;				/* Interactive mode. */
int	sflag;				/* Symbolic, not hard, link. */
int	vflag;                          /* Verbose output */

					/* System link call. */
int (*linkf)(const char *, const char *);
char   linkch;
#endif

int (*linkf)(const char *, const char *);

static int	linkit(rtems_shell_ln_globals* , const char *, const char *, int);
static void	usage(rtems_shell_ln_globals* );

static int
main_ln(rtems_shell_ln_globals* globals, int argc, char *argv[])
{
	struct stat sb;
	int ch, exitval;
	char *sourcedir;

	struct getopt_data getopt_reent;
	memset(&getopt_reent, 0, sizeof(getopt_data));

#if RTEMS_REMOVED
	setprogname(argv[0]);
	(void)setlocale(LC_ALL, "");
#endif

	while ((ch = getopt_r(argc, argv, "fhinsv", &getopt_reent)) != -1)
		switch (ch) {
		case 'f':
			fflag = 1;
			iflag = 0;
			break;
		case 'h':
		case 'n':
			hflag = 1;
			break;
		case 'i':
			iflag = 1;
			fflag = 0;
			break;
		case 's':
			sflag = 1;
			break;
		case 'v':
			vflag = 1;
			break;
		case '?':
		default:
			usage(globals);
			/* NOTREACHED */
		}

	argv += getopt_reent.optind;
	argc -= getopt_reent.optind;

	if (sflag) {
		linkf  = symlink;
		linkch = '-';
	} else {
		linkf  = link;
		linkch = '=';
	}

	switch(argc) {
	case 0:
		usage(globals);
		/* NOTREACHED */
	case 1:				/* ln target */
		exit(linkit(globals, argv[0], ".", 1));
		/* NOTREACHED */
	case 2:				/* ln target source */
		exit(linkit(globals, argv[0], argv[1], 0));
		/* NOTREACHED */
	}

					/* ln target1 target2 directory */
	sourcedir = argv[argc - 1];
	if (hflag && lstat(sourcedir, &sb) == 0 && S_ISLNK(sb.st_mode)) {
		/* we were asked not to follow symlinks, but found one at
		   the target--simulate "not a directory" error */
		errno = ENOTDIR;
		err(exit_jump, EXIT_FAILURE, "%s", sourcedir);
		/* NOTREACHED */
	}
	if (stat(sourcedir, &sb)) {
		err(exit_jump, EXIT_FAILURE, "%s", sourcedir);
		/* NOTREACHED */
	}
	if (!S_ISDIR(sb.st_mode)) {
		usage(globals);
		/* NOTREACHED */
	}
	for (exitval = 0; *argv != sourcedir; ++argv)
		exitval |= linkit(globals, *argv, sourcedir, 1);
	exit(exitval);
	/* NOTREACHED */
  return 0;
}

int
linkit(rtems_shell_ln_globals* globals, const char *source, const char *target, int isdir)
{
	struct stat sb;
	const char *p;
	char path[MAXPATHLEN];
	int ch, exists, first;

	if (!sflag) {
		/* If target doesn't exist, quit now. */
		if (stat(target, &sb)) {
			warn("%s", target);
			return (1);
		}
	}

	/* If the source is a directory (and not a symlink if hflag),
	   append the target's name. */
	if (isdir ||
	    (!lstat(source, &sb) && S_ISDIR(sb.st_mode)) ||
	    (!hflag && !stat(source, &sb) && S_ISDIR(sb.st_mode))) {
		if ((p = strrchr(target, '/')) == NULL)
			p = target;
		else
			++p;
		(void)snprintf(path, sizeof(path), "%s/%s", source, p);
		source = path;
	}

	exists = !lstat(source, &sb);

	/*
	 * If the file exists, then unlink it forcibly if -f was specified
	 * and interactively if -i was specified.
	 */
	if (fflag && exists) {
		if (unlink(source)) {
			warn("%s", source);
			return (1);
		}
	} else if (iflag && exists) {
		fflush(stdout);
		(void)fprintf(stderr, "replace %s? ", source);

		first = ch = getchar();
		while (ch != '\n' && ch != EOF)
			ch = getchar();
		if (first != 'y' && first != 'Y') {
			(void)fprintf(stderr, "not replaced\n");
			return (1);
		}

		if (unlink(source)) {
			warn("%s", source);
			return (1);
		}
	}

	/* Attempt the link. */
	if ((*linkf)(target, source)) {
		warn("%s", source);
		return (1);
	}
	if (vflag)
		(void)printf("%s %c> %s\n", source, linkch, target);

	return (0);
}

void
usage(rtems_shell_ln_globals* globals)
{
#define getprogname() "ln"
	(void)fprintf(stderr,
	    "usage:\t%s [-fhinsv] file1 file2\n\t%s [-fhinsv] file ... directory\n",
	    getprogname(), getprogname());
	exit(1);
	/* NOTREACHED */
}

rtems_shell_cmd_t rtems_shell_LN_Command = {
  "ln",                                         /* name */
  "ln ln [-fhinsv] source_file [target_file]",  /* usage */
  "files",                                      /* topic */
  rtems_shell_main_ln,                          /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
