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
 *
 *	@(#)hexdump.h	8.1 (Berkeley) 6/6/93
 * $FreeBSD: src/usr.bin/hexdump/hexdump.h,v 1.9 2004/07/11 01:11:12 tjr Exp $
 */

#include <wchar.h>

typedef struct _pr {
	struct _pr *nextpr;		/* next print unit */
#define	F_ADDRESS	0x001		/* print offset */
#define	F_BPAD		0x002		/* blank pad */
#define	F_C		0x004		/* %_c */
#define	F_CHAR		0x008		/* %c */
#define	F_DBL		0x010		/* %[EefGf] */
#define	F_INT		0x020		/* %[di] */
#define	F_P		0x040		/* %_p */
#define	F_STR		0x080		/* %s */
#define	F_U		0x100		/* %_u */
#define	F_UINT		0x200		/* %[ouXx] */
#define	F_TEXT		0x400		/* no conversions */
	u_int flags;			/* flag values */
	int bcnt;			/* byte count */
	char *cchar;			/* conversion character */
	char *fmt;			/* printf format */
	char *nospace;			/* no whitespace version */
	int mbleft;			/* bytes left of multibyte char. */
	mbstate_t mbstate;		/* conversion state */
} PR;

typedef struct _fu {
	struct _fu *nextfu;		/* next format unit */
	struct _pr *nextpr;		/* next print unit */
#define	F_IGNORE	0x01		/* %_A */
#define	F_SETREP	0x02		/* rep count set, not default */
	u_int flags;			/* flag values */
	int reps;			/* repetition count */
	int bcnt;			/* byte count */
	char *fmt;			/* format string */
} FU;

typedef struct _fs {			/* format strings */
	struct _fs *nextfs;		/* linked list of format strings */
	struct _fu *nextfu;		/* linked list of format units */
	int bcnt;
} FS;

#if 0
extern FS *fshead;			/* head of format strings list */
extern FU *endfu;			/* format at end-of-data */
extern int blocksize;			/* data block size */
extern int exitval;			/* final exit value */
extern int odmode;			/* are we acting as od(1)? */
extern int length;			/* amount of data to read */
extern off_t skip;			/* amount of data to skip at start */
enum _vflag { ALL, DUP, FIRST, WAIT };	/* -v values */
extern enum _vflag vflag;
#endif

#include <setjmp.h>
#include <stdio.h>

enum _vflag { ALL, DUP, FIRST, WAIT };	/* -v values */
typedef struct rtems_shell_hexdump_globals_t {
  FS *fshead;			/* head of format strings list */
  FU *endfu;			/* format at end-of-data */
  int blocksize;			/* data block size */
  int exitval;			/* final exit value */
  int odmode;			/* are we acting as od(1)? */
  int length;			/* amount of data to read */
  off_t skip;			/* amount of data to skip at start */
  enum _vflag vflag;

  off_t address;
  off_t eaddress;
  int ateof;
  u_char *curp;
  u_char *savp;
  int done;

  FILE* hdstdin;

  int exit_code;
  jmp_buf exit_jmp;
} rtems_shell_hexdump_globals;

#define fshead    globals->fshead
#define endfu     globals->endfu
#define blocksize globals->blocksize
#define exitval   globals->exitval
#define odmode    globals->odmode
#define length    globals->length
#define skip      globals->skip
#define vflag     globals->vflag

#define address   globals->address
#define eaddress  globals->eaddress
#define ateof     globals->ateof
#define curp      globals->curp
#define savp      globals->savp
#define done      globals->done

#define hdstdin   globals->hdstdin

#define exit_jump &(globals->exit_jmp)

#define add       rtems_shell_hexdump_add
#define addfile   rtems_shell_hexdump_addfile
#define badcnt    rtems_shell_hexdump_badcnt
#define badconv   rtems_shell_hexdump_badconv
#define badfmt    rtems_shell_hexdump_badfmt
#define badsfmt   rtems_shell_hexdump_badsfmt
#define bpad      rtems_shell_hexdump_bpad
#define conv_c    rtems_shell_hexdump_conv_c
#define conv_u    rtems_shell_hexdump_conv_u
#define display   rtems_shell_hexdump_display
#define doskip    rtems_shell_hexdump_doskip
#define escape    rtems_shell_hexdump_escape
#define get       rtems_shell_hexdump_get
#define newsyntax rtems_shell_hexdump_newsyntax
#define next      rtems_shell_hexdump_next
#define nomem     rtems_shell_hexdump_nomem
#define oldsyntax rtems_shell_hexdump_oldsyntax
#define peek      rtems_shell_hexdump_peek
#define rewrite   rtems_shell_hexdump_rewrite
#define size      rtems_shell_hexdump_size
#define usage     rtems_shell_hexdump_usage

void	 add(rtems_shell_hexdump_globals*, const char *);
void	 addfile(rtems_shell_hexdump_globals*, char *);
void	 badcnt(rtems_shell_hexdump_globals*, char *);
void	 badconv(rtems_shell_hexdump_globals*, char *);
void	 badfmt(rtems_shell_hexdump_globals*, const char *);
void	 badsfmt(rtems_shell_hexdump_globals*);
void	 bpad(PR *);
void	 conv_c(rtems_shell_hexdump_globals*, PR *, u_char *, size_t);
void	 conv_u(rtems_shell_hexdump_globals*, PR *, u_char *);
void	 display(rtems_shell_hexdump_globals*);
void	 doskip(rtems_shell_hexdump_globals*, const char *, int);
void	 escape(char *);
u_char	*get(rtems_shell_hexdump_globals*);
void	 newsyntax(rtems_shell_hexdump_globals*, int, char ***);
int	 next(rtems_shell_hexdump_globals*, char **);
void	 nomem(void);
void	 oldsyntax(rtems_shell_hexdump_globals*, int, char ***);
size_t	 peek(rtems_shell_hexdump_globals*, u_char *, size_t);
void	 rewrite(rtems_shell_hexdump_globals*, FS *);
int	 size(rtems_shell_hexdump_globals*, FS *);
void	 usage(rtems_shell_hexdump_globals*);

#define exit(ec) rtems_shell_hexdump_exit(globals, ec)

void rtems_shell_hexdump_exit(rtems_shell_hexdump_globals* globals, int code);
