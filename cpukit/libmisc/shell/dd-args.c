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

#ifndef lint
#if 0
static char sccsid[] = "@(#)args.c	8.3 (Berkeley) 4/2/94";
#endif
#endif /* not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/bin/dd/args.c,v 1.40 2004/08/15 19:10:05 rwatson Exp $");

#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "dd.h"
#include "extern-dd.h"

#define strtouq strtoul
#define strtoq strtol

static int	c_arg(const void *, const void *);
static int	c_conv(const void *, const void *);
static void	f_bs(rtems_shell_dd_globals* globals, char *);
static void	f_cbs(rtems_shell_dd_globals* globals, char *);
static void	f_conv(rtems_shell_dd_globals* globals, char *);
static void	f_count(rtems_shell_dd_globals* globals, char *);
static void	f_files(rtems_shell_dd_globals* globals, char *);
static void	f_fillchar(rtems_shell_dd_globals* globals, char *);
static void	f_ibs(rtems_shell_dd_globals* globals, char *);
static void	f_if(rtems_shell_dd_globals* globals, char *);
static void	f_obs(rtems_shell_dd_globals* globals, char *);
static void	f_of(rtems_shell_dd_globals* globals, char *);
static void	f_seek(rtems_shell_dd_globals* globals, char *);
static void	f_skip(rtems_shell_dd_globals* globals, char *);
static uintmax_t get_num(rtems_shell_dd_globals* globals, const char *);
static off_t	get_off_t(rtems_shell_dd_globals* globals, const char *);

static const struct arg {
	const char *name;
	void (*f)(rtems_shell_dd_globals* globals, char *);
	uint_least32_t set, noset;
} args[] = {
	{ "bs",		f_bs,		C_BS,	 C_BS|C_IBS|C_OBS|C_OSYNC },
	{ "cbs",	f_cbs,		C_CBS,	 C_CBS },
	{ "conv",	f_conv,		0,	 0 },
	{ "count",	f_count,	C_COUNT, C_COUNT },
	{ "files",	f_files,	C_FILES, C_FILES },
	{ "fillchar",	f_fillchar,	C_FILL,	 C_FILL },
	{ "ibs",	f_ibs,		C_IBS,	 C_BS|C_IBS },
	{ "if",		f_if,		C_IF,	 C_IF },
	{ "iseek",	f_skip,		C_SKIP,	 C_SKIP },
	{ "obs",	f_obs,		C_OBS,	 C_BS|C_OBS },
	{ "of",		f_of,		C_OF,	 C_OF },
	{ "oseek",	f_seek,		C_SEEK,	 C_SEEK },
	{ "seek",	f_seek,		C_SEEK,	 C_SEEK },
	{ "skip",	f_skip,		C_SKIP,	 C_SKIP },
};

static char *oper;

/*
 * args -- parse JCL syntax of dd.
 */
