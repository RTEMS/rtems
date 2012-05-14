/*-
 * Copyright (c) 1982, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 *	@(#)systm.h	8.7 (Berkeley) 3/29/95
 * $FreeBSD: src/sys/sys/systm.h,v 1.248 2007/01/15 15:06:27 rrs Exp $
 */


#ifndef _SYS_SYSTM_H_
#define	_SYS_SYSTM_H_

#include <rtems/rtems_bsdnet_internal.h> /* for __BSD_VA_LIST__ */
#include <machine/cpufunc.h>

#if !defined(__dead2)
#define __dead2         __attribute__((__noreturn__))
#endif

extern int securelevel;		/* system security level (see init(8)) */

extern int cold;		/* nonzero if we are doing a cold boot */
extern const char *panicstr;	/* panic message */
extern char version[];		/* system version */
extern char copyright[];	/* system copyright */

extern int physmem;		/* physical memory */

/*
 * General function declarations.
 */
int	nullop(void);
int	ureadc(int, struct uio *);
void	*hashinit(int count, int type, u_long *hashmask);
void	*phashinit(int count, int type, u_long *nentries);

void	panic(const char *, ...) __dead2;
void	boot(int) __dead2;
void	cpu_boot(int);
int	kvprintf(char const *, void (*)(int, void*), void *, int,
		      _BSD_VA_LIST_);
void	log(int, const char *, ...);
int	printf(const char *, ...);
int	sprintf(char *buf, const char *, ...);
void	uprintf(const char *, ...);
void	ttyprintf(struct tty *, const char *, ...);

#define bcopy(f,t,n) memcpy((t),(f),(n))
#define bzero(p,n) memset((p),(0),(n))

int	copystr(const void *kfaddr, void *kdaddr, size_t len,
		size_t *lencopied);
int	copyinstr(const void *udaddr, void *kaddr, size_t len,
		size_t *lencopied);
#ifndef __rtems__
/* FIXME: these clash with defines in rtems_bsdnet_internal.h */
int	copyin(const void *udaddr, void *kaddr, size_t len);
int	copyout(const void *kaddr, void *udaddr, size_t len);
#endif

int	hzto(struct timeval *tv);

#include <sys/libkern.h>

/* Timeouts */
typedef void (timeout_t)(void *); /* actual timeout function type */
typedef timeout_t *timeout_func_t; /* a pointer to this type */

void timeout(timeout_func_t, void *, int);
void untimeout(timeout_func_t, void *);

/* 
 * Common `proc' functions are declared here so that proc.h can be included
 * less often.
 */
int	tsleep(void *chan, int pri, char *wmesg, int timo);
void	wakeup(void *chan);

#endif /* !_SYS_SYSTM_H_ */
