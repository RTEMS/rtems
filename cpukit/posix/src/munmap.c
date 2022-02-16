/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2012 Chris Johns (chrisj@rtems.org)
 * Copyright (c) 2017 Gedare Bloom (gedare@rtems.org)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <rtems/posix/mmanimpl.h>
#include <rtems/posix/shmimpl.h>

int munmap(void *addr, size_t len)
{
  mmap_mapping     *mapping;
  rtems_chain_node *node;

  /*
   * Clear errno.
   */
  errno = 0;

  /*
   * Length cannot be 0.
   */
  if ( len == 0 ) {
    errno = EINVAL;
    return -1;
  }

  /* Check for illegal addresses. Watch out for address wrap. */
  if (addr + len < addr) {
    errno = EINVAL;
    return -1;
  }

  mmap_mappings_lock_obtain();

  node = rtems_chain_first (&mmap_mappings);
  while ( !rtems_chain_is_tail( &mmap_mappings, node )) {
    mapping = (mmap_mapping*) node;
    if ( ( addr >= mapping->addr ) &&
         ( addr < ( mapping->addr + mapping->len )) ) {
      rtems_chain_extract_unprotected( node );

      /* FIXME: generally need a way to clean-up the backing object, but
       * currently it only matters for MAP_SHARED shm objects. */
      if ( mapping->shm != NULL ) {
        POSIX_Shm_Attempt_delete(mapping->shm);
      }

      /* only free the mapping address for non-fixed mapping */
      if (( mapping->flags & MAP_FIXED ) != MAP_FIXED ) {
        /* only free the mapping address for non-shared mapping, because we
         * re-use the mapping address across all of the shared mappings, and
         * it is memory managed independently... */
        if (( mapping->flags & MAP_SHARED ) != MAP_SHARED ) {
          free( mapping->addr );
        }
      }
      free( mapping );
      break;
    }
    node = rtems_chain_next( node );
  }

  mmap_mappings_lock_release( );
  return 0;
}
