/*-
 * Copyright (c) 1990, 1993
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef lint
#if 0
static char sccsid[] = "@(#)hexsyntax.c	8.2 (Berkeley) 5/4/95";
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/usr.bin/hexdump/hexsyntax.c,v 1.12 2002/09/04 23:29:01 dwmalone Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define rindex(s,c)	strrchr(s,c)

#include "hexdump.h"

#define __need_getopt_newlib
#include <getopt.h>

#if RTEMS_REMOVED
off_t skip;				/* bytes to skip */
#endif

void
newsyntax(rtems_shell_hexdump_globals* globals, int argc, char ***argvp)
{
	int ch;
	char *p, **argv;

  struct getopt_data getopt_reent;
  memset(&getopt_reent, 0, sizeof(getopt_data));

	argv = *argvp;
	if ((p = rindex(argv[0], 'h')) != NULL &&
	    strcmp(p, "hd") == 0) {
		/* "Canonical" format, implies -C. */
		add(globals, "\"%08.8_Ax\n\"");
		add(globals, "\"%08.8_ax  \" 8/1 \"%02x \" \"  \" 8/1 \"%02x \" ");
		add(globals, "\"  |\" 16/1 \"%_p\" \"|\\n\"");
	}
	while ((ch = getopt_r(argc, argv, "bcCde:f:n:os:vx", &getopt_reent)) != -1)
		switch (ch) {
		case 'b':
			add(globals, "\"%07.7_Ax\n\"");
			add(globals, "\"%07.7_ax \" 16/1 \"%03o \" \"\\n\"");
			break;
		case 'c':
			add(globals, "\"%07.7_Ax\n\"");
			add(globals, "\"%07.7_ax \" 16/1 \"%3_c \" \"\\n\"");
			break;
		case 'C':
			add(globals, "\"%08.8_Ax\n\"");
			add(globals, "\"%08.8_ax  \" 8/1 \"%02x \" \"  \" 8/1 \"%02x \" ");
			add(globals, "\"  |\" 16/1 \"%_p\" \"|\\n\"");
			break;
		case 'd':
			add(globals, "\"%07.7_Ax\n\"");
			add(globals, "\"%07.7_ax \" 8/2 \"  %05u \" \"\\n\"");
			break;
		case 'e':
			add(globals, getopt_reent.optarg);
			break;
		case 'f':
			addfile(globals, getopt_reent.optarg);
			break;
		case 'n':
			if ((length = atoi(getopt_reent.optarg)) < 0)
				errx(exit_jump, 1, "%s: bad length value", getopt_reent.optarg);
			break;
		case 'o':
			add(globals, "\"%07.7_Ax\n\"");
			add(globals, "\"%07.7_ax \" 8/2 \" %06o \" \"\\n\"");
			break;
		case 's':
			if ((skip = strtoll(getopt_reent.optarg, &p, 0)) < 0)
				errx(exit_jump, 1, "%s: bad skip value", getopt_reent.optarg);
			switch(*p) {
			case 'b':
				skip *= 512;
				break;
			case 'k':
				skip *= 1024;
				break;
			case 'm':
				skip *= 1048576;
				break;
			}
			break;
		case 'v':
			vflag = ALL;
			break;
		case 'x':
			add(globals, "\"%07.7_Ax\n\"");
			add(globals, "\"%07.7_ax \" 8/2 \"   %04x \" \"\\n\"");
			break;
		case '?':
			usage(globals);
		}

	if (!fshead) {
		add(globals, "\"%07.7_Ax\n\"");
		add(globals, "\"%07.7_ax \" 8/2 \"%04x \" \"\\n\"");
	}

	*argvp += getopt_reent.optind;
}

void
usage(rtems_shell_hexdump_globals* globals)
{
	(void)fprintf(stderr, "%s\n%s\n%s\n%s\n",
"usage: hexdump [-bcCdovx] [-e fmt] [-f fmt_file] [-n length]",
"               [-s skip] [file ...]",
"       hd      [-bcdovx]  [-e fmt] [-f fmt_file] [-n length]",
"               [-s skip] [file ...]");
	exit(1);
}
