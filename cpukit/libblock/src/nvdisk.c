/*
 * nvdisk.c -- Non-volatile disk block device implementation
 *
 * Copyright (C) 2007 Chris Johns
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
#include "rtems/nvdisk.h"

/**
 * @note
 *
 * The use of pages can vary. The rtems_nvdisk_*_page set
 * routines use an absolute page number relative to the segment
 * while all other page numbera are relative to the number of
 * page descriptor pages a segment has. You need to add the
 * number of page descriptor pages (pages_desc) to the page number
 * when call the rtems_nvdisk_*_page functions.
 *
 * You must always show the page number as relative in any trace
 * or error message as device-page and if you have to
 * the page number as absolute use device~page. This
 * can be seen in the page copy routine.
 *
 * The code is like this to avoid needing the pass the pages_desc
 * value around. It is only used in selected places and so the
 * extra parameter was avoided.
 */

/**
 * Control tracing. It can be compiled out of the code for small
 * footprint targets. Leave in by default.
 */
#if !defined (RTEMS_NVDISK_TRACE)
#define RTEMS_NVDISK_TRACE 0
#endif

/**
 * NV Device Control holds the segment controls
 */
typedef struct rtems_nvdisk_device_ctl
{
  /**
   * The device this segment resides on.
   */
  uint32_t device;

  /**
   * Total number of pages in the device.
   */
  uint32_t pages;

  /**
   * Number of pages used for page checksums.
   */
  uint32_t pages_desc;

  /**
   * First block number for this device.
   */
  uint32_t block_base;

  /**
   * Device descriptor.
   */
  const rtems_nvdisk_device_desc* descriptor;
} rtems_nvdisk_device_ctl;

/**
 * The NV disk control structure for a single disk. There is one
 * for each minor disk in the system.
 */
typedef struct rtems_mvdisk
{
  rtems_device_major_number major;        /**< The driver's major number. */
  rtems_device_minor_number minor;        /**< The driver's minor number. */
  uint32_t                  flags;        /**< configuration flags. */
  uint32_t                  block_size;   /**< The block size for this disk. */
  uint32_t                  block_count;  /**< The number of available blocks. */
  rtems_nvdisk_device_ctl*  devices;      /**< The NV devices for this disk. */
  uint32_t                  device_count; /**< The number of NV devices. */
  uint32_t                  cs_pages;     /**< The num of pages of checksums. */
  rtems_id                  lock;         /**< Mutex for threading protection.*/
  uint32_t info_level;                    /**< The info trace level. */
} rtems_nvdisk;

/**
 * The array of NV disks we support.
 */
static rtems_nvdisk* rtems_nvdisks;

/**
 * The number of NV disks we have.
 */
static uint32_t rtems_nvdisk_count;

/**
 * The CRC16 factor table. Created during initialisation.
 */
static uint16_t* rtems_nvdisk_crc16_factor;

/**
 * Calculate the CRC16 checksum.
 *
 * @param _b The byte to checksum.
 * @param _c The current checksum.
 */
#define rtems_nvdisk_calc_crc16(_b, _c) \
  rtems_nvdisk_crc16_factor[((_b) ^ ((_c) & 0xff)) & 0xff] ^ (((_c) >> 8) & 0xff)

/**
 * Generate the CRC table.
 *
 * @param pattern The seed pattern for the table of factors.
 * @relval RTEMS_SUCCESSFUL The table was generated.
 * @retval RTEMS_NO_MEMORY The table could not be allocated from the heap.
 */
static rtems_status_code
rtems_nvdisk_crc16_gen_factors (uint16_t pattern)
{
  uint32_t b;

  rtems_nvdisk_crc16_factor = malloc (sizeof (uint16_t) * 256);
  if (!rtems_nvdisk_crc16_factor)
    return RTEMS_NO_MEMORY;

  for (b = 0; b < 256; b++)
  {
    uint32_t i;
    uint16_t v = b;
    for (i = 8; i--;)
      v = v & 1 ? (v >> 1) ^ pattern : v >> 1;
    rtems_nvdisk_crc16_factor[b] = v & 0xffff;
  }
  return RTEMS_SUCCESSFUL;
}

