/*-
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
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
 *
 *	from: @(#)ls.h	8.1 (Berkeley) 5/31/93
 * $FreeBSD: src/bin/ls/ls.h,v 1.23 2008/04/04 03:57:46 grog Exp $
 */

#ifndef _EXTERN_LS_H_
#define _EXTERN_LS_H_

#define NO_PRINT	1
//#define COLORLS   1

#include <sys/cdefs.h>
#include <setjmp.h>

#define major(d) rtems_filesystem_dev_major_t(d)
#define minor(d) rtems_filesystem_dev_minor_t(d)

const char *user_from_uid(uid_t uid, int nouser);

typedef struct {
  int blocksize;
  int termwidth;
  int sortkey;
  int rval;
  int output;
  time_t now;

  int f_accesstime;
  int f_column;
  int f_columnacross;
  int f_flags;
  int f_grouponly;
  int f_humanize;
  int f_inode;
  int f_listdir;
  int f_listdot;
  int f_longform;
  int f_nonprint;
  int f_nosort;
  int f_numericonly;
  int f_octal;
  int f_octal_escape;
  int f_recursive;
  int f_reversesort;
  int f_sectime;
  int f_singlecol;
  int f_size;
  int f_statustime;
  int f_stream;
  int f_type;
  int f_typedir;
  int f_whiteout;

  int exit_code;
  jmp_buf exit_jmp;
} rtems_shell_ls_globals;

#define blocksize      globals->blocksize
#define termwidth      globals->termwidth
#define sortkey        globals->sortkey
#define rval           globals->rval
#define output         globals->output
#define now            globals->now

#define f_accesstime   globals->f_accesstime
#define f_column       globals->f_column
#define f_columnacross globals->f_columnacross
#define f_flags        globals->f_flags
#define f_grouponly    globals->f_grouponly
#define f_humanize     globals->f_humanize
#define f_inode        globals->f_inode
#define f_listdir      globals->f_listdir
#define f_listdot      globals->f_listdot
#define f_longform     globals->f_longform
#define f_nonprint     globals->f_nonprint
#define f_nosort       globals->f_nosort
#define f_numericonly  globals->f_numericonly
#define f_octal        globals->f_octal
#define f_octal_escape globals->f_octal_escape
#define f_recursive    globals->f_recursive
#define f_reversesort  globals->f_reversesort
#define f_sectime      globals->f_sectime
#define f_singlecol    globals->f_singlecol
#define f_size         globals->f_size
#define f_statustime   globals->f_statustime
#define f_stream       globals->f_stream
#define f_type         globals->f_type
#define f_typedir      globals->f_typedir
#define f_whiteout     globals->f_whiteout

#define exit_jump      &(globals->exit_jmp)

void rtems_shell_ls_exit(rtems_shell_ls_globals* globals, int code);

#define exit(ec) rtems_shell_ls_exit(globals, ec)


typedef struct {
        FTSENT *list;
        u_int64_t btotal;
        u_int64_t stotal;
        int entries;
        int maxlen;
        int s_block;
        int s_flags;
        int s_group;
        int s_inode;
        int s_nlink;
        int s_size;
        int s_user;
        int s_major;
        int s_minor;
} DISPLAY;

typedef struct {
        char *user;
        char *group;
        char *flags;
        char data[1];
} NAMES;

#define acccmp        rtems_shell_ls_acccmp
#define revacccmp     rtems_shell_ls_revacccmp
#define modcmp        rtems_shell_ls_modcmp
#define revmodcmp     rtems_shell_ls_revmodcmp
#define namecmp       rtems_shell_ls_namecmp
#define revnamecmp    rtems_shell_ls_revnamecmp
#define statcmp       rtems_shell_ls_statcmp
#define revstatcmp    rtems_shell_ls_revstatcmp
#define sizecmp       rtems_shell_ls_sizecmp
#define revsizecmp    rtems_shell_ls_revsizecmp
#define printescaped  rtems_shell_ls_printescaped
#define printacol     rtems_shell_ls_printacol
#define printcol      rtems_shell_ls_printcol
#define printlong     rtems_shell_ls_printlong
#define printscol     rtems_shell_ls_printscol
#define printstream   rtems_shell_ls_printstream
#define usage         rtems_shell_ls_usage

int      acccmp(const FTSENT *, const FTSENT *);
int      revacccmp(const FTSENT *, const FTSENT *);
int      modcmp(const FTSENT *, const FTSENT *);
int      revmodcmp(const FTSENT *, const FTSENT *);
int      namecmp(const FTSENT *, const FTSENT *);
int      revnamecmp(const FTSENT *, const FTSENT *);
int      statcmp(const FTSENT *, const FTSENT *);
int      revstatcmp(const FTSENT *, const FTSENT *);
int      sizecmp(const FTSENT *, const FTSENT *);
int      revsizecmp(const FTSENT *, const FTSENT *);

int      printescaped(rtems_shell_ls_globals* globals, const char *);
void     printacol(rtems_shell_ls_globals* globals, DISPLAY *);
void     printcol(rtems_shell_ls_globals* globals, DISPLAY *);
void     printlong(rtems_shell_ls_globals* globals, DISPLAY *);
void     printscol(rtems_shell_ls_globals* globals, DISPLAY *);
void     printstream(rtems_shell_ls_globals* globals, DISPLAY *);
int      safe_print(rtems_shell_ls_globals* globals, const char *);
void     usage(rtems_shell_ls_globals* globals);

void strmode(mode_t mode, char *p);

#endif /* !_EXTERN_H_ */
