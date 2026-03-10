/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 Aaron Nyholm
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

#include <dev/io/iodev.h>

#include <rtems/imfs.h>
#include <rtems/score/assert.h>

#include <limits.h>
#include <sys/mman.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int rtems_iodev_do_init(
  rtems_iodev *iodev,
  void ( *destroy )( rtems_iodev *iodev )
);

static int rtems_iodev_ioctl(
  rtems_libio_t  *iop,
  ioctl_command_t command,
  void           *arg
);

static int rtems_iodev_mmap(
  rtems_libio_t *iop,
  void         **addr,
  size_t         len,
  int            prot,
  off_t          off
);

static int rtems_iodev_get_region( struct rtems_iodev *iodev, void *arg );

static int rtems_iodev_get_event_info( struct rtems_iodev *iodev, void *arg );

static int rtems_iodev_event_wait_on( struct rtems_iodev *iodev, void *arg );

static void rtems_iodev_node_destroy( IMFS_jnode_t *node );

static const rtems_filesystem_file_handlers_r rtems_iodev_handler = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_iodev_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_iodev_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static const IMFS_node_control
  rtems_iodev_node_control = IMFS_GENERIC_INITIALIZER(
    &rtems_iodev_handler,
    IMFS_node_initialize_generic,
    rtems_iodev_node_destroy
  );

static void rtems_iodev_node_destroy( IMFS_jnode_t *node )
{
  rtems_iodev *iodev;

  iodev = IMFS_generic_get_context_by_node( node );

  ( *iodev->destroy )( iodev );

  IMFS_node_destroy_default( node );
}

static void rtems_iodev_obtain( rtems_iodev *iodev )
{
  rtems_recursive_mutex_lock( &iodev->mutex );
}

static void rtems_iodev_release( rtems_iodev *iodev )
{
  rtems_recursive_mutex_unlock( &iodev->mutex );
}

static int rtems_iodev_ioctl(
  rtems_libio_t  *iop,
  ioctl_command_t command,
  void           *arg
)
{
  rtems_iodev *iodev = IMFS_generic_get_context_by_iop( iop );
  int          err = 0;

  rtems_iodev_obtain( iodev );

  switch ( command ) {
    case RTEMS_IODEV_IOCTL_OBTAIN:
      rtems_iodev_obtain( iodev );
      err = 0;
      break;
    case RTEMS_IODEV_IOCTL_RELEASE:
      rtems_iodev_release( iodev );
      err = 0;
      break;
    case RTEMS_IODEV_IOCTL_DEVICE_INFO:
      err = iodev->get_device_info( iodev, arg );
      break;
    case RTEMS_IODEV_IOCTL_REGION_COUNT:
      err = iodev->get_region_count( iodev, arg );
      break;
    case RTEMS_IODEV_IOCTL_REGION_GET:
      err = rtems_iodev_get_region( iodev, arg );
      break;
    case RTEMS_IODEV_IOCTL_EVENT_COUNT:
      err = iodev->get_event_count( iodev, arg );
      break;
    case RTEMS_IODEV_IOCTL_EVENT_INFO:
      err = rtems_iodev_get_event_info( iodev, arg );
      break;
    case RTEMS_IODEV_IOCTL_EVENT_WAIT:
      err = rtems_iodev_event_wait_on( iodev, arg );
      break;
    default:
      err = EINVAL;
  }

  rtems_iodev_release( iodev );
  if ( err != 0 ) {
    rtems_set_errno_and_return_minus_one( err );
  } else {
    return 0;
  }
}

