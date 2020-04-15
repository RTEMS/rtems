/*
 * Copyright (c) 2012 Chris Johns (chrisj@rtems.org)
 * Copyright (c) 2017 Gedare Bloom (gedare@rtems.org)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