#if RTEMS_NVDISK_TRACE
/**
 * Print a message to the nvdisk output and flush it.
 *
 * @param nvd The nvdisk control structure.
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_nvdisk_printf (const rtems_nvdisk* nvd, const char *format, ...)
{
  int ret = 0;
  if (nvd->info_level >= 3)
  {
    va_list args;
    va_start (args, format);
    fprintf (stdout, "nvdisk:");
    ret =  vfprintf (stdout, format, args);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (args);
  }
  return ret;
}

/**
 * Print a info message to the nvdisk output and flush it.
 *
 * @param nvd The nvdisk control structure.
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_nvdisk_info (const rtems_nvdisk* nvd, const char *format, ...)
{
  int ret = 0;
  if (nvd->info_level >= 2)
  {
    va_list args;
    va_start (args, format);
    fprintf (stdout, "nvdisk:");
    ret =  vfprintf (stdout, format, args);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (args);
  }
  return ret;
}

/**
 * Print a warning to the nvdisk output and flush it.
 *
 * @param nvd The nvdisk control structure.
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_nvdisk_warning (const rtems_nvdisk* nvd, const char *format, ...)
{
  int ret = 0;
  if (nvd->info_level >= 1)
  {
    va_list args;
    va_start (args, format);
    fprintf (stdout, "nvdisk:warning:");
    ret =  vfprintf (stdout, format, args);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (args);
  }
  return ret;
}
#endif

/**
 * Print an error to the nvdisk output and flush it.
 *
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
static int
rtems_nvdisk_error (const char *format, ...)
{
  int ret;
  va_list args;
  va_start (args, format);
  fprintf (stderr, "nvdisk:error:");
  ret =  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");
  fflush (stderr);
  va_end (args);
  return ret;
}

/**
 * Get the descriptor for a device.
 */
static const rtems_nvdisk_device_desc*
rtems_nvdisk_device_descriptor (const rtems_nvdisk* nvd, uint32_t device)
{
  return nvd->devices[device].descriptor;
}

/**
 * Read a block of data from a device.
 */
static int
rtems_nvdisk_device_read (const rtems_nvdisk* nvd,
                          uint32_t            device,
                          uint32_t            offset,
                          void*               buffer,
                          uint32_t            size)
{
  const rtems_nvdisk_device_desc*     dd;
  const rtems_nvdisk_driver_handlers* ops;
  dd  = rtems_nvdisk_device_descriptor (nvd, device);
  ops = nvd->devices[device].descriptor->nv_ops;
#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_printf (nvd, "  dev-read: %02d-%08x: s=%d",
                      device, offset, size);
#endif
  return ops->read (device, dd->flags, dd->base, offset, buffer, size);
}

/**
 * Write a block of data to a device.
 */
static int
rtems_nvdisk_device_write (const rtems_nvdisk* nvd,
                           uint32_t            device,
                           uint32_t            offset,
                           const void*         buffer,
                           uint32_t            size)
{
  const rtems_nvdisk_device_desc*     dd;
  const rtems_nvdisk_driver_handlers* ops;
  dd  = rtems_nvdisk_device_descriptor (nvd, device);
  ops = nvd->devices[device].descriptor->nv_ops;
#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_printf (nvd, "  dev-write: %02d-%08x: s=%d",
                      device, offset, size);
#endif
  return ops->write (device, dd->flags, dd->base, offset, buffer, size);
}

#if NOT_USED
/**
 * Verify the data with the data in a segment.
 */
