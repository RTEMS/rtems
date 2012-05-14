/* xdr_mbuf is derived from xdr_mem */

/* Author (mbuf specifica): Till Straumann <strauman@slac.stanford.edu>, 10/2002 */

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)xdr_mem.c 1.19 87/08/11 Copyr 1984 Sun Micro";*/
/*static char *sccsid = "from: @(#)xdr_mem.c	2.1 88/07/29 4.0 RPCSRC";*/
static char *rcsid = "$FreeBSD: src/lib/libc/xdr/xdr_mem.c,v 1.8 1999/08/28 00:02:56 peter Exp $";
#endif

/*
 * xdr_mbuf,  XDR implementation using mbuf buffers
 *
 * derived from:
 *
 * xdr_mem.h, XDR implementation using memory buffers.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * The MBUF stream is useful for BSDNET kernel (or RTEMS for that matter)
 * use.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <netinet/in.h>

#include <stdlib.h>

#define TODO

/* TODO remove: a hack because malloc is redefined */
#ifdef TODO
static inline void *
my_malloc(size_t i)
{
	return malloc(i);
}

static inline void
my_free(void *p)
{
	return free(p);
}
#endif

#define DEBUG_ASSERT	(1<<0)
#define DEBUG_VERB		(1<<1)

#define DEBUG			DEBUG_ASSERT

#define _KERNEL
#include <sys/mbuf.h>

#include <assert.h>

#if DEBUG & DEBUG_VERB || defined(TODO)
#include <stdio.h>
#endif

static bool_t	xdrmbuf_getlong_aligned(XDR *xdrs, long *lp);
static bool_t	xdrmbuf_putlong_aligned(XDR *xdrs, const long *lp);
static bool_t	xdrmbuf_getlong_unaligned(XDR *xdrs, long *lp);
static bool_t	xdrmbuf_putlong_unaligned(XDR *xdrs, const long *lp);
static bool_t	xdrmbuf_getbytes(XDR *xdrs, caddr_t addr, u_int len);
static bool_t	xdrmbuf_putbytes(XDR *xdrs, const char *addr, u_int len);
static u_int	xdrmbuf_getpos(XDR *xdrs); /* XXX w/64-bit pointers, u_int not enough! */
static bool_t	xdrmbuf_setpos(XDR *xdrs, u_int pos);
static int32_t *xdrmbuf_inline_aligned(XDR *xdrs, u_int len);
static int32_t *xdrmbuf_inline_unaligned(XDR *xdrs, u_int len);
static void	xdrmbuf_destroy(XDR *);

static struct	xdr_ops xdrmbuf_ops_aligned = {
	xdrmbuf_getlong_aligned,
	xdrmbuf_putlong_aligned,
	xdrmbuf_getbytes,
	xdrmbuf_putbytes,
	xdrmbuf_getpos,
	xdrmbuf_setpos,
	xdrmbuf_inline_aligned,
	xdrmbuf_destroy
};

static struct	xdr_ops xdrmbuf_ops_unaligned = {
	xdrmbuf_getlong_unaligned,
	xdrmbuf_putlong_unaligned,
	xdrmbuf_getbytes,
	xdrmbuf_putbytes,
	xdrmbuf_getpos,
	xdrmbuf_setpos,
	xdrmbuf_inline_unaligned,
	xdrmbuf_destroy
};

typedef struct MBPrivateRec_ {
	struct mbuf		*mchain;
	struct mbuf		*mcurrent;
	u_int			pos;		/* number of bytes contained in all MUBFS ahead
								 * of mcurrent
								 */
} MBPrivateRec, *MBPrivate;

/* NOTE: the stream position helper 'pos'
 *       must be managed by the caller!
 */
static inline void
xdrmbuf_setup(XDR *xdrs, struct mbuf *m)
{
MBPrivate	mbp = (MBPrivate)xdrs->x_base;

		mbp->mcurrent    = m;
		xdrs->x_private  = mtod(m,caddr_t);
		xdrs->x_handy    = m->m_len;
		xdrs->x_ops      = ((uintptr_t)xdrs->x_private & (sizeof(int32_t) - 1))
								? &xdrmbuf_ops_unaligned : &xdrmbuf_ops_aligned;
}

static struct mbuf *
xdrmbuf_next(XDR *xdrs)
{
struct mbuf		*rval;
MBPrivate		mbp = (MBPrivate)xdrs->x_base;

	if (mbp->mcurrent) {
		mbp->pos += mbp->mcurrent->m_len;
		rval      = mbp->mcurrent->m_next;
	} else {
		rval      = 0;
	}

	if (rval) {
		xdrmbuf_setup(xdrs, rval);
	}
#if DEBUG & DEBUG_VERB
	else {
		fprintf(stderr,"xdrmbuf: end of chain\n");
	}
#endif

	return rval;
}

/*
 * The procedure xdrmbuf_create initializes a stream descriptor for a
 * memory buffer.
 */
