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
 *	@(#)systm.h	8.7 (Berkeley) 3/29/95
 * $Id$
 */

#ifndef _SYS_SYSTM_H_
#define	_SYS_SYSTM_H_

#include <rtems/rtems_bsdnet_internal.h> /* for __BSD_VA_LIST__ */
#include <machine/cpufunc.h>

extern int securelevel;		/* system security level (see init(8)) */

extern int cold;		/* nonzero if we are doing a cold boot */
extern const char *panicstr;	/* panic message */
extern char version[];		/* system version */
extern char copyright[];	/* system copyright */

extern int nblkdev;		/* number of entries in bdevsw */
extern int nchrdev;		/* number of entries in cdevsw */
extern struct swdevt *swdevt;	/* swap-device information */
extern int nswdev;		/* number of swap devices */
extern int nswap;		/* size of swap space */

extern int selwait;		/* select timeout address */

extern u_char curpriority;	/* priority of current process */

extern int physmem;		/* physical memory */

extern dev_t dumpdev;		/* dump device */
extern long dumplo;		/* offset into dumpdev */

extern dev_t rootdev;		/* root device */
extern struct vnode *rootvp;	/* vnode equivalent to above */

extern dev_t swapdev;		/* swapping device */
extern struct vnode *swapdev_vp;/* vnode equivalent to above */

extern int boothowto;		/* reboot flags, from console subsystem */
extern int bootverbose;		/* nonzero to print verbose messages */

/*
 * General function declarations.
 */
void	Debugger(const char *msg);
int	nullop(void);
int	eopnotsupp(void);
int	einval(void);
int	seltrue(dev_t dev, int which, struct proc *p);
int	ureadc(int, struct uio *);
void	*hashinit(int count, int type, u_long *hashmask);
void	*phashinit(int count, int type, u_long *nentries);

void	panic(const char *, ...)) __dead2;
void	boot(int)) __dead2;
void	cpu_boot(int);
void	tablefull(const char *);
int	addlog(const char *, ...);
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
int	copyin(const void *udaddr, void *kaddr, size_t len);
int	copyout(const void *kaddr, void *udaddr, size_t len);

int	fubyte(const void *base);
int	fuibyte(const void *base);
int	subyte(void *base, int byte);
int	suibyte(void *base, int byte);
int	fuword(const void *base);
int	suword(void *base, int word);
int	susword(void *base, int word);

int	hzto(struct timeval *tv);
void	realitexpire(void *);

struct clockframe;
void	hardclock(struct clockframe *frame);
void	softclock(void);
void	statclock(struct clockframe *frame);

void	startprofclock(struct proc *);
void	stopprofclock(struct proc *);
void	setstatclockrate(int hzrate);

void	hardupdate(long);
void	hardpps(struct timeval *tvp, long usec);

#include <sys/libkern.h>

/* Initialize the world */
extern void consinit(void);
extern void usrinfoinit(void);
extern void cpu_initclocks(void);
extern void vntblinit(void);
extern void nchinit(void);

/* Finalize the world. */
void	shutdown_nice(void);

/*
 * Kernel to clock driver interface.
 */
void	inittodr(time_t base);
void	resettodr(void);
void	startrtclock(void);

/* Timeouts */
typedef void (timeout_t)(void *); /* actual timeout function type */
typedef timeout_t *timeout_func_t; /* a pointer to this type */

void timeout(timeout_func_t, void *, int);
void untimeout(timeout_func_t, void *);
void	logwakeup(void);

/* Various other callout lists that modules might want to know about */
/* shutdown callout list definitions */
typedef void (*bootlist_fn)(int,void *);
int at_shutdown(bootlist_fn function, void *arg, int);
int rm_at_shutdown(bootlist_fn function, void *arg);
#define SHUTDOWN_PRE_SYNC 0
#define SHUTDOWN_POST_SYNC 1

/* forking */ /* XXX not yet */
typedef void (*forklist_fn)(struct proc *parent,struct proc *child,int flags);
int at_fork(forklist_fn function);
int rm_at_fork(forklist_fn function);

/* exiting */
typedef void (*exitlist_fn)(struct proc *procp);
int at_exit(exitlist_fn function);
int rm_at_exit(exitlist_fn function);

/* Not exactly a callout LIST, but a callout entry.. 			*/
/* Allow an external module to define a hardware watchdog tickler	*/
/* Normally a process would do this, but there are times when the	*/
/* kernel needs to be able to hold off the watchdog, when the process	*/
/* is not active, e.g. when dumping core. Costs us a whole 4 bytes to	*/
/* make this generic. the variable is in kern_shutdown.c */
typedef void (*watchdog_tickle_fn)(void);
extern watchdog_tickle_fn wdog_tickler;


/* 
 * Common `proc' functions are declared here so that proc.h can be included
 * less often.
 */
int	tsleep(void *chan, int pri, char *wmesg, int timo);
void	wakeup(void *chan);

#endif /* !_SYS_SYSTM_H_ */
