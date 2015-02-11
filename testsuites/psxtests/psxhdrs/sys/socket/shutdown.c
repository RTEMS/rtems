/**
 *  @file
 *
 *  This test file is used to verify that the shutdown() method has the
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
  int     how;

  /* use all values */
  how = SHUT_RD;
  how = SHUT_WR;
  how = SHUT_RDWR;

  return shutdown(sockfd, how);
}
