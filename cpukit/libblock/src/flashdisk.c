/*
 * flashdisk.c -- Flash disk block device implementation
 *
 * Copyright (C) 2007 Chris Johns
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 * The use of pages can vary. The rtems_fdisk_seg_*_page set
 * routines use an absolute page number relative to the segment
 * while all other page numbers are relative to the number of
 * page descriptor pages a segment has. You need to add the
 * number of page descriptor pages (pages_desc) to the page number
 * when call the rtems_fdisk_seg_*_page functions.
 *
 * You must always show the page number as relative in any trace
 * or error message as device-segment-page and if you have to
 * the page number as absolute use device-segment~page. This
 * can be seen in the page copy routine.
 *
 * The code is like this to avoid needing the pass the pages_desc
 * value around. It is only used in selected places and so the
 * extra parameter was avoided.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "rtems/blkdev.h"
#include "rtems/diskdevs.h"
#include "rtems/flashdisk.h"

/**
 * Control tracing. It can be compiled out of the code for small
 * footprint targets. Leave in by default.
 */
#if !defined (RTEMS_FDISK_TRACE)
#define RTEMS_FDISK_TRACE 1
#endif

/**
 * The start of a segment has a segment control table. This hold the CRC and
 * block number for the page.
 *
 * @todo A sequence number for the block could be added. This would
 *       mean a larger descriptor size. Need to make the sequence
 *       large like 20+ bits so a large file system would not have
 *       more blocks available than the sequence number.
 */
typedef struct rtems_fdisk_page_desc
{
  uint16_t crc;       /**< The page's checksum. */
  uint16_t flags;     /**< The flags for the page. */
  uint32_t block;     /**< The block number. */
} rtems_fdisk_page_desc;

/**
 * Flag the page as active.
 */
#define RTEMS_FDISK_PAGE_ACTIVE (1 << 0)

/**
 * Flag the page as used.
 */
#define RTEMS_FDISK_PAGE_USED (1 << 1)

/**
 * Flash Segment Control holds the pointer to the segment, number of
 * pages, various page stats and the memory copy of the page descriptors.
 */
typedef struct rtems_fdisk_segment_ctl
{
  /**
   * Segments with available pages are maintained as a linked list.
   */
  struct rtems_fdisk_segment_ctl* next;

  /**
   * The descriptor provided by the low-level driver.
   */
  const rtems_fdisk_segment_desc* descriptor;

  /**
   * The device this segment resides on.
   */
  uint32_t device;

  /**
   * The segment in the device. This must be within the
   * segment descriptor.
   */
  uint32_t segment;

  /**
   * The in-memory ocpy of the page descriptors found at
   * the start of the segment in the flash device.
   */
  rtems_fdisk_page_desc* page_descriptors;

  /*
   * Page stats.
   *
   * A bad page does not checksum or is not erased or has invalid flags.
   */
  uint32_t pages;         /**< Total number of pages in the segment. */
  uint32_t pages_desc;    /**< Number of pages used for page descriptors. */
  uint32_t pages_active;  /**< Number of pages flagged as active. */
  uint32_t pages_used;    /**< Number of pages flagged as used. */
  uint32_t pages_bad;     /**< Number of pages detected as bad. */

  uint32_t failed;        /**< The segment has failed. */

  uint32_t erased;        /**< Counter to debugging. Wear support would
                               remove this. */
} rtems_fdisk_segment_ctl;

/**
 * Segment control table queue.
 */
typedef struct rtems_fdisk_segment_ctl_queue
{
  rtems_fdisk_segment_ctl* head;
  rtems_fdisk_segment_ctl* tail;
  uint32_t                 count;
} rtems_fdisk_segment_ctl_queue;

/**
 * Flash Device Control holds the segment controls
 */
typedef struct rtems_fdisk_device_ctl
{
  rtems_fdisk_segment_ctl*       segments;      /**< Segment controls. */
  uint32_t                       segment_count; /**< Segment control count. */
  const rtems_fdisk_device_desc* descriptor;    /**< Device descriptor. */
} rtems_fdisk_device_ctl;

/**
 * The Block control holds the segment and page with the data.
 */
typedef struct rtems_fdisk_block_ctl
{
  rtems_fdisk_segment_ctl* segment; /**< The segment with the block. */
  uint32_t                 page;    /**< The page in the segment. */
} rtems_fdisk_block_ctl;

/**
 * The virtual block table holds the mapping for blocks as seen by the disk
 * drivers to the device, segment and page numbers of the physical device.
 */
typedef struct rtems_flashdisk
{
  rtems_device_major_number major;         /**< The driver's major number. */
  rtems_device_minor_number minor;         /**< The driver's minor number. */

  uint32_t flags;                          /**< configuration flags. */

  uint32_t compact_segs;                   /**< Max segs to compact at once. */
  uint32_t avail_compact_segs;             /**< The number of segments when
                                                compaction occurs when writing. */

  uint32_t               block_size;       /**< The block size for this disk. */
  rtems_fdisk_block_ctl* blocks;           /**< The block to segment-page
                                                mappings. */
  uint32_t block_count;                    /**< The number of avail. blocks. */
  uint32_t unavail_blocks;                 /**< The number of unavail blocks. */
  uint32_t starvation_threshold;           /**< Erased blocks starvation threshold. */
  uint32_t erased_blocks;                  /**< The number of erased blocks. */

  rtems_fdisk_device_ctl* devices;         /**< The flash devices for this
                                                disk. */
  uint32_t                device_count;    /**< The number of flash devices. */

  rtems_fdisk_segment_ctl_queue available; /**< The queue of segments with
                                                available pages. */
  rtems_fdisk_segment_ctl_queue used;      /**< The list of segments with all
                                                pages used. */
  rtems_fdisk_segment_ctl_queue erase;     /**< The list of segments to be
                                                erased. */
  rtems_fdisk_segment_ctl_queue failed;    /**< The list of segments that failed
                                                when being erased. */
  rtems_id lock;                           /**< Mutex for threading protection.*/

  uint8_t* copy_buffer;                    /**< Copy buf used during compacting */

  uint32_t info_level;                     /**< The info trace level. */

  uint32_t starvations;                    /**< Erased blocks starvations counter. */
} rtems_flashdisk;

/**
 * The array of flash disks we support.
 */
static rtems_flashdisk* rtems_flashdisks;

/**
 * The number of flash disks we have.
 */
static uint32_t rtems_flashdisk_count;

/**
 * The CRC16 factor table. Created during initialisation.
 */
static uint16_t* rtems_fdisk_crc16_factor;

/**
 * Calculate the CRC16 checksum.
 *
 * @param _b The byte to checksum.
 * @param _c The current checksum.
 */
#define rtems_fdisk_calc_crc16(_b, _c) \
  rtems_fdisk_crc16_factor[((_b) ^ ((_c) & 0xff)) & 0xff] ^ (((_c) >> 8) & 0xff)

/**
 * Generate the CRC table.
 *
 * @param pattern The seed pattern for the table of factors.
 * @relval RTEMS_SUCCESSFUL The table was generated.
 * @retval RTEMS_NO_MEMORY The table could not be allocated from the heap.
 */
static rtems_status_code
rtems_fdisk_crc16_gen_factors (uint16_t pattern)
{
  uint32_t b;

  rtems_fdisk_crc16_factor = malloc (sizeof (uint16_t) * 256);
  if (!rtems_fdisk_crc16_factor)
    return RTEMS_NO_MEMORY;

  for (b = 0; b < 256; b++)
  {
    uint32_t i;
    uint16_t v = b;
    for (i = 8; i--;)
      v = v & 1 ? (v >> 1) ^ pattern : v >> 1;
    rtems_fdisk_crc16_factor[b] = v & 0xffff;
  }
  return RTEMS_SUCCESSFUL;
}

#if RTEMS_FDISK_TRACE
/**
 * Print a message to the flash disk output and flush it.
 *
 * @param fd The flashdisk control structure.
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_fdisk_printf (const rtems_flashdisk* fd, const char *format, ...)
{
  int ret = 0;
  if (fd->info_level >= 3)
  {
    va_list args;
    va_start (args, format);
    fprintf (stdout, "fdisk:");
    ret =  vfprintf (stdout, format, args);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (args);
  }
  return ret;
}

static bool
rtems_fdisk_is_erased_blocks_starvation (rtems_flashdisk* fd)
{
  bool starvation = fd->erased_blocks < fd->starvation_threshold;

  if (starvation)
    fd->starvations++;

  return starvation;
}

/**
 * Print a info message to the flash disk output and flush it.
 *
 * @param fd The flashdisk control structure.
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_fdisk_info (const rtems_flashdisk* fd, const char *format, ...)
{
  int ret = 0;
  if (fd->info_level >= 2)
  {
    va_list args;
    va_start (args, format);
    fprintf (stdout, "fdisk:");
    ret =  vfprintf (stdout, format, args);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (args);
  }
  return ret;
}

/**
 * Print a warning to the flash disk output and flush it.
 *
 * @param fd The flashdisk control structure.
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_fdisk_warning (const rtems_flashdisk* fd, const char *format, ...)
{
  int ret = 0;
  if (fd->info_level >= 1)
  {
    va_list args;
    va_start (args, format);
    fprintf (stdout, "fdisk:warning:");
    ret =  vfprintf (stdout, format, args);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (args);
  }
  return ret;
}
#endif

/**
 * Print an error to the flash disk output and flush it.
 *
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_fdisk_error (const char *format, ...)
{
  int ret;
  va_list args;
  va_start (args, format);
  fprintf (stderr, "fdisk:error:");
  ret =  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");
  fflush (stderr);
  va_end (args);
  return ret;
}

/**
 * Print an abort message, flush it then abort the program.
 *
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 */
static void
rtems_fdisk_abort (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  fprintf (stderr, "fdisk:abort:");
  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");
  fflush (stderr);
  va_end (args);
  exit (1);
}

