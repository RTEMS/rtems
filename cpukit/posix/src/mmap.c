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
#include <sys/stat.h>
#include <unistd.h>

#include "rtems/libio_.h"

#include <rtems/posix/mmanimpl.h> 

#define RTEMS_MUTEX_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
   RTEMS_NO_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/**
 * Mmap chain of mappings.
 */
rtems_chain_control mmap_mappings;

/**
 * The id of the MMAP lock.
 */
static rtems_id mmap_mappings_lock;

/**
 * Create the lock.
 */
static
bool mmap_mappings_lock_create(
  void
)
{
  /*
   * Lock the mapping table. We only create a lock if a call is made. First we
   * test if a mapping lock is present. If one is present we lock it. If not
   * the libio lock is locked and we then test the mapping lock again. If not
   * present we create the mapping lock then release libio lock.
   */
  if ( mmap_mappings_lock == 0 ) {
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    rtems_chain_initialize_empty( &mmap_mappings );
    rtems_semaphore_obtain( rtems_libio_semaphore,
                            RTEMS_WAIT, RTEMS_NO_TIMEOUT );
    if ( mmap_mappings_lock == 0 )
      sc = rtems_semaphore_create( rtems_build_name( 'M', 'M', 'A', 'P' ),
                                   1,
                                   RTEMS_MUTEX_ATTRIBS,
                                   RTEMS_NO_PRIORITY,
                                   &mmap_mappings_lock );
    rtems_semaphore_release( rtems_libio_semaphore );
    if ( sc != RTEMS_SUCCESSFUL ) {
      errno = EINVAL;
      return false;
    }
  }
  return true;
}

bool mmap_mappings_lock_obtain(
  void
)
{
  if ( mmap_mappings_lock_create( ) ) {
    rtems_status_code sc;
    sc = rtems_semaphore_obtain( mmap_mappings_lock,
                                 RTEMS_WAIT, RTEMS_NO_TIMEOUT );
    if ( sc != RTEMS_SUCCESSFUL ) {
      errno = EINVAL;
      return false;
    }
  }
  return true;
}

bool mmap_mappings_lock_release(
  void
)
{
  rtems_status_code sc;
  sc = rtems_semaphore_release( mmap_mappings_lock );
  if (( sc != RTEMS_SUCCESSFUL ) && ( errno == 0 )) {
    errno = EINVAL;
    return false;
  }
  return true;
}

void *mmap(
  void *addr, size_t len, int prot, int flags, int fildes, off_t off
)
{
  struct stat   sb;
  mmap_mapping* mapping;
  ssize_t       r;
  
  /*
   * Clear errno.
   */
  errno = 0;
  
  /*
   * Get a stat of the file to get the dev + inode number and to make sure the
   * fd is ok. The normal libio calls cannot be used because we need to return
   * MAP_FAILED on error and they return -1 directly without coming back to
   * here.
   */
  if ( fstat( fildes, &sb ) < 0 ) {
    errno = EBADF;
    return MAP_FAILED;
  }

  if ( len == 0 ) {
    errno = EINVAL;
    return MAP_FAILED;
  }

  /*
   * Check the type of file we have and make sure it is supported.
   */
  if ( S_ISDIR( sb.st_mode ) || S_ISLNK( sb.st_mode )) {
    errno = ENODEV;
    return MAP_FAILED;
  }
  
  /*
   * We can provide read, write and execute because the memory in RTEMS does
   * not normally have protections but we cannot hide access to memory.
   */
  if ( prot == PROT_NONE ) {
    errno = EINVAL;
    return MAP_FAILED;
  }
  
  /*
   * Check to see if the mapping is valid for the file.
   */
  if ( S_ISREG( sb.st_mode )
       && (( off >= sb.st_size ) || (( off + len ) >= sb.st_size ))) {
    errno = EOVERFLOW;
    return MAP_FAILED;
  }

  /*
   * Obtain the mmap lock. Sets errno on failure.
   */
  if ( !mmap_mappings_lock_obtain( ) )
    return MAP_FAILED;

  if (( flags & MAP_FIXED ) == MAP_FIXED ) {
    rtems_chain_node* node = rtems_chain_first (&mmap_mappings);
    while ( !rtems_chain_is_tail( &mmap_mappings, node )) {
      /*
       * If the map is fixed see if this address is already mapped. At this
       * point in time if there is an overlap in the mappings we return an
       * error.
       */
      mapping = (mmap_mapping*) node;
      if ( ( addr >= mapping->addr ) &&
           ( addr < ( mapping->addr + mapping->len )) ) {
        mmap_mappings_lock_release( );
        errno = ENXIO;
        return MAP_FAILED;
      }
      node = rtems_chain_next( node );
    }
  }

  mapping = malloc( sizeof( mmap_mapping ));
  if ( !mapping ) {
    mmap_mappings_lock_release( );
    errno = ENOMEM;
    return NULL;
  }

  memset( mapping, 0, sizeof( mmap_mapping ));

  mapping->len = len;
  mapping->flags = flags;
  
  if (( flags & MAP_FIXED ) != MAP_FIXED ) {
    mapping->addr = malloc( len );
    if ( !mapping->addr ) {
      mmap_mappings_lock_release( );
      free( mapping );
      errno = ENOMEM;
      return MAP_FAILED;
    }

    /*
     * Do not seek on character devices, pipes or sockets.
     */
    if ( S_ISREG( sb.st_mode ) || S_ISBLK( sb.st_mode ) ) {
      if ( lseek( fildes, off, SEEK_SET ) < 0 ) {
        mmap_mappings_lock_release( );
        free( mapping->addr );
        free( mapping );
        return MAP_FAILED;
      }
    }
  }

  r = read( fildes, mapping->addr, len );

  if ( r != len ) {
    mmap_mappings_lock_release( );
    free( mapping->addr );
    free( mapping );
    errno = ENXIO;
    return MAP_FAILED;
  }

  rtems_chain_append( &mmap_mappings, &mapping->node );

  mmap_mappings_lock_release( );
  
  return mapping->addr;
}
