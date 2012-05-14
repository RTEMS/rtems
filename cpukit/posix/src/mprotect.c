/*
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

/*
 *  12.2.3 Change Memory Protection, P1003.1b-1996, p. 277.
 *
 *  This is not a functional version but the SPARC backend for at least
 *  gcc 2.8.1 plus gnat 3.13p and gcc 3.0.1 require it to be there and
 *  return 0.
 *
 *  As of gcc 4.2.2, the gcc SPARC backend doesn't appear to have a
 *  way to call this for RTEMS anymore but it doesn't hurt to leave it.
 */

int mprotect(
  const void *addr __attribute__((unused)),
  size_t len __attribute__((unused)),
  int prot __attribute__((unused)) )
{
  return 0;
}