void
jcl(rtems_shell_dd_globals* globals, char **argv)
{
	struct arg *ap, tmp;
	char *arg;

  oper = NULL;

	in.dbsz = out.dbsz = 512;

	while ((oper = *++argv) != NULL) {
//		if ((oper = strdup(oper)) == NULL)
//			errx(exit_jump, 1, "unable to allocate space for the argument \"%s\"", *argv);
		if ((arg = strchr(oper, '=')) == NULL)
			errx(exit_jump, 1, "unknown operand %s", oper);
		*arg++ = '\0';
		if (!*arg)
			errx(exit_jump, 1, "no value specified for %s", oper);
		tmp.name = oper;
		if (!(ap = (struct arg *)bsearch(&tmp, args,
		    sizeof(args)/sizeof(struct arg), sizeof(struct arg),
		    c_arg)))
			errx(exit_jump, 1, "unknown operand %s", tmp.name);
		if (ddflags & ap->noset)
			errx(exit_jump, 1, "%s: illegal argument combination or already set",
			    tmp.name);
		ddflags |= ap->set;
		ap->f(globals, arg);
	}

	/* Final sanity checks. */

	if (ddflags & C_BS) {
		/*
		 * Bs is turned off by any conversion -- we assume the user
		 * just wanted to set both the input and output block sizes
		 * and didn't want the bs semantics, so we don't warn.
		 */
		if (ddflags & (C_BLOCK | C_LCASE | C_SWAB | C_UCASE |
		    C_UNBLOCK))
			ddflags &= ~C_BS;

		/* Bs supersedes ibs and obs. */
		if (ddflags & C_BS && ddflags & (C_IBS | C_OBS))
			warnx("bs supersedes ibs and obs");
	}

	/*
	 * Ascii/ebcdic and cbs implies block/unblock.
	 * Block/unblock requires cbs and vice-versa.
	 */
	if (ddflags & (C_BLOCK | C_UNBLOCK)) {
		if (!(ddflags & C_CBS))
			errx(exit_jump, 1, "record operations require cbs");
		if (cbsz == 0)
			errx(exit_jump, 1, "cbs cannot be zero");
		cfunc = ddflags & C_BLOCK ? block : unblock;
	} else if (ddflags & C_CBS) {
		if (ddflags & (C_ASCII | C_EBCDIC)) {
			if (ddflags & C_ASCII) {
				ddflags |= C_UNBLOCK;
				cfunc = unblock;
			} else {
				ddflags |= C_BLOCK;
				cfunc = block;
			}
		} else
			errx(exit_jump, 1, "cbs meaningless if not doing record operations");
	} else
		cfunc = def;

	/*
	 * Bail out if the calculation of a file offset would overflow.
	 */
	if (in.offset > OFF_MAX / (ssize_t)in.dbsz ||
	    out.offset > OFF_MAX / (ssize_t)out.dbsz)
		errx(exit_jump, 1, "seek offsets cannot be larger than %jd",
		    (intmax_t)OFF_MAX);
}

static int
c_arg(const void *a, const void *b)
{

	return (strcmp(((const struct arg *)a)->name,
	    ((const struct arg *)b)->name));
}

static void
f_bs(rtems_shell_dd_globals* globals, char *arg)
{
	uintmax_t res;

	res = get_num(globals, arg);
	if (res < 1 || res > SSIZE_MAX)
		errx(exit_jump, 1, "bs must be between 1 and %jd", (intmax_t)SSIZE_MAX);
	in.dbsz = out.dbsz = (size_t)res;
}

static void
f_cbs(rtems_shell_dd_globals* globals, char *arg)
{
	uintmax_t res;

	res = get_num(globals, arg);
	if (res < 1 || res > SSIZE_MAX)
		errx(exit_jump, 1, "cbs must be between 1 and %jd", (intmax_t)SSIZE_MAX);
	cbsz = (size_t)res;
}

static void
f_count(rtems_shell_dd_globals* globals, char *arg)
{
	intmax_t res;

	res = (intmax_t)get_num(globals, arg);
	if (res < 0)
		errx(exit_jump, 1, "count cannot be negative");
	if (res == 0)
		cpy_cnt = (uintmax_t)-1;
	else
		cpy_cnt = (uintmax_t)res;
}

static void
f_files(rtems_shell_dd_globals* globals, char *arg)
{

	files_cnt = get_num(globals, arg);
	if (files_cnt < 1)
		errx(exit_jump, 1, "files must be between 1 and %jd", (uintmax_t)-1);
}

static void
f_fillchar(rtems_shell_dd_globals* globals, char *arg)
{

	if (strlen(arg) != 1)
		errx(exit_jump, 1, "need exactly one fill char");

	fill_char = arg[0];
}

static void
f_ibs(rtems_shell_dd_globals* globals, char *arg)
{
	uintmax_t res;

	if (!(ddflags & C_BS)) {
		res = get_num(globals, arg);
		if (res < 1 || res > SSIZE_MAX)
			errx(exit_jump, 1, "ibs must be between 1 and %jd",
			    (intmax_t)SSIZE_MAX);
		in.dbsz = (size_t)res;
	}
}

