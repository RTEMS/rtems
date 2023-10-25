/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

/*
 * This file contains an implementation of a basic JFFS2 filesystem adapter for
 * the NandPsu peripheral that uses the entirety of the available NAND chip(s)
 * for a JFFS2 filesystem or up to the maximum size possible. If an
 * implementation would prefer to only use a portion of the NAND flash chip,
 * this template would need rework to account for a reduced size and possibly a
 * start offset while also taking into account the driver's handling of bad
 * blocks and how that might affect the offset.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include <bsp/jffs2_xnandpsu.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>
#include <rtems/thread.h>
#include <dev/nand/xnandpsu_bbm.h>

typedef struct {
  rtems_jffs2_flash_control super;
  XNandPsu *nandpsu;
  rtems_mutex access_lock;
} flash_control;

static flash_control *get_flash_control(rtems_jffs2_flash_control *super)
{
  return (flash_control *) super;
}

static int flash_read(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  unsigned char *buffer,
  size_t size_of_buffer
)
{
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  rtems_status_code sc;

  rtems_mutex_lock(&(get_flash_control(super)->access_lock));
  sc = XNandPsu_Read(nandpsu, offset, size_of_buffer, buffer);
  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  if (sc) {
    return -EIO;
  }
  return 0;
}

static int flash_write(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  const unsigned char *buffer,
  size_t size_of_buffer
)
{
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  rtems_status_code sc;

  rtems_mutex_lock(&(get_flash_control(super)->access_lock));
  sc = XNandPsu_Write(nandpsu, offset, size_of_buffer, (void *)buffer);
  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  if (sc) {
    return -EIO;
  }
  return 0;
}

static int flash_erase(
  rtems_jffs2_flash_control *super,
  uint32_t offset
)
{
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  rtems_status_code sc;
  uint64_t BlockSize = nandpsu->Geometry.BlockSize;

  if (offset > nandpsu->Geometry.DeviceSize) {
    return -EIO;
  }

  /* Perform erase operation. */
  rtems_mutex_lock(&(get_flash_control(super)->access_lock));
  sc = XNandPsu_Erase(nandpsu, RTEMS_ALIGN_DOWN(offset, BlockSize), BlockSize);
  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  if (sc ) {
    return -EIO;
  }

  return 0;
}

static int flash_block_is_bad(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  bool *bad
)
{
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  uint32_t BlockIndex;
  uint8_t BlockData;
  uint8_t BlockShift;
  uint8_t BlockType;
  uint32_t BlockOffset;

  assert(bad);

  if (offset > nandpsu->Geometry.DeviceSize) {
    return -EIO;
  }

  *bad = true;

  BlockIndex = offset / nandpsu->Geometry.BlockSize;

  rtems_mutex_lock(&(get_flash_control(super)->access_lock));

  /* XNandPsu_IsBlockBad() is insufficient for this use case */
  BlockOffset = BlockIndex >> XNANDPSU_BBT_BLOCK_SHIFT;
  BlockShift = XNandPsu_BbtBlockShift(BlockIndex);
  BlockData = nandpsu->Bbt[BlockOffset];
  BlockType = (BlockData >> BlockShift) & XNANDPSU_BLOCK_TYPE_MASK;

  if (BlockType == XNANDPSU_BLOCK_GOOD) {
    *bad = false;
  }

  int TargetBlockIndex = BlockIndex % nandpsu->Geometry.NumTargetBlocks;
  /* The last 4 blocks of every device target are reserved for the BBT */
  if (nandpsu->Geometry.NumTargetBlocks - TargetBlockIndex <= 4) {
    *bad = true;
  }

  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  return 0;
}

static int flash_block_mark_bad(
  rtems_jffs2_flash_control *super,
  uint32_t offset
)
{
  rtems_status_code sc;
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  uint32_t BlockIndex;

  if (offset > nandpsu->Geometry.DeviceSize) {
    return -EIO;
  }

  BlockIndex = offset / nandpsu->Geometry.BlockSize;

  rtems_mutex_lock(&(get_flash_control(super)->access_lock));
  sc = XNandPsu_MarkBlockBad(nandpsu, BlockIndex);
  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  if ( sc != XST_SUCCESS ) {
    return -EIO;
  }
  return RTEMS_SUCCESSFUL;
}