static int
rtems_nvdisk_device_verify (const rtems_nvdisk* nvd,
                            uint32_t            device,
                            uint32_t            offset,
                            const void*         buffer,
                            uint32_t            size)
{
  const rtems_nvdisk_device_desc*     dd;
  const rtems_nvdisk_driver_handlers* ops;
  dd  = rtems_nvdisk_device_descriptor (nvd, device);
  ops = nvd->devices[device].descriptor->nv_ops;
#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_printf (nvd, "  seg-verify: %02d-%08x: s=%d",
                      device, offset, size);
#endif
  return ops->verify (device, dd->flags, dd->base, offset, buffer, size);
}
#endif

/**
 * Read a page of data from the device.
 */
static int
rtems_nvdisk_read_page (const rtems_nvdisk* nvd,
                        uint32_t            device,
                        uint32_t            page,
                        void*               buffer)
{
  return rtems_nvdisk_device_read (nvd, device,
                                   page * nvd->block_size, buffer,
                                   nvd->block_size);
}

/**
 * Write a page of data to a device.
 */
static int
rtems_nvdisk_write_page (const rtems_nvdisk* nvd,
                         uint32_t            device,
                         uint32_t            page,
                         const void*         buffer)
{
  return rtems_nvdisk_device_write (nvd, device,
                                    page * nvd->block_size,
                                    buffer, nvd->block_size);
}

/**
 * Read the checksum from the device.
 */
static int
rtems_nvdisk_read_checksum (const rtems_nvdisk* nvd,
                            uint32_t            device,
                            uint32_t            page,
                            uint16_t*           cs)
{
  return rtems_nvdisk_device_read (nvd, device,
                                   page * sizeof (uint16_t),
                                   cs, sizeof (uint16_t));
}

/**
 * Write the checksum to the device.
 */
static int
rtems_nvdisk_write_checksum (const rtems_nvdisk* nvd,
                             uint32_t            device,
                             uint32_t            page,
                             const uint16_t      cs)
{
  return rtems_nvdisk_device_write (nvd, device,
                                    page * sizeof (uint16_t),
                                    &cs, sizeof (uint16_t));
}

/**
 * Calculate the pages in a device give the device descriptor and the
 * page size.
 *
 * @param dd The device descriptor.
 * @param page_size The page size in bytes.
 */
static uint32_t
rtems_nvdisk_pages_in_device (const rtems_nvdisk*             nvd,
                              const rtems_nvdisk_device_desc* dd)
{
  return dd->size / nvd->block_size;
}

/**
 * Calculate the number of pages needed to hold the page descriptors.
 * The calculation need to round up.
 */
static uint32_t
rtems_nvdisk_page_desc_pages (const rtems_nvdisk*             nvd,
                              const rtems_nvdisk_device_desc* dd)
{
  uint32_t pages = rtems_nvdisk_pages_in_device (nvd, dd);
  uint32_t bytes = pages * sizeof (uint16_t);
  return ((bytes - 1) / nvd->block_size) + 1;
}

/**
 * Calculate the checksum of a page.
 */
static uint16_t
rtems_nvdisk_page_checksum (const uint8_t* buffer, uint32_t page_size)
{
  uint16_t cs = 0xffff;
  uint32_t i;

  for (i = 0; i < page_size; i++, buffer++)
    cs = rtems_nvdisk_calc_crc16 (cs, *buffer);

  return cs;
}

/**
 * Map a block to a device.
 */
static rtems_nvdisk_device_ctl*
rtems_nvdisk_get_device (rtems_nvdisk* nvd, uint32_t block)
{
  uint32_t device;

  if (block >= nvd->block_count)
  {
    rtems_nvdisk_error ("read-block: bad block: %d", block);
    return NULL;
  }

  for (device = 0; device < nvd->device_count; device++)
  {
    rtems_nvdisk_device_ctl* dc = &nvd->devices[device];
    if ((block >= dc->block_base) &&
        (block < (dc->block_base + dc->pages - dc->pages_desc)))
      return dc;
  }

  rtems_nvdisk_error ("map-block:%d: no device/page map found", block);

  return NULL;
}

