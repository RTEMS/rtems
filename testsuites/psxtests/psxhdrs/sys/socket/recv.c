/**
 *  @file
 *
 *  This test file is used to verify that the recv() method has the
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

ssize_t test(void);

ssize_t test(void)
{
  int     sockfd = 4;
  int     buffer;
  void   *buf = &buffer;
  size_t  len = sizeof(buffer);
  int     flags = 7;


  return recv(sockfd, buf, len, flags);
}
