/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <rtems/bdbuf.h>
#include <rtems/bdpart.h>
#include <rtems/dosfs.h>
#include <rtems/endian.h>
#include <rtems/fsmount.h>

#define RTEMS_BDPART_MBR_PARTITION_TYPE( type) \
  { \
    (type), 0xa2U, 0x2eU, 0x38U, \
    0x38U, 0xb5U, 0xdeU, 0x11U, \
    0xbcU, 0x13U, 0x00U, 0x1dU, \
    0x09U, 0xb0U, 0x5fU, 0xa4U \
  }

#define RTEMS_BDPART_BLOCK_SIZE 512

#define RTEMS_BDPART_MBR_CYLINDER_SIZE 63

#define RTEMS_BDPART_MBR_TABLE_ENTRY_SIZE 16

#define RTEMS_BDPART_MBR_OFFSET_TABLE_0 446

#define RTEMS_BDPART_MBR_OFFSET_TABLE_1 \
  (RTEMS_BDPART_MBR_OFFSET_TABLE_0 + RTEMS_BDPART_MBR_TABLE_ENTRY_SIZE)

#define RTEMS_BDPART_MBR_OFFSET_DISK_ID 440

#define RTEMS_BDPART_MBR_OFFSET_SIGNATURE_0 510

#define RTEMS_BDPART_MBR_OFFSET_SIGNATURE_1 511

#define RTEMS_BDPART_MBR_SIGNATURE_0 0x55U

#define RTEMS_BDPART_MBR_SIGNATURE_1 0xaaU

#define RTEMS_BDPART_MBR_OFFSET_BEGIN 8

#define RTEMS_BDPART_MBR_OFFSET_SIZE 12

#define RTEMS_BDPART_MBR_OFFSET_TYPE 4

#define RTEMS_BDPART_MBR_OFFSET_FLAGS 0

#define RTEMS_BDPART_PARTITION_READ_MAX 32

static const uuid_t RTEMS_BDPART_MBR_MASTER_TYPE =
  RTEMS_BDPART_MBR_PARTITION_TYPE( RTEMS_BDPART_MBR_EMPTY);

void rtems_bdpart_to_partition_type( uint8_t mbr_type, uuid_t type)
{
  type [0] = mbr_type;
  memcpy( type + 1, RTEMS_BDPART_MBR_MASTER_TYPE + 1, sizeof( uuid_t) - 1);
}