/**
 * Get the page for a block in a device.
 */
static uint32_t
rtems_nvdisk_get_page (rtems_nvdisk_device_ctl* dc,
                       uint32_t                 block)
{
  return block - dc->block_base;
}

/**
 * Read a block. The block is checked to see if the page referenced
 * is valid and the page has a valid crc.
 *
 * @param nvd The rtems_nvdisk control table.
 * @param block The block number to read.
 * @param buffer The buffer to write the data into.
 * @return 0 No error.
 * @return EIO Invalid block number or crc.
 */
static int
rtems_nvdisk_read_block (rtems_nvdisk* nvd, uint32_t block, uint8_t* buffer)
{
  rtems_nvdisk_device_ctl* dc;
  uint32_t                 page;
  uint16_t                 crc;
  uint16_t                 cs;
  int                      ret;

  dc = rtems_nvdisk_get_device (nvd, block);

  if (!dc)
    return EIO;

  page = rtems_nvdisk_get_page (dc, block);

#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_info (nvd, " read-block:%d=>%02d-%03d, cs:%04x",
                     block, dc->device, page, crc);
#endif

  ret = rtems_nvdisk_read_checksum (nvd, dc->device, page, &crc);

  if (ret)
    return ret;

  if (crc == 0xffff)
  {
#if RTEMS_NVDISK_TRACE
    rtems_nvdisk_warning (nvd, "read-block: crc not set: %d", block);
#endif
    memset (buffer, 0, nvd->block_size);
    return 0;
  }

  ret = rtems_nvdisk_read_page (nvd, dc->device, page + dc->pages_desc, buffer);

  if (ret)
    return ret;

  cs = rtems_nvdisk_page_checksum (buffer, nvd->block_size);

  if (cs != crc)
  {
    rtems_nvdisk_error ("read-block: crc failure: %d: buffer:%04x page:%04x",
                        block, cs, crc);
    return EIO;
  }

  return 0;
}

/**
 * Write a block.
 *
 * @param nvd The rtems_nvdisk control table.
 * @param block The block number to read.
 * @param block_size The size of the block. Must match what we have.
 * @param buffer The buffer to write the data into.
 * @return 0 No error.
 * @return EIO Invalid block size, block number, segment pointer, crc,
 *             page flags.
 */
static int
rtems_nvdisk_write_block (rtems_nvdisk*        nvd,
                          uint32_t             block,
                          const unsigned char* buffer)
{
  rtems_nvdisk_device_ctl* dc;
  uint32_t                 page;
  uint16_t                 cs;
  int                      ret;

  dc = rtems_nvdisk_get_device (nvd, block);

  if (!dc)
    return EIO;

  page = rtems_nvdisk_get_page (dc, block);

  cs = rtems_nvdisk_page_checksum (buffer, nvd->block_size);

#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_info (nvd, " write-block:%d=>%02d-%03d", block, dc->device, page);
#endif

  ret = rtems_nvdisk_write_page (nvd, dc->device, page + dc->pages_desc, buffer);

  if (ret)
    return ret;

  return rtems_nvdisk_write_checksum (nvd, dc->device, page, cs);
}

/**
 * Disk READ request handler. This primitive copies data from the
 * flash disk to the supplied buffer and invoke the callout function
 * to inform upper layer that reading is completed.
 *
 * @param req Pointer to the READ block device request info.
 * @retval int The ioctl return value.
 */
static int
rtems_nvdisk_read (rtems_nvdisk* nvd, rtems_blkdev_request* req)
{
  rtems_blkdev_sg_buffer* sg = req->bufs;
  uint32_t                bufs;
  int                     ret = 0;

#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_info (nvd, "read: blocks=%d", req->bufnum);
#endif

  for (bufs = 0; (ret == 0) && (bufs < req->bufnum); bufs++, sg++)
  {
    uint8_t* data;
    uint32_t nvb;
    uint32_t b;
    nvb = sg->length / nvd->block_size;
    data = sg->buffer;
    for (b = 0; b < nvb; b++, data += nvd->block_size)
    {
      ret = rtems_nvdisk_read_block (nvd, sg->block + b, data);
      if (ret)
        break;
    }
  }

  req->status = ret ? RTEMS_IO_ERROR : RTEMS_SUCCESSFUL;
  req->req_done (req->done_arg, req->status);

  return ret;
}