static int flash_read_oob_locked(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  uint8_t *oobbuf,
  uint32_t ooblen
)
{
  uint8_t *spare_bytes;
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  uint32_t SpareBytesPerPage = nandpsu->Geometry.SpareBytesPerPage;

  if (offset > nandpsu->Geometry.DeviceSize) {
    return -EIO;
  }

  /* Can't request more spare bytes than exist */
  if (ooblen > SpareBytesPerPage * nandpsu->Geometry.PagesPerBlock) {
    return -EIO;
  }

  /* Get page index */
  uint32_t PageIndex = offset / nandpsu->Geometry.BytesPerPage;

  spare_bytes = rtems_malloc(SpareBytesPerPage);
  if (spare_bytes == NULL) {
    return -ENOMEM;
  }

  while (ooblen) {
    int rv = XNandPsu_ReadSpareBytes(nandpsu, PageIndex, spare_bytes);
    /* no guarantee oobbuf can hold all of spare bytes, so read and then copy */
    uint32_t readlen = SpareBytesPerPage;
    if (ooblen < readlen) {
	    readlen = ooblen;
    }

    if (rv) {
      free(spare_bytes);
      return -EIO;
    }

    memcpy(oobbuf, spare_bytes, readlen);

    PageIndex++;
    ooblen -= readlen;
    oobbuf += readlen;
  }
  free(spare_bytes);
  return RTEMS_SUCCESSFUL;
}

static int flash_read_oob(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  uint8_t *oobbuf,
  uint32_t ooblen
)
{
  rtems_mutex_lock(&(get_flash_control(super)->access_lock));
  int ret = flash_read_oob_locked(super, offset, oobbuf, ooblen);
  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  return ret;
}

static int flash_write_oob(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  uint8_t *oobbuf,
  uint32_t ooblen
)
{
  rtems_status_code sc;
  uint8_t *spare_bytes;
  uint8_t *buffer = oobbuf;
  XNandPsu *nandpsu = get_flash_control(super)->nandpsu;
  uint32_t SpareBytesPerPage = nandpsu->Geometry.SpareBytesPerPage;

  if (offset > nandpsu->Geometry.DeviceSize) {
    return -EIO;
  }

  /* Writing a page spare area to large will result in ignored data.  */
  if (ooblen > SpareBytesPerPage) {
    return -EIO;
  }

  spare_bytes = rtems_malloc(SpareBytesPerPage);
  if (spare_bytes == NULL) {
    return -ENOMEM;
  }

  /* Writing a page spare area to small will result in invalid accesses */
  rtems_mutex_lock(&(get_flash_control(super)->access_lock));
  if (ooblen < SpareBytesPerPage) {
    int rv = flash_read_oob_locked(super, offset, spare_bytes, SpareBytesPerPage);
    if (rv) {
      free(spare_bytes);
      rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
      return rv;
    }
    buffer = spare_bytes;
    memcpy(buffer, oobbuf, ooblen);
  }

  /* Get page index */
  uint32_t PageIndex = offset / nandpsu->Geometry.BytesPerPage;

  sc = XNandPsu_WriteSpareBytes(nandpsu, PageIndex, buffer);
  rtems_mutex_unlock(&(get_flash_control(super)->access_lock));
  free(spare_bytes);

  if ( sc != XST_SUCCESS ) {
    return -EIO;
  }
  return RTEMS_SUCCESSFUL;
}

static uint32_t flash_get_oob_size(
  rtems_jffs2_flash_control *super
)
{
  flash_control *self = get_flash_control(super);

  return self->nandpsu->Geometry.SpareBytesPerPage;
}

static flash_control flash_instance = {
  .super = {
    .read = flash_read,
    .write = flash_write,
    .erase = flash_erase,
    .block_is_bad = flash_block_is_bad,
    .block_mark_bad = flash_block_mark_bad,
    .oob_read = flash_read_oob,
    .oob_write = flash_write_oob,
    .get_oob_size = flash_get_oob_size,
  }
};

static rtems_jffs2_compressor_control compressor_instance = {
  .compress = rtems_jffs2_compressor_rtime_compress,
  .decompress = rtems_jffs2_compressor_rtime_decompress
};

static rtems_jffs2_mount_data mount_data;

int xilinx_zynqmp_nand_jffs2_initialize(
  const char *mount_dir,
  XNandPsu *NandInstPtr
)
{
  flash_instance.super.block_size = NandInstPtr->Geometry.BlockSize;

  uint64_t max_size = 0x100000000LU - flash_instance.super.block_size;

  /* JFFS2 maximum FS size is one block less than 4GB */
  if (NandInstPtr->Geometry.DeviceSize > max_size) {
    flash_instance.super.flash_size = max_size;
  } else {
    flash_instance.super.flash_size = NandInstPtr->Geometry.DeviceSize;
  }

  flash_instance.super.write_size = NandInstPtr->Geometry.BytesPerPage;
  flash_instance.nandpsu = NandInstPtr;
  rtems_mutex_init(&flash_instance.access_lock, "XNandPsu JFFS2 adapter lock");
  mount_data.flash_control = &flash_instance.super;
  mount_data.compressor_control = &compressor_instance;

  int rv = 0;
  rv = mount(
    NULL,
    mount_dir,
    RTEMS_FILESYSTEM_TYPE_JFFS2,
    RTEMS_FILESYSTEM_READ_WRITE,
    &mount_data
  );
  if ( rv != 0 ) {
    return rv;
  }

  return 0;
}
