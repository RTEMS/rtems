/*	$NetBSD: err.h,v 1.14 2005/02/03 04:39:32 perry Exp $	*/

/*-
 * Copyright (c) 1993
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
 *	@(#)err.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _ERR_H_
#define	_ERR_H_

/*
 * Don't use va_list in the err/warn prototypes.   Va_list is typedef'd in two
 * places (<machine/varargs.h> and <machine/stdarg.h>), so if we include one
 * of them here we may collide with the utility's includes.  It's unreasonable
 * for utilities to have to include one of them to include err.h, so we get
 * _BSD_VA_LIST_ from <machine/ansi.h> and use it.
 */
#include <machine/ansi.h>
#include <sys/cdefs.h>

#include <stdarg.h>
#define _BSD_VA_LIST_ va_list

#define __dead

#define err    rtems_shell_err
#define verr   rtems_shell_verr
#define errx   rtems_shell_errx
#define verrx  rtems_shell_verrx
#define warn   rtems_shell_warn
#define vwarn  rtems_shell_vwarn
#define warnx  rtems_shell_warnx
#define vwarnx rtems_shell_vwarnx

#include <setjmp.h>

extern  jmp_buf rtems_shell_bsd_exit_recover;

__BEGIN_DECLS
__dead void	err(jmp_buf*, int, const char *, ...)
		    __attribute__((__noreturn__, __format__(__printf__, 3, 4)));
__dead void	verr(jmp_buf*, int, const char *, _BSD_VA_LIST_)
		    __attribute__((__noreturn__, __format__(__printf__, 3, 0)));
__dead void	errx(jmp_buf*, int, const char *, ...)
		    __attribute__((__noreturn__, __format__(__printf__, 3, 4)));
__dead void	verrx(jmp_buf*, int, const char *, _BSD_VA_LIST_)
		    __attribute__((__noreturn__, __format__(__printf__, 3, 0)));
void		warn(const char *, ...)
		    __attribute__((__format__(__printf__, 1, 2)));
void		vwarn(const char *, _BSD_VA_LIST_)
		    __attribute__((__format__(__printf__, 1, 0)));
void		warnx(const char *, ...)
		    __attribute__((__format__(__printf__, 1, 2)));
void		vwarnx(const char *, _BSD_VA_LIST_)
		    __attribute__((__format__(__printf__, 1, 0)));
__END_DECLS

#endif /* !_ERR_H_ */