/**
 * Flash disk WRITE request handler. This primitive copies data from
 * supplied buffer to NV disk and invoke the callout function to inform
 * upper layer that writing is completed.
 *
 * @param req Pointers to the WRITE block device request info.
 * @retval int The ioctl return value.
 */
static int
rtems_nvdisk_write (rtems_nvdisk* nvd, rtems_blkdev_request* req)
{
  rtems_blkdev_sg_buffer* sg = req->bufs;
  uint32_t                bufs;
  int                     ret = 0;

#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_info (nvd, "write: blocks=%d", req->bufnum);
#endif

  for (bufs = 0; (ret == 0) && (bufs < req->bufnum); bufs++, sg++)
  {
    uint8_t* data;
    uint32_t nvb;
    uint32_t b;
    nvb = sg->length / nvd->block_size;
    data = sg->buffer;
    for (b = 0; b < nvb; b++, data += nvd->block_size)
    {
      ret = rtems_nvdisk_write_block (nvd, sg->block + b, data);
      if (ret)
        break;
    }
  }

  req->status = ret ? RTEMS_IO_ERROR : RTEMS_SUCCESSFUL;
  req->req_done (req->done_arg, req->status);

  return 0;
}

/**
 * NV disk erase disk sets all the checksums for 0xffff.
 *
 * @param nvd The nvdisk data.
 * @retval int The ioctl return value.
 */
static int
rtems_nvdisk_erase_disk (rtems_nvdisk* nvd)
{
  uint32_t device;

#if RTEMS_NVDISK_TRACE
  rtems_nvdisk_info (nvd, "erase-disk");
#endif

  for (device = 0; device < nvd->device_count; device++)
  {
    rtems_nvdisk_device_ctl* dc = &nvd->devices[device];
    uint32_t                 page;
    for (page = 0; page < (dc->pages - dc->pages_desc); page++)
    {
      int ret = rtems_nvdisk_write_checksum (nvd, dc->device, page, 0xffff);
      if (ret)
        return ret;
    }
  }

  return 0;
}

/**
 * NV disk IOCTL handler.
 *
 * @param dd Disk device.
 * @param req IOCTL request code.
 * @param argp IOCTL argument.
 * @retval The IOCTL return value
 */
static int
rtems_nvdisk_ioctl (rtems_disk_device *dd, uint32_t req, void* argp)
{
  dev_t                     dev = rtems_disk_get_device_identifier (dd);
  rtems_device_minor_number minor = rtems_filesystem_dev_minor_t (dev);
  rtems_blkdev_request*     r = argp;
  rtems_status_code         sc;

  if (minor >= rtems_nvdisk_count)
  {
    errno = ENODEV;
    return -1;
  }

  if (rtems_nvdisks[minor].device_count == 0)
  {
    errno = ENODEV;
    return -1;
  }

  errno = 0;

  sc = rtems_semaphore_obtain (rtems_nvdisks[minor].lock, RTEMS_WAIT, 0);
  if (sc != RTEMS_SUCCESSFUL)
    errno = EIO;
  else
  {
    errno = 0;
    switch (req)
    {
      case RTEMS_BLKIO_REQUEST:
        switch (r->req)
        {
          case RTEMS_BLKDEV_REQ_READ:
            errno = rtems_nvdisk_read (&rtems_nvdisks[minor], r);
            break;

          case RTEMS_BLKDEV_REQ_WRITE:
            errno = rtems_nvdisk_write (&rtems_nvdisks[minor], r);
            break;

          default:
            errno = EINVAL;
            break;
        }
        break;

      case RTEMS_NVDISK_IOCTL_ERASE_DISK:
        errno = rtems_nvdisk_erase_disk (&rtems_nvdisks[minor]);
        break;

      case RTEMS_NVDISK_IOCTL_INFO_LEVEL:
        rtems_nvdisks[minor].info_level = (uintptr_t) argp;
        break;

      default:
        rtems_blkdev_ioctl (dd, req, argp);
        break;
    }

    sc = rtems_semaphore_release (rtems_nvdisks[minor].lock);
    if (sc != RTEMS_SUCCESSFUL)
      errno = EIO;
  }

  return errno == 0 ? 0 : -1;
}

