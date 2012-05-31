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

#include <sys/stat.h>
#include <fcntl.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/bdpart.h>
#include <rtems/endian.h>

#define RTEMS_BDPART_MBR_PARTITION_TYPE( type) \
  { \
    (type), 0xa2U, 0x2eU, 0x38U, \
    0x38U, 0xb5U, 0xdeU, 0x11U, \
    0xbcU, 0x13U, 0x00U, 0x1dU, \
    0x09U, 0xb0U, 0x5fU, 0xa4U \
  }

static const uuid_t RTEMS_BDPART_MBR_MASTER_TYPE =
  RTEMS_BDPART_MBR_PARTITION_TYPE( RTEMS_BDPART_MBR_EMPTY);

void rtems_bdpart_to_partition_type( uint8_t mbr_type, uuid_t type)
{
  type [0] = mbr_type;
  memcpy( type + 1, RTEMS_BDPART_MBR_MASTER_TYPE + 1, sizeof( uuid_t) - 1);
}

bool rtems_bdpart_to_mbr_partition_type(
  const uuid_t type,
  uint8_t *mbr_type
)
{
  *mbr_type = rtems_bdpart_mbr_partition_type( type);

  return memcmp(
    type + 1,
    RTEMS_BDPART_MBR_MASTER_TYPE + 1,
    sizeof( uuid_t) - 1
  ) == 0;
}

/*
 * FIXME: This code should the deviceio interface and not the bdbug interface.
 */
rtems_status_code rtems_bdpart_get_disk_data(
  const char *disk_name,
  int *fd_ptr,
  rtems_disk_device **dd_ptr,
  rtems_blkdev_bnum *disk_end
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  int fd = -1;
  rtems_disk_device *dd = NULL;
  rtems_blkdev_bnum disk_begin = 0;
  rtems_blkdev_bnum block_size = 0;

  /* Open device file */
  fd = open( disk_name, O_RDWR);
  if (fd < 0) {
    sc = RTEMS_INVALID_NAME;
    goto error;
  }

  /* Get disk handle */
  rv = rtems_disk_fd_get_disk_device( fd, &dd);
  if (rv != 0) {
    sc = RTEMS_INVALID_NAME;
    goto error;
  }

  /* Get disk begin, end and block size */
  disk_begin = dd->start;
  *disk_end = dd->size;
  block_size = dd->block_size;

  /* Check block size */
  if (block_size < RTEMS_BDPART_BLOCK_SIZE) {
    sc = RTEMS_IO_ERROR;
    goto error;
  }

  /* Check that we have do not have a logical disk */
  if (disk_begin != 0) {
    sc = RTEMS_IO_ERROR;
    goto error;
  }

error:

  if (sc == RTEMS_SUCCESSFUL && fd_ptr != NULL && dd_ptr != NULL) {
    *fd_ptr = fd;
    *dd_ptr = dd;
  } else {
    close( fd);
  }

  return sc;
}

static bool rtems_bdpart_is_valid_record( const uint8_t *data)
{
  return data [RTEMS_BDPART_MBR_OFFSET_SIGNATURE_0]
      == RTEMS_BDPART_MBR_SIGNATURE_0
    && data [RTEMS_BDPART_MBR_OFFSET_SIGNATURE_1]
      == RTEMS_BDPART_MBR_SIGNATURE_1;
}

static rtems_blkdev_bnum rtems_bdpart_next_ebr( const uint8_t *data)
{
  rtems_blkdev_bnum begin =
    rtems_uint32_from_little_endian( data + RTEMS_BDPART_MBR_OFFSET_BEGIN);
  uint8_t type = data [RTEMS_BDPART_MBR_OFFSET_TYPE];

  if (type == RTEMS_BDPART_MBR_EXTENDED) {
    return begin;
  } else {
    return 0;
  }
}

