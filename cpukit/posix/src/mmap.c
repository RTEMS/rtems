/**
 * @file
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/mman.h>

void *mmap(
  void   *addr,
  size_t  lenhth,
  int     prot,
  int     flags,
  int     fildes,
  off_t   off
)
{
  (void) addr;
  (void) lenhth;
  (void) prot;
  (void) flags;
  (void) fildes;
  (void) off;
  return MAP_FAILED;
}
