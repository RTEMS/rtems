/* $NetBSD: cp.c,v 1.39 2005/10/24 12:59:07 kleink Exp $ */

/*
 * Copyright (c) 1988, 1993, 1994
 *  The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * David Hitz of Auspex Systems Inc.
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
__COPYRIGHT(
"@(#) Copyright (c) 1988, 1993, 1994\n\
  The Regents of the University of California.  All rights reserved.\n");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)cp.c  8.5 (Berkeley) 4/29/95";
#else
__RCSID("$NetBSD: cp.c,v 1.39 2005/10/24 12:59:07 kleink Exp $");
#endif
#endif /* not lint */
#endif
/*
 * Cp copies source files to target files.
 *
 * The global PATH_T structure "to" always contains the path to the
 * current target file.  Since fts(3) does not change directories,
 * this path can be either absolute or dot-relative.
 *
 * The basic algorithm is to initialize "to" and use fts(3) to traverse
 * the file hierarchy rooted in the argument list.  A trivial case is the
 * case of 'cp file1 file2'.  The more interesting case is the case of
 * 'cp file1 file2 ... fileN dir' where the hierarchy is traversed and the
 * path (relative to the root of the traversal) is appended to dir (stored
 * in "to") to form the final target path.
 */

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#define __need_getopt_newlib
#include <getopt.h>

#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fts.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"

#include "extern-cp.h"

#define S_ISTXT 0

#define	STRIP_TRAILING_SLASH(p) {					\
        while ((p).p_end > (p).p_path + 1 && (p).p_end[-1] == '/')	\
                *--(p).p_end = 0;					\
}

enum op { FILE_TO_FILE, FILE_TO_DIR, DIR_TO_DNE };

static int Rflag, rflag;

static int main_cp(rtems_shell_cp_globals* cp_globals, int, char *[]);
static int copy(rtems_shell_cp_globals* cp_globals, char *[], enum op, int);
static int mastercmp(const FTSENT **, const FTSENT **);

static void
rtems_shell_cp_exit (rtems_shell_cp_globals* cp_global, int code __attribute__((unused)))
{
  longjmp (cp_global->exit_jmp, 1);
}

int
rtems_shell_main_cp(int argc, char *argv[])
{
  rtems_shell_cp_globals cp_globals;
  memset (&cp_globals, 0, sizeof (cp_globals));
  Rflag = rflag = 0;
  if (setjmp (cp_globals.exit_jmp) == 0)
    return main_cp (&cp_globals, argc, argv);
  return 1;
}

