/**
 *  @file
 *
 *  This test file is used to verify that the socket() method has the
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
  int     domain;
  int     type;
  int     protocol = 1;

  /* use primary values */
  domain = AF_UNIX;
  domain = AF_LOCAL;
  domain = AF_INET;

  /* use some types */
  type = SOCK_STREAM;
  type = SOCK_DGRAM;
  type = SOCK_SEQPACKET;
  type = SOCK_RAW;
  type = SOCK_RDM;

  return socket(domain, type, protocol);
}
