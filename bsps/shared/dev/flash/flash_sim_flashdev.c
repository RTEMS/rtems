/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Flash
 *
 * @brief Flashdev backend simulating NAND in memory
 */

/*
 * Copyright (C) 2025 On-Line Applications Research (OAR) Corporation
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
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

#include <dev/flash/flash_sim_flashdev.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/basedefs.h>
#include <stdlib.h>
#include <string.h>

#define NAND_SIM_FLASH_MAX_REGIONS ( (size_t) 32 )

/**
 * @brief Struct allocating memory space for flash regions. Used by
 * rtems_flashdev to store region allocations.
 */
typedef struct nand_priv_data {
  rtems_flashdev_region regions[ NAND_SIM_FLASH_MAX_REGIONS ];
  uint32_t bit_allocator;
  struct flash_sim_flashdev_attributes attr;
  unsigned char *area;
  unsigned char *oob;
} nand_priv_data;

static uint32_t flash_sim_get_jedec_id( rtems_flashdev *flash )
{
  (void) flash;

  return 0x00ABCDEF;
}

static void complete_operation_delay(
  uint64_t start_time,
  uint64_t target_delay
)
{
  if ( target_delay == 0 ) {
    return;
  }
  while ( rtems_clock_get_uptime_nanoseconds() - start_time < target_delay ) {
    ;
  }
}

static int flash_sim_get_flash_type(
  rtems_flashdev *flash,
  rtems_flashdev_flash_type *type
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *type = attr->type;

  return 0;
}

static int flash_sim_read_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count,
  void *buffer
)
{
  uint64_t start_time = rtems_clock_get_uptime_nanoseconds();
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  unsigned char *chunk = &flash_driver->area[ offset ];

  memcpy( buffer, chunk, count );

  complete_operation_delay( start_time, flash_driver->attr.read_delay_ns );

  return 0;
}

static int flash_sim_write_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count,
  const void *buffer
)
{
  uint64_t start_time = rtems_clock_get_uptime_nanoseconds();
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  unsigned char *chunk = &flash_driver->area[ offset ];
  const uint8_t *cbuff = buffer;
  size_t i;

  for ( i = 0; i < count; ++i ) {
    chunk[ i ] &= cbuff[ i ];
  }

  complete_operation_delay( start_time, flash_driver->attr.write_delay_ns );

  return 0;
}

static void erase_sector( struct nand_priv_data *flash_driver, uint64_t offset )
{
  uint64_t start_time = rtems_clock_get_uptime_nanoseconds();
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;
  uint64_t sector_size = attr->pages_per_sector * attr->page_size_bytes;
  uint64_t sector_oob_size = attr->pages_per_sector * attr->page_oob_bytes;
  uint64_t page_index = offset / attr->page_size_bytes;
  uint64_t oob_offset = page_index * attr->page_oob_bytes;
  unsigned char *chunk = &flash_driver->area[ offset ];
  unsigned char *oobchunk = &flash_driver->oob[ oob_offset ];

  memset( chunk, 0xff, sector_size );

  /* Erase also resets the OOB space for the entire sector */
  if ( attr->type == RTEMS_FLASHDEV_NAND ) {
    memset( oobchunk, 0xff, sector_oob_size );
  }

  complete_operation_delay( start_time, attr->erase_delay_ns );
}

static int flash_sim_erase_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;
  uint64_t sector_size = attr->page_size_bytes * attr->pages_per_sector;
  uintptr_t aligned_start = RTEMS_ALIGN_DOWN( offset, sector_size );
  uintptr_t aligned_end = RTEMS_ALIGN_UP( offset + count, sector_size );
  uint64_t sector_count = ( aligned_end - aligned_start ) / sector_size;

  for ( int i = 0; i < sector_count; i++ ) {
    erase_sector( flash_driver, aligned_start + i * sector_size );
  }

  return 0;
}

