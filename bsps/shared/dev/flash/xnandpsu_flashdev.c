/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Flash
 *
 * @brief Flashdev backend XNandPsu peripheral
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

#include <assert.h>
#include <dev/flash/xnandpsu_flashdev.h>
#include <dev/nand/xnandpsu_bbm.h>
#include <errno.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <rtems/malloc.h>
#include <rtems/score/basedefs.h>
#include <rtems/thread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define XNANDPSU_FLASH_MAX_REGIONS ( (size_t) 32 )

/**
 * @brief Struct allocating memory space for flash regions. Used by
 * rtems_flashdev to store region allocations.
 */
typedef struct nand_priv_data {
  rtems_flashdev_region regions[ XNANDPSU_FLASH_MAX_REGIONS ];
  uint32_t bit_allocator;
  XNandPsu *nandpsu;
} nand_priv_data;

static uint32_t xnandpsu_get_jedec_id( rtems_flashdev *flash )
{
  (void) flash;

  return 0x00ABCDEF;
}

static int xnandpsu_get_flash_type(
  rtems_flashdev *flash,
  rtems_flashdev_flash_type *type
)
{
  (void) flash;

  *type = RTEMS_FLASHDEV_NAND;

  return 0;
}

static int xnandpsu_read_wrapper(
    rtems_flashdev *flash,
    uintptr_t offset,
    size_t count,
    void *buffer
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  rtems_status_code sc;

  sc = XNandPsu_Read( nandpsu, offset, count, buffer );
  if ( sc ) {
    return -EIO;
  }

  return 0;
}

static int xnandpsu_write_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count,
  const void *buffer
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  rtems_status_code sc;

  sc = XNandPsu_Write( nandpsu, offset, count, (void *) buffer );
  if ( sc ) {
    return -EIO;
  }

  return 0;
}

static int xnandpsu_sector_health(
  rtems_flashdev *flash,
  off_t search_offset,
  uint8_t *sector_bad
);

static int xnandpsu_erase_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  int rv;
  uint64_t block_size = nandpsu->Geometry.BlockSize;
  uint64_t end_offset = RTEMS_ALIGN_UP( offset + count, block_size );
  uint64_t start_offset = RTEMS_ALIGN_DOWN( offset, block_size );
  uint64_t erase_count = ( end_offset - start_offset ) / block_size;

  if ( offset > nandpsu->Geometry.DeviceSize ) {
    return -EIO;
  }

  /* Perform erase operation. */
  for ( int i = 0; i < erase_count; i++ ) {
    uint8_t sector_bad = 0;
    uint64_t block_start = start_offset + i * block_size;
    rv = xnandpsu_sector_health( flash, block_start, &sector_bad );

    if ( rv != 0 || sector_bad != 0 ) {
      /*
       * Skip erasure of "bad" sectors from this interface. Maintenance of such
       * sectors should occur outside of flashdev since flashdev does not
       * directly manage the bad sectors of a flash device or their management
       * across multiple stacked devices behind the controller.
       */
      continue;
    }

    rv = XNandPsu_Erase( nandpsu, block_start, block_size );
    if ( rv != 0 ) {
      return -EIO;
    }
  }

  return 0;
}

static int xnandpsu_page_info_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *page_offset,
  size_t *page_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *page_size = nandpsu->Geometry.BytesPerPage;
  *page_offset = RTEMS_ALIGN_DOWN( search_offset, *page_size );

  return 0;
}

static int xnandpsu_page_info_by_index(
  rtems_flashdev *flash,
  off_t search_index,
  off_t *page_offset,
  size_t *page_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *page_size = nandpsu->Geometry.BytesPerPage;
  *page_offset = *page_size * search_index;

  return 0;
}

static int xnandpsu_page_count( rtems_flashdev *flash, int *page_count )
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *page_count = nandpsu->Geometry.DeviceSize / nandpsu->Geometry.BytesPerPage;

  return 0;
}

static int xnandpsu_write_block_size(
  rtems_flashdev *flash,
  size_t *write_block_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *write_block_size = nandpsu->Geometry.SpareBytesPerPage;

  return 0;
}