/**
 * Initialise the segment control queue.
 */
static void
rtems_fdisk_segment_queue_init (rtems_fdisk_segment_ctl_queue* queue)
{
  queue->head = queue->tail = 0;
  queue->count = 0;
}

/**
 * Push to the head of the segment control queue.
 */
static void
rtems_fdisk_segment_queue_push_head (rtems_fdisk_segment_ctl_queue* queue,
                                     rtems_fdisk_segment_ctl*       sc)
{
  if (sc)
  {
    sc->next = queue->head;
    queue->head = sc;

    if (queue->tail == 0)
      queue->tail = sc;
    queue->count++;
  }
}

/**
 * Pop the head of the segment control queue.
 */
static rtems_fdisk_segment_ctl*
rtems_fdisk_segment_queue_pop_head (rtems_fdisk_segment_ctl_queue* queue)
{
  if (queue->head)
  {
    rtems_fdisk_segment_ctl* sc = queue->head;

    queue->head = sc->next;
    if (!queue->head)
      queue->tail = 0;

    queue->count--;

    sc->next = 0;

    return sc;
  }

  return 0;
}

/**
 * Push to the tail of the segment control queue.
 */
static void
rtems_fdisk_segment_queue_push_tail (rtems_fdisk_segment_ctl_queue* queue,
                                     rtems_fdisk_segment_ctl*       sc)
{
  if (sc)
  {
    sc->next = 0;

    if (queue->head)
    {
      queue->tail->next = sc;
      queue->tail       = sc;
    }
    else
    {
      queue->head = queue->tail = sc;
    }

    queue->count++;
  }
}

/**
 * Remove from the segment control queue.
 */
static void
rtems_fdisk_segment_queue_remove (rtems_fdisk_segment_ctl_queue* queue,
                                  rtems_fdisk_segment_ctl*       sc)
{
  rtems_fdisk_segment_ctl* prev = 0;
  rtems_fdisk_segment_ctl* it = queue->head;

  /*
   * Do not change sc->next as sc could be on another queue.
   */

  while (it)
  {
    if (sc == it)
    {
      if (prev == 0)
      {
        queue->head = sc->next;
        if (queue->head == 0)
          queue->tail = 0;
      }
      else
      {
        prev->next = sc->next;
        if (queue->tail == sc)
          queue->tail = prev;
      }
      sc->next = 0;
      queue->count--;
      break;
    }

    prev = it;
    it = it->next;
  }
}

/**
 * Insert into the segment control queue before the specific
 * segment control item.
 */
static void
rtems_fdisk_segment_queue_insert_before (rtems_fdisk_segment_ctl_queue* queue,
                                         rtems_fdisk_segment_ctl*       item,
                                         rtems_fdisk_segment_ctl*       sc)
{
  if (item)
  {
    rtems_fdisk_segment_ctl** prev = &queue->head;
    rtems_fdisk_segment_ctl*  it = queue->head;

    while (it)
    {
      if (item == it)
      {
        sc->next = item;
        *prev = sc;
        queue->count++;
        return;
      }

      prev = &it->next;
      it = it->next;
    }
  }

  rtems_fdisk_segment_queue_push_tail (queue, sc);
}

/**
 * Count the number of elements on the list.
 */
static uint32_t
rtems_fdisk_segment_queue_count (rtems_fdisk_segment_ctl_queue* queue)
{
  return queue->count;
}

/**
 * Count the number of elements on the list.
 */
static uint32_t
rtems_fdisk_segment_count_queue (rtems_fdisk_segment_ctl_queue* queue)
{
  rtems_fdisk_segment_ctl* sc = queue->head;
  uint32_t                 count = 0;

  while (sc)
  {
    count++;
    sc = sc->next;
  }

  return count;
}

/**
 * See if a segment control is present on this queue.
 */
static bool
rtems_fdisk_segment_queue_present (rtems_fdisk_segment_ctl_queue* queue,
                                   rtems_fdisk_segment_ctl*       sc)
{
  rtems_fdisk_segment_ctl*  it = queue->head;

  while (it)
  {
    if (it == sc)
      return true;
    it = it->next;
  }

  return false;
}

/**
 * Format a string with the queue status.
 */
static void
rtems_fdisk_queue_status (rtems_flashdisk*         fd,
                          rtems_fdisk_segment_ctl* sc,
                          char                     queues[5])
{
  queues[0] = rtems_fdisk_segment_queue_present (&fd->available, sc) ? 'A' : '-';
  queues[1] = rtems_fdisk_segment_queue_present (&fd->used, sc)      ? 'U' : '-';
  queues[2] = rtems_fdisk_segment_queue_present (&fd->erase, sc)     ? 'E' : '-';
  queues[3] = rtems_fdisk_segment_queue_present (&fd->failed, sc)    ? 'F' : '-';
  queues[4] = '\0';
}

/**
 * Check if the page descriptor is erased.
 */
static bool
rtems_fdisk_page_desc_erased (const rtems_fdisk_page_desc* pd)
{
  return ((pd->crc == 0xffff) &&
          (pd->flags == 0xffff) &&
          (pd->block == 0xffffffff)) ? true : false;
}

/**
 * Check if the flags are set. The flags are inverted as we can
 * only set a flag by changing it from 1 to 0.
 */
static bool
rtems_fdisk_page_desc_flags_set (rtems_fdisk_page_desc* pd, uint16_t flags)
{
  return (pd->flags & flags) == 0 ? true : false;
}

/**
 * Check if the flags are clear. The flags are inverted as we can
 * only set a flag by changing it from 1 to 0.
 */
static bool
rtems_fdisk_page_desc_flags_clear (rtems_fdisk_page_desc* pd, uint16_t flags)
{
  return (pd->flags & flags) == flags ? true : false;
}

/**
 * Set the flags. Setting means clear the bit to 0.
 */
static void
rtems_fdisk_page_desc_set_flags (rtems_fdisk_page_desc* pd, uint16_t flags)
{
  pd->flags &= ~flags;
}

/**
 * Get the segment descriptor for a device and segment. There are
 * no range checks.
 */
static const rtems_fdisk_segment_desc*
rtems_fdisk_seg_descriptor (const rtems_flashdisk* fd,
                            uint32_t               device,
                            uint32_t               segment)
{
  return fd->devices[device].segments[segment].descriptor;
}

/**
 * Count the segments for a device.
 */
static uint32_t
rtems_fdisk_count_segments (const rtems_fdisk_device_desc* dd)
{
  uint32_t count = 0;
  uint32_t segment;
  for (segment = 0; segment < dd->segment_count; segment++)
    count += dd->segments[segment].count;
  return count;
}

/**
 * Calculate the pages in a segment give the segment size and the
 * page size.
 *
 * @param sd The segment descriptor.
 * @param page_size The page size in bytes.
 */
static uint32_t
rtems_fdisk_pages_in_segment (const rtems_fdisk_segment_desc* sd,
                              uint32_t                        page_size)
{
  return sd->size / page_size;
}

/**
 * Calculate the number of pages needed to hold the page descriptors.
 * The calculation need to round up.
 *
 * The segment control contains the number of pages used as descriptors
 * and should be used rather than this call where possible.
 */
static uint32_t
rtems_fdisk_page_desc_pages (const rtems_fdisk_segment_desc* sd,
                             uint32_t                        page_size)
{
  uint32_t pages = rtems_fdisk_pages_in_segment (sd, page_size);
  uint32_t bytes = pages * sizeof (rtems_fdisk_page_desc);
  return ((bytes - 1) / page_size) + 1;
}

/**
 * The number of available pages is the total pages less the
 * active, used and bad pages.
 */
static uint32_t
rtems_fdisk_seg_pages_available (const rtems_fdisk_segment_ctl* sc)
{
  return sc->pages - (sc->pages_active + sc->pages_used + sc->pages_bad);
}
/**
 * Find the next available page in a segment.
 */
static uint32_t
rtems_fdisk_seg_next_available_page (rtems_fdisk_segment_ctl* sc)
{
  rtems_fdisk_page_desc* pd = &sc->page_descriptors[0];
  uint32_t               page;

  for (page = 0; page < sc->pages; page++, pd++)
    if (rtems_fdisk_page_desc_erased (pd))
      break;

  return page;
}

/**
 * Find the segment on the queue that has the most free pages.
 */
static rtems_fdisk_segment_ctl*
rtems_fdisk_seg_most_available (const rtems_fdisk_segment_ctl_queue* queue)
{
  rtems_fdisk_segment_ctl* sc      = queue->head;
  rtems_fdisk_segment_ctl* biggest = queue->head;

  while (sc)
  {
    if (rtems_fdisk_seg_pages_available (sc) >
        rtems_fdisk_seg_pages_available (biggest))
      biggest = sc;
    sc = sc->next;
  }

  return biggest;
}

/**
 * Is the segment all used ?
 */
#if 0
static bool
rtems_fdisk_seg_pages_all_used (const rtems_fdisk_segment_ctl* sc)
{
  return sc->pages == (sc->pages_used + sc->pages_bad) ? true : false;
}
#endif

/**
 * Calculate the blocks in a device. This is the number of
 * pages less the pages hold page descriptors. This call be used
 * early in the initialisation process and does not rely on
 * the system being fully initialised.
 *
 * @param dd The device descriptor.
 * @param page_size The page size in bytes.
 */