static int flash_sim_page_info_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *page_offset,
  size_t *page_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *page_size = attr->page_size_bytes;
  *page_offset = RTEMS_ALIGN_DOWN( search_offset, attr->page_size_bytes );
  return 0;
}

static int flash_sim_page_info_by_index(
  rtems_flashdev *flash,
  off_t search_index,
  off_t *page_offset,
  size_t *page_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *page_size = attr->page_size_bytes;
  *page_offset = *page_size * search_index;
  return 0;
}

static int flash_sim_page_count( rtems_flashdev *flash, int *page_count )
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *page_count = attr->total_sectors * attr->pages_per_sector;
  return 0;
}

static int flash_sim_write_block_size(
  rtems_flashdev *flash,
  size_t *write_block_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *write_block_size = attr->page_size_bytes;
  return 0;
}

static int flash_sim_sector_count( rtems_flashdev *flash, int *sector_count )
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *sector_count = attr->total_sectors;

  return 0;
}

static int flash_sim_sector_info_by_offset(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *sector_offset,
  size_t *sector_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;
  uint64_t sector_size_bytes = attr->pages_per_sector * attr->page_size_bytes;

  *sector_offset = RTEMS_ALIGN_DOWN( search_offset, sector_size_bytes );
  *sector_size = attr->pages_per_sector * attr->page_size_bytes;

  return 0;
}

static int flash_sim_sector_health(
  rtems_flashdev *flash,
  off_t search_offset,
  uint8_t *sector_bad
)
{
  (void) flash;
  (void) search_offset;

  *sector_bad = 0;

  return 0;
}

static int flash_sim_oob_bytes_per_page(
  rtems_flashdev *flash,
  size_t *oob_bytes_per_page
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;

  *oob_bytes_per_page = attr->page_oob_bytes;

  return 0;
}

static int flash_sim_oob_read(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t size_of_buffer,
  void *buffer
)
{
  uint64_t start_time = rtems_clock_get_uptime_nanoseconds();
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;
  uint32_t page_index = offset / attr->page_size_bytes;
  uint32_t oob_offset = page_index * attr->page_oob_bytes;
  unsigned char *chunk = &flash_driver->oob[ oob_offset ];

  memcpy( buffer, chunk, size_of_buffer );

  complete_operation_delay( start_time, attr->read_delay_ns );

  return 0;
}

static int flash_sim_oob_write(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t size_of_buffer,
  const void *buffer
)
{
  uint64_t start_time = rtems_clock_get_uptime_nanoseconds();
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  struct flash_sim_flashdev_attributes *attr = &flash_driver->attr;
  uint32_t page_index = offset / attr->page_size_bytes;
  uint32_t oob_offset = page_index * attr->page_oob_bytes;
  unsigned char *chunk = &flash_driver->oob[ oob_offset ];
  const uint8_t *cbuff = buffer;
  size_t i;

  for ( i = 0; i < size_of_buffer; ++i ) {
    chunk[ i ] &= cbuff[ i ];
  }

  complete_operation_delay( start_time, attr->write_delay_ns );

  return 0;
}

static int flash_sim_sector_mark_bad( rtems_flashdev *flashdev, off_t offset )
{
  (void) flashdev;
  (void) offset;

  return 0;
}

static void flashdev_free(
  struct flash_sim_flashdev_attributes *attr,
  void *ptr
)
{
  if ( attr->free != NULL ) {
    attr->free( ptr );
  } else {
    free(ptr);
  }
}

static void *flashdev_malloc(
  struct flash_sim_flashdev_attributes *attr,
  size_t size
)
{
  if (attr->alloc != NULL) {
    return attr->alloc(size);
  }

  return malloc(size);
}

static void free_nand_priv( nand_priv_data *ntable )
{
  if ( ntable == NULL ) {
    return;
  }

  if ( ntable->area != NULL ) {
    flashdev_free( &ntable->attr, ntable->area );
  }
  if ( ntable->oob != NULL ) {
    flashdev_free( &ntable->attr, ntable->oob );
  }
  free( ntable );
}

