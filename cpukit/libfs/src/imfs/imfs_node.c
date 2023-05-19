/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Node Support
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <rtems/imfsimpl.h>

IMFS_jnode_t *IMFS_initialize_node(
  IMFS_jnode_t *node,
  const IMFS_node_control *node_control,
  const char *name,
  size_t namelen,
  mode_t mode,
  void *arg
)
{
  time_t now;

  if ( namelen > IMFS_NAME_MAX ) {
    errno = ENAMETOOLONG;

    return NULL;
  }

  /*
   *  Fill in the basic information
   */
  node->name = name;
  node->namelen = namelen;
  node->reference_count = 1;
  node->st_nlink = 1;
  node->control = node_control;

  /*
   *  Fill in the mode and permission information for the jnode structure.
   */
  node->st_mode = mode;
  node->st_uid = geteuid();
  node->st_gid = getegid();

  /*
   *  Now set all the times.
   */

  now = _IMFS_get_time();
  node->stat_atime = now;
  node->stat_mtime = now;
  node->stat_ctime = now;

  return (*node_control->node_initialize)( node, arg );
}

int IMFS_node_clone( rtems_filesystem_location_info_t *loc )
{
  IMFS_jnode_t *node = loc->node_access;

  ++node->reference_count;

  return 0;
}

void IMFS_node_destroy( IMFS_jnode_t *node )
{
  IMFS_assert( node->reference_count == 0 );

  (*node->control->node_destroy)( node );
}

void IMFS_node_free( const rtems_filesystem_location_info_t *loc )
{
  IMFS_jnode_t *node = loc->node_access;

  --node->reference_count;

  if ( node->reference_count == 0 ) {
    IMFS_node_destroy( node );
  }
}

IMFS_jnode_t *IMFS_node_remove_default(
  IMFS_jnode_t *node
)
{
  return node;
}

void IMFS_do_nothing_destroy( IMFS_jnode_t *node )
{
  (void) node;
}