static void
f_if(rtems_shell_dd_globals* globals, char *arg)
{

	in.name = strdup(arg);
}

static void
f_obs(rtems_shell_dd_globals* globals, char *arg)
{
	uintmax_t res;

	if (!(ddflags & C_BS)) {
		res = get_num(globals, arg);
		if (res < 1 || res > SSIZE_MAX)
			errx(exit_jump, 1, "obs must be between 1 and %jd",
			    (intmax_t)SSIZE_MAX);
		out.dbsz = (size_t)res;
	}
}

static void
f_of(rtems_shell_dd_globals* globals, char *arg)
{

	out.name = strdup(arg);
}

static void
f_seek(rtems_shell_dd_globals* globals, char *arg)
{

	out.offset = get_off_t(globals, arg);
}

static void
f_skip(rtems_shell_dd_globals* globals, char *arg)
{

	in.offset = get_off_t(globals, arg);
}

static const struct conv {
	const char *name;
	uint_least32_t set, noset;
	const u_char *ctab_;
} clist[] = {
	{ "ascii",	C_ASCII,	C_EBCDIC,	e2a_POSIX },
	{ "block",	C_BLOCK,	C_UNBLOCK,	NULL },
	{ "ebcdic",	C_EBCDIC,	C_ASCII,	a2e_POSIX },
	{ "ibm",	C_EBCDIC,	C_ASCII,	a2ibm_POSIX },
	{ "lcase",	C_LCASE,	C_UCASE,	NULL },
	{ "noerror",	C_NOERROR,	0,		NULL },
	{ "notrunc",	C_NOTRUNC,	0,		NULL },
	{ "oldascii",	C_ASCII,	C_EBCDIC,	e2a_32V },
	{ "oldebcdic",	C_EBCDIC,	C_ASCII,	a2e_32V },
	{ "oldibm",	C_EBCDIC,	C_ASCII,	a2ibm_32V },
	{ "osync",	C_OSYNC,	C_BS,		NULL },
	{ "pareven",	C_PAREVEN,	C_PARODD|C_PARSET|C_PARNONE, NULL},
	{ "parnone",	C_PARNONE,	C_PARODD|C_PARSET|C_PAREVEN, NULL},
	{ "parodd",	C_PARODD,	C_PAREVEN|C_PARSET|C_PARNONE, NULL},
	{ "parset",	C_PARSET,	C_PARODD|C_PAREVEN|C_PARNONE, NULL},
	{ "sparse",	C_SPARSE,	0,		NULL },
	{ "swab",	C_SWAB,		0,		NULL },
	{ "sync",	C_SYNC,		0,		NULL },
	{ "ucase",	C_UCASE,	C_LCASE,	NULL },
	{ "unblock",	C_UNBLOCK,	C_BLOCK,	NULL },
};

static void
f_conv(rtems_shell_dd_globals* globals, char *arg)
{
	struct conv *cp, tmp;

	while (arg != NULL) {
		tmp.name = strsep(&arg, ",");
		cp = bsearch(&tmp, clist, sizeof(clist) / sizeof(struct conv),
		    sizeof(struct conv), c_conv);
		if (cp == NULL)
			errx(exit_jump, 1, "unknown conversion %s", tmp.name);
		if (ddflags & cp->noset)
			errx(exit_jump, 1, "%s: illegal conversion combination", tmp.name);
		ddflags |= cp->set;
		if (cp->ctab_)
			ctab = cp->ctab_;
	}
}

static int
c_conv(const void *a, const void *b)
{

	return (strcmp(((const struct conv *)a)->name,
	    ((const struct conv *)b)->name));
}

