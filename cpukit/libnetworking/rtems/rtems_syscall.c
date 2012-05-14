#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdarg.h>
/* #include <stdlib.h> */
#include <stdio.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/proc.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/route.h>

/*
 *  Since we are "in the kernel", these do not get prototyped in sys/socket.h
 */
ssize_t	send(int, const void *, size_t, int);
ssize_t	recv(int, void *, size_t, int);

/*
 * Hooks to RTEMS I/O system
 */
static const rtems_filesystem_file_handlers_r socket_handlers;

/*
 * Convert an RTEMS file descriptor to a BSD socket pointer.
 */
struct socket *
rtems_bsdnet_fdToSocket (int fd)
{
  rtems_libio_t *iop;

  /* same as rtems_libio_check_fd(_fd) but different return */
  if ((uint32_t)fd >= rtems_libio_number_iops) {
    errno = EBADF;
    return NULL;
  }
  iop = &rtems_libio_iops[fd];

  /* same as rtems_libio_check_is_open(iop) but different return */
  if ((iop->flags & LIBIO_FLAGS_OPEN) == 0) {
    errno = EBADF;
    return NULL;
  }

  if (iop->pathinfo.handlers != &socket_handlers) {
    errno = ENOTSOCK;
    return NULL;
  }

  if (iop->data1 == NULL)
    errno = EBADF;
  return iop->data1;
}

/*
 * Create an RTEMS file descriptor for a socket
 */
static int
rtems_bsdnet_makeFdForSocket (void *so)
{
  rtems_libio_t *iop;
  int fd;

  iop = rtems_libio_allocate();
  if (iop == 0)
      rtems_set_errno_and_return_minus_one( ENFILE );

  fd = iop - rtems_libio_iops;
  iop->flags |= LIBIO_FLAGS_WRITE | LIBIO_FLAGS_READ;
  iop->data0 = fd;
  iop->data1 = so;
  iop->pathinfo.handlers = &socket_handlers;
  iop->pathinfo.mt_entry = &rtems_filesystem_null_mt_entry;
  rtems_filesystem_location_add_to_mt_entry(&iop->pathinfo);
  return fd;
}

/*
 * Package system call argument into mbuf.
 */
static int
sockargstombuf (struct mbuf **mp, const void *buf, int buflen, int type)
{
	struct mbuf *m;

	if ((u_int)buflen > MLEN)
		return (EINVAL);
	m = m_get(M_WAIT, type);
	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	memcpy (mtod(m, caddr_t), buf, buflen);
	*mp = m;
	if (type == MT_SONAME) {
		struct sockaddr *sa;
		sa = mtod(m, struct sockaddr *);
		sa->sa_len = buflen;
	}
	return 0;
}

/*
 *********************************************************************
 *                       BSD-style entry points                      *
 *********************************************************************
 */
int
socket (int domain, int type, int protocol)
{
	int fd;
	int error;
	struct socket *so;

	rtems_bsdnet_semaphore_obtain ();
	error = socreate(domain, &so, type, protocol, NULL);
	if (error == 0) {
		fd = rtems_bsdnet_makeFdForSocket (so);
		if (fd < 0)
			soclose (so);
	}
	else {
		errno = error;
		fd = -1;
	}
	rtems_bsdnet_semaphore_release ();
	return fd;
}

int
bind (int s, struct sockaddr *name, int namelen)
{
	int error;
	int ret = -1;
	struct socket *so;
	struct mbuf *nam;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) != NULL) {
		error = sockargstombuf (&nam, name, namelen, MT_SONAME);
		if (error == 0) {
			error = sobind (so, nam);
			if (error == 0)
				ret = 0;
			else
				errno = error;
			m_freem (nam);
		}
		else {
			errno = error;
		}
	}
	rtems_bsdnet_semaphore_release ();
	return ret;
}

int
connect (int s, struct sockaddr *name, int namelen)
{
	int error;
	int ret = -1;
	struct socket *so;
	struct mbuf *nam;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		errno = EALREADY;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = sockargstombuf (&nam, name, namelen, MT_SONAME);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = soconnect (so, nam);
	if (error)
		goto bad;
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		m_freem(nam);
		errno = EINPROGRESS;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0) {
		error = soconnsleep (so);
		if (error)
			break;
	}
	if (error == 0) {
		error = so->so_error;
		so->so_error = 0;
	}
    bad:
	so->so_state &= ~SS_ISCONNECTING;
	m_freem (nam);
	if (error)
		errno = error;
	else
		ret = 0;
	rtems_bsdnet_semaphore_release ();
	return ret;
}