static uint32_t
rtems_fdisk_blocks_in_device (const rtems_fdisk_device_desc* dd,
                              uint32_t                       page_size)
{
  uint32_t count = 0;
  uint32_t s;
  for (s = 0; s < dd->segment_count; s++)
  {
    const rtems_fdisk_segment_desc* sd = &dd->segments[s];
    count +=
      (rtems_fdisk_pages_in_segment (sd, page_size) -
       rtems_fdisk_page_desc_pages (sd, page_size)) * sd->count;
  }
  return count;
}

/**
 * Read a block of data from a segment.
 */
static int
rtems_fdisk_seg_read (const rtems_flashdisk*         fd,
                      const rtems_fdisk_segment_ctl* sc,
                      uint32_t                       offset,
                      void*                          buffer,
                      uint32_t                       size)
{
  uint32_t                           device;
  uint32_t                           segment;
  const rtems_fdisk_segment_desc*    sd;
  const rtems_fdisk_driver_handlers* ops;
  device = sc->device;
  segment = sc->segment;
  sd = rtems_fdisk_seg_descriptor (fd, device, segment);
  ops = fd->devices[device].descriptor->flash_ops;
#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, "  seg-read: %02d-%03d: o=%08x s=%d",
                      device, segment, offset, size);
#endif
  return ops->read (sd, device, segment, offset, buffer, size);
}

/**
 * Write a block of data to a segment. It is assumed the
 * location in the segment is erased and able to take the
 * data.
 */
static int
rtems_fdisk_seg_write (const rtems_flashdisk*   fd,
                       rtems_fdisk_segment_ctl* sc,
                       uint32_t                 offset,
                       const void*              buffer,
                       uint32_t                 size)
{
  int ret;
  uint32_t                           device;
  uint32_t                           segment;
  const rtems_fdisk_segment_desc*    sd;
  const rtems_fdisk_driver_handlers* ops;
  device = sc->device;
  segment = sc->segment;
  sd = rtems_fdisk_seg_descriptor (fd, device, segment);
  ops = fd->devices[device].descriptor->flash_ops;
#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, "  seg-write: %02d-%03d: o=%08x s=%d",
                      device, segment, offset, size);
#endif
  ret = ops->write (sd, device, segment, offset, buffer, size);
  if (ret)
    sc->failed = true;

  return ret;
}

/**
 * Blank check the area of a segment.
 */
static int
rtems_fdisk_seg_blank_check (const rtems_flashdisk*   fd,
                             rtems_fdisk_segment_ctl* sc,
                             uint32_t                 offset,
                             uint32_t                 size)
{
  uint32_t                           device;
  uint32_t                           segment;
  const rtems_fdisk_segment_desc*    sd;
  const rtems_fdisk_driver_handlers* ops;
  device = sc->device;
  segment = sc->segment;
  sd = rtems_fdisk_seg_descriptor (fd, device, segment);
  ops = fd->devices[device].descriptor->flash_ops;
#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, "  seg-blank: %02d-%03d: o=%08x s=%d",
                      device, segment, offset, size);
#endif
  return ops->blank (sd, device, segment, offset, size);
}
/**
 * Verify the data with the data in a segment.
 */
static int
rtems_fdisk_seg_verify (const rtems_flashdisk* fd,
                        uint32_t               device,
                        uint32_t               segment,
                        uint32_t               offset,
                        const void*            buffer,
                        uint32_t               size)
{
  const rtems_fdisk_segment_desc*    sd;
  const rtems_fdisk_driver_handlers* ops;
  sd  = rtems_fdisk_seg_descriptor (fd, device, segment);
  ops = fd->devices[device].descriptor->flash_ops;
#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, "  seg-verify: %02d-%03d: o=%08x s=%d",
                      device, segment, offset, size);
#endif
  return ops->verify (sd, device, segment, offset, buffer, size);
}

/**
 * Blank check a page of data in a segment.
 */
static int
rtems_fdisk_seg_blank_check_page (const rtems_flashdisk*   fd,
                                  rtems_fdisk_segment_ctl* sc,
                                  uint32_t                 page)
{
  return rtems_fdisk_seg_blank_check (fd, sc,
                                      page * fd->block_size, fd->block_size);
}

/**
 * Read a page of data from a segment.
 */
static int
rtems_fdisk_seg_read_page (const rtems_flashdisk*   fd,
                           rtems_fdisk_segment_ctl* sc,
                           uint32_t                 page,
                           void*                    buffer)
{
  return rtems_fdisk_seg_read (fd, sc,
                               page * fd->block_size, buffer, fd->block_size);
}

/**
 * Write a page of data to a segment.
 */
static int
rtems_fdisk_seg_write_page (rtems_flashdisk*         fd,
                            rtems_fdisk_segment_ctl* sc,
                            uint32_t                 page,
                            const void*              buffer)
{
  if ((fd->flags & RTEMS_FDISK_BLANK_CHECK_BEFORE_WRITE))
  {
    int ret = rtems_fdisk_seg_blank_check_page (fd, sc, page);
    if (ret)
      return ret;
  }
  --fd->erased_blocks;
  return rtems_fdisk_seg_write (fd, sc,
                                page * fd->block_size, buffer, fd->block_size);
}

/**
 * Verify a page of data with the data in the segment.
 */
static int
rtems_fdisk_seg_verify_page (const rtems_flashdisk* fd,
                             uint32_t               device,
                             uint32_t               segment,
                             uint32_t               page,
                             const void*            buffer)
{
  return rtems_fdisk_seg_verify (fd, device, segment,
                                 page * fd->block_size, buffer, fd->block_size);
}

/**
 * Copy a page of data from one segment to another segment.
 */
static int
rtems_fdisk_seg_copy_page (rtems_flashdisk*         fd,
                           rtems_fdisk_segment_ctl* src_sc,
                           uint32_t                 src_page,
                           rtems_fdisk_segment_ctl* dst_sc,
                           uint32_t                 dst_page)
{
  int ret;
#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, "  seg-copy-page: %02d-%03d~%03d=>%02d-%03d~%03d",
                      src_sc->device, src_sc->segment, src_page,
                      dst_sc->device, dst_sc->segment, dst_page);
#endif
  ret = rtems_fdisk_seg_read_page (fd, src_sc, src_page,
                                   fd->copy_buffer);
  if (ret)
    return ret;
  return rtems_fdisk_seg_write_page (fd, dst_sc, dst_page,
                                     fd->copy_buffer);
}

/**
 * Write the page descriptor to a segment. This code assumes the page
 * descriptors are located at offset 0 in the segment.
 */
static int
rtems_fdisk_seg_write_page_desc (const rtems_flashdisk*       fd,
                                 rtems_fdisk_segment_ctl*     sc,
                                 uint32_t                     page,
                                 const rtems_fdisk_page_desc* page_desc)
{
  uint32_t offset = page * sizeof (rtems_fdisk_page_desc);
  if ((fd->flags & RTEMS_FDISK_BLANK_CHECK_BEFORE_WRITE))
  {
    int ret = rtems_fdisk_seg_blank_check (fd, sc,
                                           offset,
                                           sizeof (rtems_fdisk_page_desc));
    if (ret)
      return ret;
  }
  return rtems_fdisk_seg_write (fd, sc, offset,
                                page_desc, sizeof (rtems_fdisk_page_desc));
}

/**
 * Write the page descriptor flags to a segment. This code assumes the page
 * descriptors are located at offset 0 in the segment.
 */
static int
rtems_fdisk_seg_write_page_desc_flags (const rtems_flashdisk*       fd,
                                       rtems_fdisk_segment_ctl*     sc,
                                       uint32_t                     page,
                                       const rtems_fdisk_page_desc* page_desc)
{
  uint32_t offset = ((page * sizeof (rtems_fdisk_page_desc)) +
                     ((uint8_t*) &page_desc->flags) - ((uint8_t*) page_desc));
  if ((fd->flags & RTEMS_FDISK_BLANK_CHECK_BEFORE_WRITE))
  {
    uint16_t flash_flags;
    int      ret;
    ret = rtems_fdisk_seg_read (fd, sc, offset,
                                &flash_flags, sizeof (flash_flags));
    if (ret)
      return ret;
    if ((flash_flags & page_desc->flags) != page_desc->flags)
    {
      rtems_fdisk_error ("  seg-write-page-flags: %02d-%03d-%03d: "
                         "flags not erased: 0x%04 -> 0x%04x",
                         sc->device, sc->segment, page,
                         flash_flags, page_desc->flags);
      return ret;
    }
  }
  return rtems_fdisk_seg_write (fd, sc, offset,
                                &page_desc->flags, sizeof (page_desc->flags));
}

/**
 * Erase a device.
 */
static int
rtems_fdisk_device_erase (const rtems_flashdisk* fd, uint32_t device)
{
  const rtems_fdisk_driver_handlers* ops;
  ops = fd->devices[device].descriptor->flash_ops;
#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, " device-erase: %02d", device);
#endif
  return ops->erase_device (fd->devices[device].descriptor, device);
}

/**
 * Erase all flash.
 */
static int
rtems_fdisk_erase_flash (const rtems_flashdisk* fd)
{
  uint32_t device;
  for (device = 0; device < fd->device_count; device++)
  {
    int ret;

#if RTEMS_FDISK_TRACE
    rtems_fdisk_info (fd, " erase-flash:%02d", device);
#endif

    ret = rtems_fdisk_device_erase (fd, device);

    if (ret != 0)
      return ret;
  }
  return 0;
}

