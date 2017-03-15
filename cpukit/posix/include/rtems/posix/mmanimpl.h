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

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME: add Doxygen */

/**
 * Every mmap'ed region has a mapping.
 */
typedef struct mmap_mappings_s {
  rtems_chain_node node;  /**< The mapping chain's node */
  void*            addr;  /**< The address of the mapped memory */
  size_t           len;   /**< The length of memory mapped */
  int              flags; /**< The mapping flags */
  rtems_libio_t   *iop;   /**< The mapped object's file descriptor pointer */
  bool             is_shared_shm; /**< True if MAP_SHARED of shared memory */
} mmap_mapping;

extern rtems_chain_control mmap_mappings;

bool mmap_mappings_lock_obtain( void );
bool mmap_mappings_lock_release( void );

#ifdef __cplusplus
}
#endif

#endif
