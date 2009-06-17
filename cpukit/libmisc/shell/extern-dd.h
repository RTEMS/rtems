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
 *
 *	@(#)extern.h	8.3 (Berkeley) 4/2/94
 * $FreeBSD: src/bin/dd/extern.h,v 1.15 2004/08/15 19:10:05 rwatson Exp $
 */

#ifndef _EXTERN_DD_H_
#define _EXTERN_DD_H_

#include <setjmp.h>

typedef struct rtems_shell_dd_globals_t {
  IO	in, out;		/* input/output state */
  STAT	st;			/* statistics */
  void	(*cfunc)(struct rtems_shell_dd_globals_t* globals);		/* conversion function */
  uintmax_t cpy_cnt;		/* # of blocks to copy */
  u_int	ddflags;		/* conversion options */
  size_t	cbsz;			/* conversion block size */
  uintmax_t files_cnt;	/* # of files to copy */
  const	u_char *ctab;		/* conversion table */
  char	fill_char;		/* Character to fill with if defined */
  u_char casetab[256];
  int exit_code;
  jmp_buf exit_jmp;
} rtems_shell_dd_globals;

#define in            globals->in
#define out           globals->out
#define st            globals->st
#define cfunc         globals->cfunc
#define cpy_cnt       globals->cpy_cnt
#define ddflags       globals->ddflags
#define cbsz          globals->cbsz
#define files_cnt     globals->files_cnt
#define casetab       globals->casetab
#define ctab          globals->ctab
#define fill_char     globals->fill_char
#define exit_jump     &(globals->exit_jmp)

#define block         rtems_shell_dd_block
#define block_close   rtems_shell_dd_block_close
#define dd_out        rtems_shell_dd_dd_out
#define def           rtems_shell_dd_def
#define def_close     rtems_shell_dd_def_close
#define jcl           rtems_shell_dd_jcl
#define pos_in        rtems_shell_dd_pos_in
#define pos_out       rtems_shell_dd_pos_out
#define summary       rtems_shell_dd_summary
#define summaryx      rtems_shell_dd_summaryx
#define terminate     rtems_shell_dd_terminate
#define unblock       rtems_shell_dd_unblock
#define unblock_close rtems_shell_dd_unblock_close

void block(rtems_shell_dd_globals* );
void block_close(rtems_shell_dd_globals* );
void dd_out(rtems_shell_dd_globals* , int);
void def(rtems_shell_dd_globals* globals);
void def_close(rtems_shell_dd_globals* );
void jcl(rtems_shell_dd_globals* , char **);
void pos_in(rtems_shell_dd_globals* );
void pos_out(rtems_shell_dd_globals* );
void summary(rtems_shell_dd_globals* );
void summaryx(rtems_shell_dd_globals* , int);
void terminate(int);
void unblock(rtems_shell_dd_globals* globals);
void unblock_close(rtems_shell_dd_globals* globals);

extern const u_char a2e_32V[256], a2e_POSIX[256];
extern const u_char e2a_32V[256], e2a_POSIX[256];
extern const u_char a2ibm_32V[256], a2ibm_POSIX[256];

void rtems_shell_dd_exit(rtems_shell_dd_globals* globals, int code);

#define exit(ec) rtems_shell_dd_exit(globals, ec)

#endif /* !_EXTERN_H_ */