static void flash_sim_priv_destroy( rtems_flashdev *flash )
{
  free_nand_priv( flash->driver );
  free( flash->region_table );
}

rtems_flashdev *flash_sim_flashdev_init(
  struct flash_sim_flashdev_attributes *attr
)
{
  uint64_t total_pages;
  nand_priv_data *ntable = calloc( 1, sizeof( nand_priv_data ) );

  if ( ntable == NULL ) {
    return NULL;
  }

  if ( attr != NULL ) {
    ntable->attr = *attr;
    if ( attr->type == RTEMS_FLASHDEV_NAND && attr->page_oob_bytes == 0 ) {
      free_nand_priv( ntable );
      return NULL;
    }
  } else {
    /* No attributes provided, set defaults */
    ntable->attr.read_delay_ns = 0;
    ntable->attr.write_delay_ns = 0;
    ntable->attr.erase_delay_ns = 0;
    ntable->attr.page_size_bytes = 1024;
    ntable->attr.page_oob_bytes = 32;
    ntable->attr.pages_per_sector = 16;
    ntable->attr.total_sectors = 8;
    ntable->attr.type = RTEMS_FLASHDEV_NAND;
    ntable->attr.alloc = NULL;
    ntable->attr.free = NULL;
  }

  rtems_flashdev_region_table *ftable =
    calloc( 1, sizeof( rtems_flashdev_region_table ) );

  if ( ftable == NULL ) {
    free_nand_priv( ntable );
    return NULL;
  }

  total_pages = ntable->attr.total_sectors * ntable->attr.pages_per_sector;

  ntable->area = flashdev_malloc( &ntable->attr, total_pages * ntable->attr.page_size_bytes );
  if ( ntable->area == NULL ) {
    free_nand_priv( ntable );
    return NULL;
  }
  memset( ntable->area, 0xff, total_pages * ntable->attr.page_size_bytes );

  if ( ntable->attr.type == RTEMS_FLASHDEV_NAND ) {
    ntable->oob = flashdev_malloc( &ntable->attr, total_pages * ntable->attr.page_oob_bytes );
    if ( ntable->oob == NULL ) {
      free_nand_priv( ntable );
      return NULL;
    }
    memset( ntable->oob, 0xff, total_pages * ntable->attr.page_oob_bytes );
  }

  ftable->regions = &ntable->regions[ 0 ];
  ftable->max_regions = NAND_SIM_FLASH_MAX_REGIONS;
  ftable->bit_allocator = &ntable->bit_allocator;

  rtems_flashdev *flash =
    rtems_flashdev_alloc_and_init( sizeof( rtems_flashdev ) );

  if ( flash == NULL ) {
    free_nand_priv( ntable );
    free( ftable );
    return NULL;
  }

  flash->driver = ntable;
  flash->priv_destroy = &flash_sim_priv_destroy;
  flash->read = &flash_sim_read_wrapper;
  flash->write = &flash_sim_write_wrapper;
  flash->erase = &flash_sim_erase_wrapper;
  flash->jedec_id = &flash_sim_get_jedec_id;
  flash->flash_type = &flash_sim_get_flash_type;
  flash->page_info_by_offset = &flash_sim_page_info_by_off;
  flash->page_info_by_index = &flash_sim_page_info_by_index;
  flash->page_count = &flash_sim_page_count;
  flash->write_block_size = &flash_sim_write_block_size;
  flash->sector_info_by_offset = &flash_sim_sector_info_by_offset;
  flash->sector_count = &flash_sim_sector_count;
  flash->region_table = ftable;
  if ( ntable->attr.type == RTEMS_FLASHDEV_NAND ) {
    flash->oob_bytes_per_page = &flash_sim_oob_bytes_per_page;
    flash->oob_read = &flash_sim_oob_read;
    flash->oob_write = &flash_sim_oob_write;
    flash->sector_mark_bad = &flash_sim_sector_mark_bad;
    flash->sector_health = &flash_sim_sector_health;
  }

  return flash;
}
