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
  void   *addr __attribute__((unused)),
  size_t  lenhth __attribute__((unused)),
  int     prot __attribute__((unused)),
  int     flags __attribute__((unused)),
  int     fildes __attribute__((unused)),
  off_t   off
)
{
  return MAP_FAILED;
}