/**
 * NV disk device driver initialization.
 *
 * @todo Memory clean up on error is really badly handled.
 *
 * @param major NV disk major device number.
 * @param minor Minor device number, not applicable.
 * @param arg Initialization argument, not applicable.
 */
rtems_device_driver
rtems_nvdisk_initialize (rtems_device_major_number major,
                        rtems_device_minor_number minor,
                        void*                     arg __attribute__((unused)))
{
  const rtems_nvdisk_config* c = rtems_nvdisk_configuration;
  rtems_nvdisk*              nvd;
  rtems_status_code          sc;

  sc = rtems_disk_io_initialize ();
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  sc = rtems_nvdisk_crc16_gen_factors (0x8408);
  if (sc != RTEMS_SUCCESSFUL)
      return sc;

  rtems_nvdisks = calloc (rtems_nvdisk_configuration_size,
                          sizeof (rtems_nvdisk));

  if (!rtems_nvdisks)
    return RTEMS_NO_MEMORY;

  for (minor = 0; minor < rtems_nvdisk_configuration_size; minor++, c++)
  {
    char     name[] = RTEMS_NVDISK_DEVICE_BASE_NAME "a";
    dev_t    dev = rtems_filesystem_make_dev_t (major, minor);
    uint32_t device;
    uint32_t blocks = 0;

    nvd = &rtems_nvdisks[minor];

    name [sizeof(RTEMS_NVDISK_DEVICE_BASE_NAME)] += minor;

    nvd->major        = major;
    nvd->minor        = minor;
    nvd->flags        = c->flags;
    nvd->block_size   = c->block_size;
    nvd->info_level   = c->info_level;

    nvd->devices = calloc (c->device_count, sizeof (rtems_nvdisk_device_ctl));
    if (!nvd->devices)
      return RTEMS_NO_MEMORY;

    for (device = 0; device < c->device_count; device++)
    {
      rtems_nvdisk_device_ctl* dc = &nvd->devices[device];

      dc->device     = device;
      dc->pages      = rtems_nvdisk_pages_in_device (nvd, &c->devices[device]);
      dc->pages_desc = rtems_nvdisk_page_desc_pages (nvd, &c->devices[device]);
      dc->block_base = blocks;

      blocks += dc->pages - dc->pages_desc;

      dc->descriptor = &c->devices[device];
    }

    nvd->block_count  = blocks;
    nvd->device_count = c->device_count;

    sc = rtems_disk_create_phys(dev, c->block_size, blocks,
                                rtems_nvdisk_ioctl, NULL, name);
    if (sc != RTEMS_SUCCESSFUL)
    {
      rtems_nvdisk_error ("disk create phy failed");
      return sc;
    }

    sc = rtems_semaphore_create (rtems_build_name ('N', 'V', 'D', 'K'), 1,
                                 RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE |
                                 RTEMS_INHERIT_PRIORITY, 0, &nvd->lock);
    if (sc != RTEMS_SUCCESSFUL)
    {
      rtems_nvdisk_error ("disk lock create failed");
      return sc;
    }
  }

  rtems_nvdisk_count = rtems_nvdisk_configuration_size;

  return RTEMS_SUCCESSFUL;
}
