/**
 * @file
 *
 * @brief Internal Support for POSIX 1003.1b 6.3.1 - map pages of memory
 *
 */

/*
 * Copyright (c) 2012 Chris Johns
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MMANIMPL_H
#define _RTEMS_POSIX_MMANIMPL_H

#include <rtems/libio_.h>
#include <rtems/chain.h> /* FIXME: use score chains for proper layering? */
#include <rtems/posix/shm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME: add Doxygen */

/**
 * Every mmap'ed region has a mapping.
 */
typedef struct mmap_mappings_s {
  rtems_chain_node   node;  /**< The mapping chain's node */
  void*              addr;  /**< The address of the mapped memory */
  size_t             len;   /**< The length of memory mapped */
  int                flags; /**< The mapping flags */
  POSIX_Shm_Control *shm;   /**< The shared memory object or NULL */
} mmap_mapping;

extern rtems_chain_control mmap_mappings;

static inline void mmap_mappings_lock_obtain( void )
{
  rtems_libio_lock();
}

static inline void mmap_mappings_lock_release( void )
{
  rtems_libio_unlock();
}

#ifdef __cplusplus
}
#endif

#endif