void
xdrmbuf_create(XDR *xdrs, struct mbuf *mbuf, enum xdr_op op)
{
MBPrivate mbp;

	xdrs->x_op = op;
	mbp = (MBPrivate)my_malloc(sizeof(*mbp));
	assert( mbp );
	xdrs->x_base  = (caddr_t) mbp;

	mbp->mchain   = mbuf;
	mbp->pos      = 0;

#if DEBUG & DEBUG_VERB
	{
	struct mbuf *mbf;
	fprintf(stderr,"Dumping chain:\n");
	for (mbf = mbuf; mbf; mbf=mbf->m_next) {
		int ii;
		fprintf(stderr,"MBUF------------");
		for (ii=0; ii<mbf->m_len; ii++) {
			fprintf(stderr,"%02x ",mtod(mbf,char*)[ii]);
			if (ii%16==0)
				fputc('\n',stderr);
		}
		fputc('\n',stderr);
	}
	}
#endif

	xdrmbuf_setup(xdrs, mbuf);
}

static void
xdrmbuf_destroy(XDR *xdrs)
{
MBPrivate	mbp = (MBPrivate)xdrs->x_base;
#if 0 /* leave destroying the chain to the user */
struct mbuf *m = mbp->mchain;

	rtems_bsdnet_semaphore_obtain();
		m_freem(m);
	rtems_bsdnet_semaphore_release();
#endif

	my_free(mbp);
}

static bool_t
xdrmbuf_getlong_aligned(register XDR *xdrs, register long *lp)
{
	while ( (signed int)(xdrs->x_handy -= sizeof(int32_t)) < 0) {
		if ((xdrs->x_handy += sizeof(int32_t)) == 0) {
			/* handy was 0 on entry; request a new buffer.
			 * Coded this way, so the most frequently executed
			 * path needs only one comparison...
			 */
			if (!xdrmbuf_next(xdrs))
				return FALSE;
		} else {
			/* uh-oh an aligned long spread over two MBUFS ??
			 * let the unaligned handler deal with this rare
			 * situation.
			 */
			return xdrmbuf_getlong_unaligned(xdrs,lp);
		}
	}
	*lp = ntohl(*(int32_t *)(xdrs->x_private));
	xdrs->x_private += sizeof(int32_t);
#if DEBUG & DEBUG_VERB
	fprintf(stderr,"Got aligned long %x\n",*lp);
#endif
	return (TRUE);
}

static bool_t
xdrmbuf_putlong_aligned(
	XDR *xdrs,
	const long *lp)
{
fprintf(stderr,"TODO: xdrmbuf_putlong_aligned() is unimplemented\n");
	return FALSE;
#if 0
	if ((xdrs->x_handy -= sizeof(int32_t)) < 0)
		return (FALSE);
	*(int32_t *)xdrs->x_private = htonl(*lp);
	xdrs->x_private += sizeof(int32_t);
	return (TRUE);
#endif
}

static bool_t
xdrmbuf_getlong_unaligned(
	XDR *xdrs,
	long *lp)
{
union {
	int32_t l;
	char	c[sizeof(int32_t)];
}	u;

register int  i,j;
register char *cp,*sp;

	i = xdrs->x_handy - sizeof(int32_t);

	/* handle the most common case first */
	if ( i >= 0 ) {

		xdrs->x_handy   = i;
		sp			    = (char*)xdrs->x_private;
		xdrs->x_private = sp + sizeof(int32_t);

#ifdef CANDO_UNALIGNED
		{
			*lp              = ntohl(*(int32_t *)sp);
#        if DEBUG & DEBUG_VERB
			fprintf(stderr,"Got unaligned long %x (%i remaining)\n",*lp, xdrs->x_handy);
#        endif
			return TRUE;
		}
#else   /* machine can't do unaligned access */
		{
			u.c[0]          = *sp;
			u.c[1]          = *++sp;
			u.c[2]          = *++sp;
			u.c[3]          = *++sp;

			goto done;
		}
#endif /* CANDO_UNALIGNED */
	}

	/* here the messy 'crossing buffers' business starts */


	j  = sizeof(int32_t);

	cp = u.c-1;

	/* NOTE: on entry to this section,  handy < j holds */
	do {
		sp = ((char*)xdrs->x_private)-1;

		if ( (i=xdrs->x_handy) >= j ) {
			/* more data in the buffer than we need:
			 * copy everything we need and goto 'done'
			 */
			xdrs->x_handy    = i-j;
			do {
				*++cp = *++sp;
			} while (--j > 0);
			xdrs->x_private  = (caddr_t)++sp;

			goto done;

		} else {
			/* not enough data - copy as much as possible
			 * then get retrieve the next MBUF and start
			 * over
			 */
			j-=i;
			while (i--)
				*++cp = *++sp;
			if (!xdrmbuf_next(xdrs))
				return FALSE;
#if DEBUG & DEBUG_VERB
			fprintf(stderr,"getlong_unaligned: crossed mbuf boundary\n");
#endif
		}
	} while (j > 0);

done:

	*lp = ntohl(u.l);

#if DEBUG & DEBUG_VERB
	fprintf(stderr,"Got unaligned long %x (%i remaining)\n",*lp, xdrs->x_handy);
#endif
	return (TRUE);
}

