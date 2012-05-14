/*
 *  socketpair() for RTEMS
 *
 *  This file exists primarily to document what is required to provide
 *  a functional implementation of socketpair() for RTEMS.
 *
 *  The socketpair() service requires that the "local domain" sockets
 *  be functional.  This is denoted by the domain constants AF_LOCAL
 *  and AF_UNIX and the protocol constants PF_LOCAL and PF_UNIX.  The
 *  local domain functionality is implemented in the file kern/uipc_usrreq.c
 *  which was not part of the initial port of the FreeBSD stack to
 *  RTEMS.
 *
 *  The FreeBSD socketpair implementation appears to be dependent on
 *  file system features which are not available currently in RTEMS.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

int socketpair (int domain, int type, int protocol, int *rsv)
{
  if ( !rsv ) {
    errno = EFAULT;
    return -1;
  }

  /*
   *  Yes, we do not support socketpair() so this is really paranoid.
   *  But it ensures that someone calling this routine and ignoring
   *  the return status will get errors from subsequent socket calls.
   */
  rsv[ 0 ] = -1;
  rsv[ 1 ] = -1;
  errno = ENOSYS;
  return -1;
}
