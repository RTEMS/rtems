/*
 * Copyright (C) 2023 Aaron Nyholm
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

#include <dev/flash/flashdev.h>

#include <rtems/imfs.h>
#include <rtems/score/assert.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define RTEMS_FLASHDEV_REGION_ALLOC_FULL 0xFFFFFFFFUL
#define RTEMS_FLASHDEV_REGION_UNDEFINED 0xFFFFFFFFUL
#define RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH 32

#define RTEMS_FLASHDEV_BITALLOC_LENGTH(t) \
  (t->max_regions/RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH)
#define RTEMS_FLASHDEV_BITALLOC_FINAL_BITS(t) \
  (t->max_regions%RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH)

static int rtems_flashdev_do_init(
  rtems_flashdev *flash,
  void ( *destroy )( rtems_flashdev *flash )
);

static int rtems_flashdev_read_write(
  rtems_libio_t *iop,
  const void *write_buff,
  void *read_buff,
  size_t count
);

static int rtems_flashdev_ioctl_erase(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  void *arg
);

static off_t rtems_flashdev_get_region_offset(
  rtems_flashdev *flash,
  rtems_libio_t *iop
);

static size_t rtems_flashdev_get_region_size(
  rtems_flashdev *flash,
  rtems_libio_t *iop
);

static int rtems_flashdev_ioctl_set_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  void *arg
);

static int rtems_flashdev_ioctl_create_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  rtems_flashdev_region *region_in
);

static int rtems_flashdev_ioctl_update_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  rtems_flashdev_region *region_in
);

static int rtems_flashdev_ioctl_clear_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop
);

static uint32_t rtems_flashdev_ioctl_jedec_id(
  rtems_flashdev *flash
);

static uint32_t rtems_flashdev_ioctl_flash_type(
  rtems_flashdev *flash,
  void *arg
);

static int rtems_flashdev_ioctl_pageinfo_offset(
  rtems_flashdev *flash,
  void *arg
);

static int rtems_flashdev_ioctl_pageinfo_index(
  rtems_flashdev *flash,
  void *arg
);

static int rtems_flashdev_ioctl_page_count(
  rtems_flashdev *flash,
  void *arg
);

static int rtems_flashdev_ioctl_write_block_size(
  rtems_flashdev *flash,
  void *arg
);

static int rtems_flashdev_get_addr(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  size_t count,
  off_t *addr
);

static int rtems_flashdev_get_abs_addr(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  size_t count,
  off_t *addr
);

static int rtems_flashdev_update_and_return(
  rtems_libio_t *iop,
  int status,
  size_t count,
  off_t new_offset
);

static uint32_t rtems_flashdev_find_unallocated_region(
  rtems_flashdev_region_table *region_table
);

static uint32_t rtems_flashdev_set_region(
  rtems_flashdev_region_table *region_table,
  int index
);

static uint32_t rtems_flashdev_unset_region(
  rtems_flashdev_region_table *region_table,
  int index
);

static uint32_t rtems_flashdev_check_allocation(
  rtems_flashdev_region_table *region_table,
  int index
);

static int rtems_flashdev_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
);

static int rtems_flashdev_close(
  rtems_libio_t *iop
);

static ssize_t rtems_flashdev_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
);

static ssize_t rtems_flashdev_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
);

static int rtems_flashdev_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *arg
);

static off_t rtems_flashdev_lseek(
  rtems_libio_t *iop,
  off_t offset,
  int whence
);

static void rtems_flashdev_node_destroy(
  IMFS_jnode_t *node
);

static const rtems_filesystem_file_handlers_r rtems_flashdev_handler = {
  .open_h = rtems_flashdev_open,
  .close_h = rtems_flashdev_close,
  .read_h = rtems_flashdev_read,
  .write_h = rtems_flashdev_write,
  .ioctl_h = rtems_flashdev_ioctl,
  .lseek_h = rtems_flashdev_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev };

static const IMFS_node_control
  rtems_flashdev_node_control = IMFS_GENERIC_INITIALIZER(
    &rtems_flashdev_handler,
    IMFS_node_initialize_generic,
    rtems_flashdev_node_destroy
);

static void rtems_flashdev_node_destroy(
  IMFS_jnode_t *node
)
{
  rtems_flashdev *flash;

  flash = IMFS_generic_get_context_by_node( node );

  ( *flash->destroy )( flash );

  IMFS_node_destroy_default( node );
}

static uint32_t rtems_flashdev_get_region_index(
  rtems_libio_t *iop
)
{
  return (uint32_t)iop->data0;
}

static int rtems_flashdev_is_region_defined(
  rtems_libio_t *iop
)
{
  return (rtems_flashdev_get_region_index( iop ) != RTEMS_FLASHDEV_REGION_UNDEFINED);
}

static void rtems_flashdev_set_region_index(
  rtems_libio_t *iop,
  uint32_t index
)
{
  iop->data0 = index;
}

static int rtems_flashdev_check_offset_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  off_t offset
)
{
  if ( ( rtems_flashdev_is_region_defined( iop ) ) &&
       ( offset > rtems_flashdev_get_region_size( flash, iop ) ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  return 0;
}

static void rtems_flashdev_obtain( rtems_flashdev *flash )
{
  rtems_recursive_mutex_lock( &flash->mutex );
}

static void rtems_flashdev_release( rtems_flashdev *flash )
{
  rtems_recursive_mutex_unlock( &flash->mutex );
}

static ssize_t rtems_flashdev_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  return rtems_flashdev_read_write( iop, NULL, buffer, count );
}

static ssize_t rtems_flashdev_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  return rtems_flashdev_read_write( iop, buffer, NULL, count);
}

static int rtems_flashdev_read_write(
  rtems_libio_t *iop,
  const void *write_buff,
  void *read_buff,
  size_t count
)
{
  rtems_flashdev *flash = IMFS_generic_get_context_by_iop( iop );
  off_t addr;
  int status;

  if ( read_buff == NULL && write_buff == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* Get flash address */
  status = rtems_flashdev_get_addr( flash, iop, count, &addr );
  if ( status < 0 ) {
    return status;
  }

  /* Read or Write to flash */
  rtems_flashdev_obtain( flash );
  if ( read_buff != NULL ) {
    status = ( *flash->read )( flash, addr, count, read_buff );
  } else if ( write_buff != NULL ) {
    status = ( *flash->write )( flash, addr, count, write_buff );
  }
  rtems_flashdev_release( flash );

  /* Update offset and return */
  return rtems_flashdev_update_and_return( iop, status, count, addr + count );
}