static int rtems_iodev_mmap(
  rtems_libio_t *iop,
  void         **addr,
  size_t         len,
  int            prot,
  off_t          off
)
{
  rtems_iodev        *iodev = IMFS_generic_get_context_by_iop( iop );
  rtems_iodev_region *regions;
  size_t              region_count = 0;

  if ( len == 0 || *addr != NULL || off < 0 ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( prot != ( PROT_READ | PROT_WRITE ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  iodev->get_region_count( iodev, &region_count );
  if ( (size_t) off > region_count ) {
    rtems_set_errno_and_return_minus_one( EOVERFLOW );
  }

  iodev->get_regions( iodev, &regions );
  if ( len > regions[ off ].size ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  *addr = regions[ off ].address;

  return 0;
}

static int rtems_iodev_get_region( struct rtems_iodev *iodev, void *arg )
{
  rtems_iodev_region *regions;
  rtems_iodev_region *region;
  size_t              region_count = 0;

  region = (rtems_iodev_region *) arg;

  iodev->get_region_count( iodev, &region_count );
  if ( region->index >= region_count ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  iodev->get_regions( iodev, &regions );
  region->address = regions[ region->index ].address;
  region->size = regions[ region->index ].size;
  region->name = regions[ region->index ].name;

  return 0;
}

static int rtems_iodev_get_event_info( struct rtems_iodev *iodev, void *arg )
{
  rtems_iodev_event_info *e_info;
  int                     status;
  size_t                  event_count;

  e_info = (rtems_iodev_event_info *) arg;

  iodev->get_event_count( iodev, &event_count );
  if ( e_info->index >= event_count ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( iodev->get_event_info == NULL ) {
    rtems_set_errno_and_return_minus_one( ENODEV );
  }

  status = ( *iodev->get_event_info )( iodev, e_info );

  return status;
}

static int rtems_iodev_event_wait_on( struct rtems_iodev *iodev, void *arg )
{
  rtems_iodev_event_args *e_args;
  int                     status;
  size_t                  event_count;

  e_args = (rtems_iodev_event_args *) arg;

  iodev->get_event_count( iodev, &event_count );
  if ( e_args->index >= event_count ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( iodev->event_wait == NULL ) {
    rtems_set_errno_and_return_minus_one( ENODEV );
  }

  status = ( *iodev->event_wait )( iodev, e_args );

  return status;
}

int rtems_iodev_register( rtems_iodev *iodev, const char *iodev_path )
{
  int rv;

  rv = IMFS_make_generic_node(
    iodev_path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &rtems_iodev_node_control,
    iodev
  );

  return rv;
}

int rtems_iodev_unregister_and_destroy( const char *iodev_path )
{
  return unlink( iodev_path );
}

static int rtems_iodev_do_init(
  rtems_iodev *iodev,
  void ( *destroy )( rtems_iodev *iodev )
)
{
  rtems_recursive_mutex_init( &iodev->mutex, "RTEMS_IODEV device" );
  iodev->destroy = destroy;
  /* other fields are guaranteed nulled by caller */

  return 0;
}

void rtems_iodev_destroy_unregistered( rtems_iodev *iodev )
{
  ( *iodev->destroy )( iodev );
}

static void iodev_destroy_internal( rtems_iodev *iodev )
{
  if ( iodev->priv_destroy != NULL ) {
    ( *iodev->priv_destroy )( iodev );
  }

  rtems_recursive_mutex_destroy( &iodev->mutex );
}

static void iodev_destroy_and_free_internal( rtems_iodev *iodev )
{
  if ( iodev == NULL ) {
    return;
  }
  iodev_destroy_internal( iodev );
  free( iodev );
  iodev = NULL;
}

int rtems_iodev_init( rtems_iodev *iodev )
{
  memset( iodev, 0, sizeof( *iodev ) );

  return rtems_iodev_do_init( iodev, iodev_destroy_internal );
}

rtems_iodev *rtems_iodev_alloc_and_init( size_t size )
{
  rtems_iodev *iodev = NULL;

  if ( size >= sizeof( *iodev ) ) {
    iodev = calloc( 1, size );
    if ( iodev != NULL ) {
      int rv;

      rv = rtems_iodev_do_init( iodev, iodev_destroy_and_free_internal );
      if ( rv != 0 ) {
        rtems_recursive_mutex_destroy( &iodev->mutex );
        free( iodev );
        return NULL;
      }
    }
  }

  return iodev;
}
