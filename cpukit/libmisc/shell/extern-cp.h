/* $NetBSD: extern.h,v 1.12 2005/10/15 18:22:18 christos Exp $ */

/*-
 * Copyright (c) 1991, 1993, 1994
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
 *
 *	@(#)extern.h	8.2 (Berkeley) 4/1/94
 */

#ifndef _EXTERN_CP_H_
#define _EXTERN_CP_H_

#define PATH_T       RTEMS_SHELL_CP_PATH_T

typedef struct {
	char *p_end;			/* pointer to NULL at end of path */
	char *target_end;		/* pointer to end of target base */
	char p_path[MAXPATHLEN + 1];	/* pointer to the start of a path */
} PATH_T;

#include <setjmp.h>

typedef struct {
  PATH_T to;
  int info;
  uid_t myuid;
  int fflag, iflag, lflag, nflag, pflag, vflag;
  mode_t myumask;
  jmp_buf exit_jmp;
} rtems_shell_cp_globals;

#define to           cp_globals->to
#define info         cp_globals->info
#define myuid        cp_globals->myuid
#define fflag        cp_globals->fflag
#define iflag        cp_globals->iflag
#define lflag        cp_globals->lflag
#define nflag        cp_globals->nflag
#define pflag        cp_globals->pflag
#define vflag        cp_globals->vflag
#define myumask      cp_globals->myumask
#define exit_jump    &(cp_globals->exit_jmp)

#define copy_fifo    rtems_shell_cp_copy_fifo
#define copy_file    rtems_shell_cp_copy_file
#define copy_link    rtems_shell_cp_copy_link
#define copy_special rtems_shell_cp_copy_special
#define set_utimes   rtems_shell_cp_set_utimes
#define setfile      rtems_shell_cp_setfile
#define usage        rtems_shell_cp_usage

#include <sys/cdefs.h>

__BEGIN_DECLS
int	copy_fifo(rtems_shell_cp_globals* cp_globals, struct stat *, int);
int	copy_file(rtems_shell_cp_globals* cp_globals, FTSENT *, int);
int	copy_link(rtems_shell_cp_globals* cp_globals, FTSENT *, int);
int	copy_special(rtems_shell_cp_globals* cp_globals, struct stat *, int);
int	set_utimes(const char *, struct stat *);
int	setfile(rtems_shell_cp_globals* cp_globals, struct stat *, int);
int	preserve_dir_acls(struct stat *, char *, char *);
int	preserve_fd_acls(int, int);
void usage(rtems_shell_cp_globals* cp_globals);

__END_DECLS

#endif /* !_EXTERN_H_ */
