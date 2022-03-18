/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief Create an IMFS Node
 */
/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/imfs.h>

#include <stdlib.h>
#include <string.h>

IMFS_jnode_t *IMFS_create_node(
  const rtems_filesystem_location_info_t *parentloc,
  const IMFS_node_control *node_control,
  size_t node_size,
  const char *name,
  size_t namelen,
  mode_t mode,
  void *arg
)
{
  IMFS_jnode_t *allocated_node;
  char         *allocated_name;
  IMFS_jnode_t *node;

  allocated_node = calloc( 1, node_size + namelen );
  if ( allocated_node == NULL ) {
    errno = ENOMEM;

    return NULL;
  }

  allocated_name = (char *) allocated_node + node_size;
  allocated_name = memcpy( allocated_name, name, namelen );
  node = IMFS_initialize_node(
    allocated_node,
    node_control,
    allocated_name,
    namelen,
    mode,
    arg
  );
  if ( node != NULL ) {
    IMFS_jnode_t *parent = parentloc->node_access;

    /*
     *  This node MUST have a parent, so put it in that directory list.
     */
    IMFS_assert( parent != NULL );
    IMFS_add_to_directory( parent, node );
  } else {
    free( allocated_node );
  }

  return node;
}