static uint8_t rtems_bdpart_mbr_partition_type(
  const uuid_t type
)
{
  return type [0];
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

static void rtems_bdpart_type_to_string(
  const uuid_t type,
  char str [37]
)
{
  uuid_unparse_lower( type, str);
}

/*
 * FIXME: This code should the deviceio interface and not the bdbug interface.
 */
static rtems_status_code rtems_bdpart_get_disk_data(
  const char *disk_name,
  dev_t *disk,
  rtems_blkdev_bnum *disk_end
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  rtems_blkdev_bnum disk_begin = 0;
  rtems_blkdev_bnum block_size = 0;
  rtems_disk_device *dd = NULL;
  struct stat st;

  /* Get disk handle */
  rv = stat( disk_name, &st);
  if (rv != 0) {
    return RTEMS_INVALID_NAME;
  }
  *disk = st.st_rdev;

  /* Get disk begin, end and block size */
  dd = rtems_disk_obtain( *disk);
  if (dd == NULL) {
    return RTEMS_INVALID_NAME;
  }
  disk_begin = dd->start;
  *disk_end = dd->size;
  block_size = dd->block_size;
  sc = rtems_disk_release( dd);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Check block size */
  if (block_size < RTEMS_BDPART_BLOCK_SIZE) {
    return RTEMS_IO_ERROR;
  }

  /* Check that we have do not have a logical disk */
  if (disk_begin != 0) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

static int rtems_bdpart_partition_compare( const void *aa, const void *bb)
{
  const rtems_bdpart_partition *a = aa;
  const rtems_bdpart_partition *b = bb;

  if (a->begin < b->begin) {
    return -1;
  } else if (a->begin == b->begin) {
    return 0;
  } else {
    return 1;
  }
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

static rtems_status_code rtems_bdpart_read_record(
  dev_t disk,
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
  sc = rtems_bdbuf_read( disk, index, block);
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

static rtems_status_code rtems_bdpart_new_record(
  dev_t disk,
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
  sc = rtems_bdbuf_read( disk, index, block);
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
  dev_t disk = 0;
  size_t i = 0;
  const uint8_t *data = NULL;

  /* Check parameter */
  if (format == NULL || pt == NULL || count == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Set count to a save value */
  *count = 0;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &disk, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Read MBR */
  sc = rtems_bdpart_read_record( disk, 0, &block);
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
    sc = rtems_bdpart_read_record( disk, ebr, &block);
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

  if (block != NULL) {
    rtems_bdbuf_release( block);
  }

  return esc;
}

void rtems_bdpart_sort( rtems_bdpart_partition *pt, size_t count)
{
  qsort( pt, count, sizeof( *pt), rtems_bdpart_partition_compare);
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
  dev_t disk = 0;
  size_t ppc = 0; /* Primary partition count */
  size_t i = 0;
  uint8_t *data = NULL;

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
  sc = rtems_bdpart_get_disk_data( disk_name, &disk, &disk_end);
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
  sc = rtems_bdpart_new_record( disk, 0, &block);
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
      sc = rtems_bdpart_new_record( disk, ebr, &block);
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

  if (block != NULL) {
    rtems_bdbuf_sync( block);
  }

  return esc;
}

rtems_status_code rtems_bdpart_create(
  const char *disk_name,
  const rtems_bdpart_format *format,
  rtems_bdpart_partition *pt,
  const unsigned *dist,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bool dos_compatibility = format != NULL
    && format->type == RTEMS_BDPART_FORMAT_MBR
    && format->mbr.dos_compatibility;
  rtems_blkdev_bnum disk_end = 0;
  rtems_blkdev_bnum pos = 0;
  rtems_blkdev_bnum dist_sum = 0;
  rtems_blkdev_bnum record_space =
    dos_compatibility ? RTEMS_BDPART_MBR_CYLINDER_SIZE : 1;
  rtems_blkdev_bnum overhead = 0;
  rtems_blkdev_bnum free_space = 0;
  dev_t disk = 0;
  size_t i = 0;

  /* Check if we have something to do */
  if (count == 0) {
    /* Nothing to do */
    return RTEMS_SUCCESSFUL;
  }

  /* Check parameter */
  if (format == NULL || pt == NULL || dist == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &disk, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Get distribution sum and check for overflow */
  for (i = 0; i < count; ++i) {
    unsigned prev_sum = dist_sum;

    dist_sum += dist [i];

    if (dist_sum < prev_sum) {
      return RTEMS_INVALID_NUMBER;
    }

    if (dist [i] == 0) {
      return RTEMS_INVALID_NUMBER;
    }
  }

  /* Check format */
  if (format->type != RTEMS_BDPART_FORMAT_MBR) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  /* Align end of disk on cylinder boundary if necessary */
  if (dos_compatibility) {
    disk_end -= (disk_end % record_space);
  }

  /*
   * We need at least space for the MBR and the compatibility space for the
   * first primary partition.
   */
  overhead += record_space;

  /*
   * In case we need an extended partition and logical partitions we have to
   * account for the space of each EBR.
   */
  if (count > 4) {
    overhead += (count - 3) * record_space;
  }

  /*
   * Account space to align every partition on cylinder boundaries if
   * necessary.
   */
  if (dos_compatibility) {
    overhead += (count - 1) * record_space;
  }

  /* Check disk space */
  if ((overhead + count) > disk_end) {
    return RTEMS_IO_ERROR;
  }

  /* Begin of first primary partition */
  pos = record_space;

  /* Space for partitions */
  free_space = disk_end - overhead;

  for (i = 0; i < count; ++i) {
    rtems_bdpart_partition *p = pt + i;

    /* Partition size */
    rtems_blkdev_bnum s = free_space * dist [i];
    if (s < free_space || s < dist [i]) {
      /* TODO: Calculate without overflow */
      return RTEMS_INVALID_NUMBER;
    }
    s /= dist_sum;

    /* Ensure that the partition is not empty */
    if (s == 0) {
      s = 1;
    }

    /* Align partition upwards */
    s += record_space - (s % record_space);

    /* Partition begin and end */
    p->begin = pos;
    pos += s;
    p->end = pos;

    /* Reserve space for the EBR if necessary */
    if (count > 4 && i > 2) {
      p->begin += record_space;
    }
  }

  /* Expand the last partition to the disk end */
  pt [count - 1].end = disk_end;

  return RTEMS_SUCCESSFUL;
}

#define RTEMS_BDPART_NUMBER_SIZE 4

rtems_status_code rtems_bdpart_register(
  const char *disk_name,
  const rtems_bdpart_partition *pt,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;
  rtems_blkdev_bnum disk_end = 0;
  dev_t disk = 0;
  dev_t logical_disk = 0;
  char *logical_disk_name = NULL;
  char *logical_disk_marker = NULL;
  size_t disk_name_size = strlen( disk_name);
  size_t i = 0;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &disk, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Get the disk device identifier */
  rtems_filesystem_split_dev_t( disk, major, minor);

  /* Create logical disk name */
  logical_disk_name = malloc( disk_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (logical_disk_name == NULL) {
    return RTEMS_NO_MEMORY;
  }
  strncpy( logical_disk_name, disk_name, disk_name_size);
  logical_disk_marker = logical_disk_name + disk_name_size;

  /* Create a logical disk for each partition */
  for (i = 0; i < count; ++i) {
    const rtems_bdpart_partition *p = pt + i;
    int rv = 0;

    /* New minor number */
    ++minor;

    /* Create a new device identifier */
    logical_disk = rtems_filesystem_make_dev_t( major, minor);

    /* Set partition number for logical disk name */
    rv = snprintf( logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
    if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
      esc = RTEMS_INVALID_NAME;
      goto cleanup;
    }

    /* Create logical disk */
    sc = rtems_disk_create_log(
      logical_disk,
      disk,
      p->begin,
      p->end - p->begin,
      logical_disk_name
    );
    if (sc != RTEMS_SUCCESSFUL) {
      esc = sc;
      goto cleanup;
    }
  }

cleanup:

  free( logical_disk_name);

  return esc;
}

rtems_status_code rtems_bdpart_register_from_disk( const char *disk_name)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdpart_format format;
  rtems_bdpart_partition pt [RTEMS_BDPART_PARTITION_NUMBER_HINT];
  size_t count = RTEMS_BDPART_PARTITION_NUMBER_HINT;

  /* Read partitions */
  sc = rtems_bdpart_read( disk_name, &format, pt, &count);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Register partitions */
  return rtems_bdpart_register( disk_name, pt, count);
}

rtems_status_code rtems_bdpart_unregister(
  const char *disk_name,
  const rtems_bdpart_partition *pt __attribute__((unused)),
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;
  rtems_blkdev_bnum disk_end = 0;
  dev_t disk = 0;
  dev_t logical_disk = 0;
  size_t i = 0;

  /* Get disk data */
  sc = rtems_bdpart_get_disk_data( disk_name, &disk, &disk_end);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Get the disk device identifier */
  rtems_filesystem_split_dev_t( disk, major, minor);

  /* Create a logical disk for each partition */
  for (i = 0; i < count; ++i) {
    /* New minor number */
    ++minor;

    /* Get the device identifier */
    logical_disk = rtems_filesystem_make_dev_t( major, minor);

    /* Delete logical disk */
    sc = rtems_disk_delete( logical_disk);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_bdpart_mount(
  const char *disk_name,
  const rtems_bdpart_partition *pt __attribute__((unused)),
  size_t count,
  const char *mount_base
)
{
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  const char *disk_file_name = strrchr( disk_name, '/');
  char *logical_disk_name = NULL;
  char *logical_disk_marker = NULL;
  char *mount_point = NULL;
  char *mount_marker = NULL;
  size_t disk_file_name_size = 0;
  size_t disk_name_size = strlen( disk_name);
  size_t mount_base_size = strlen( mount_base);
  size_t i = 0;

  /* Create logical disk name base */
  logical_disk_name = malloc( disk_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (logical_disk_name == NULL) {
    return RTEMS_NO_MEMORY;
  }
  strncpy( logical_disk_name, disk_name, disk_name_size);

  /* Get disk file name */
  if (disk_file_name != NULL) {
    disk_file_name += 1;
    disk_file_name_size = strlen( disk_file_name);
  } else {
    disk_file_name = disk_name;
    disk_file_name_size = disk_name_size;
  }

  /* Create mount point base */
  mount_point = malloc( mount_base_size + 1 + disk_file_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (mount_point == NULL) {
    esc = RTEMS_NO_MEMORY;
    goto cleanup;
  }
  strncpy( mount_point, mount_base, mount_base_size);
  mount_point [mount_base_size] = '/';
  strncpy( mount_point + mount_base_size + 1, disk_file_name, disk_file_name_size);

  /* Markers */
  logical_disk_marker = logical_disk_name + disk_name_size;
  mount_marker = mount_point + mount_base_size + 1 + disk_file_name_size;

  /* Mount supported file systems for each partition */
  for (i = 0; i < count; ++i) {
    /* Create logical disk name */
    int rv = snprintf( logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
    if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
      esc = RTEMS_INVALID_NAME;
      goto cleanup;
    }

    /* Create mount point */
    strncpy( mount_marker, logical_disk_marker, RTEMS_BDPART_NUMBER_SIZE);
    rv = rtems_fsmount_create_mount_point( mount_point);
    if (rv != 0) {
      esc = RTEMS_IO_ERROR;
      goto cleanup;
    }

    /* Mount */
    rv = mount(
      NULL,
      &msdos_ops,
      0,
      logical_disk_name,
      mount_point
    );
    if (rv != 0) {
      rmdir( mount_point);
    }
  }

cleanup:

  free( logical_disk_name);
  free( mount_point);

  return esc;
}

rtems_status_code rtems_bdpart_unmount(
  const char *disk_name,
  const rtems_bdpart_partition *pt __attribute__((unused)),
  size_t count,
  const char *mount_base
)
{
  rtems_status_code esc = RTEMS_SUCCESSFUL;
  const char *disk_file_name = strrchr( disk_name, '/');
  char *mount_point = NULL;
  char *mount_marker = NULL;
  size_t disk_file_name_size = 0;
  size_t disk_name_size = strlen( disk_name);
  size_t mount_base_size = strlen( mount_base);
  size_t i = 0;

  /* Get disk file name */
  if (disk_file_name != NULL) {
    disk_file_name += 1;
    disk_file_name_size = strlen( disk_file_name);
  } else {
    disk_file_name = disk_name;
    disk_file_name_size = disk_name_size;
  }

  /* Create mount point base */
  mount_point = malloc( mount_base_size + 1 + disk_file_name_size + RTEMS_BDPART_NUMBER_SIZE);
  if (mount_point == NULL) {
    esc = RTEMS_NO_MEMORY;
    goto cleanup;
  }
  strncpy( mount_point, mount_base, mount_base_size);
  mount_point [mount_base_size] = '/';
  strncpy( mount_point + mount_base_size + 1, disk_file_name, disk_file_name_size);

  /* Marker */
  mount_marker = mount_point + mount_base_size + 1 + disk_file_name_size;

  /* Mount supported file systems for each partition */
  for (i = 0; i < count; ++i) {
    /* Create mount point */
    int rv = snprintf( mount_marker, RTEMS_BDPART_NUMBER_SIZE, "%zu", i + 1);
    if (rv >= RTEMS_BDPART_NUMBER_SIZE) {
      esc = RTEMS_INVALID_NAME;
      goto cleanup;
    }

    /* Unmount */
    rv = unmount( mount_point);
    if (rv == 0) {
      /* Remove mount point */
      rv = rmdir( mount_point);
      if (rv != 0) {
        esc = RTEMS_IO_ERROR;
        goto cleanup;
      }
    }
  }

cleanup:

  free( mount_point);

  return esc;
}

void rtems_bdpart_dump( const rtems_bdpart_partition *pt, size_t count)
{
  size_t i = 0;

  printf(
    "-------------------------------------------------------------------------------\n"
    "                                PARTITION TABLE\n"
    "------------+------------+-----------------------------------------------------\n"
    " BEGIN      | END        | TYPE\n"
    "------------+------------+-----------------------------------------------------\n"
  );

  for (i = 0; i < count; ++i) {
    const rtems_bdpart_partition *p = pt + i;
    const char *type = NULL;
    char type_buffer [52];
    uint8_t type_mbr = 0;

    if (rtems_bdpart_to_mbr_partition_type( p->type, &type_mbr)) {
      switch (type_mbr) {
        case RTEMS_BDPART_MBR_FAT_12:
          type = "FAT 12";
          break;
        case RTEMS_BDPART_MBR_FAT_16:
          type = "FAT 16";
          break;
        case RTEMS_BDPART_MBR_FAT_16_LBA:
          type = "FAT 16 LBA";
          break;
        case RTEMS_BDPART_MBR_FAT_32:
          type = "FAT 32";
          break;
        case RTEMS_BDPART_MBR_FAT_32_LBA:
          type = "FAT 32 LBA";
          break;
        case RTEMS_BDPART_MBR_DATA:
          type = "DATA";
          break;
        default:
          snprintf( type_buffer, sizeof( type_buffer), "0x%02" PRIx8, type_mbr);
          type = type_buffer;
          break;
      }
    } else {
      rtems_bdpart_type_to_string( p->type, type_buffer);
      type = type_buffer;
    }

    printf(
      " %10lu | %10lu |%52s\n",
      (unsigned long) p->begin,
      (unsigned long) p->end,
      type
    );
  }

  puts( "------------+------------+-----------------------------------------------------");
}
