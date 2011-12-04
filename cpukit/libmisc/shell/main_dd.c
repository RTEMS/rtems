/*-
 * Copyright (c) 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Keith Muller of the University of California, San Diego and Lance
 * Visser of Convex Computer Corporation.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if 0
#ifndef lint
static char const copyright[] =
"@(#) Copyright (c) 1991, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)dd.c	8.5 (Berkeley) 4/2/94";
#endif /* not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/bin/dd/dd.c,v 1.43 2004/08/15 19:10:05 rwatson Exp $");
#endif

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>

#include <sys/param.h>
#include <sys/stat.h>
#if RTEMS_REMOVED
#include <sys/conf.h>
#include <sys/disklabel.h>
#endif
#include <sys/filio.h>
#include <sys/time.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dd.h"
#include "extern-dd.h"

#ifndef __unused
#define __unused __attribute((__unused__))
#endif

#define DD_DEFFILEMODE 0

static void dd_close(rtems_shell_dd_globals* globals);
static void dd_in(rtems_shell_dd_globals* globals);
static void getfdtype(rtems_shell_dd_globals* globals, IO *);
static void setup(rtems_shell_dd_globals* globals);

#if RTEMS_REMOVED
IO	in, out;		/* input/output state */
STAT	st;			/* statistics */
void	(*cfunc)(void);		/* conversion function */
uintmax_t cpy_cnt;		/* # of blocks to copy */
static off_t	pending = 0;	/* pending seek if sparse */
u_int	ddflags = 0;		/* conversion options */
size_t	cbsz;			/* conversion block size */
uintmax_t files_cnt = 1;	/* # of files to copy */
const	u_char *ctab;		/* conversion table */
char	fill_char;		/* Character to fill with if defined */
#endif

static off_t	pending = 0;	/* pending seek if sparse */

void
rtems_shell_dd_exit (rtems_shell_dd_globals* globals, int code)
{
  globals->exit_code = code;
  longjmp (globals->exit_jmp, 1);
}

static int main_dd(rtems_shell_dd_globals* globals, int argc, char *argv[]);

static int
rtems_shell_main_dd(int argc, char *argv[])
{
  rtems_shell_dd_globals  dd_globals;
  rtems_shell_dd_globals* globals = &dd_globals;
  memset (globals, 0, sizeof (dd_globals));
  pending = 0;
  ddflags = 0;
  files_cnt = 1;
  dd_globals.exit_code = 1;
  if (setjmp (dd_globals.exit_jmp) == 0)
    dd_globals.exit_code = main_dd (globals, argc, argv);
  if (in.fd)
    close(in.fd);
  if (out.fd)
    close(out.fd);
  if (in.name)
    free((void*)in.name);
  if (out.name)
    free((void*)out.name);
  if (in.db)
    free(in.db);
  if (out.db && (in.db != out.db))
    free(out.db);
  return dd_globals.exit_code;
}

int
main_dd(rtems_shell_dd_globals* globals, int argc __unused, char *argv[])
{
	(void)setlocale(LC_CTYPE, "");
	jcl(globals, argv);
	setup(globals);

#if RTEMS_REMOVED
	(void)signal(SIGINFO, summaryx);
	(void)signal(SIGINT, terminate);

	atexit(summary);
#endif

	while (files_cnt--)
		dd_in(globals);

	dd_close(globals);
	exit(0);
	/* NOTREACHED */
	return 0;
}

static int
parity(u_char c)
{
	int i;

	i = c ^ (c >> 1) ^ (c >> 2) ^ (c >> 3) ^
	    (c >> 4) ^ (c >> 5) ^ (c >> 6) ^ (c >> 7);
	return (i & 1);
}

