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

int test( void );

int test( void )
{
  void *addr = NULL;
  size_t len = 0;

  return munmap( addr, len );
}
