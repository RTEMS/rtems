/*
 * Copyright (c) 2012 Chris Johns (chrisj@rtems.org)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <rtems/posix/mmanimpl.h> 

int munmap(
  void *addr, size_t len
)
{
  mmap_mapping*     mapping;
  rtems_chain_node* node;
  
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

  /*
   * Obtain the mmap lock. Sets errno on failure.
   */
  if ( !mmap_mappings_lock_obtain( ))
    return -1;

  node = rtems_chain_first (&mmap_mappings);
  while ( !rtems_chain_is_tail( &mmap_mappings, node )) {
    /*
     * If the map is fixed see if this address is already mapped. At this
     * point in time if there is an overlap in the mappings we return an
     * error.
     */
    mapping = (mmap_mapping*) node;
    if ( ( addr >= mapping->addr ) &&
         ( addr < ( mapping->addr + mapping->len )) ) {
      rtems_chain_extract( node );
      if (( mapping->flags & MAP_FIXED ) != MAP_FIXED ) {
        free( mapping->addr );
        free( mapping );
      }
      break;
    }
    node = rtems_chain_next( node );
  }

  mmap_mappings_lock_release( );
  return 0;
}