static void
setup(rtems_shell_dd_globals* globals)
{
	u_int cnt;
	struct timeval tv;

	if (in.name == NULL) {
		in.name = "stdin";
		in.fd = STDIN_FILENO;
	} else {
		in.fd = open(in.name, O_RDONLY, 0);
		if (in.fd == -1)
			err(exit_jump, 1, "%s", in.name);
	}

	getfdtype(globals, &in);

	if (files_cnt > 1 && !(in.flags & ISTAPE))
		errx(exit_jump, 1, "files is not supported for non-tape devices");

	if (out.name == NULL) {
		/* No way to check for read access here. */
		out.fd = STDOUT_FILENO;
		out.name = "stdout";
	} else {
#define	OFLAGS \
    (O_CREAT | (ddflags & (C_SEEK | C_NOTRUNC) ? 0 : O_TRUNC))
		out.fd = open(out.name, O_RDWR | OFLAGS, DD_DEFFILEMODE);
		/*
		 * May not have read access, so try again with write only.
		 * Without read we may have a problem if output also does
		 * not support seeks.
		 */
		if (out.fd == -1) {
			out.fd = open(out.name, O_WRONLY | OFLAGS, DD_DEFFILEMODE);
			out.flags |= NOREAD;
		}
		if (out.fd == -1)
			err(exit_jump, 1, "%s", out.name);
	}

	getfdtype(globals, &out);

	/*
	 * Allocate space for the input and output buffers.  If not doing
	 * record oriented I/O, only need a single buffer.
	 */
	if (!(ddflags & (C_BLOCK | C_UNBLOCK))) {
		if ((in.db = malloc(out.dbsz + in.dbsz - 1)) == NULL)
			err(exit_jump, 1, "input buffer");
		out.db = in.db;
	} else if ((in.db = malloc(MAX(in.dbsz, cbsz) + cbsz)) == NULL ||
	    (out.db = malloc(out.dbsz + cbsz)) == NULL)
		err(exit_jump, 1, "output buffer");
	in.dbp = in.db;
	out.dbp = out.db;

	/* Position the input/output streams. */
	if (in.offset)
		pos_in(globals);
	if (out.offset)
		pos_out(globals);

	/*
	 * Truncate the output file.  If it fails on a type of output file
	 * that it should _not_ fail on, error out.
	 */
	if ((ddflags & (C_OF | C_SEEK | C_NOTRUNC)) == (C_OF | C_SEEK) &&
	    out.flags & ISTRUNC)
		if (ftruncate(out.fd, out.offset * out.dbsz) == -1)
			err(exit_jump, 1, "truncating %s", out.name);

	if (ddflags & (C_LCASE  | C_UCASE | C_ASCII | C_EBCDIC | C_PARITY)) {
		if (ctab != NULL) {
			for (cnt = 0; cnt <= 0377; ++cnt)
				casetab[cnt] = ctab[cnt];
		} else {
			for (cnt = 0; cnt <= 0377; ++cnt)
				casetab[cnt] = cnt;
		}
		if ((ddflags & C_PARITY) && !(ddflags & C_ASCII)) {
			/*
			 * If the input is not EBCDIC, and we do parity
			 * processing, strip input parity.
			 */
			for (cnt = 200; cnt <= 0377; ++cnt)
				casetab[cnt] = casetab[cnt & 0x7f];
		}
		if (ddflags & C_LCASE) {
			for (cnt = 0; cnt <= 0377; ++cnt)
				casetab[cnt] = tolower(casetab[cnt]);
		} else if (ddflags & C_UCASE) {
			for (cnt = 0; cnt <= 0377; ++cnt)
				casetab[cnt] = toupper(casetab[cnt]);
		}
		if ((ddflags & C_PARITY)) {
			/*
			 * This should strictly speaking be a no-op, but I
			 * wonder what funny LANG settings could get us.
			 */
			for (cnt = 0; cnt <= 0377; ++cnt)
				casetab[cnt] = casetab[cnt] & 0x7f;
		}
		if ((ddflags & C_PARSET)) {
			for (cnt = 0; cnt <= 0377; ++cnt)
				casetab[cnt] = casetab[cnt] | 0x80;
		}
		if ((ddflags & C_PAREVEN)) {
			for (cnt = 0; cnt <= 0377; ++cnt)
				if (parity(casetab[cnt]))
					casetab[cnt] = casetab[cnt] | 0x80;
		}
		if ((ddflags & C_PARODD)) {
			for (cnt = 0; cnt <= 0377; ++cnt)
				if (!parity(casetab[cnt]))
					casetab[cnt] = casetab[cnt] | 0x80;
		}

		ctab = casetab;
	}

	(void)gettimeofday(&tv, (struct timezone *)NULL);
	st.start = tv.tv_sec + tv.tv_usec * 1e-6;
}