static bool_t
xdrmbuf_putlong_unaligned(
	XDR *xdrs,
	const long *lp )
{

	fprintf(stderr,"TODO: xdrmbuf_putlong_unaligned() is unimplemented\n");
	return FALSE;
#if 0
	{
	int32_t l;

	if ((xdrs->x_handy -= sizeof(int32_t)) < 0)
		return (FALSE);
	l = htonl(*lp);
	memcpy(xdrs->x_private, &l, sizeof(int32_t));
	xdrs->x_private += sizeof(int32_t);
	return (TRUE);
	}
#endif
}

static bool_t
xdrmbuf_getbytes(
	XDR *xdrs,
	caddr_t addr,
	u_int len)
{
#if DEBUG & DEBUG_VERB
int	olen=len,bufs=0;
#endif

#if DEBUG & DEBUG_VERB
	fprintf(stderr,"wanting %i bytes (have %i)\n",olen,xdrs->x_handy);
#endif

	while (len>0) {
		if (xdrs->x_handy >= len) {
			memcpy(addr, xdrs->x_private, len);
			xdrs->x_private += len;
			xdrs->x_handy   -= len;
#if 0 /* save a couple of instructions */
			len = 0;
#else
			goto done;
#endif
		} else {
			if (xdrs->x_handy > 0) {
				memcpy(addr, xdrs->x_private, xdrs->x_handy);
				len  -= xdrs->x_handy;
				addr += xdrs->x_handy;
			}
			if (!xdrmbuf_next(xdrs))
				return FALSE;
#if DEBUG & DEBUG_VERB
			bufs++;
#endif
		}
	}
done:
#if DEBUG & DEBUG_VERB
	fprintf(stderr,"Got %i bytes (out of %i mbufs)\n",olen,bufs);
#endif
	return (TRUE);
}

static bool_t
xdrmbuf_putbytes(
	XDR *xdrs,
	const char *addr,
	u_int len )
{

	fprintf(stderr,"TODO: xdrmbuf_putbytes() is unimplemented\n");
	return FALSE;
#if 0
	if ((xdrs->x_handy -= len) < 0)
		return (FALSE);
	memcpy(xdrs->x_private, addr, len);
	xdrs->x_private += len;
	return (TRUE);
#endif
}

static u_int
xdrmbuf_getpos(
	XDR *xdrs)
{
#if 1
MBPrivate	mbp  = (MBPrivate)xdrs->x_base;
struct mbuf *m   = mbp->mcurrent;
u_int       rval = mbp->pos;

	if (m) {
		rval += xdrs->x_private - mtod(m, void*);
	}
#else
struct mbuf *m;
u_int 		rval = 0;
MBPrivate	mbp  = (MBPrivate)xdrs->x_base;

	for ( m = mbp->mchain; m && m != mbp->mcurrent; m = m->m_next )
		rval += m->m_len;
	if (m) {
		rval += (u_long)xdrs->x_private - mtod(m, u_long);
	}

#endif
	return rval;
}

static bool_t
xdrmbuf_setpos(
	XDR *xdrs,
	u_int pos)
{
struct		mbuf *m;
MBPrivate	mbp   = (MBPrivate)xdrs->x_base;

	if (pos >= mbp->pos) {
		pos      -= mbp->pos;
		m         = mbp->mcurrent;
	} else {
		m         = mbp->mchain;
		mbp->pos  = 0;
	}

	while ( m && pos >= m->m_len ) {
		pos      -= m->m_len;
		mbp->pos += m->m_len;
		m         = m->m_next;
	}

	if (m) {
		xdrmbuf_setup(xdrs, m);
		xdrs->x_private += pos;
		return TRUE;
	}

	return 0 == pos ? TRUE : FALSE;
}

static int32_t *
xdrmbuf_inline_aligned(
	XDR *xdrs,
	u_int len)
{
int32_t	*buf = 0;

	if (xdrs->x_handy == 0 && !xdrmbuf_next(xdrs))
		return 0;

	if (xdrs->x_handy >= len) {
		xdrs->x_handy -= len;
		buf = (int32_t *) xdrs->x_private;
		xdrs->x_private += len;
#if DEBUG & DEBUG_VERB
		fprintf(stderr,"Got %i aligned inline bytes at %x\n", len, buf);
#endif
	}
#if DEBUG & DEBUG_VERB
	else {
		fprintf(stderr,"Skipped %i aligned inline bytes\n",len);
	}
#endif
	return (buf);
}

static int32_t *
xdrmbuf_inline_unaligned(
	XDR *xdrs,
	u_int len )
{
	return (0);
}
