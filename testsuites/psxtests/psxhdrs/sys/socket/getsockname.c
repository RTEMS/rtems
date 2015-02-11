/**
 *  @file
 *
 *  This test file is used to verify that the getsockname() method has the
 *  correct signature.
 */

/*
 *  COPYRIGHT (c) 2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/socket.h>

int test(void);

int test(void)
{
  int              sockfd = 4;
  struct  sockaddr addr;
  socklen_t        addrlen;

  return getsockname(sockfd, &addr, &addrlen);
}