/**
 * Calculate the checksum of a page in a segment.
 */
static uint16_t
rtems_fdisk_page_checksum (const uint8_t* buffer, uint32_t page_size)
{
  uint16_t cs = 0xffff;
  uint32_t i;

  for (i = 0; i < page_size; i++, buffer++)
    cs = rtems_fdisk_calc_crc16 (cs, *buffer);

  return cs;
}

/**
 * Erase the segment.
 */
static int
rtems_fdisk_erase_segment (rtems_flashdisk* fd, rtems_fdisk_segment_ctl* sc)
{
  int                                ret;
  uint32_t                           device;
  uint32_t                           segment;
  const rtems_fdisk_segment_desc*    sd;
  const rtems_fdisk_driver_handlers* ops;
  device = sc->device;
  segment = sc->segment;
  sd = rtems_fdisk_seg_descriptor (fd, device, segment);
  ops = fd->devices[device].descriptor->flash_ops;
  ret = ops->erase (sd, device, segment);
  if (ret)
  {
    rtems_fdisk_error (" erase-segment:%02d-%03d: "      \
                       "segment erase failed: %s (%d)",
                       sc->device, sc->segment, strerror (ret), ret);
    sc->failed = true;
    if (!rtems_fdisk_segment_queue_present (&fd->failed, sc))
      rtems_fdisk_segment_queue_push_tail (&fd->failed, sc);
    return ret;
  }

  fd->erased_blocks += sc->pages;
  sc->erased++;

  memset (sc->page_descriptors, 0xff, sc->pages_desc * fd->block_size);

  sc->pages_active = 0;
  sc->pages_used   = 0;
  sc->pages_bad    = 0;

  sc->failed = false;

  /*
   * Push to the tail of the available queue. It is a very
   * simple type of wear reduction. Every other available
   * segment will now get a go.
   */
  rtems_fdisk_segment_queue_push_tail (&fd->available, sc);

  return 0;
}

/**
 * Erase used segment.
 */
static int
rtems_fdisk_erase_used (rtems_flashdisk* fd)
{
  rtems_fdisk_segment_ctl* sc;
  int                      latched_ret = 0;

  while ((sc = rtems_fdisk_segment_queue_pop_head (&fd->erase)))
  {
    /*
     * The segment will either end up on the available queue or
     * the failed queue.
     */
    int ret = rtems_fdisk_erase_segment (fd, sc);
    if (ret && !latched_ret)
      latched_ret = ret;
  }

  return latched_ret;
}

/**
 * Queue a segment. This is done after some of the stats for the segment
 * have been changed and this may effect the order the segment pages have in
 * the queue of available pages.
 *
 * @param fd The flash disk control table.
 * @param sc The segment control table to be reallocated
 */
static void
rtems_fdisk_queue_segment (rtems_flashdisk* fd, rtems_fdisk_segment_ctl* sc)
{
#if RTEMS_FDISK_TRACE
  rtems_fdisk_info (fd, " queue-seg:%02d-%03d: p=%d a=%d u=%d b=%d f=%s n=%s",
                    sc->device, sc->segment,
                    sc->pages, sc->pages_active, sc->pages_used, sc->pages_bad,
                    sc->failed ? "FAILED" : "no", sc->next ? "set" : "null");
#endif

  /*
   * If the segment has failed then check the failed queue and append
   * if not failed.
   */
  if (sc->failed)
  {
    if (!rtems_fdisk_segment_queue_present (&fd->failed, sc))
      rtems_fdisk_segment_queue_push_tail (&fd->failed, sc);
    return;
  }

  /*
   * Remove the queue from the available or used queue.
   */
  rtems_fdisk_segment_queue_remove (&fd->available, sc);
  rtems_fdisk_segment_queue_remove (&fd->used, sc);

  /*
   * Are all the pages in the segment used ?
   * If they are and the driver has been configured to background
   * erase place the segment on the used queue. If not configured
   * to background erase perform the erase now.
   *
   */
  if (rtems_fdisk_seg_pages_available (sc) == 0)
  {
    if (sc->pages_active)
    {
      /*
       * Keep the used queue sorted by the most number of used
       * pages. When we compact we want to move the pages into
       * a new segment and cover more than one segment.
       */
      rtems_fdisk_segment_ctl* seg = fd->used.head;

      while (seg)
      {
        if (sc->pages_used > seg->pages_used)
          break;
        seg = seg->next;
      }

      if (seg)
        rtems_fdisk_segment_queue_insert_before (&fd->used, seg, sc);
      else
        rtems_fdisk_segment_queue_push_tail (&fd->used, sc);
    }
    else
    {
      if ((fd->flags & RTEMS_FDISK_BACKGROUND_ERASE))
        rtems_fdisk_segment_queue_push_tail (&fd->erase, sc);
      else
        rtems_fdisk_erase_segment (fd, sc);
    }
  }
  else
  {
    /*
     * The segment has pages available so place back onto the
     * available list. The list is sorted from the least number
     * of available pages to the most. This approach means
     * the pages of a partially filled segment will be filled
     * before moving onto another emptier segment. This keeps
     * empty segments longer aiding compaction.
     *
     * The down side is the wear effect as a single segment
     * could be used more than segment. This will not be
     * addressed until wear support is added.
     *
     * @note Wear support can be added by having counts for
     * for the number of times a segment is erased. This
     * available list is then sorted on the least number
     * of available pages then empty segments are sorted
     * on the least number of erases the segment has.
     *
     * The erase count can be stored in specially flaged
     * pages and contain a counter (32bits?) and 32 bits
     * for each segment. When a segment is erased a
     * bit is cleared for that segment. When 32 erasers
     * has occurred the page is re-written to the flash
     * with all the counters updated with the number of
     * bits cleared and all bits set back to 1.
     */
    rtems_fdisk_segment_ctl* seg = fd->available.head;

    while (seg)
    {
      if (rtems_fdisk_seg_pages_available (sc) <
          rtems_fdisk_seg_pages_available (seg))
        break;
      seg = seg->next;
    }

    if (seg)
      rtems_fdisk_segment_queue_insert_before (&fd->available, seg, sc);
    else
      rtems_fdisk_segment_queue_push_tail (&fd->available, sc);
  }
}

static int
rtems_fdisk_recycle_segment (rtems_flashdisk*         fd,
                                    rtems_fdisk_segment_ctl* ssc,
                                    rtems_fdisk_segment_ctl* dsc,
                                    uint32_t *pages)
{
  int      ret;
  uint32_t spage;
  uint32_t used = 0;
  uint32_t active = 0;

  for (spage = 0; spage < ssc->pages; spage++)
  {
    rtems_fdisk_page_desc* spd = &ssc->page_descriptors[spage];

    if (!dsc && ssc->pages_active > 0)
    {
      rtems_fdisk_error ("recycle: no available dst segment");
      return EIO;
    }

    if (rtems_fdisk_page_desc_flags_set (spd, RTEMS_FDISK_PAGE_ACTIVE) &&
        !rtems_fdisk_page_desc_flags_set (spd, RTEMS_FDISK_PAGE_USED))
    {
      uint32_t               dst_pages;
      rtems_fdisk_page_desc* dpd;
      uint32_t               dpage;

      dpage = rtems_fdisk_seg_next_available_page (dsc);
      dpd   = &dsc->page_descriptors[dpage];

      active++;

      if (dpage >= dsc->pages)
      {
        rtems_fdisk_error ("recycle: %02d-%03d: " \
                           "no page desc available: %d",
                           dsc->device, dsc->segment,
                           rtems_fdisk_seg_pages_available (dsc));
        dsc->failed = true;
        rtems_fdisk_queue_segment (fd, dsc);
        rtems_fdisk_segment_queue_push_head (&fd->used, ssc);
        return EIO;
      }

#if RTEMS_FDISK_TRACE
      rtems_fdisk_info (fd, "recycle: %02d-%03d-%03d=>%02d-%03d-%03d",
                        ssc->device, ssc->segment, spage,
                        dsc->device, dsc->segment, dpage);
#endif
      ret = rtems_fdisk_seg_copy_page (fd, ssc,
                                       spage + ssc->pages_desc,
                                       dsc,
                                       dpage + dsc->pages_desc);
      if (ret)
      {
        rtems_fdisk_error ("recycle: %02d-%03d-%03d=>" \
                           "%02d-%03d-%03d: "             \
                           "copy page failed: %s (%d)",
                           ssc->device, ssc->segment, spage,
                           dsc->device, dsc->segment, dpage,
                           strerror (ret), ret);
        rtems_fdisk_queue_segment (fd, dsc);
        rtems_fdisk_segment_queue_push_head (&fd->used, ssc);
        return ret;
      }

      *dpd = *spd;

      ret = rtems_fdisk_seg_write_page_desc (fd,
                                             dsc,
                                             dpage, dpd);

      if (ret)
      {
        rtems_fdisk_error ("recycle: %02d-%03d-%03d=>"   \
                           "%02d-%03d-%03d: copy pd failed: %s (%d)",
                           ssc->device, ssc->segment, spage,
                           dsc->device, dsc->segment, dpage,
                           strerror (ret), ret);
        rtems_fdisk_queue_segment (fd, dsc);
        rtems_fdisk_segment_queue_push_head (&fd->used, ssc);
        return ret;
      }

      dsc->pages_active++;

      /*
       * No need to set the used bit on the source page as the
       * segment will be erased. Power down could be a problem.
       * We do the stats to make sure everything is as it should
       * be.
       */

      ssc->pages_active--;
      ssc->pages_used++;

      fd->blocks[spd->block].segment = dsc;
      fd->blocks[spd->block].page    = dpage;

      /*
       * Place the segment on to the correct queue.
       */
      rtems_fdisk_queue_segment (fd, dsc);

      /*
       * Get new destination segment if necessary.
       */
      dst_pages = rtems_fdisk_seg_pages_available (dsc);
      if (dst_pages == 0)
        dsc = rtems_fdisk_seg_most_available (&fd->available);

      (*pages)--;
    }
    else if (rtems_fdisk_page_desc_erased (spd))
    {
      --fd->erased_blocks;
    }
    else
    {
      used++;
    }
  }

#if RTEMS_FDISK_TRACE
  rtems_fdisk_printf (fd, "ssc end: %d-%d: p=%ld, a=%ld, u=%ld",
                      ssc->device, ssc->segment,
                      pages, active, used);
#endif
  if (ssc->pages_active != 0)
  {
    rtems_fdisk_error ("compacting: ssc pages not 0: %d",
                       ssc->pages_active);
  }

  ret = rtems_fdisk_erase_segment (fd, ssc);

  return ret;
}