static int rtems_flashdev_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *arg
)
{
  rtems_flashdev *flash = IMFS_generic_get_context_by_iop( iop );
  int err = 0;

  rtems_flashdev_obtain( flash );

  switch ( command ) {
    case RTEMS_FLASHDEV_IOCTL_OBTAIN:
      rtems_flashdev_obtain( flash );
      err = 0;
      break;
    case RTEMS_FLASHDEV_IOCTL_RELEASE:
      rtems_flashdev_release( flash );
      err = 0;
      break;
    case RTEMS_FLASHDEV_IOCTL_JEDEC_ID:
      *( (uint32_t *) arg ) = rtems_flashdev_ioctl_jedec_id( flash );
      err = 0;
      break;
    case RTEMS_FLASHDEV_IOCTL_ERASE:
      err = rtems_flashdev_ioctl_erase( flash, iop, arg );
      break;
    case RTEMS_FLASHDEV_IOCTL_REGION_SET:
      err = rtems_flashdev_ioctl_set_region( flash, iop, arg );
      break;
    case RTEMS_FLASHDEV_IOCTL_REGION_UNSET:
      err = rtems_flashdev_ioctl_clear_region( flash, iop );
      break;
    case RTEMS_FLASHDEV_IOCTL_TYPE:
      err = rtems_flashdev_ioctl_flash_type( flash, arg );
      break;
    case RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_OFFSET:
      err = rtems_flashdev_ioctl_pageinfo_offset( flash, arg );
      break;
    case RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_INDEX:
      err = rtems_flashdev_ioctl_pageinfo_index( flash, arg );
      break;
    case RTEMS_FLASHDEV_IOCTL_PAGE_COUNT:
      err = rtems_flashdev_ioctl_page_count( flash, arg );
      break;
    case RTEMS_FLASHDEV_IOCTL_WRITE_BLOCK_SIZE:
      err = rtems_flashdev_ioctl_write_block_size( flash, arg );
      break;
    default:
      err = EINVAL;
  }

  rtems_flashdev_release( flash );
  if ( err != 0 ) {
    rtems_set_errno_and_return_minus_one( err );
  } else {
    return 0;
  }
}