static int xnandpsu_sector_count(
  rtems_flashdev *flash,
  int *sector_count
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *sector_count = nandpsu->Geometry.DeviceSize / nandpsu->Geometry.BlockSize;

  return 0;
}

static int xnandpsu_sector_info_by_offset(
  rtems_flashdev *flash,
  off_t search_off,
  off_t *sector_off,
  size_t *sector_size
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *sector_off = RTEMS_ALIGN_DOWN( search_off, nandpsu->Geometry.BlockSize );
  *sector_size = nandpsu->Geometry.BlockSize;

  if ( *sector_off > nandpsu->Geometry.DeviceSize ) {
    return -EIO;
  }

  return 0;
}

static int xnandpsu_sector_health(
  rtems_flashdev *flash,
  off_t search_offset,
  uint8_t *sector_bad
)
{
  uint32_t BlockIndex;
  uint8_t BlockData;
  uint8_t BlockShift;
  uint8_t BlockType;
  uint32_t BlockOffset;
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  off_t sector_off;

  sector_off = RTEMS_ALIGN_DOWN( search_offset, nandpsu->Geometry.BlockSize );

  if ( sector_off > nandpsu->Geometry.DeviceSize ) {
    return -EIO;
  }

  *sector_bad = 1;

  BlockIndex = sector_off / nandpsu->Geometry.BlockSize;

  /* XNandPsu_IsBlockBad() is insufficient for this use case */
  BlockOffset = BlockIndex >> XNANDPSU_BBT_BLOCK_SHIFT;
  BlockShift = XNandPsu_BbtBlockShift( BlockIndex );
  BlockData = nandpsu->Bbt[ BlockOffset ];
  BlockType = ( BlockData >> BlockShift ) & XNANDPSU_BLOCK_TYPE_MASK;

  if ( BlockType == XNANDPSU_BLOCK_GOOD ) {
    *sector_bad = 0;
  }

  int TargetBlockIndex = BlockIndex % nandpsu->Geometry.NumTargetBlocks;
  /* The last 4 blocks of every device target are reserved for the BBT */
  if ( nandpsu->Geometry.NumTargetBlocks - TargetBlockIndex <= 4 ) {
    *sector_bad = 1;
  }

  return 0;
}

static int xnandpsu_oob_bytes_per_page(
  rtems_flashdev *flash,
  size_t *oob_bytes_per_page
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;

  *oob_bytes_per_page = nandpsu->Geometry.SpareBytesPerPage;

  return 0;
}

static int xnandpsu_oob_read(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t ooblen,
  void *oobbuf
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  uint8_t *spare_bytes;
  uint32_t SpareBytesPerPage = nandpsu->Geometry.SpareBytesPerPage;

  if ( offset > nandpsu->Geometry.DeviceSize ) {
    return -EIO;
  }

  /* Can't request more spare bytes than exist */
  if ( ooblen > SpareBytesPerPage * nandpsu->Geometry.PagesPerBlock ) {
    return -EIO;
  }

  /* Get page index */
  uint32_t PageIndex = offset / nandpsu->Geometry.BytesPerPage;

  spare_bytes = rtems_malloc( SpareBytesPerPage );
  if ( spare_bytes == NULL ) {
    return -ENOMEM;
  }

  while ( ooblen != 0 ) {
    int rv = XNandPsu_ReadSpareBytes( nandpsu, PageIndex, spare_bytes );
    /* no guarantee oobbuf can hold all of spare bytes, so read and then copy */
    uint32_t readlen = SpareBytesPerPage;
    if ( ooblen < readlen ) {
      readlen = ooblen;
    }

    if ( rv != 0 ) {
      free( spare_bytes );
      return -EIO;
    }

    memcpy( oobbuf, spare_bytes, readlen );

    PageIndex++;
    ooblen -= readlen;
    oobbuf += readlen;
  }
  free( spare_bytes );

  return 0;
}