/**
 * Compact the used segments to free what is available. Find the segment
 * with the most available number of pages and see if we have
 * used segments that will fit. The used queue is sorted on the least
 * number of active pages.
 */
static int
rtems_fdisk_compact (rtems_flashdisk* fd)
{
  int ret;
  rtems_fdisk_segment_ctl* dsc;
  rtems_fdisk_segment_ctl* ssc;
  uint32_t compacted_segs = 0;
  uint32_t pages;

  if (rtems_fdisk_is_erased_blocks_starvation (fd))
  {
#if RTEMS_FDISK_TRACE
    rtems_fdisk_printf (fd, " resolve starvation");
#endif

    ssc = rtems_fdisk_segment_queue_pop_head (&fd->used);
    if (!ssc)
      ssc = rtems_fdisk_segment_queue_pop_head (&fd->available);

    if (ssc)
    {
      dsc = rtems_fdisk_seg_most_available (&fd->available);
      if (dsc)
      {
        ret = rtems_fdisk_recycle_segment (fd, ssc, dsc, &pages);
        if (ret)
          return ret;
      }
      else
      {
        rtems_fdisk_error ("compacting: starvation");
        return EIO;
      }
    }
    else
    {
      rtems_fdisk_error ("compacting: nothing to recycle");
      return EIO;
    }
  }

  while (fd->used.head)
  {
    uint32_t                 dst_pages;
    uint32_t                 segments;

#if RTEMS_FDISK_TRACE
    rtems_fdisk_printf (fd, " compacting");
#endif

    dsc = rtems_fdisk_seg_most_available (&fd->available);

    if (dsc == 0)
    {
      rtems_fdisk_error ("compacting: no available segments to compact too");
      return EIO;
    }

    ssc = fd->used.head;
    dst_pages = rtems_fdisk_seg_pages_available (dsc);
    segments = 0;
    pages = 0;

#if RTEMS_FDISK_TRACE
    rtems_fdisk_printf (fd, " dsc:%02d-%03d: most available",
                        dsc->device, dsc->segment);
#endif

    /*
     * Count the number of segments that have active pages that fit into
     * the destination segment. Also limit the number of segments that
     * we handle during one compaction. A lower number means less aggressive
     * compaction or less delay when compacting but it may mean the disk
     * will fill.
     */

    while (ssc &&
           ((pages + ssc->pages_active) < dst_pages) &&
           ((compacted_segs + segments) < fd->compact_segs))
    {
      pages += ssc->pages_active;
      segments++;
      ssc = ssc->next;
    }

    /*
     * We need a source segment and have pages to copy and
     * compacting one segment to another is silly. Compaction needs
     * to free at least one more segment.
     */

    if (!ssc || (pages == 0) || ((compacted_segs + segments) == 1))
    {
#if RTEMS_FDISK_TRACE
      rtems_fdisk_printf (fd, " nothing to compact");
#endif
      break;
    }

#if RTEMS_FDISK_TRACE
    rtems_fdisk_printf (fd, " ssc scan: %d-%d: p=%ld, seg=%ld",
                        ssc->device, ssc->segment,
                        pages, segments);
#endif

    rtems_fdisk_segment_queue_remove (&fd->available, dsc);

    /*
     * We now copy the pages to the new segment.
     */

    while (pages)
    {
      ssc = rtems_fdisk_segment_queue_pop_head (&fd->used);

      if (ssc)
      {
        ret = rtems_fdisk_recycle_segment (fd, ssc, dsc, &pages);
        if (ret)
          return ret;
      }
    }

    compacted_segs += segments;
  }

  return 0;
}

/**
 * Recover the block mappings from the devices.
 */
static int
rtems_fdisk_recover_block_mappings (rtems_flashdisk* fd)
{
  uint32_t device;

  /*
   * Clear the queues.
   */
  rtems_fdisk_segment_queue_init (&fd->available);
  rtems_fdisk_segment_queue_init (&fd->used);
  rtems_fdisk_segment_queue_init (&fd->erase);
  rtems_fdisk_segment_queue_init (&fd->failed);

  /*
   * Clear the lock mappings.
   */
  memset (fd->blocks, 0, fd->block_count * sizeof (rtems_fdisk_block_ctl));

  /*
   * Scan each segment or each device recovering the valid pages.
   */
  fd->erased_blocks = 0;
  fd->starvation_threshold = 0;
  for (device = 0; device < fd->device_count; device++)
  {
    uint32_t segment;
    for (segment = 0; segment < fd->devices[device].segment_count; segment++)
    {
      rtems_fdisk_segment_ctl*        sc = &fd->devices[device].segments[segment];
      const rtems_fdisk_segment_desc* sd = sc->descriptor;
      rtems_fdisk_page_desc*          pd;
      uint32_t                        page;
      int                             ret;

#if RTEMS_FDISK_TRACE
      rtems_fdisk_info (fd, "recover-block-mappings:%02d-%03d", device, segment);
#endif

      sc->pages_desc = rtems_fdisk_page_desc_pages (sd, fd->block_size);
      sc->pages =
        rtems_fdisk_pages_in_segment (sd, fd->block_size) - sc->pages_desc;
      if (sc->pages > fd->starvation_threshold)
        fd->starvation_threshold = sc->pages;

      sc->pages_active = 0;
      sc->pages_used   = 0;
      sc->pages_bad    = 0;

      sc->failed = false;

      if (!sc->page_descriptors)
        sc->page_descriptors = malloc (sc->pages_desc * fd->block_size);

      if (!sc->page_descriptors)
        rtems_fdisk_abort ("no memory for page descriptors");

      pd = sc->page_descriptors;

      /*
       * The page descriptors are always at the start of the segment. Read
       * the descriptors off the device into the segment control page
       * descriptors.
       *
       * @todo It may be better to ask the driver to get these value
       *       so NAND flash could be better supported.
       */
      ret = rtems_fdisk_seg_read (fd, sc, 0, (void*) pd,
                                  sc->pages_desc * fd->block_size);

      if (ret)
      {
        rtems_fdisk_error ("recover-block-mappings:%02d-%03d: " \
                           "read page desc failed: %s (%d)",
                           device, segment, strerror (ret), ret);
        return ret;
      }

      /*
       * Check each page in the segement for valid pages.
       * Update the stats for the segment so we know how many pages
       * are active and how many are used.
       *
       * If the page is active see if the block is with-in range and
       * if the block is a duplicate.
       */
      for (page = 0; page < sc->pages; page++, pd++)
      {
        if (rtems_fdisk_page_desc_erased (pd))
        {
          /*
           * Is the page erased ?
           */
          ret = rtems_fdisk_seg_blank_check_page (fd, sc,
                                                  page + sc->pages_desc);

          if (ret == 0)
          {
            ++fd->erased_blocks;
          }
          else
          {
#if RTEMS_FDISK_TRACE
            rtems_fdisk_warning (fd, "page not blank: %d-%d-%d",
                                 device, segment, page, pd->block);
#endif
            rtems_fdisk_page_desc_set_flags (pd, RTEMS_FDISK_PAGE_USED);

            ret = rtems_fdisk_seg_write_page_desc (fd, sc,
                                                   page, pd);

            if (ret)
            {
              rtems_fdisk_error ("forcing page to used failed: %d-%d-%d",
                                 device, segment, page);
            }

            sc->pages_used++;
          }
        }
        else
        {
          if (rtems_fdisk_page_desc_flags_set (pd, RTEMS_FDISK_PAGE_USED))
          {
            sc->pages_used++;
          }
          else if (rtems_fdisk_page_desc_flags_set (pd, RTEMS_FDISK_PAGE_ACTIVE))
          {
            if (pd->block >= fd->block_count)
            {
#if RTEMS_FDISK_TRACE
              rtems_fdisk_warning (fd,
                                   "invalid block number: %d-%d-%d: block: %d",
                                   device, segment, page, pd->block);
#endif
              sc->pages_bad++;
            }
            else if (fd->blocks[pd->block].segment)
            {
              /**
               * @todo
               * This may need more work later. Maybe a counter is stored with
               * each block so we can tell which is the later block when
               * duplicates appear. A power down with a failed wirte could cause
               * a duplicate.
               */
              const rtems_fdisk_segment_ctl* bsc = fd->blocks[pd->block].segment;
              rtems_fdisk_error ("duplicate block: %d-%d-%d: " \
                                 "duplicate: %d-%d-%d",
                                 bsc->device, bsc->segment,
                                 fd->blocks[pd->block].page,
                                 device, segment, page);
              sc->pages_bad++;
            }
            else
            {
              /**
               * @todo
               * Add start up crc checks here.
               */
              fd->blocks[pd->block].segment = sc;
              fd->blocks[pd->block].page    = page;

              /*
               * The page is active.
               */
              sc->pages_active++;
            }
          }
          else
            sc->pages_bad++;
        }
      }

      /*
       * Place the segment on to the correct queue.
       */
      rtems_fdisk_queue_segment (fd, sc);
    }
  }

  return 0;
}

