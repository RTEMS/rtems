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

int munmap(
  void   *addr __attribute__((unused)),
  size_t  length __attribute__((unused))
)
{
  return -1;
}
