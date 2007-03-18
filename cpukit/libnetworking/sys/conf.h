/*-
 * Copyright (c) 1990, 1993
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
 *	@(#)conf.h	8.5 (Berkeley) 1/9/95
 * $Id$
 */

#ifndef _SYS_CONF_H_
#define	_SYS_CONF_H_

/*
 * Definitions of device driver entry switches
 */

struct buf;
struct proc;
struct tty;
struct uio;
struct vnode;

typedef int d_open_t(dev_t dev, int oflags, int devtype, struct proc *p);
typedef int d_close_t(dev_t dev, int fflag, int devtype, struct proc *p);
typedef void d_strategy_t(struct buf *bp);
typedef int d_ioctl_t(dev_t dev, int cmd, caddr_t data,
			   int fflag, struct proc *p);
typedef int d_dump_t(dev_t dev);
typedef int d_psize_t(dev_t dev);

typedef int d_read_t(dev_t dev, struct uio *uio, int ioflag);
typedef int d_write_t(dev_t dev, struct uio *uio, int ioflag);
typedef void d_stop_t(struct tty *tp, int rw);
typedef int d_reset_t(dev_t dev);
typedef struct tty *d_devtotty_t(dev_t dev);
typedef int d_select_t(dev_t dev, int which, struct proc *p);
typedef int d_mmap_t(dev_t dev, int offset, int nprot);

typedef int l_open_t(dev_t dev, struct tty *tp);
typedef int l_close_t(struct tty *tp, int flag);
typedef int l_read_t(struct tty *tp, struct uio *uio, int flag);
typedef int l_write_t(struct tty *tp, struct uio *uio, int flag);
typedef int l_ioctl_t(struct tty *tp, int cmd, caddr_t data,
			   int flag, struct proc *p);
typedef int l_rint_t(int c, struct tty *tp);
typedef int l_start_t(struct tty *tp);
typedef int l_modem_t(struct tty *tp, int flag);

/*
 * Block device switch table
 */
struct bdevsw {
	d_open_t	*d_open;
	d_close_t	*d_close;
	d_strategy_t	*d_strategy;
	d_ioctl_t	*d_ioctl;
	d_dump_t	*d_dump;
	d_psize_t	*d_psize;
	int		d_flags;
	char 		*d_name;	/* name of the driver e.g. audio */
	struct cdevsw	*d_cdev; 	/* cross pointer to the cdev */
	int		d_maj;		/* the major number we were assigned */
};

#ifdef _KERNEL
extern struct bdevsw *bdevsw[];
#endif

/*
 * Character device switch table
 */
struct cdevsw {
	d_open_t	*d_open;
	d_close_t	*d_close;
	d_read_t	*d_read;
	d_write_t	*d_write;
	d_ioctl_t	*d_ioctl;
	d_stop_t	*d_stop;
	d_reset_t	*d_reset;	/* XXX not used */
	d_devtotty_t	*d_devtotty;
	d_select_t	*d_select;
	d_mmap_t	*d_mmap;
	d_strategy_t	*d_strategy;
	char		*d_name;	/* see above */
	struct bdevsw	*d_bdev;
	int		d_maj;
};

#ifdef _KERNEL
extern struct cdevsw *cdevsw[];
#endif

/*
 * Swap device table
 */
struct swdevt {
	dev_t	sw_dev;
	int	sw_flags;
	int	sw_nblks;
	struct	vnode *sw_vp;
};
#define	SW_FREED	0x01
#define	SW_SEQUENTIAL	0x02
#define	sw_freed	sw_flags	/* XXX compat */

#include <machine/conf.h>

#endif /* !_SYS_CONF_H_ */