/**
 * Read a block. The block is checked to see if the page referenced
 * is valid and the page has a valid crc.
 *
 * @param fd The rtems_flashdisk control table.
 * @param block The block number to read.
 * @param buffer The buffer to write the data into.
 * @return 0 No error.
 * @return EIO Invalid block size, block number, segment pointer, crc,
 *             page flags.
 */
static bool
rtems_fdisk_read_block (rtems_flashdisk* fd,
                        uint32_t         block,
                        uint8_t*         buffer)
{
  rtems_fdisk_block_ctl*   bc;
  rtems_fdisk_segment_ctl* sc;
  rtems_fdisk_page_desc*   pd;

#if RTEMS_FDISK_TRACE
  rtems_fdisk_info (fd, "read-block:%d", block);
#endif

  /*
   * Broken out to allow info messages when testing.
   */

  if (block >= (fd->block_count - fd->unavail_blocks))
  {
    rtems_fdisk_error ("read-block: block out of range: %d", block);
    return EIO;
  }

  bc = &fd->blocks[block];

  if (!bc->segment)
  {
#if RTEMS_FDISK_TRACE
    rtems_fdisk_info (fd, "read-block: no segment mapping: %d", block);
#endif
    memset (buffer, 0xff, fd->block_size);
    return 0;
  }

  sc = fd->blocks[block].segment;
  pd = &sc->page_descriptors[bc->page];

#if RTEMS_FDISK_TRACE
  rtems_fdisk_info (fd,
                    " read:%d=>%02d-%03d-%03d: p=%d a=%d u=%d b=%d n=%s: " \
                    "f=%04x c=%04x b=%d",
                    block, sc->device, sc->segment, bc->page,
                    sc->pages, sc->pages_active, sc->pages_used, sc->pages_bad,
                    sc->next ? "set" : "null",
                    pd->flags, pd->crc, pd->block);
#endif

  if (rtems_fdisk_page_desc_flags_set (pd, RTEMS_FDISK_PAGE_ACTIVE))
  {
    if (rtems_fdisk_page_desc_flags_clear (pd, RTEMS_FDISK_PAGE_USED))
    {
      uint16_t cs;

      /*
       * We use the segment page offset not the page number used in the
       * driver. This skips the page descriptors.
       */
      int ret = rtems_fdisk_seg_read_page (fd, sc,
                                           bc->page + sc->pages_desc, buffer);

      if (ret)
      {
#if RTEMS_FDISK_TRACE
        rtems_fdisk_info (fd,
                          "read-block:%02d-%03d-%03d: read page failed: %s (%d)",
                          sc->device, sc->segment, bc->page,
                          strerror (ret), ret);
#endif
        return ret;
      }

      cs = rtems_fdisk_page_checksum (buffer, fd->block_size);

      if (cs == pd->crc)
        return 0;

      rtems_fdisk_error ("read-block: crc failure: %d: buffer:%04x page:%04x",
                         block, cs, pd->crc);
    }
    else
    {
      rtems_fdisk_error ("read-block: block points to used page: %d: %d-%d-%d",
                         block, sc->device, sc->segment, bc->page);
    }
  }
  else
  {
    rtems_fdisk_error ("read-block: block page not active: %d: %d-%d-%d",
                       block, sc->device, sc->segment, bc->page);
  }

  return EIO;
}

/**
 * Write a block. The block:
 *
 *  # May never have existed in flash before this write.
 *  # Exists and needs to be moved to a new page.
 *
 * If the block does not exist in flash we need to get the next
 * segment available to place the page into. The segments with
 * available pages are held on the avaliable list sorted on least
 * number of available pages as the primary key. Currently there
 * is no secondary key. Empty segments are at the end of the list.
 *
 * If the block already exists we need to set the USED bit in the
 * current page's flags. This is a single byte which changes a 1 to
 * a 0 and can be done with a single 16 bit write. The driver for
 * 8 bit devices should only attempt the write on the changed bit.
 *
 * @param fd The rtems_flashdisk control table.
 * @param block The block number to read.
 * @param block_size The size of the block. Must match what we have.
 * @param buffer The buffer to write the data into.
 * @return 0 No error.
 * @return EIO Invalid block size, block number, segment pointer, crc,
 *             page flags.
 */
static int
rtems_fdisk_write_block (rtems_flashdisk* fd,
                         uint32_t         block,
                         const uint8_t*   buffer)
{
  rtems_fdisk_block_ctl*   bc;
  rtems_fdisk_segment_ctl* sc;
  rtems_fdisk_page_desc*   pd;
  uint32_t                 page;
  int                      ret;

#if RTEMS_FDISK_TRACE
  rtems_fdisk_info (fd, "write-block:%d", block);
#endif

  /*
   * Broken out to allow info messages when testing.
   */

  if (block >= (fd->block_count - fd->unavail_blocks))
  {
    rtems_fdisk_error ("write-block: block out of range: %d", block);
    return EIO;
  }

  bc = &fd->blocks[block];

  /*
   * Does the page exist in flash ?
   */
  if (bc->segment)
  {
    sc = bc->segment;
    pd = &sc->page_descriptors[bc->page];

#if RTEMS_FDISK_TRACE
    rtems_fdisk_info (fd, " write:%02d-%03d-%03d: flag used",
                      sc->device, sc->segment, bc->page);
#endif

    /*
     * The page exists in flash so see if the page has been changed.
     */
    if (rtems_fdisk_seg_verify_page (fd, sc->device, sc->segment,
                                     bc->page + sc->pages_desc, buffer) == 0)
    {
#if RTEMS_FDISK_TRACE
      rtems_fdisk_info (fd, "write-block:%d=>%02d-%03d-%03d: page verified",
                        block, sc->device, sc->segment, bc->page);
#endif
      return 0;
    }

    /*
     * The page exists in flash so we need to set the used flag
     * in the page descriptor. The descriptor is in memory with the
     * segment control block. We can assume this memory copy
     * matches the flash device.
     */

    rtems_fdisk_page_desc_set_flags (pd, RTEMS_FDISK_PAGE_USED);

    ret = rtems_fdisk_seg_write_page_desc_flags (fd, sc, bc->page, pd);

    if (ret)
    {
#if RTEMS_FDISK_TRACE
      rtems_fdisk_info (fd, " write:%02d-%03d-%03d: "      \
                        "write used page desc failed: %s (%d)",
                        sc->device, sc->segment, bc->page,
                        strerror (ret), ret);
#endif
    }
    else
    {
      sc->pages_active--;
      sc->pages_used++;
    }

    /*
     * If possible reuse this segment. This will mean the segment
     * needs to be removed from the available list and placed
     * back if space is still available.
     */
    rtems_fdisk_queue_segment (fd, sc);

    /*
     * If no background compacting then compact in the forground.
     * If we compact we ignore the error as there is little we
     * can do from here. The write may will work.
     */
    if ((fd->flags & RTEMS_FDISK_BACKGROUND_COMPACT) == 0)
      rtems_fdisk_compact (fd);
  }

  /*
   * Is it time to compact the disk ?
   *
   * We override the background compaction configruation.
   */
  if (rtems_fdisk_segment_count_queue (&fd->available) <=
      fd->avail_compact_segs)
    rtems_fdisk_compact (fd);

  /*
   * Get the next avaliable segment.
   */
  sc = rtems_fdisk_segment_queue_pop_head (&fd->available);

  /*
   * Is the flash disk full ?
   */
  if (!sc)
  {
    /*
     * If compacting is configured for the background do it now
     * to see if we can get some space back.
     */
    if ((fd->flags & RTEMS_FDISK_BACKGROUND_COMPACT))
      rtems_fdisk_compact (fd);

    /*
     * Try again for some free space.
     */
    sc = rtems_fdisk_segment_queue_pop_head (&fd->available);

    if (!sc)
    {
      rtems_fdisk_error ("write-block: no available pages");
      return ENOSPC;
    }
  }

#if RTEMS_FDISK_TRACE
  if (fd->info_level >= 3)
  {
    char queues[5];
    rtems_fdisk_queue_status (fd, sc, queues);
    rtems_fdisk_info (fd, " write:%d=>%02d-%03d: queue check: %s",
                      block, sc->device, sc->segment, queues);
  }
#endif

  /*
   * Find the next avaliable page in the segment.
   */

  pd = sc->page_descriptors;

  for (page = 0; page < sc->pages; page++, pd++)
  {
    if (rtems_fdisk_page_desc_erased (pd))
    {
      pd->crc   = rtems_fdisk_page_checksum (buffer, fd->block_size);
      pd->block = block;

      bc->segment = sc;
      bc->page    = page;

      rtems_fdisk_page_desc_set_flags (pd, RTEMS_FDISK_PAGE_ACTIVE);

#if RTEMS_FDISK_TRACE
      rtems_fdisk_info (fd, " write:%d=>%02d-%03d-%03d: write: " \
                        "p=%d a=%d u=%d b=%d n=%s: f=%04x c=%04x b=%d",
                        block, sc->device, sc->segment, page,
                        sc->pages, sc->pages_active, sc->pages_used,
                        sc->pages_bad, sc->next ? "set" : "null",
                        pd->flags, pd->crc, pd->block);
#endif

      /*
       * We use the segment page offset not the page number used in the
       * driver. This skips the page descriptors.
       */
      ret = rtems_fdisk_seg_write_page (fd, sc, page + sc->pages_desc, buffer);
      if (ret)
      {
#if RTEMS_FDISK_TRACE
        rtems_fdisk_info (fd, "write-block:%02d-%03d-%03d: write page failed: " \
                          "%s (%d)", sc->device, sc->segment, page,
                          strerror (ret), ret);
#endif
      }
      else
      {
        ret = rtems_fdisk_seg_write_page_desc (fd, sc, page, pd);
        if (ret)
        {
#if RTEMS_FDISK_TRACE
          rtems_fdisk_info (fd, "write-block:%02d-%03d-%03d: "  \
                            "write page desc failed: %s (%d)",
                            sc->device, sc->segment, bc->page,
                            strerror (ret), ret);
#endif
        }
        else
        {
          sc->pages_active++;
        }
      }

      rtems_fdisk_queue_segment (fd, sc);

      if (rtems_fdisk_is_erased_blocks_starvation (fd))
        rtems_fdisk_compact (fd);

      return ret;
    }
  }

  rtems_fdisk_error ("write-block: no erased page descs in segment: %d-%d",
                     sc->device, sc->segment);

  sc->failed = true;
  rtems_fdisk_queue_segment (fd, sc);

  return EIO;
}