static void
getfdtype(rtems_shell_dd_globals* globals, IO *io)
{
	struct stat sb;
#if RTEMS_REMOVED
	int type;
#endif

	if (fstat(io->fd, &sb) == -1)
		err(exit_jump, 1, "%s", io->name);
	if (S_ISREG(sb.st_mode))
		io->flags |= ISTRUNC;
#if RTEMS_REMOVED
	if (S_ISCHR(sb.st_mode) || S_ISBLK(sb.st_mode)) {
		if (ioctl(io->fd, FIODTYPE, &type) == -1) {
			err(exit_jump, 1, "%s", io->name);
		} else {
			if (type & D_TAPE)
				io->flags |= ISTAPE;
			else if (type & (D_DISK | D_MEM))
				io->flags |= ISSEEK;
			if (S_ISCHR(sb.st_mode) && (type & D_TAPE) == 0)
				io->flags |= ISCHR;
		}
		return;
	}
#else
  io->flags |= ISSEEK;
#endif
	errno = 0;
	if (lseek(io->fd, (off_t)0, SEEK_CUR) == -1 && errno == ESPIPE)
		io->flags |= ISPIPE;
	else
		io->flags |= ISSEEK;
}

static void
dd_in(rtems_shell_dd_globals* globals)
{
	ssize_t n;

	for (;;) {
		switch (cpy_cnt) {
		case -1:			/* count=0 was specified */
			return;
		case 0:
			break;
		default:
			if (st.in_full + st.in_part >= (uintmax_t)cpy_cnt)
				return;
			break;
		}

		/*
		 * Zero the buffer first if sync; if doing block operations,
		 * use spaces.
		 */
		if (ddflags & C_SYNC) {
			if (ddflags & C_FILL)
				memset(in.dbp, fill_char, in.dbsz);
			else if (ddflags & (C_BLOCK | C_UNBLOCK))
				memset(in.dbp, ' ', in.dbsz);
			else
				memset(in.dbp, 0, in.dbsz);
		}

		n = read(in.fd, in.dbp, in.dbsz);
		if (n == 0) {
			in.dbrcnt = 0;
			return;
		}

		/* Read error. */
		if (n == -1) {
			/*
			 * If noerror not specified, die.  POSIX requires that
			 * the warning message be followed by an I/O display.
			 */
			if (!(ddflags & C_NOERROR))
				err(exit_jump, 1, "%s", in.name);
			warn("%s", in.name);
			summary(globals);

			/*
			 * If it's a seekable file descriptor, seek past the
			 * error.  If your OS doesn't do the right thing for
			 * raw disks this section should be modified to re-read
			 * in sector size chunks.
			 */
			if (in.flags & ISSEEK &&
			    lseek(in.fd, (off_t)in.dbsz, SEEK_CUR))
				warn("%s", in.name);

			/* If sync not specified, omit block and continue. */
			if (!(ddflags & C_SYNC))
				continue;

			/* Read errors count as full blocks. */
			in.dbcnt += in.dbrcnt = in.dbsz;
			++st.in_full;

		/* Handle full input blocks. */
		} else if ((size_t)n == in.dbsz) {
			in.dbcnt += in.dbrcnt = n;
			++st.in_full;

		/* Handle partial input blocks. */
		} else {
			/* If sync, use the entire block. */
			if (ddflags & C_SYNC)
				in.dbcnt += in.dbrcnt = in.dbsz;
			else
				in.dbcnt += in.dbrcnt = n;
			++st.in_part;
		}

		/*
		 * POSIX states that if bs is set and no other conversions
		 * than noerror, notrunc or sync are specified, the block
		 * is output without buffering as it is read.
		 */
		if (ddflags & C_BS) {
			out.dbcnt = in.dbcnt;
			dd_out(globals, 1);
			in.dbcnt = 0;
			continue;
		}

		if (ddflags & C_SWAB) {
			if ((n = in.dbrcnt) & 1) {
				++st.swab;
				--n;
			}
			swab(in.dbp, in.dbp, (size_t)n);
		}

		in.dbp += in.dbrcnt;
		(*cfunc)(globals);
	}
}