static rtems_status_code rtems_bdpart_read_mbr_partition(
  const uint8_t *data,
  rtems_bdpart_partition **p,
  const rtems_bdpart_partition *p_end,
  rtems_blkdev_bnum *ep_begin
)
{
  rtems_blkdev_bnum begin =
    rtems_uint32_from_little_endian( data + RTEMS_BDPART_MBR_OFFSET_BEGIN);
  rtems_blkdev_bnum size =
    rtems_uint32_from_little_endian( data + RTEMS_BDPART_MBR_OFFSET_SIZE);
  rtems_blkdev_bnum end = begin + size;
  uint8_t type = data [RTEMS_BDPART_MBR_OFFSET_TYPE];

  if (type == RTEMS_BDPART_MBR_EMPTY) {
    return RTEMS_SUCCESSFUL;
  } else if (*p == p_end) {
    return RTEMS_TOO_MANY;
  } else if (begin >= end) {
    return RTEMS_IO_ERROR;
  } else if (type == RTEMS_BDPART_MBR_EXTENDED) {
    if (ep_begin != NULL) {
      *ep_begin = begin;
    }
  } else {
    /* Increment partition index */
    ++(*p);

    /* Clear partition */
    memset( *p, 0, sizeof( rtems_bdpart_partition));

    /* Set values */
    (*p)->begin = begin;
    (*p)->end = end;
    rtems_bdpart_to_partition_type( type, (*p)->type);
    (*p)->flags = data [RTEMS_BDPART_MBR_OFFSET_FLAGS];
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code rtems_bdpart_read_record(
  rtems_disk_device *dd,
  rtems_blkdev_bnum index,
  rtems_bdbuf_buffer **block
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Release previous block if necessary */
  if (*block != NULL) {
    sc = rtems_bdbuf_release( *block);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }
  }

  /* Read the record block */
  sc = rtems_bdbuf_read( dd, index, block);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* just in case block did not get filled in */
  if ( *block == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Check MBR signature */
  if (!rtems_bdpart_is_valid_record( (*block)->buffer)) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_bdpart_read(
  const char *disk_name,
  rtems_bdpart_format *format,
  rtems_bdpart_partition *pt,
  size_t *count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *block = NULL;
  rtems_bdpart_partition *p = pt - 1;
  const rtems_bdpart_partition *p_end = pt + (count != NULL ? *count : 0);
  rtems_blkdev_bnum ep_begin = 0; /* Extended partition begin */
  rtems_blkdev_bnum ebr = 0; /* Extended boot record block index */
  rtems_blkdev_bnum disk_end = 0;
  size_t i = 0;
  const uint8_t *data = NULL;
  int fd = -1;
  rtems_disk_device *dd = NULL;

  /* Check parameter */
  if (format == NULL || pt == NULL || count == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Set count to a save value */
  *count = 0;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &fd, &dd, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Read MBR */
  sc = rtems_bdpart_read_record( dd, 0, &block);
  if (sc != RTEMS_SUCCESSFUL) {
    esc = sc;
    goto cleanup;
  }

  /* Read the first partition entry */
  data = block->buffer + RTEMS_BDPART_MBR_OFFSET_TABLE_0;
  sc = rtems_bdpart_read_mbr_partition( data, &p, p_end, &ep_begin);
  if (sc != RTEMS_SUCCESSFUL) {
    esc = sc;
    goto cleanup;
  }

  /* Determine if we have a MBR or GPT format */
  if (rtems_bdpart_mbr_partition_type( p->type) == RTEMS_BDPART_MBR_GPT) {
    esc = RTEMS_NOT_IMPLEMENTED;
    goto cleanup;
  }

  /* Set format */
  format->type = RTEMS_BDPART_FORMAT_MBR;
  format->mbr.disk_id = rtems_uint32_from_little_endian(
    block->buffer + RTEMS_BDPART_MBR_OFFSET_DISK_ID
  );
  format->mbr.dos_compatibility = true;

  /* Iterate through the rest of the primary partition table */
  for (i = 1; i < 4; ++i) {
    data += RTEMS_BDPART_MBR_TABLE_ENTRY_SIZE;

    sc = rtems_bdpart_read_mbr_partition( data, &p, p_end, &ep_begin);
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }
  }

  /* Iterate through the logical partitions within the extended partition */
  ebr = ep_begin;
  while (ebr != 0) {
    rtems_blkdev_bnum tmp = 0;

    /* Read EBR */
    sc = rtems_bdpart_read_record( dd, ebr, &block);
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }

    /* Read first partition entry */
    sc = rtems_bdpart_read_mbr_partition(
      block->buffer + RTEMS_BDPART_MBR_OFFSET_TABLE_0,
      &p,
      p_end,
      NULL
    );
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }

    /* Adjust partition begin */
    tmp = p->begin + ebr;
    if (tmp > p->begin) {
      p->begin = tmp;
    } else {
      esc = RTEMS_IO_ERROR;
      goto cleanup;
    }

    /* Adjust partition end */
    tmp = p->end + ebr;
    if (tmp > p->end) {
      p->end = tmp;
    } else {
      esc = RTEMS_IO_ERROR;
      goto cleanup;
    }

    /* Read second partition entry for next EBR block */
    ebr = rtems_bdpart_next_ebr(
      block->buffer + RTEMS_BDPART_MBR_OFFSET_TABLE_1
    );
    if (ebr != 0) {
      /* Adjust partition EBR block index */
      tmp = ebr + ep_begin;
      if (tmp > ebr) {
        ebr = tmp;
      } else {
        esc = RTEMS_IO_ERROR;
        goto cleanup;
      }
    }
  }

  /* Return partition count */
  *count = (size_t) (p - pt + 1);

cleanup:

  if (fd >= 0) {
    close( fd);
  }

  if (block != NULL) {
    rtems_bdbuf_release( block);
  }

  return esc;
}