/**
 * Disk READ request handler. This primitive copies data from the
 * flash disk to the supplied buffer and invoke the callout function
 * to inform upper layer that reading is completed.
 *
 * @param req Pointer to the READ block device request info.
 * @retval 0 Always.  The request done callback contains the status.
 */
static int
rtems_fdisk_read (rtems_flashdisk* fd, rtems_blkdev_request* req)
{
  rtems_blkdev_sg_buffer* sg = req->bufs;
  uint32_t                buf;
  int                     ret = 0;

  for (buf = 0; (ret == 0) && (buf < req->bufnum); buf++, sg++)
  {
    uint8_t* data;
    uint32_t fb;
    uint32_t b;
    fb = sg->length / fd->block_size;
    data = sg->buffer;
    for (b = 0; b < fb; b++, data += fd->block_size)
    {
      ret = rtems_fdisk_read_block (fd, sg->block + b, data);
      if (ret)
        break;
    }
  }

  req->status = ret ? RTEMS_IO_ERROR : RTEMS_SUCCESSFUL;
  req->req_done (req->done_arg, req->status);

  return 0;
}

/**
 * Flash disk WRITE request handler. This primitive copies data from
 * supplied buffer to flash disk and invoke the callout function to inform
 * upper layer that writing is completed.
 *
 * @param req Pointers to the WRITE block device request info.
 * @retval 0 Always.  The request done callback contains the status.
 */
static int
rtems_fdisk_write (rtems_flashdisk* fd, rtems_blkdev_request* req)
{
  rtems_blkdev_sg_buffer* sg = req->bufs;
  uint32_t                buf;
  int                     ret = 0;

  for (buf = 0; (ret == 0) && (buf < req->bufnum); buf++, sg++)
  {
    uint8_t* data;
    uint32_t fb;
    uint32_t b;
    fb = sg->length / fd->block_size;
    data = sg->buffer;
    for (b = 0; b < fb; b++, data += fd->block_size)
    {
      ret = rtems_fdisk_write_block (fd, sg->block + b, data);
      if (ret)
        break;
    }
  }

  req->status = ret ? RTEMS_IO_ERROR : RTEMS_SUCCESSFUL;
  req->req_done (req->done_arg, req->status);

  return 0;
}

/**
 * Flash disk erase disk.
 *
 * @param fd The flashdisk data.
 * @retval int The ioctl return value.
 */
static int
rtems_fdisk_erase_disk (rtems_flashdisk* fd)
{
  uint32_t device;
  int      ret;

#if RTEMS_FDISK_TRACE
  rtems_fdisk_info (fd, "erase-disk");
#endif

  ret = rtems_fdisk_erase_flash (fd);

  if (ret == 0)
  {
    for (device = 0; device < fd->device_count; device++)
    {
      if (!fd->devices[device].segments)
        return ENOMEM;

      ret = rtems_fdisk_recover_block_mappings (fd);
      if (ret)
        break;
    }
  }

  return ret;
}

/**
 * Flash Disk Monitoring data is return in the monitoring data
 * structure.
 */
static int
rtems_fdisk_monitoring_data (rtems_flashdisk*          fd,
                             rtems_fdisk_monitor_data* data)
{
  uint32_t i;
  uint32_t j;

  data->block_size     = fd->block_size;
  data->block_count    = fd->block_count;
  data->unavail_blocks = fd->unavail_blocks;
  data->device_count   = fd->device_count;

  data->blocks_used = 0;
  for (i = 0; i < fd->block_count; i++)
    if (fd->blocks[i].segment)
      data->blocks_used++;

  data->segs_available = rtems_fdisk_segment_count_queue (&fd->available);
  data->segs_used      = rtems_fdisk_segment_count_queue (&fd->used);
  data->segs_failed    = rtems_fdisk_segment_count_queue (&fd->failed);

  data->segment_count = 0;
  data->page_count    = 0;
  data->pages_desc    = 0;
  data->pages_active  = 0;
  data->pages_used    = 0;
  data->pages_bad     = 0;
  data->seg_erases    = 0;

  for (i = 0; i < fd->device_count; i++)
  {
    data->segment_count += fd->devices[i].segment_count;

    for (j = 0; j < fd->devices[i].segment_count; j++)
    {
      rtems_fdisk_segment_ctl* sc = &fd->devices[i].segments[j];

      data->page_count   += sc->pages;
      data->pages_desc   += sc->pages_desc;
      data->pages_active += sc->pages_active;
      data->pages_used   += sc->pages_used;
      data->pages_bad    += sc->pages_bad;
      data->seg_erases   += sc->erased;
    }
  }

  data->info_level = fd->info_level;
  return 0;
}

/**
 * Print to stdout the status of the driver. This is a debugging aid.
 */
static int
rtems_fdisk_print_status (rtems_flashdisk* fd)
{
#if RTEMS_FDISK_TRACE
  uint32_t current_info_level = fd->info_level;
  uint32_t total;
  uint32_t count;
  uint32_t device;

  fd->info_level = 3;

  rtems_fdisk_printf (fd,
                      "Flash Disk Driver Status : %d.%d", fd->major, fd->minor);

  rtems_fdisk_printf (fd, "Block count\t%d", fd->block_count);
  rtems_fdisk_printf (fd, "Unavail blocks\t%d", fd->unavail_blocks);
  rtems_fdisk_printf (fd, "Starvation threshold\t%d", fd->starvation_threshold);
  rtems_fdisk_printf (fd, "Starvations\t%d", fd->starvations);
  count = rtems_fdisk_segment_count_queue (&fd->available);
  total = count;
  rtems_fdisk_printf (fd, "Available queue\t%ld (%ld)",
                      count, rtems_fdisk_segment_queue_count (&fd->available));
  count = rtems_fdisk_segment_count_queue (&fd->used);
  total += count;
  rtems_fdisk_printf (fd, "Used queue\t%ld (%ld)",
                      count, rtems_fdisk_segment_queue_count (&fd->used));
  count = rtems_fdisk_segment_count_queue (&fd->erase);
  total += count;
  rtems_fdisk_printf (fd, "Erase queue\t%ld (%ld)",
                      count, rtems_fdisk_segment_queue_count (&fd->erase));
  count = rtems_fdisk_segment_count_queue (&fd->failed);
  total += count;
  rtems_fdisk_printf (fd, "Failed queue\t%ld (%ld)",
                      count, rtems_fdisk_segment_queue_count (&fd->failed));

  count = 0;
  for (device = 0; device < fd->device_count; device++)
    count += fd->devices[device].segment_count;

  rtems_fdisk_printf (fd, "Queue total\t%ld of %ld, %s", total, count,
                      total == count ? "ok" : "MISSING");

  rtems_fdisk_printf (fd, "Device count\t%d", fd->device_count);

  for (device = 0; device < fd->device_count; device++)
  {
    uint32_t block;
    uint32_t seg;

    rtems_fdisk_printf (fd, " Device\t\t%ld", device);
    rtems_fdisk_printf (fd, "  Segment count\t%ld",
                        fd->devices[device].segment_count);

    for (seg = 0; seg < fd->devices[device].segment_count; seg++)
    {
      rtems_fdisk_segment_ctl* sc = &fd->devices[device].segments[seg];
      uint32_t                 page;
      uint32_t                 erased = 0;
      uint32_t                 active = 0;
      uint32_t                 used = 0;
      bool                     is_active = false;
      char                     queues[5];

      rtems_fdisk_queue_status (fd, sc, queues);

      for (page = 0; page < sc->pages; page++)
      {
        if (rtems_fdisk_page_desc_erased (&sc->page_descriptors[page]))
          erased++;
        else if (rtems_fdisk_page_desc_flags_set (&sc->page_descriptors[page],
                                                  RTEMS_FDISK_PAGE_ACTIVE))
        {
          if (rtems_fdisk_page_desc_flags_set (&sc->page_descriptors[page],
                                               RTEMS_FDISK_PAGE_USED))
            used++;
          else
          {
            active++;
            is_active = true;
          }
        }

        for (block = 0; block < fd->block_count; block++)
        {
          if ((fd->blocks[block].segment == sc) &&
              (fd->blocks[block].page == page) && !is_active)
            rtems_fdisk_printf (fd,
                                "    %ld\t not active when mapped by block %ld",
                                page, block);
        }
      }

      count = 0;
      for (block = 0; block < fd->block_count; block++)
      {
        if (fd->blocks[block].segment == sc)
          count++;
      }

      rtems_fdisk_printf (fd, "  %3ld %s p:%3ld a:%3ld/%3ld" \
                          " u:%3ld/%3ld e:%3ld/%3ld br:%ld",
                          seg, queues,
                          sc->pages, sc->pages_active, active,
                          sc->pages_used, used, erased,
                          sc->pages - (sc->pages_active +
                                       sc->pages_used + sc->pages_bad),
                          count);
    }
  }

  {
    rtems_fdisk_segment_ctl* sc = fd->used.head;
    int count = 0;
    rtems_fdisk_printf (fd, "Used List:");
    while (sc)
    {
      rtems_fdisk_printf (fd, "  %3d %02d:%03d u:%3ld",
                          count, sc->device, sc->segment, sc->pages_used);
      sc = sc->next;
      count++;
    }
  }
  fd->info_level = current_info_level;

  return 0;
#else
  return ENOSYS;
#endif
}