static int xnandpsu_oob_write(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t ooblen,
  const void *oobbuf
)
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  rtems_status_code sc;
  uint8_t *spare_bytes;
  uint8_t *buffer = (void *) oobbuf;
  uint32_t SpareBytesPerPage = nandpsu->Geometry.SpareBytesPerPage;

  if ( offset > nandpsu->Geometry.DeviceSize ) {
    return -EIO;
  }

  /* Writing a page spare area to large will result in ignored data.  */
  if ( ooblen > SpareBytesPerPage ) {
    return -EIO;
  }

  spare_bytes = rtems_malloc( SpareBytesPerPage );
  if ( spare_bytes == NULL ) {
    return -ENOMEM;
  }

  /* Writing a page spare area to small will result in invalid accesses */
  if ( ooblen < SpareBytesPerPage ) {
    int rv = xnandpsu_oob_read( flash, offset, SpareBytesPerPage, spare_bytes );
    if ( rv != 0 ) {
      free( spare_bytes );
      return rv;
    }
    buffer = spare_bytes;
    memcpy( buffer, oobbuf, ooblen );
  }

  /* Get page index */
  uint32_t PageIndex = offset / nandpsu->Geometry.BytesPerPage;

  sc = XNandPsu_WriteSpareBytes( nandpsu, PageIndex, buffer );
  free( spare_bytes );

  if ( sc != XST_SUCCESS ) {
    return -EIO;
  }

  return 0;
}

static int xnandpsu_sector_mark_bad( rtems_flashdev *flash, off_t offset )
{
  struct nand_priv_data *flash_driver = (struct nand_priv_data *) flash->driver;
  XNandPsu *nandpsu = flash_driver->nandpsu;
  rtems_status_code sc;
  uint32_t BlockIndex;

  if ( offset > nandpsu->Geometry.DeviceSize ) {
    return -EIO;
  }

  BlockIndex = offset / nandpsu->Geometry.BlockSize;

  sc = XNandPsu_MarkBlockBad( nandpsu, BlockIndex );
  if ( sc != XST_SUCCESS ) {
    return -EIO;
  }

  return 0;
}

static void xnandpsu_priv_destroy( rtems_flashdev *flash )
{
  free( flash->driver );
  free( flash->region_table );
}

rtems_flashdev *xnandpsu_flashdev_init( XNandPsu *nandpsu )
{
  nand_priv_data *ntable =
    calloc( 1, sizeof( nand_priv_data ) );

  if ( ntable == NULL ) {
    return NULL;
  }

  ntable->nandpsu = nandpsu;

  rtems_flashdev_region_table *ftable =
    calloc( 1, sizeof( rtems_flashdev_region_table ) );

  if ( ftable == NULL ) {
    free( ntable );
    return NULL;
  }

  ftable->regions = &( ntable->regions[ 0 ] );
  ftable->max_regions = XNANDPSU_FLASH_MAX_REGIONS;
  ftable->bit_allocator = &( ntable->bit_allocator );

  rtems_flashdev *flash =
    rtems_flashdev_alloc_and_init( sizeof( rtems_flashdev ) );

  if ( flash == NULL ) {
    free( ntable );
    free( ftable );
    return NULL;
  }

  flash->driver = ntable;
  flash->priv_destroy = &xnandpsu_priv_destroy;
  flash->read = &xnandpsu_read_wrapper;
  flash->write = &xnandpsu_write_wrapper;
  flash->erase = &xnandpsu_erase_wrapper;
  flash->jedec_id = &xnandpsu_get_jedec_id;
  flash->flash_type = &xnandpsu_get_flash_type;
  flash->page_info_by_offset = &xnandpsu_page_info_by_off;
  flash->page_info_by_index = &xnandpsu_page_info_by_index;
  flash->page_count = &xnandpsu_page_count;
  flash->write_block_size = &xnandpsu_write_block_size;
  flash->sector_info_by_offset = &xnandpsu_sector_info_by_offset;
  flash->sector_count = &xnandpsu_sector_count;
  flash->oob_bytes_per_page = &xnandpsu_oob_bytes_per_page;
  flash->oob_read = &xnandpsu_oob_read;
  flash->oob_write = &xnandpsu_oob_write;
  flash->sector_mark_bad = &xnandpsu_sector_mark_bad;
  flash->sector_health = &xnandpsu_sector_health;
  flash->region_table = ftable;

  return flash;
}