int
listen (int s, int backlog)
{
	int error;
	int ret = -1;
	struct socket *so;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) != NULL) {
		error = solisten (so, backlog);
		if (error == 0)
			ret = 0;
		else
			errno = error;
	}
	rtems_bsdnet_semaphore_release ();
	return ret;
}

int
accept (int s, struct sockaddr *name, int *namelen)
{
	int fd;
	struct socket *head, *so;
	struct mbuf *nam;

	rtems_bsdnet_semaphore_obtain ();
	if ((head = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if ((head->so_options & SO_ACCEPTCONN) == 0) {
		errno = EINVAL;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
        if ((head->so_state & SS_NBIO) && head->so_comp.tqh_first == NULL) {
                errno = EWOULDBLOCK;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
        while (head->so_comp.tqh_first == NULL && head->so_error == 0) {
                if (head->so_state & SS_CANTRCVMORE) {
                        head->so_error = ECONNABORTED;
                        break;
                }
		head->so_error = soconnsleep (head);
        }
	if (head->so_error) {
		errno = head->so_error;
		head->so_error = 0;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}

	so = head->so_comp.tqh_first;
	TAILQ_REMOVE(&head->so_comp, so, so_list);
	head->so_qlen--;

	fd = rtems_bsdnet_makeFdForSocket (so);
	if (fd < 0) {
		TAILQ_INSERT_HEAD(&head->so_comp, so, so_list);
		head->so_qlen++;
		soconnwakeup (head);
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	so->so_state &= ~SS_COMP;
	so->so_head = NULL;

	nam = m_get(M_WAIT, MT_SONAME);
	(void) soaccept(so, nam);
	if (name) {
		 /* check length before it is destroyed */
		if (*namelen > nam->m_len)
			*namelen = nam->m_len;
		memcpy (name, mtod(nam, caddr_t), *namelen);
	}
	m_freem(nam);
	rtems_bsdnet_semaphore_release ();
	return (fd);

}

/*
 *  Shutdown routine
 */

int
shutdown (int s, int how)
{
      struct socket *so;
      int error;

      rtems_bsdnet_semaphore_obtain ();
      if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
              rtems_bsdnet_semaphore_release ();
              return -1;
      }
      error = soshutdown(so, how);
      rtems_bsdnet_semaphore_release ();
      if (error) {
              errno = error;
              return -1;
      }
      return 0;
}

/*
 * All `transmit' operations end up calling this routine.
 */
ssize_t
sendmsg (int s, const struct msghdr *mp, int flags)
{
	int ret = -1;
	int error;
	struct uio auio;
	struct iovec *iov;
	struct socket *so;
	struct mbuf *to;
	struct mbuf *control = NULL;
	int i;
	int len;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_offset = 0;
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if ((auio.uio_resid += iov->iov_len) < 0) {
			errno = EINVAL;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	if (mp->msg_name) {
		error = sockargstombuf (&to, mp->msg_name, mp->msg_namelen, MT_SONAME);
		if (error) {
			errno = error;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	else {
		to = NULL;
	}
	if (mp->msg_control) {
		if (mp->msg_controllen < sizeof (struct cmsghdr)) {
			errno = EINVAL;
			if (to)
				m_freem(to);
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
		sockargstombuf (&control, mp->msg_control, mp->msg_controllen, MT_CONTROL);
	}
	else {
		control = NULL;
	}
	len = auio.uio_resid;
	error = sosend (so, to, &auio, (struct mbuf *)0, control, flags);
	if (error) {
		if (auio.uio_resid != len && (error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error)
		errno = error;
	else
		ret = len - auio.uio_resid;
	if (to)
		m_freem(to);
	rtems_bsdnet_semaphore_release ();
	return (ret);
}

/*
 * Send a message to a host
 */
ssize_t
sendto (int s, const void *buf, size_t buflen, int flags, const struct sockaddr *to, int tolen)
{
	struct msghdr msg;
	struct iovec iov;

	iov.iov_base = (void *)buf;
	iov.iov_len = buflen;
	msg.msg_name = (caddr_t)to;
	msg.msg_namelen = tolen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	return sendmsg (s, &msg, flags);
}

/*
 * All `receive' operations end up calling this routine.
 */
ssize_t
recvmsg (int s, struct msghdr *mp, int flags)
{
	int ret = -1;
	int error;
	struct uio auio;
	struct iovec *iov;
	struct socket *so;
	struct mbuf *from = NULL, *control = NULL;
	int i;
	int len;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_offset = 0;
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if ((auio.uio_resid += iov->iov_len) < 0) {
			errno = EINVAL;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	len = auio.uio_resid;
	mp->msg_flags = flags;
	error = soreceive (so, &from, &auio, (struct mbuf **)NULL,
			mp->msg_control ? &control : (struct mbuf **)NULL,
			&mp->msg_flags);
	if (error) {
		if (auio.uio_resid != len && (error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error) {
		errno = error;
	}
	else {
		ret = len - auio.uio_resid;
		if (mp->msg_name) {
			len = mp->msg_namelen;
			if ((len <= 0) || (from == NULL)) {
				len = 0;
			}
			else {
				if (len > from->m_len)
					len = from->m_len;
				memcpy (mp->msg_name, mtod(from, caddr_t), len);
			}
			mp->msg_namelen = len;
		}
		if (mp->msg_control) {
			struct mbuf *m;
			void *ctlbuf;

			len = mp->msg_controllen;
			m = control;
			mp->msg_controllen = 0;
			ctlbuf = mp->msg_control;

			while (m && (len > 0)) {
				unsigned int tocopy;

				if (len >= m->m_len)
					tocopy = m->m_len;
				else {
					mp->msg_flags |= MSG_CTRUNC;
					tocopy = len;
				}
				memcpy(ctlbuf, mtod(m, caddr_t), tocopy);
				ctlbuf += tocopy;
				len -= tocopy;
				m = m->m_next;
			}
			mp->msg_controllen = ctlbuf - mp->msg_control;
		}
	}
	if (from)
		m_freem (from);
	if (control)
		m_freem (control);
	rtems_bsdnet_semaphore_release ();
	return (ret);
}

/*
 * Receive a message from a host
 */
ssize_t
recvfrom (int s, void *buf, size_t buflen, int flags, const struct sockaddr *from, int *fromlen)
{
	struct msghdr msg;
	struct iovec iov;
	int ret;

	iov.iov_base = buf;
	iov.iov_len = buflen;
	msg.msg_name = (caddr_t)from;
	if (fromlen)
		msg.msg_namelen = *fromlen;
	else
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	ret = recvmsg (s, &msg, flags);
	if ((from != NULL) && (fromlen != NULL) && (ret >= 0))
		*fromlen = msg.msg_namelen;
	return ret;
}

int
setsockopt (int s, int level, int name, const void *val, int len)
{
	struct socket *so;
	struct mbuf *m = NULL;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (len > MLEN) {
		errno = EINVAL;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (val) {
		error = sockargstombuf (&m, val, len, MT_SOOPTS);
		if (error) {
			errno = error;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	error = sosetopt(so, level, name, m);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	rtems_bsdnet_semaphore_release ();
	return 0;
}

int
getsockopt (int s, int level, int name, void *aval, int *avalsize)
{
	struct socket *so;
	struct mbuf *m = NULL, *m0;
	char *val = aval;
	int i, op, valsize;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (val)
		valsize = *avalsize;
	else
		valsize = 0;
	if (((error = sogetopt(so, level, name, &m)) == 0) && val && valsize && m) {
		op = 0;
		while (m && op < valsize) {
			i = valsize - op;
			if (i > m->m_len)
				i = m->m_len;
			memcpy (val, mtod(m, caddr_t), i);
			op += i;
			val += i;
			m0 = m;
			MFREE (m0, m);
		}
		*avalsize = op;
	}
	if (m != NULL)
		(void) m_free(m);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	rtems_bsdnet_semaphore_release ();
	return 0;
}

static int
getpeersockname (int s, struct sockaddr *name, int *namelen, int pflag)
{
	struct socket *so;
	struct mbuf *m;
	int len = *namelen;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	m = m_getclr(M_WAIT, MT_SONAME);
	if (m == NULL) {
		errno = ENOBUFS;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (pflag)
		error = (*so->so_proto->pr_usrreqs->pru_peeraddr)(so, m);
	else
		error = (*so->so_proto->pr_usrreqs->pru_sockaddr)(so, m);
	if (error) {
		m_freem(m);
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (len > m->m_len) {
		len = m->m_len;
		*namelen = len;
	}
	memcpy (name, mtod(m, caddr_t), len);
	m_freem (m);
	rtems_bsdnet_semaphore_release ();
	return 0;
}

int
getpeername (int s, struct sockaddr *name, int *namelen)
{
	return getpeersockname (s, name, namelen, 1);
}
int
getsockname (int s, struct sockaddr *name, int *namelen)
{
	return getpeersockname (s, name, namelen, 0);
}

int
sysctl(int *name, u_int namelen, void *oldp,
       size_t *oldlenp, void *newp, size_t newlen)
{
  int    error;
	size_t j;

  rtems_bsdnet_semaphore_obtain ();
  error = userland_sysctl (0, name, namelen, oldp, oldlenp, 1, newp, newlen, &j);
  rtems_bsdnet_semaphore_release ();

  if (oldlenp)
    *oldlenp = j;

  if (error)
  {
    errno = error;
    return -1;
  }
  return 0;
}

/*
 ************************************************************************
 *                      RTEMS I/O HANDLER ROUTINES                      *
 ************************************************************************
 */
static int
rtems_bsdnet_close (rtems_libio_t *iop)
{
	struct socket *so;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = iop->data1) == NULL) {
		errno = EBADF;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = soclose (so);
	rtems_bsdnet_semaphore_release ();
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

static ssize_t
rtems_bsdnet_read (rtems_libio_t *iop, void *buffer, size_t count)
{
	return recv (iop->data0, buffer, count, 0);
}

static ssize_t
rtems_bsdnet_write (rtems_libio_t *iop, const void *buffer, size_t count)
{
	return send (iop->data0, buffer, count, 0);
}

static int
so_ioctl (rtems_libio_t *iop, struct socket *so, uint32_t   command, void *buffer)
{
	switch (command) {
	case FIONBIO:
		if (*(int *)buffer) {
			iop->flags |= O_NONBLOCK;
			so->so_state |= SS_NBIO;
		}
		else {
			iop->flags &= ~O_NONBLOCK;
			so->so_state &= ~SS_NBIO;
		}
		return 0;

	case FIONREAD:
		*(int *)buffer = so->so_rcv.sb_cc;
		return 0;
	}

	if (IOCGROUP(command) == 'i')
		return ifioctl (so, command, buffer, NULL);
	if (IOCGROUP(command) == 'r')
		return rtioctl (command, buffer, NULL);
	return (*so->so_proto->pr_usrreqs->pru_control)(so, command, buffer, 0);
}

static int
rtems_bsdnet_ioctl (rtems_libio_t *iop, uint32_t   command, void *buffer)
{
	struct socket *so;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = iop->data1) == NULL) {
		errno = EBADF;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = so_ioctl (iop, so, command, buffer);
	rtems_bsdnet_semaphore_release ();
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

static int
rtems_bsdnet_fcntl (rtems_libio_t *iop, int cmd)
{
	struct socket *so;

	if (cmd == F_SETFL) {
		rtems_bsdnet_semaphore_obtain ();
		if ((so = iop->data1) == NULL) {
			rtems_bsdnet_semaphore_release ();
			return EBADF;
		}
		if (iop->flags & LIBIO_FLAGS_NO_DELAY)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		rtems_bsdnet_semaphore_release ();
	}
        return 0;
}

static int
rtems_bsdnet_fstat (const rtems_filesystem_location_info_t *loc, struct stat *sp)
{
	sp->st_mode = S_IFSOCK;
	return 0;
}

static const rtems_filesystem_file_handlers_r socket_handlers = {
	rtems_filesystem_default_open,		/* open */
	rtems_bsdnet_close,			/* close */
	rtems_bsdnet_read,			/* read */
	rtems_bsdnet_write,			/* write */
	rtems_bsdnet_ioctl,			/* ioctl */
	rtems_filesystem_default_lseek,		/* lseek */
	rtems_bsdnet_fstat,			/* fstat */
	rtems_filesystem_default_ftruncate,	/* ftruncate */
	rtems_filesystem_default_fsync_or_fdatasync,	/* fsync */
	rtems_filesystem_default_fsync_or_fdatasync,	/* fdatasync */
	rtems_bsdnet_fcntl 			/* fcntl */
};
