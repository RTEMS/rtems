/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/bdpart.h>
#include <rtems/endian.h>

static void rtems_bdpart_write_mbr_partition(
  uint8_t *data,
  uint32_t begin,
  uint32_t size,
  uint8_t type,
  uint8_t flags
)
{
  rtems_uint32_to_little_endian( begin, data + RTEMS_BDPART_MBR_OFFSET_BEGIN);
  rtems_uint32_to_little_endian( size, data + RTEMS_BDPART_MBR_OFFSET_SIZE);
  data [RTEMS_BDPART_MBR_OFFSET_TYPE] = type;
  data [RTEMS_BDPART_MBR_OFFSET_FLAGS] = flags;
}

static rtems_status_code rtems_bdpart_new_record(
  rtems_disk_device *dd,
  rtems_blkdev_bnum index,
  rtems_bdbuf_buffer **block
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Synchronize previous block if necessary */
  if (*block != NULL) {
    sc = rtems_bdbuf_sync( *block);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }
  }

  /* Read the new record block (this accounts for disk block sizes > 512) */
  sc = rtems_bdbuf_read( dd, index, block);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* just in case block did not get filled in */
  if ( *block == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Clear record */
  memset( (*block)->buffer, 0, RTEMS_BDPART_BLOCK_SIZE);

  /* Write signature */
  (*block)->buffer [RTEMS_BDPART_MBR_OFFSET_SIGNATURE_0] =
    RTEMS_BDPART_MBR_SIGNATURE_0;
  (*block)->buffer [RTEMS_BDPART_MBR_OFFSET_SIGNATURE_1] =
    RTEMS_BDPART_MBR_SIGNATURE_1;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_bdpart_write(
  const char *disk_name,
  const rtems_bdpart_format *format,
  const rtems_bdpart_partition *pt,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  bool dos_compatibility = format != NULL
    && format->type == RTEMS_BDPART_FORMAT_MBR
    && format->mbr.dos_compatibility;
  rtems_bdbuf_buffer *block = NULL;
  rtems_blkdev_bnum disk_end = 0;
  rtems_blkdev_bnum record_space =
    dos_compatibility ? RTEMS_BDPART_MBR_CYLINDER_SIZE : 1;
  size_t ppc = 0; /* Primary partition count */
  size_t i = 0;
  uint8_t *data = NULL;
  int fd = -1;
  rtems_disk_device *dd = NULL;

  /* Check if we have something to do */
  if (count == 0) {
    /* Nothing to do */
    return RTEMS_SUCCESSFUL;
  }

  /* Check parameter */
  if (format == NULL || pt == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &fd, &dd, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Align end of disk on cylinder boundary if necessary */
  if (dos_compatibility) {
    disk_end -= (disk_end % record_space);
  }

  /* Check that we have a consistent partition table */
  for (i = 0; i < count; ++i) {
    const rtems_bdpart_partition *p = pt + i;

    /* Check that begin and end are proper within the disk */
    if (p->begin >= disk_end || p->end > disk_end) {
      esc = RTEMS_INVALID_NUMBER;
      goto cleanup;
    }

    /* Check that begin and end are valid */
    if (p->begin >= p->end) {
      esc = RTEMS_INVALID_NUMBER;
      goto cleanup;
    }

    /* Check that partitions do not overlap */
    if (i > 0 && pt [i - 1].end > p->begin) {
      esc = RTEMS_INVALID_NUMBER;
      goto cleanup;
    }
  }

  /* Check format */
  if (format->type != RTEMS_BDPART_FORMAT_MBR) {
    esc = RTEMS_NOT_IMPLEMENTED;
    goto cleanup;
  }

  /*
   * Set primary partition count.  If we have more than four partitions we need
   * an extended partition which will contain the partitions of number four and
   * above as logical partitions.  If we have four or less partitions we can
   * use the primary partition table.
   */
  ppc = count <= 4 ? count : 3;

  /*
   * Check that the first primary partition starts at head one and sector one
   * under the virtual one head and 63 sectors geometry if necessary.
   */
  if (dos_compatibility && pt [0].begin != RTEMS_BDPART_MBR_CYLINDER_SIZE) {
    esc = RTEMS_INVALID_NUMBER;
    goto cleanup;
  }

  /*
   * Check that we have enough space for the EBRs.  The partitions with number
   * four and above are logical partitions if we have more than four partitions
   * in total.  The logical partitions are contained in the extended partition.
   * Each logical partition is described via one EBR preceding the partition.
   * The space for the EBR and maybe some space which is needed for DOS
   * compatibility resides between the partitions.  So there have to be gaps of
   * the appropriate size between the partitions.
   */
  for (i = ppc; i < count; ++i) {
    if ((pt [i].begin - pt [i - 1].end) < record_space) {
      esc = RTEMS_INVALID_NUMBER;
      goto cleanup;
    }
  }

  /* Check that we can convert the parition descriptions to the MBR format */
  for (i = 0; i < count; ++i) {
    uint8_t type = 0;

    const rtems_bdpart_partition *p = pt + i;

    /* Check type */
    if (!rtems_bdpart_to_mbr_partition_type( p->type, &type)) {
      esc =  RTEMS_INVALID_ID;
      goto cleanup;
    }

    /* Check flags */
    if (p->flags > 0xffU) {
      esc = RTEMS_INVALID_ID;
      goto cleanup;
    }

    /* Check ID */
    /* TODO */
  }

  /* New MBR */
  sc = rtems_bdpart_new_record( dd, 0, &block);
  if (sc != RTEMS_SUCCESSFUL) {
    esc = sc;
    goto cleanup;
  }

  /* Write disk ID */
  rtems_uint32_to_little_endian(
    format->mbr.disk_id,
    block->buffer + RTEMS_BDPART_MBR_OFFSET_DISK_ID
  );

  /* Write primary partition table */
  data = block->buffer + RTEMS_BDPART_MBR_OFFSET_TABLE_0;
  for (i = 0; i < ppc; ++i) {
    const rtems_bdpart_partition *p = pt + i;

    /* Write partition entry */
    rtems_bdpart_write_mbr_partition(
      data,
      p->begin,
      p->end - p->begin,
      rtems_bdpart_mbr_partition_type( p->type),
      (uint8_t) p->flags
    );

    data += RTEMS_BDPART_MBR_TABLE_ENTRY_SIZE;
  }

  /* Write extended partition with logical partitions if necessary */
  if (ppc != count) {
    rtems_blkdev_bnum ebr = 0; /* Extended boot record block index */

    /* Begin of extended partition */
    rtems_blkdev_bnum ep_begin = pt [ppc].begin - record_space;

    /* Write extended partition */
    rtems_bdpart_write_mbr_partition(
      data,
      ep_begin,
      disk_end - ep_begin,
      RTEMS_BDPART_MBR_EXTENDED,
      0
    );

    /* Write logical partitions */
    for (i = ppc; i < count; ++i) {
      const rtems_bdpart_partition *p = pt + i;

      /* Write second partition entry */
      if (i > ppc) {
        rtems_blkdev_bnum begin = p->begin - record_space;

        rtems_bdpart_write_mbr_partition(
          block->buffer + RTEMS_BDPART_MBR_OFFSET_TABLE_1,
          begin - ep_begin,
          disk_end - begin,
          RTEMS_BDPART_MBR_EXTENDED,
          0
        );
      }

      /* New EBR */
      ebr = p->begin - record_space;
      sc = rtems_bdpart_new_record( dd, ebr, &block);
      if (sc != RTEMS_SUCCESSFUL) {
        esc = sc;
        goto cleanup;
      }

      /* Write first partition entry */
      rtems_bdpart_write_mbr_partition(
        block->buffer + RTEMS_BDPART_MBR_OFFSET_TABLE_0,
        record_space,
        p->end - p->begin,
        rtems_bdpart_mbr_partition_type( p->type),
        (uint8_t) p->flags
      );
    }
  }

cleanup:

  if (fd >= 0) {
    close( fd);
  }

  if (block != NULL) {
    rtems_bdbuf_sync( block);
  }

  return esc;
}
