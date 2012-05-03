/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File Systems Directory Hash provides a 32bit hash of a string. This is
 * used to search a directory.
 */

#if !defined (_RTEMS_RFS_DIR_HASH_H_)
#define _RTEMS_RFS_DIR_HAS_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Compute a hash of the key over the length of string.
 *
 * @param key The key to calculate the hash of.
 * @param length The length of the key in bytes.
 * @return uint32_t The hash.
 */
uint32_t rtems_rfs_dir_hash (const void *key, size_t length);

#endif