static off_t rtems_flashdev_lseek(
  rtems_libio_t *iop,
  off_t offset,
  int whence
)
{
  off_t tmp_offset;
  rtems_flashdev *flash = IMFS_generic_get_context_by_iop( iop );

  if ( offset < 0 ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  switch ( whence ) {
    case SEEK_CUR:
      tmp_offset = iop->offset + offset;
      break;
    case SEEK_SET:
      tmp_offset = offset;
      break;
    case SEEK_END:
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( ( rtems_flashdev_is_region_defined(iop) ) &&
       ( tmp_offset > rtems_flashdev_get_region_size( flash, iop ) ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  iop->offset = tmp_offset;
  return iop->offset;
}

static int rtems_flashdev_close(
  rtems_libio_t *iop
)
{
  rtems_flashdev *flash = IMFS_generic_get_context_by_iop( iop );
  rtems_flashdev_ioctl_clear_region( flash, iop );
  return rtems_filesystem_default_close( iop );
}

static int rtems_flashdev_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  int ret = rtems_filesystem_default_open( iop, path, oflag, mode );
  rtems_flashdev_set_region_index(iop, RTEMS_FLASHDEV_REGION_UNDEFINED);
  return ret;
}

int rtems_flashdev_register(
  rtems_flashdev *flash,
  const char *flash_path
)
{
  int rv;
  rtems_flashdev_region_table *table = flash->region_table;
  int alloc_array_len;

  rv = IMFS_make_generic_node(
    flash_path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &rtems_flashdev_node_control,
    flash
  );

  if ( rv != 0 ) {
    ( *flash->destroy )( flash );
  }

  alloc_array_len = RTEMS_FLASHDEV_BITALLOC_LENGTH(table) +
    ((RTEMS_FLASHDEV_BITALLOC_FINAL_BITS(table)) != 0);

  memset(table->bit_allocator, 0, alloc_array_len);

  return rv;
}

static int rtems_flashdev_do_init(
  rtems_flashdev *flash,
  void ( *destroy )( rtems_flashdev *flash )
)
{
  rtems_recursive_mutex_init( &flash->mutex, "RTEMS_FLASHDEV Flash" );
  flash->destroy = destroy;
  flash->read = NULL;
  flash->write = NULL;
  flash->erase = NULL;
  flash->jedec_id = NULL;
  flash->flash_type = NULL;
  flash->page_info_by_offset = NULL;
  flash->page_info_by_index = NULL;
  flash->page_count = NULL;
  flash->write_block_size = NULL;
  flash->region_table = NULL;
  return 0;
}

void rtems_flashdev_destroy( rtems_flashdev *flash )
{
  rtems_recursive_mutex_destroy( &flash->mutex );
}

void rtems_flashdev_destroy_and_free( rtems_flashdev *flash )
{
  if ( flash == NULL ) {
    return;
  }
  rtems_recursive_mutex_destroy( &( flash->mutex ) );
  free( flash );
  flash = NULL;
  return;
}

int rtems_flashdev_init( rtems_flashdev *flash )
{
  memset( flash, 0, sizeof( *flash ) );

  return rtems_flashdev_do_init( flash, rtems_flashdev_destroy );
}

rtems_flashdev *rtems_flashdev_alloc_and_init( size_t size )
{
  rtems_flashdev *flash = NULL;

  if ( size >= sizeof( *flash ) ) {
    flash = calloc( 1, size );
    if ( flash != NULL ) {
      int rv;

      rv = rtems_flashdev_do_init( flash, rtems_flashdev_destroy_and_free );
      if ( rv != 0 ) {
        rtems_recursive_mutex_destroy( &flash->mutex );
        free( flash );
        return NULL;
      }
    }
  }

  return flash;
}

static int rtems_flashdev_get_addr(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  size_t count,
  off_t *addr
)
{
  off_t new_offset;

  /* Check address is in valid region */
  new_offset = iop->offset + count;

  if (rtems_flashdev_check_offset_region(flash, iop, new_offset)) {
    return -1;
  }

  /* Get address for operation */
  if ( !rtems_flashdev_is_region_defined( iop ) ) {
    *addr = iop->offset;
  } else {
    *addr = ( iop->offset + rtems_flashdev_get_region_offset( flash, iop ) );
  }
  return 0;
}

static int rtems_flashdev_get_abs_addr(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  size_t count,
  off_t *addr
)
{
  off_t new_offset;

  /* Check address is in valid region */
  new_offset = *addr + count;

  if (rtems_flashdev_check_offset_region(flash, iop, new_offset)) {
    return -1;
  }

  /* Get address for operation */
  if ( rtems_flashdev_is_region_defined( iop ) ) {
    *addr = ( *addr + rtems_flashdev_get_region_offset( flash, iop ) );
  }
  return 0;
}
static int rtems_flashdev_update_and_return(
  rtems_libio_t *iop,
  int status,
  size_t count,
  off_t new_offset
)
{
  /* Update offset and return */
  if ( status == 0 ) {
    iop->offset = new_offset;
    return count;
  } else {
    rtems_set_errno_and_return_minus_one( status );
  }
}

static int rtems_flashdev_ioctl_erase(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  void *arg
)
{
  rtems_flashdev_region *erase_args_1;
  off_t new_offset;
  int status;

  if ( flash->erase == NULL ) {
    return 0;
  }

  erase_args_1 = (rtems_flashdev_region *) arg;
  /* Check erasing valid region */
  new_offset = erase_args_1->offset;
  status = rtems_flashdev_get_abs_addr(flash, iop, erase_args_1->size, &new_offset);
  if ( status < 0 ) {
    return status;
  }

  /* Erase flash */
  status = ( *flash->erase )( flash, new_offset, erase_args_1->size );
  return status;
}

static int rtems_flashdev_ioctl_set_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  void *arg
)
{
  rtems_flashdev_region *region_in;
  rtems_flashdev_region_table *table = flash->region_table;
  region_in = (rtems_flashdev_region *) arg;

  if (flash->region_table == NULL) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  if ( !rtems_flashdev_is_region_defined( iop ) ) {
    if (
      rtems_flashdev_find_unallocated_region(table)
        == RTEMS_FLASHDEV_REGION_ALLOC_FULL
    )
    {
      /* New region to allocate and all regions allocated */
      rtems_set_errno_and_return_minus_one( ENOMEM );
    } else {
      /* New region to allocate and space to allocate region */
      return rtems_flashdev_ioctl_create_region( flash, iop, region_in );
    }
  } else {
    /* Updating existing region */
    return rtems_flashdev_ioctl_update_region( flash, iop, region_in );
  }

}

static int rtems_flashdev_ioctl_create_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  rtems_flashdev_region *region_in
)
{
  int i;
  rtems_flashdev_region_table *table = flash->region_table;

  /* Find unallocated region slot */
  i = rtems_flashdev_find_unallocated_region(flash->region_table);
  if (i == RTEMS_FLASHDEV_REGION_ALLOC_FULL) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  /* Set region values */
  table->regions[ i ].offset = region_in->offset;
  table->regions[ i ].size = region_in->size;

  /* Set region as allocated and link iop */
  rtems_flashdev_set_region(flash->region_table, i);
  rtems_flashdev_set_region_index( iop, i );

  return 0;
}

static int rtems_flashdev_ioctl_update_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop,
  rtems_flashdev_region *region_in
)
{
  uint32_t region_index = rtems_flashdev_get_region_index( iop );
  rtems_flashdev_region_table *table = flash->region_table;

  /**
   * If region index is larger then maximum region index or region
   * index at given index is undefined return an error.
   */
  if (
       ( region_index >= flash->region_table->max_regions ) ||
       ( rtems_flashdev_check_allocation( table, region_index ) == 0)
     )
  {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* Set region values */
  table->regions[ region_index ].offset = region_in->offset;
  table->regions[ region_index ].size = region_in->size;

  return 0;
}

static int rtems_flashdev_ioctl_clear_region(
  rtems_flashdev *flash,
  rtems_libio_t *iop
)
{
  uint32_t region_index = rtems_flashdev_get_region_index( iop );

  if (flash->region_table == NULL) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  /* Check region to clear */
  if ( region_index == RTEMS_FLASHDEV_REGION_UNDEFINED ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* Clear region */
  rtems_flashdev_unset_region( flash->region_table, region_index );
  rtems_flashdev_set_region_index( iop, RTEMS_FLASHDEV_REGION_UNDEFINED );
  return 0;
}

static off_t rtems_flashdev_get_region_offset(
  rtems_flashdev *flash,
  rtems_libio_t *iop
)
{
  /* Region is already checked to be defined */
  assert( rtems_flashdev_get_region_index( iop ) != RTEMS_FLASHDEV_REGION_UNDEFINED );
  rtems_flashdev_region_table *table = flash->region_table;
  return table->regions[ rtems_flashdev_get_region_index( iop ) ].offset;
}

static size_t rtems_flashdev_get_region_size(
  rtems_flashdev *flash,
  rtems_libio_t *iop
)
{
  /* Region is already checked to be defined */
  assert( rtems_flashdev_get_region_index( iop ) != RTEMS_FLASHDEV_REGION_UNDEFINED );
  rtems_flashdev_region_table *table = flash->region_table;
  return table->regions[ rtems_flashdev_get_region_index( iop ) ].size;
}

static uint32_t rtems_flashdev_ioctl_jedec_id( rtems_flashdev *flash )
{
  if ( flash->jedec_id == NULL ) {
    return 0;
  } else {
    return ( *flash->jedec_id )( flash );
  }
}

static uint32_t rtems_flashdev_ioctl_flash_type(
  rtems_flashdev *flash,
  void *arg
)
{
  rtems_flashdev_flash_type *type = (rtems_flashdev_flash_type*)arg;
  if ( flash->flash_type == NULL ) {
    return 0;
  } else {
    return ( *flash->flash_type )( flash, type );
  }
}

static int rtems_flashdev_ioctl_pageinfo_offset(
  rtems_flashdev *flash,
  void *arg
)
{
  rtems_flashdev_ioctl_page_info *page_info;

  if ( arg == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  if ( flash->page_info_by_offset == NULL ) {
    return 0;
  } else {
    page_info = (rtems_flashdev_ioctl_page_info *) arg;
    return ( *flash->page_info_by_offset )( flash,
                                         page_info->location,
                                         &page_info->page_info.offset,
                                         &page_info->page_info.size );
  }
}

static int rtems_flashdev_ioctl_pageinfo_index( rtems_flashdev *flash,
                                                void *arg )
{
  rtems_flashdev_ioctl_page_info *page_info;

  if ( arg == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  if ( flash->page_info_by_index == NULL ) {
    return 0;
  } else {
    page_info = (rtems_flashdev_ioctl_page_info *) arg;
    return ( *flash->page_info_by_index )( flash,
                                           page_info->location,
                                           &page_info->page_info.offset,
                                           &page_info->page_info.size );
  }
}

static int rtems_flashdev_ioctl_page_count( rtems_flashdev *flash, void *arg )
{
  if ( arg == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  if ( flash->page_count == NULL ) {
    return 0;
  } else {
    return ( *flash->page_count )( flash, ( (int *) arg ) );
  }
}

static int rtems_flashdev_ioctl_write_block_size(
  rtems_flashdev *flash,
  void *arg
)
{
  if ( arg == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  if ( flash->write_block_size == NULL ) {
    return 0;
  } else {
    return ( *flash->write_block_size )( flash, ( (size_t *) arg ) );
  }
}

static uint32_t rtems_flashdev_find_unallocated_region(
  rtems_flashdev_region_table *region_table
)
{
  int array_index = 0;
  int bit_index = 0;
  int shift;

  while ( bit_index < region_table->max_regions) {
    /* Get uint32_t holding the ith bit */
    array_index = bit_index / RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;
    shift = bit_index % RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;

    /* Check if region available in next BITALLOC_LENGTH regions */
    if (
        (shift == 0) &&
        (region_table->bit_allocator[ array_index ] == RTEMS_FLASHDEV_REGION_ALLOC_FULL)
    )
    {
      bit_index = bit_index + RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;
      continue;
    }

    /* Check individual bit */
    if ( ! ( ( ( region_table->bit_allocator[ array_index ] ) >> shift ) & 1UL ) ) {
      return bit_index;
    }

    bit_index++;
  }

  return RTEMS_FLASHDEV_REGION_ALLOC_FULL;
}

static uint32_t rtems_flashdev_set_region(
  rtems_flashdev_region_table *region_table,
  int index
)
{
  int array_index = index / RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;
  int shift = index % RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;

  region_table->bit_allocator[ array_index ] |= 1UL << shift;

  return index;
}

static uint32_t rtems_flashdev_unset_region(
  rtems_flashdev_region_table *region_table,
  int index
)
{
  int array_index = index / RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;
  int shift = index % RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;

  region_table->bit_allocator[ array_index ] &= ~( 1UL << shift );

  return index;
}

static uint32_t rtems_flashdev_check_allocation(
  rtems_flashdev_region_table *region_table,
  int index
)
{
  int array_index = index / RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;
  int shift = index%RTEMS_FLASHDEV_REGION_BITALLOC_LENGTH;

  return ( ( region_table->bit_allocator[ array_index ] >> shift ) & 1UL );
}
