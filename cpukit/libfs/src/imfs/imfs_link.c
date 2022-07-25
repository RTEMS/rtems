/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Create a New Link Node
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

#include <rtems/imfsimpl.h>

static const IMFS_node_control IMFS_node_control_hard_link;

int IMFS_link(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *targetloc,
  const char *name,
  size_t namelen
)
{
  IMFS_jnode_t *new_node;
  IMFS_jnode_t *target;

  target = targetloc->node_access;

  /*
   *  Verify this node can be linked to.
   */
  if ( target->st_nlink >= LINK_MAX )
    rtems_set_errno_and_return_minus_one( EMLINK );

  /*
   *  Create a new link node.
   */
  new_node = IMFS_create_node(
    parentloc,
    &IMFS_node_control_hard_link,
    sizeof( IMFS_link_t ),
    name,
    namelen,
    IMFS_STAT_FMT_HARD_LINK | ( S_IRWXU | S_IRWXG | S_IRWXO ),
    target
  );

  if ( !new_node )
    rtems_set_errno_and_return_minus_one( ENOMEM );

  /*
   * Increment the link count of the node being pointed to.
   */
  target->reference_count++;
  target->st_nlink++;
  IMFS_update_ctime( target );

  return 0;
}

static int IMFS_stat_hard_link(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_link_t *hard_link = loc->node_access;
  rtems_filesystem_location_info_t targetloc = *loc;

  targetloc.node_access = hard_link->link_node;
  IMFS_Set_handlers( &targetloc );

  return (targetloc.handlers->fstat_h)( &targetloc, buf );
}

static const rtems_filesystem_file_handlers_r IMFS_link_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat_hard_link,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static IMFS_jnode_t *IMFS_node_initialize_hard_link(
  IMFS_jnode_t *node,
  void *arg
)
{
  IMFS_link_t *hard_link = (IMFS_link_t *) node;

  hard_link->link_node = arg;

  return node;
}

static IMFS_jnode_t *IMFS_node_remove_hard_link(
  IMFS_jnode_t *node
)
{
  IMFS_link_t *hard_link = (IMFS_link_t *) node;
  IMFS_jnode_t *target = hard_link->link_node;

  _Assert( target != NULL );

  if ( target->st_nlink == 1 ) {
    target = (*target->control->node_remove)( target );
    if ( target == NULL ) {
      node = NULL;
    }
  } else {
    --target->st_nlink;
    IMFS_update_ctime( target );
  }

  if ( target != NULL ) {
    --target->reference_count;

    if ( target->reference_count == 0 ) {
      IMFS_node_destroy( target );
    }
  }

  return node;
}

static const IMFS_node_control IMFS_node_control_hard_link = {
  .handlers = &IMFS_link_handlers,
  .node_initialize = IMFS_node_initialize_hard_link,
  .node_remove = IMFS_node_remove_hard_link,
  .node_destroy = IMFS_node_destroy_default
};