/**
 * Flash disk IOCTL handler.
 *
 * @param dd Disk device.
 * @param req IOCTL request code.
 * @param argp IOCTL argument.
 * @retval The IOCTL return value
 */
static int
rtems_fdisk_ioctl (rtems_disk_device *dd, uint32_t req, void* argp)
{
  dev_t                     dev = rtems_disk_get_device_identifier (dd);
  rtems_device_minor_number minor = rtems_filesystem_dev_minor_t (dev);
  rtems_blkdev_request*     r = argp;
  rtems_status_code         sc;

  errno = 0;

  sc = rtems_semaphore_obtain (rtems_flashdisks[minor].lock, RTEMS_WAIT, 0);
  if (sc != RTEMS_SUCCESSFUL)
    errno = EIO;
  else
  {
    errno = 0;
    switch (req)
    {
      case RTEMS_BLKIO_REQUEST:
        if ((minor >= rtems_flashdisk_count) ||
            (rtems_flashdisks[minor].device_count == 0))
        {
          errno = ENODEV;
        }
        else
        {
          switch (r->req)
          {
            case RTEMS_BLKDEV_REQ_READ:
              errno = rtems_fdisk_read (&rtems_flashdisks[minor], r);
              break;

            case RTEMS_BLKDEV_REQ_WRITE:
              errno = rtems_fdisk_write (&rtems_flashdisks[minor], r);
              break;

            default:
              errno = EINVAL;
              break;
          }
        }
        break;

      case RTEMS_FDISK_IOCTL_ERASE_DISK:
        errno = rtems_fdisk_erase_disk (&rtems_flashdisks[minor]);
        break;

      case RTEMS_FDISK_IOCTL_COMPACT:
        errno = rtems_fdisk_compact (&rtems_flashdisks[minor]);
        break;

      case RTEMS_FDISK_IOCTL_ERASE_USED:
        errno = rtems_fdisk_erase_used (&rtems_flashdisks[minor]);
        break;

      case RTEMS_FDISK_IOCTL_MONITORING:
        errno = rtems_fdisk_monitoring_data (&rtems_flashdisks[minor],
                                             (rtems_fdisk_monitor_data*) argp);
        break;

      case RTEMS_FDISK_IOCTL_INFO_LEVEL:
        rtems_flashdisks[minor].info_level = (uintptr_t) argp;
        break;

      case RTEMS_FDISK_IOCTL_PRINT_STATUS:
        errno = rtems_fdisk_print_status (&rtems_flashdisks[minor]);
        break;

      default:
        rtems_blkdev_ioctl (dd, req, argp);
        break;
    }

    sc = rtems_semaphore_release (rtems_flashdisks[minor].lock);
    if (sc != RTEMS_SUCCESSFUL)
      errno = EIO;
  }

  return errno == 0 ? 0 : -1;
}

/**
 * Flash disk device driver initialization.
 *
 * @todo Memory clean up on error is really badly handled.
 *
 * @param major Flash disk major device number.
 * @param minor Minor device number, not applicable.
 * @param arg Initialization argument, not applicable.
 */
rtems_device_driver
rtems_fdisk_initialize (rtems_device_major_number major,
                        rtems_device_minor_number minor,
                        void*                     arg __attribute__((unused)))
{
  const rtems_flashdisk_config* c = rtems_flashdisk_configuration;
  rtems_flashdisk*              fd;
  rtems_status_code             sc;

  sc = rtems_disk_io_initialize ();
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  sc = rtems_fdisk_crc16_gen_factors (0x8408);
  if (sc != RTEMS_SUCCESSFUL)
      return sc;

  rtems_flashdisks = calloc (rtems_flashdisk_configuration_size,
                             sizeof (rtems_flashdisk));

  if (!rtems_flashdisks)
    return RTEMS_NO_MEMORY;

  for (minor = 0; minor < rtems_flashdisk_configuration_size; minor++, c++)
  {
    char     name[] = RTEMS_FLASHDISK_DEVICE_BASE_NAME "a";
    dev_t    dev = rtems_filesystem_make_dev_t (major, minor);
    uint32_t device;
    uint32_t blocks = 0;
    int      ret;

    fd = &rtems_flashdisks[minor];

    name [sizeof(RTEMS_FLASHDISK_DEVICE_BASE_NAME)] += minor;

    fd->major              = major;
    fd->minor              = minor;
    fd->flags              = c->flags;
    fd->compact_segs       = c->compact_segs;
    fd->avail_compact_segs = c->avail_compact_segs;
    fd->block_size         = c->block_size;
    fd->unavail_blocks     = c->unavail_blocks;
    fd->info_level         = c->info_level;

    for (device = 0; device < c->device_count; device++)
      blocks += rtems_fdisk_blocks_in_device (&c->devices[device],
                                              c->block_size);

    /*
     * One copy buffer of a page size.
     */
    fd->copy_buffer = malloc (c->block_size);
    if (!fd->copy_buffer)
      return RTEMS_NO_MEMORY;

    fd->blocks = calloc (blocks, sizeof (rtems_fdisk_block_ctl));
    if (!fd->blocks)
      return RTEMS_NO_MEMORY;

    fd->block_count = blocks;

    fd->devices = calloc (c->device_count, sizeof (rtems_fdisk_device_ctl));
    if (!fd->devices)
      return RTEMS_NO_MEMORY;

    sc = rtems_semaphore_create (rtems_build_name ('F', 'D', 'S', 'K'), 1,
                                 RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE |
                                 RTEMS_INHERIT_PRIORITY, 0, &fd->lock);
    if (sc != RTEMS_SUCCESSFUL)
    {
      rtems_fdisk_error ("disk lock create failed");
      free (fd->copy_buffer);
      free (fd->blocks);
      free (fd->devices);
      return sc;
    }

    sc = rtems_disk_create_phys(dev, c->block_size,
                                blocks - fd->unavail_blocks,
                                rtems_fdisk_ioctl, NULL, name);
    if (sc != RTEMS_SUCCESSFUL)
    {
      rtems_semaphore_delete (fd->lock);
      rtems_disk_delete (dev);
      free (fd->copy_buffer);
      free (fd->blocks);
      free (fd->devices);
      rtems_fdisk_error ("disk create phy failed");
      return sc;
    }

    for (device = 0; device < c->device_count; device++)
    {
      rtems_fdisk_segment_ctl* sc;
      uint32_t                 segment_count;
      uint32_t                 segment;

      segment_count = rtems_fdisk_count_segments (&c->devices[device]);

      fd->devices[device].segments = calloc (segment_count,
                                             sizeof (rtems_fdisk_segment_ctl));
      if (!fd->devices[device].segments)
      {
        rtems_disk_delete (dev);
        rtems_semaphore_delete (fd->lock);
        free (fd->copy_buffer);
        free (fd->blocks);
        free (fd->devices);
        return RTEMS_NO_MEMORY;
      }

      sc = fd->devices[device].segments;

      for (segment = 0; segment < c->devices[device].segment_count; segment++)
      {
        const rtems_fdisk_segment_desc* sd;
        uint32_t                        seg_segment;

        sd = &c->devices[device].segments[segment];

        for (seg_segment = 0; seg_segment < sd->count; seg_segment++, sc++)
        {
          sc->descriptor = sd;
          sc->device     = device;
          sc->segment    = seg_segment;
          sc->erased     = 0;
        }
      }

      fd->devices[device].segment_count = segment_count;
      fd->devices[device].descriptor    = &c->devices[device];
    }

    fd->device_count = c->device_count;

    ret = rtems_fdisk_recover_block_mappings (fd);
    if (ret)
    {
      rtems_disk_delete (dev);
      rtems_semaphore_delete (fd->lock);
      free (fd->copy_buffer);
      free (fd->blocks);
      free (fd->devices);
      rtems_fdisk_error ("recovery of disk failed: %s (%d)",
                         strerror (ret), ret);
      return ret;
    }

    ret = rtems_fdisk_compact (fd);
    if (ret)
    {
      rtems_disk_delete (dev);
      rtems_semaphore_delete (fd->lock);
      free (fd->copy_buffer);
      free (fd->blocks);
      free (fd->devices);
      rtems_fdisk_error ("compacting of disk failed: %s (%d)",
                         strerror (ret), ret);
      return ret;
    }
  }

  rtems_flashdisk_count = rtems_flashdisk_configuration_size;

  return RTEMS_SUCCESSFUL;
}
