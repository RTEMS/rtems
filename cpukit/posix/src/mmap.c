/**
 * @file
 */

/*
 * Copyright (c) 2012 Chris Johns (chrisj@rtems.org)
 * Copyright (c) 2017 Gedare Bloom (gedare@rtems.org)
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
#include <rtems/posix/shmimpl.h>

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
  /* FIXME: double-checked locking anti-pattern. */
  if ( mmap_mappings_lock == 0 ) {
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    rtems_chain_initialize_empty( &mmap_mappings );
    rtems_semaphore_obtain( rtems_libio_semaphore,
                            RTEMS_WAIT, RTEMS_NO_TIMEOUT );
    /* FIXME: account for semaphore in confdefs, or maybe just use the
     * rtems_libio_semaphore? */
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

/* Helper function only gets called for mmap mappings of shared memory objects
 * with the MAP_SHARED flag.
 */
static void *shm_mmap( rtems_libio_t *iop, size_t len, int prot, off_t off)
{
  POSIX_Shm_Control *shm = iop_to_shm( iop );
  void *m;

  _Objects_Allocator_lock();

  m = (*shm->shm_object.ops->object_mmap)( &shm->shm_object, len, prot, off);
  if ( m != NULL ) {
    /* Keep a reference in the shared memory to prevent its removal. */
    ++shm->reference_count;

    /* Update atime */
    _POSIX_Shm_Update_atime(shm);
  }

  _Objects_Allocator_unlock();

  return m;
}

void *mmap(
  void *addr, size_t len, int prot, int flags, int fildes, off_t off
)
{
  struct stat     sb;
  mmap_mapping   *mapping;
  ssize_t         r;
  rtems_libio_t  *iop;
  bool            map_fixed;

  map_fixed = (flags & MAP_FIXED) == MAP_FIXED;

  /* Clear errno. */
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

  /* fstat ensures we have a good file descriptor. Hold on to iop. */
  iop = rtems_libio_iop( fildes );

  if ( len == 0 ) {
    errno = EINVAL;
    return MAP_FAILED;
  }

  /* Check the type of file we have and make sure it is supported. */
  if ( S_ISDIR( sb.st_mode ) || S_ISLNK( sb.st_mode )) {
    errno = ENODEV;
    return MAP_FAILED;
  }

  /*
   * We can provide read, write and execute because the memory in RTEMS does
   * not normally have protections but we cannot hide access to memory.
   */
  if ( prot == PROT_NONE ) {
    errno = ENOTSUP;
    return MAP_FAILED;
  }

  /* Either MAP_SHARED or MAP_PRIVATE must be defined, but not both */
  if ( (flags & MAP_SHARED) == MAP_SHARED ) {
    if ( (flags & MAP_PRIVATE) == MAP_PRIVATE ) {
      errno = EINVAL;
      return MAP_FAILED;
    }
  } else if ( (flags & MAP_PRIVATE != MAP_PRIVATE) ) {
    errno = EINVAL;
    return MAP_FAILED;
  }

  /*
   * We can not normally provide restriction of write access. Reject any
   * attempt to map without write permission, since we are not able to
   * prevent a write from succeeding.
   */
  if ( PROT_WRITE != (prot & PROT_WRITE) ) {
    errno = ENOTSUP;
    return MAP_FAILED;
  }

  /* Check to see if the mapping is valid for a regular file. */
  if ( S_ISREG( sb.st_mode )
  /* FIXME: Should this be using strict inequality (>) comparisons? It would
   * be valid to map a region exactly equal to the st_size, e.g. see below. */
       && (( off >= sb.st_size ) || (( off + len ) >= sb.st_size ))) {
    errno = EOVERFLOW;
    return MAP_FAILED;
  }

  /*
   * Check to see if the mapping is valid for other file/object types.
   * Does this satisfy for devices?
   */
  if ( sb.st_size < off + len ) {
    errno = ENXIO;
    return MAP_FAILED;
  }

  /* Do not seek on character devices, pipes, sockets, or memory objects. */
  if ( S_ISREG( sb.st_mode ) || S_ISBLK( sb.st_mode ) ) {
    if ( lseek( fildes, off, SEEK_SET ) < 0 ) {
      return MAP_FAILED;
    }
  }

  /* Create the mapping */
  mapping = malloc( sizeof( mmap_mapping ));
  if ( !mapping ) {
    errno = ENOMEM;
    return NULL;
  }
  memset( mapping, 0, sizeof( mmap_mapping ));
  mapping->len = len;
  mapping->flags = flags;
  mapping->iop = iop;

  /*
   * HACK: We should have a better generic way to distinguish between
   * shm objects and other mmap'd files. We need to know at munmap time
   * if the mapping is to a shared memory object in order to refcnt shms.
   * We could do this by providing mmap in the file operations if needed.
   */
  if ( S_ISREG( sb.st_mode ) || S_ISBLK( sb.st_mode ) ||
       S_ISCHR( sb.st_mode ) || S_ISFIFO( sb.st_mode ) ||
       S_ISSOCK( sb.st_mode ) ) {
    mapping->is_shared_shm = false;
  } else {
    mapping->is_shared_shm = true;
  }

  /*
   * MAP_SHARED currently is only supported for shared memory objects.
   */
  if ( (MAP_SHARED == (flags & MAP_SHARED)) && (mapping->is_shared_shm == false) ) {
    free( mapping );
    errno = ENOTSUP;
    return MAP_FAILED;
  }

  if ( map_fixed ) {
    mapping->addr = addr;
  } else if ( MAP_PRIVATE == (flags & MAP_PRIVATE) ) {
    /* private mappings of shared memory do not need special treatment. */
    mapping->is_shared_shm = false;
    mapping->addr = malloc( len );
    if ( !mapping->addr ) {
      free( mapping );
      errno = ENOMEM;
      return MAP_FAILED;
    }
  }

  /* MAP_FIXED is not supported for shared memory objects with MAP_SHARED. */
  if ( map_fixed && mapping->is_shared_shm ) {
    free( mapping );
    errno = ENOTSUP;
    return MAP_FAILED;
  }

  /* Lock access to mmap_mappings. Sets errno on failure. */
  if ( !mmap_mappings_lock_obtain( ) )
    return MAP_FAILED;

  if ( map_fixed ) {
    rtems_chain_node* node = rtems_chain_first (&mmap_mappings);
    while ( !rtems_chain_is_tail( &mmap_mappings, node )) {
      /*
       * If the map is fixed see if this address is already mapped. At this
       * point in time if there is an overlap in the mappings we return an
       * error. POSIX allows us to also return successfully by unmapping
       * the overlapping prior mappings.
       */
      mapping = (mmap_mapping*) node;
      if ( ( addr >= mapping->addr ) &&
           ( addr < ( mapping->addr + mapping->len )) ) {
        free( mapping );
        mmap_mappings_lock_release( );
        errno = ENXIO;
        return MAP_FAILED;
      }
      node = rtems_chain_next( node );
    }
  }

  /* Populate the data */
  if ( MAP_PRIVATE == (flags & MAP_PRIVATE) ) {
    /*
     * Use read() for private mappings. This updates atime as needed.
     * Changes to the underlying object will NOT be reflected in the mapping.
     * The underlying object can be removed while the mapping exists.
     */
    r = read( fildes, mapping->addr, len );

    if ( r != len ) {
      mmap_mappings_lock_release( );
      if ( !map_fixed ) {
        free( mapping->addr );
      }
      free( mapping );
      errno = ENXIO;
      return MAP_FAILED;
    }
  } else if ( MAP_SHARED == (flags & MAP_SHARED) ) {
    /* Currently only shm objects can be MAP_SHARED. */
    mapping->addr = shm_mmap(iop, len, prot, off);
  }

  /* add an extra reference to the file associated with fildes that
   * is not removed by a subsequent close().  This reference shall be removed
   * when there are no more mappings to the file. */
  rtems_libio_increment_mapping_refcnt(iop);

  rtems_chain_append( &mmap_mappings, &mapping->node );

  mmap_mappings_lock_release( );

  return mapping->addr;
}