int
main_cp(rtems_shell_cp_globals* cp_globals, int argc, char *argv[])
{
	struct stat to_stat, tmp_stat;
	enum op type;
	int Hflag, Lflag, Pflag, ch, fts_options, r, have_trailing_slash;
	char *target;
  struct getopt_data getopt_reent;

	Hflag = Lflag = Pflag = 0;
  memset(&getopt_reent, 0, sizeof(getopt_data));

	while ((ch = getopt_r(argc, argv, "HLPRafilnprv", &getopt_reent)) != -1)
		switch (ch) {
		case 'H':
			Hflag = 1;
			Lflag = Pflag = 0;
			break;
		case 'L':
			Lflag = 1;
			Hflag = Pflag = 0;
			break;
		case 'P':
			Pflag = 1;
			Hflag = Lflag = 0;
			break;
		case 'R':
			Rflag = 1;
			break;
		case 'a':
			Pflag = 1;
			pflag = 1;
			Rflag = 1;
			Hflag = Lflag = 0;
			break;
		case 'f':
			fflag = 1;
			iflag = nflag = 0;
			break;
		case 'i':
			iflag = 1;
			fflag = nflag = 0;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'n':
			nflag = 1;
			fflag = iflag = 0;
			break;
		case 'p':
			pflag = 1;
			break;
		case 'r':
			rflag = Lflag = 1;
			Hflag = Pflag = 0;
			break;
		case 'v':
			vflag = 1;
			break;
		default:
			usage(cp_globals);
			break;
		}
	argc -= getopt_reent.optind;
	argv += getopt_reent.optind;

	if (argc < 2)
		usage(cp_globals);

	fts_options = FTS_NOCHDIR | FTS_PHYSICAL;
	if (Rflag && rflag)
		errx(exit_jump, 1, "the -R and -r options may not be specified together");
	if (rflag)
		Rflag = 1;
	if (Rflag) {
		if (Hflag)
			fts_options |= FTS_COMFOLLOW;
		if (Lflag) {
			fts_options &= ~FTS_PHYSICAL;
			fts_options |= FTS_LOGICAL;
		}
	} else {
		fts_options &= ~FTS_PHYSICAL;
		fts_options |= FTS_LOGICAL | FTS_COMFOLLOW;
	}
#if 0
	(void)signal(SIGINFO, siginfo);
#endif

	/* Save the target base in "to". */
	target = argv[--argc];
	if (strlcpy(to.p_path, target, sizeof(to.p_path)) >= sizeof(to.p_path))
		errx(exit_jump, 1, "%s: name too long", target);
	to.p_end = to.p_path + strlen(to.p_path);
        if (to.p_path == to.p_end) {
		*to.p_end++ = '.';
		*to.p_end = 0;
	}
	have_trailing_slash = (to.p_end[-1] == '/');
	if (have_trailing_slash)
		STRIP_TRAILING_SLASH(to);
	to.target_end = to.p_end;

	/* Set end of argument list for fts(3). */
	argv[argc] = NULL;

	/*
	 * Cp has two distinct cases:
	 *
	 * cp [-R] source target
	 * cp [-R] source1 ... sourceN directory
	 *
	 * In both cases, source can be either a file or a directory.
	 *
	 * In (1), the target becomes a copy of the source. That is, if the
	 * source is a file, the target will be a file, and likewise for
	 * directories.
	 *
	 * In (2), the real target is not directory, but "directory/source".
	 */
	r = stat(to.p_path, &to_stat);
	if (r == -1 && errno != ENOENT)
		err(exit_jump, 1, "%s", to.p_path);
	if (r == -1 || !S_ISDIR(to_stat.st_mode)) {
		/*
		 * Case (1).  Target is not a directory.
		 */
		if (argc > 1)
			errx(exit_jump, 1, "%s is not a directory", to.p_path);

		/*
		 * Need to detect the case:
		 *	cp -R dir foo
		 * Where dir is a directory and foo does not exist, where
		 * we want pathname concatenations turned on but not for
		 * the initial mkdir().
		 */
		if (r == -1) {
			if (Rflag && (Lflag || Hflag))
				stat(*argv, &tmp_stat);
			else
				lstat(*argv, &tmp_stat);

			if (S_ISDIR(tmp_stat.st_mode) && Rflag)
				type = DIR_TO_DNE;
			else
				type = FILE_TO_FILE;
		} else
			type = FILE_TO_FILE;

		if (have_trailing_slash && type == FILE_TO_FILE) {
			if (r == -1)
				errx(exit_jump, 1, "directory %s does not exist",
				     to.p_path);
			else
				errx(exit_jump, 1, "%s is not a directory", to.p_path);
		}
	} else
		/*
		 * Case (2).  Target is a directory.
		 */
		type = FILE_TO_DIR;

  return copy(cp_globals, argv, type, fts_options);
}

