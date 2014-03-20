/**
 * @file
 *
 * @brief Provides a 32bit Hash of a String used to Search a Directory
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File Systems Directory Hash provides a 32bit hash of a string. This is
 * used to search a directory.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined (_RTEMS_RFS_DIR_HASH_H_)
#define _RTEMS_RFS_DIR_HAS_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Compute a hash of the key over the length of string.
 *
 * @param[in] key is a pointer to the key to calculate the hash of.
 * @param[in] length is the length of the key in bytes.
 *
 * @retval hash The computed uint32_t hash.
 */
uint32_t rtems_rfs_dir_hash (const void *key, size_t length);

#endif