/*
 * Clean up any remaining I/O and flush output.  If necessary, the output file
 * is truncated.
 */
static void
dd_close(rtems_shell_dd_globals* globals)
{
	if (cfunc == def)
		def_close(globals);
	else if (cfunc == block)
		block_close(globals);
	else if (cfunc == unblock)
		unblock_close(globals);
	if (ddflags & C_OSYNC && out.dbcnt && out.dbcnt < out.dbsz) {
		if (ddflags & C_FILL)
			memset(out.dbp, fill_char, out.dbsz - out.dbcnt);
		else if (ddflags & (C_BLOCK | C_UNBLOCK))
			memset(out.dbp, ' ', out.dbsz - out.dbcnt);
		else
			memset(out.dbp, 0, out.dbsz - out.dbcnt);
		out.dbcnt = out.dbsz;
	}
	if (out.dbcnt || pending)
		dd_out(globals, 1);
}

void
dd_out(rtems_shell_dd_globals* globals, int force)
{
	u_char *outp;
	size_t cnt, i, n;
	ssize_t nw;
	static int warned;
	int sparse;

	/*
	 * Write one or more blocks out.  The common case is writing a full
	 * output block in a single write; increment the full block stats.
	 * Otherwise, we're into partial block writes.  If a partial write,
	 * and it's a character device, just warn.  If a tape device, quit.
	 *
	 * The partial writes represent two cases.  1: Where the input block
	 * was less than expected so the output block was less than expected.
	 * 2: Where the input block was the right size but we were forced to
	 * write the block in multiple chunks.  The original versions of dd(1)
	 * never wrote a block in more than a single write, so the latter case
	 * never happened.
	 *
	 * One special case is if we're forced to do the write -- in that case
	 * we play games with the buffer size, and it's usually a partial write.
	 */
	outp = out.db;
	for (n = force ? out.dbcnt : out.dbsz;; n = out.dbsz) {
		for (cnt = n;; cnt -= nw) {
			sparse = 0;
			if (ddflags & C_SPARSE) {
				sparse = 1;	/* Is buffer sparse? */
				for (i = 0; i < cnt; i++)
					if (outp[i] != 0) {
						sparse = 0;
						break;
					}
			}
			if (sparse && !force) {
				pending += cnt;
				nw = cnt;
			} else {
				if (pending != 0) {
					if (force)
						pending--;
					if (lseek(out.fd, pending, SEEK_CUR) ==
					    -1)
						err(exit_jump, 2, "%s: seek error creating sparse file",
						    out.name);
					if (force)
						write(out.fd, outp, 1);
					pending = 0;
				}
				if (cnt)
					nw = write(out.fd, outp, cnt);
				else
					return;
			}

			if (nw <= 0) {
				if (nw == 0)
					errx(exit_jump, 1, "%s: end of device", out.name);
				if (errno != EINTR)
					err(exit_jump, 1, "%s", out.name);
				nw = 0;
			}
			outp += nw;
			st.bytes += nw;
			if ((size_t)nw == n) {
				if (n != out.dbsz)
					++st.out_part;
				else
					++st.out_full;
				break;
			}
			++st.out_part;
			if ((size_t)nw == cnt)
				break;
			if (out.flags & ISTAPE)
				errx(exit_jump, 1, "%s: short write on tape device",
				    out.name);
			if (out.flags & ISCHR && !warned) {
				warned = 1;
				warnx("%s: short write on character device",
				    out.name);
			}
		}
		if ((out.dbcnt -= n) < out.dbsz)
			break;
	}

	/* Reassemble the output block. */
	if (out.dbcnt)
		(void)memmove(out.db, out.dbp - out.dbcnt, out.dbcnt);
	out.dbp = out.db + out.dbcnt;
}

rtems_shell_cmd_t rtems_shell_DD_Command = {
  "dd",                                                /* name */
  "dd [OPERAND]...",                                   /* usage */
  "files",                                             /* topic */
  rtems_shell_main_dd,                                 /* command */
  NULL,                                                /* alias */
  NULL                                                 /* next */
};