int
copy(rtems_shell_cp_globals* cp_globals,
     char *argv[], enum op type, int fts_options)
{
	struct stat to_stat;
	FTS *ftsp;
	FTSENT *curr;
	int base = 0, dne, badcp, rval;
	size_t nlen;
	char *p, *target_mid;
	mode_t mask, mode;

	/*
	 * Keep an inverted copy of the umask, for use in correcting
	 * permissions on created directories when not using -p.
	 */
	mask = ~umask(0777);
	umask(~mask);

	if ((ftsp = fts_open(argv, fts_options, mastercmp)) == NULL)
		err(exit_jump, 1, "fts_open");
	for (badcp = rval = 0; (curr = fts_read(ftsp)) != NULL; badcp = 0) {
		switch (curr->fts_info) {
		case FTS_NS:
		case FTS_DNR:
		case FTS_ERR:
			warnx("%s: %s",
			    curr->fts_path, strerror(curr->fts_errno));
			badcp = rval = 1;
			continue;
		case FTS_DC:			/* Warn, continue. */
			warnx("%s: directory causes a cycle", curr->fts_path);
			badcp = rval = 1;
			continue;
		default:
			;
		}

		/*
		 * If we are in case (2) or (3) above, we need to append the
                 * source name to the target name.
                 */
		if (type != FILE_TO_FILE) {
			/*
			 * Need to remember the roots of traversals to create
			 * correct pathnames.  If there's a directory being
			 * copied to a non-existent directory, e.g.
			 *	cp -R a/dir noexist
			 * the resulting path name should be noexist/foo, not
			 * noexist/dir/foo (where foo is a file in dir), which
			 * is the case where the target exists.
			 *
			 * Also, check for "..".  This is for correct path
			 * concatenation for paths ending in "..", e.g.
			 *	cp -R .. /tmp
			 * Paths ending in ".." are changed to ".".  This is
			 * tricky, but seems the easiest way to fix the problem.
			 *
			 * XXX
			 * Since the first level MUST be FTS_ROOTLEVEL, base
			 * is always initialized.
			 */
			if (curr->fts_level == FTS_ROOTLEVEL) {
				if (type != DIR_TO_DNE) {
					p = strrchr(curr->fts_path, '/');
					base = (p == NULL) ? 0 :
					    (int)(p - curr->fts_path + 1);

					if (!strcmp(&curr->fts_path[base],
					    ".."))
						base += 1;
				} else
					base = curr->fts_pathlen;
			}

			p = &curr->fts_path[base];
			nlen = curr->fts_pathlen - base;
			target_mid = to.target_end;
			if (*p != '/' && target_mid[-1] != '/')
				*target_mid++ = '/';
			*target_mid = 0;
			if (target_mid - to.p_path + nlen >= PATH_MAX) {
				warnx("%s%s: name too long (not copied)",
				    to.p_path, p);
				badcp = rval = 1;
				continue;
			}
			(void)strncat(target_mid, p, nlen);
			to.p_end = target_mid + nlen;
			*to.p_end = 0;
			STRIP_TRAILING_SLASH(to);
		}

		if (curr->fts_info == FTS_DP) {
			/*
			 * We are nearly finished with this directory.  If we
			 * didn't actually copy it, or otherwise don't need to
			 * change its attributes, then we are done.
			 */
			if (!curr->fts_number)
				continue;
			/*
			 * If -p is in effect, set all the attributes.
			 * Otherwise, set the correct permissions, limited
			 * by the umask.  Optimise by avoiding a chmod()
			 * if possible (which is usually the case if we
			 * made the directory).  Note that mkdir() does not
			 * honour setuid, setgid and sticky bits, but we
			 * normally want to preserve them on directories.
			 */
			if (pflag) {
				if (setfile(cp_globals, curr->fts_statp, -1))
					rval = 1;
				if (preserve_dir_acls(curr->fts_statp,
				    curr->fts_accpath, to.p_path) != 0)
					rval = 1;
			} else {
				mode = curr->fts_statp->st_mode;
				if ((mode & (S_ISUID | S_ISGID | S_ISTXT)) ||
				    ((mode | S_IRWXU) & mask) != (mode & mask))
					if (chmod(to.p_path, mode & mask) != 0){
						warn("chmod: %s", to.p_path);
						rval = 1;
					}
			}
			continue;
		}

		/* Not an error but need to remember it happened */
		if (stat(to.p_path, &to_stat) == -1)
			dne = 1;
		else {
			if (to_stat.st_dev == curr->fts_statp->st_dev &&
			    to_stat.st_ino == curr->fts_statp->st_ino) {
				warnx("%s and %s are identical (not copied).",
				    to.p_path, curr->fts_path);
				badcp = rval = 1;
				if (S_ISDIR(curr->fts_statp->st_mode))
					(void)fts_set(ftsp, curr, FTS_SKIP);
				continue;
			}
			if (!S_ISDIR(curr->fts_statp->st_mode) &&
			    S_ISDIR(to_stat.st_mode)) {
				warnx("cannot overwrite directory %s with "
				    "non-directory %s",
				    to.p_path, curr->fts_path);
				badcp = rval = 1;
				continue;
			}
			dne = 0;
		}

		switch (curr->fts_statp->st_mode & S_IFMT) {
		case S_IFLNK:
			/* Catch special case of a non-dangling symlink */
			if ((fts_options & FTS_LOGICAL) ||
			    ((fts_options & FTS_COMFOLLOW) &&
			    curr->fts_level == 0)) {
				if (copy_file(cp_globals, curr, dne))
					badcp = rval = 1;
			} else {
				if (copy_link(cp_globals, curr, !dne))
					badcp = rval = 1;
			}
			break;
		case S_IFDIR:
			if (!Rflag) {
				warnx("%s is a directory (not copied).",
				    curr->fts_path);
				(void)fts_set(ftsp, curr, FTS_SKIP);
				badcp = rval = 1;
				break;
			}
			/*
			 * If the directory doesn't exist, create the new
			 * one with the from file mode plus owner RWX bits,
			 * modified by the umask.  Trade-off between being
			 * able to write the directory (if from directory is
			 * 555) and not causing a permissions race.  If the
			 * umask blocks owner writes, we fail..
			 */
			if (dne) {
				if (mkdir(to.p_path,
				    curr->fts_statp->st_mode | S_IRWXU) < 0)
					err(exit_jump, 1, "%s", to.p_path);
			} else if (!S_ISDIR(to_stat.st_mode)) {
				errno = ENOTDIR;
				err(exit_jump, 1, "%s", to.p_path);
			}
			/*
			 * Arrange to correct directory attributes later
			 * (in the post-order phase) if this is a new
			 * directory, or if the -p flag is in effect.
			 */
			curr->fts_number = pflag || dne;
			break;
		case S_IFBLK:
		case S_IFCHR:
			if (Rflag) {
				if (copy_special(cp_globals, curr->fts_statp, !dne))
					badcp = rval = 1;
			} else {
				if (copy_file(cp_globals, curr, dne))
					badcp = rval = 1;
			}
			break;
		case S_IFSOCK:
			warnx("%s is a socket (not copied).",
				    curr->fts_path);
		case S_IFIFO:
			if (Rflag) {
				if (copy_fifo(cp_globals, curr->fts_statp, !dne))
					badcp = rval = 1;
			} else {
				if (copy_file(cp_globals, curr, dne))
					badcp = rval = 1;
			}
			break;
		default:
			if (copy_file(cp_globals, curr, dne))
				badcp = rval = 1;
			break;
		}
		if (vflag && !badcp)
			(void)printf("%s -> %s\n", curr->fts_path, to.p_path);
	}
	if (errno)
		err(exit_jump, 1, "fts_read");
	fts_close(ftsp);
	return (rval);
}

/*
 * mastercmp --
 *	The comparison function for the copy order.  The order is to copy
 *	non-directory files before directory files.  The reason for this
 *	is because files tend to be in the same cylinder group as their
 *	parent directory, whereas directories tend not to be.  Copying the
 *	files first reduces seeking.
 */
int
mastercmp(const FTSENT **a, const FTSENT **b)
{
	int a_info, b_info;

	a_info = (*a)->fts_info;
	if (a_info == FTS_ERR || a_info == FTS_NS || a_info == FTS_DNR)
		return (0);
	b_info = (*b)->fts_info;
	if (b_info == FTS_ERR || b_info == FTS_NS || b_info == FTS_DNR)
		return (0);
	if (a_info == FTS_D)
		return (-1);
	if (b_info == FTS_D)
		return (1);
	return (0);
}

rtems_shell_cmd_t rtems_shell_CP_Command = {
  "cp",                                                /* name */
  "cp [-R [-H | -L | -P]] [-f | -i] [-pv] src target", /* usage */
  "files",                                             /* topic */
  rtems_shell_main_cp,                                 /* command */
  NULL,                                                /* alias */
  NULL                                                 /* next */
};

