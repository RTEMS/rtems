/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 Gedare Bloom
 * COPYRIGHT (c) 2013-2017 Chris Johns <chrisj@rtems.org>
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

/**
 * @file
 *
 * @ingroup rtems_fdt
 *
 * @brief RTEMS Flattened Device Tree Implementation
 *
 * Internal implementation Support for loading, managing and accessing FDT blobs in RTEMS.
 */

/**
 * @defgroup rtems_fdt_InternalAPI RTEMS FDT API Internals
 *
 * @brief RTEMS FDT Internal Information
 *
 * This concerns implementation information about the RTEMS FDT.
 */

#ifndef RTEMS_FDTIMPL_H
#define RTEMS_FDTIMPL_H

#include <rtems/chain.h>

/**
 * An index for quick access to the FDT by name or offset.
 */

typedef struct {
  const char *name;   /**< The full path of the FDT item. */
  int         offset; /**< The offset of the item in the FDT blob. */
} rtems_fdt_index_entry;

typedef struct {
  int num_entries;                /**< The number of entries in this index. */
  rtems_fdt_index_entry *entries; /**< An ordered set of entries which we
                                          *  can binary search. */
  char *names; /**< Storage allocated for all the path names. */
} rtems_fdt_index;

/**
 * A blob descriptor.
 */
struct rtems_fdt_blob {
  rtems_chain_node node; /**< The node's link in the chain. */
  const void      *blob; /**< The FDT blob. */
  const char      *name; /**< The name of the blob. */
  int              refs; /**< The number of active references of the blob. */
  rtems_fdt_index
    index; /**< The index used for quick access to items in the blob. */
};

/**
 * The global FDT data. This structure is allocated on the heap when the first
 * call to load a FDT blob is made and released when all blobs have been
 * unloaded..
 */
typedef struct {
  rtems_mutex         lock;  /**< The FDT lock id */
  rtems_chain_control blobs; /**< List if loaded blobs. */
  const char         *paths; /**< Search paths for blobs. */
} rtems_fdt_data;

/**
 * @brief Check if a handle had a valid blob assigned.
 *
 * @param handle The handle to check.
 * @param fdt The FDT instance.
 * @retval true The handle has a valid blob.
 * @retval false The handle does not have a valid blob.
 *
 * @note It does not acquire a lock to ensure consistency of the blobs chain.
 *       It should be called with the @a fdt->lock() held.
 *
 * @see rtems_fdt_valid_handle().
 */
static inline bool rtems_fdt_valid_handle_unprotected(
  const rtems_fdt_handle *handle,
  const rtems_fdt_data   *fdt
)
{
  if ( handle && handle->blob ) {
    rtems_chain_node *node;

    node = rtems_chain_first( &fdt->blobs );

    while ( !rtems_chain_is_tail( &fdt->blobs, node ) ) {
      rtems_fdt_blob *blob = (rtems_fdt_blob *) node;
      if ( handle->blob == blob ) {
        return true;
      }
      node = rtems_chain_next( node );
    }
  }
  return false;
}

#endif