/*
 * Convert an expression of the following forms to a uintmax_t.
 * 	1) A positive decimal number.
 *	2) A positive decimal number followed by a 'b' or 'B' (mult by 512).
 *	3) A positive decimal number followed by a 'k' or 'K' (mult by 1 << 10).
 *	4) A positive decimal number followed by a 'm' or 'M' (mult by 1 << 20).
 *	5) A positive decimal number followed by a 'g' or 'G' (mult by 1 << 30).
 *	5) A positive decimal number followed by a 'w' or 'W' (mult by sizeof int).
 *	6) Two or more positive decimal numbers (with/without [BbKkMmGgWw])
 *	   separated by 'x' or 'X' (also '*' for backwards compatibility),
 *	   specifying the product of the indicated values.
 */
static uintmax_t
get_num(rtems_shell_dd_globals* globals, const char *val)
{
	uintmax_t num, mult, prevnum;
	char *expr;

	errno = 0;
	num = strtouq(val, &expr, 0);
	if (errno != 0)				/* Overflow or underflow. */
		err(exit_jump, 1, "%s", oper);

	if (expr == val)			/* No valid digits. */
		errx(exit_jump, 1, "%s: illegal numeric value", oper);

	mult = 0;
	switch (*expr) {
	case 'B':
	case 'b':
		mult = UINT32_C(512);
		break;
	case 'K':
	case 'k':
		mult = UINT32_C(1) << 10;
		break;
	case 'M':
	case 'm':
		mult = UINT32_C(1) << 20;
		break;
	case 'G':
	case 'g':
		mult = UINT32_C(1) << 30;
		break;
	case 'W':
	case 'w':
		mult = sizeof(int);
		break;
	default:
		;
	}

	if (mult != 0) {
		prevnum = num;
		num *= mult;
		/* Check for overflow. */
		if (num / mult != prevnum)
			goto erange;
		expr++;
	}

	switch (*expr) {
		case '\0':
			break;
		case '*':			/* Backward compatible. */
		case 'X':
		case 'x':
			mult = get_num(globals, expr + 1);
			prevnum = num;
			num *= mult;
			if (num / mult == prevnum)
				break;
erange:
			errx(exit_jump, 1, "%s: %s", oper, strerror(ERANGE));
		default:
			errx(exit_jump, 1, "%s: illegal numeric value", oper);
	}
	return (num);
}

/*
 * Convert an expression of the following forms to an off_t.  This is the
 * same as get_num(), but it uses signed numbers.
 *
 * The major problem here is that an off_t may not necessarily be a intmax_t.
 */
static off_t
get_off_t(rtems_shell_dd_globals* globals, const char *val)
{
	intmax_t num, mult, prevnum;
	char *expr;

	errno = 0;
	num = strtoq(val, &expr, 0);
	if (errno != 0)				/* Overflow or underflow. */
		err(exit_jump, 1, "%s", oper);

	if (expr == val)			/* No valid digits. */
		errx(exit_jump, 1, "%s: illegal numeric value", oper);

	mult = 0;
	switch (*expr) {
	case 'B':
	case 'b':
		mult = UINT32_C(512);
		break;
	case 'K':
	case 'k':
		mult = UINT32_C(1) << 10;
		break;
	case 'M':
	case 'm':
		mult = UINT32_C(1) << 20;
		break;
	case 'G':
	case 'g':
		mult = UINT32_C(1) << 30;
		break;
	case 'W':
	case 'w':
		mult = sizeof(int);
		break;
	}

	if (mult != 0) {
		prevnum = num;
		num *= mult;
		/* Check for overflow. */
		if ((prevnum > 0) != (num > 0) || num / mult != prevnum)
			goto erange;
		expr++;
	}

	switch (*expr) {
		case '\0':
			break;
		case '*':			/* Backward compatible. */
		case 'X':
		case 'x':
			mult = (intmax_t)get_off_t(globals, expr + 1);
			prevnum = num;
			num *= mult;
			if ((prevnum > 0) == (num > 0) && num / mult == prevnum)
				break;
erange:
			errx(exit_jump, 1, "%s: %s", oper, strerror(ERANGE));
		default:
			errx(exit_jump, 1, "%s: illegal numeric value", oper);
	}
	return (num);
}
