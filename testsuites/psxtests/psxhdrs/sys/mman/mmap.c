/*
 * This test file is used to verify that the header files associated with
 * invoking this function are correct.
 */

/*
 *  Copyright (c) 2016 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/mman.h>

void* test( void );

void* test( void )
{
  void *addr = NULL;
  size_t len = 0;
  int prot = 0, flags = 0, fildes = 0;
  off_t off = 0;

  return mmap( addr, len, prot, flags, fildes, off );
}
