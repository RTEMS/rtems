/*
 * RTEMS Project (http://www.rtems.org/)
 *
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
 */
/**
 * Provide flash support for the AM26LV160 device.
 *
 * The M29W160D is the same device.
 */

#include <stdio.h>
#include <errno.h>

#include <rtems.h>

#include <libchip/am29lv160.h>

#ifndef AM26LV160_ERROR_TRACE
#define AM26LV160_ERROR_TRACE (0)
#endif

/**
 * Boot blocks at the top
 */
const rtems_fdisk_segment_desc rtems_am29lv160t_segments[4] =
{
  {
    .count =   31,
    .segment = 0,
    .offset =  0x00000000,
    .size =    RTEMS_FDISK_KBYTES (64)
  },
  {
    .count =   1,
    .segment = 31,
    .offset =  0x001f0000,
    .size =    RTEMS_FDISK_KBYTES (32)
  },
  {
    .count =   2,
    .segment = 32,
    .offset =  0x001f8000,
    .size =    RTEMS_FDISK_KBYTES (8)
  },
  {
    .count =   1,
    .segment = 34,
    .offset =  0x001fc000,
    .size =    RTEMS_FDISK_KBYTES (16)
  }
};

/**
 * Boot blocks at the bottom.
 */
const rtems_fdisk_segment_desc rtems_am29lv160b_segments[] =
{
  {
    .count =   1,
    .segment = 0,
    .offset =  0x00000000,
    .size =    RTEMS_FDISK_KBYTES (16)
  },
  {
  .  count =   2,
    .segment = 1,
    .offset =  0x00004000,
    .size =    RTEMS_FDISK_KBYTES (8)
  },
  {
    .count =   1,
    .segment = 3,
    .offset =  0x00008000,
    .size =    RTEMS_FDISK_KBYTES (32)
  },
  {
    .count =   31,
    .segment = 4,
    .offset =  0x00010000,
    .size =    RTEMS_FDISK_KBYTES (64)
  }
};

static int
rtems_am29lv160_blank (const rtems_fdisk_segment_desc* sd,
                       uint32_t                        device,
                       uint32_t                        segment,
                       uint32_t                        offset,
                       uint32_t                        size)
{
  const rtems_am29lv160_config* ac = &rtems_am29lv160_configuration[device];
  volatile uint8_t*             seg_8 = ac->base;
  volatile uint32_t*            seg_32;
  uint32_t                      count;

  offset += sd->offset + (segment - sd->segment) * sd->size;

  seg_8 += offset;

  count = offset & (sizeof (uint32_t) - 1);
  size -= count;

  while (count--)
    if (*seg_8++ != 0xff)
    {
#if AM26LV160_ERROR_TRACE
      printf ("AM26LV160: blank check error: %p = 0x%02x\n",
              seg_8 - 1, *(seg_8 - 1));
#endif
      return EIO;
    }

  seg_32 = (volatile uint32_t*) seg_8;

  count = size  / sizeof (uint32_t);
  size -= count * sizeof (uint32_t);

  while (count--)
    if (*seg_32++ != 0xffffffff)
    {
#if AM26LV160_ERROR_TRACE
      printf ("AM26LV160: blank check error: %p = 0x%08lx\n",
              seg_32 - 1, *(seg_32 - 1));
#endif
      return EIO;
    }

  seg_8 = (volatile uint8_t*) seg_32;

  while (size--)
    if (*seg_8++ != 0xff)
    {
#if AM26LV160_ERROR_TRACE
      printf ("AM26LV160: blank check error: %p = 0x%02x\n",
              seg_8 - 1, *(seg_8 - 1));
#endif
      return EIO;
    }

  return 0;
}

static int
rtems_am29lv160_verify (const rtems_fdisk_segment_desc* sd,
                        uint32_t                        device,
                        uint32_t                        segment,
                        uint32_t                        offset,
                        const void*                     buffer,
                        uint32_t                        size)
{
  const rtems_am29lv160_config* ac = &rtems_am29lv160_configuration[device];
  const uint8_t*                addr = ac->base;

  addr += (sd->offset + (segment - sd->segment) * sd->size) + offset;

  if (memcmp (addr, buffer, size) != 0)
    return EIO;

  return 0;
}

static int
rtems_am29lv160_toggle_wait_8 (volatile uint8_t* status)
{
  while (1)
  {
    volatile uint8_t status1 = *status;
    volatile uint8_t status2 = *status;

    if (((status1 ^ status2) & (1 << 6)) == 0)
      return 0;

    if ((status1 & (1 << 5)) != 0)
    {
      status1 = *status;
      status2 = *status;

      if (((status1 ^ status2) & (1 << 6)) == 0)
        return 0;

#if AM26LV160_ERROR_TRACE
      printf ("AM26LV160: error bit detected: %p = 0x%04x\n",
              status, status1);
#endif

      *status = 0xf0;
      return EIO;
    }
  }
}

static int
rtems_am29lv160_toggle_wait_16 (volatile uint16_t* status)
{
  while (1)
  {
    volatile uint16_t status1 = *status;
    volatile uint16_t status2 = *status;

    if (((status1 ^ status2) & (1 << 6)) == 0)
      return 0;

    if ((status1 & (1 << 5)) != 0)
    {
      status1 = *status;
      status2 = *status;

      if (((status1 ^ status2) & (1 << 6)) == 0)
        return 0;

#if AM26LV160_ERROR_TRACE
      printf ("AM26LV160: error bit detected: %p = 0x%04x/0x%04x\n",
              status, status1, status2);
#endif

      *status = 0xf0;
      return EIO;
    }
  }
}

static int
rtems_am29lv160_write_data_8 (volatile uint8_t* base,
                              uint32_t          offset,
                              const uint8_t*    data,
                              uint32_t          size)
{
  volatile uint8_t*     seg = base + offset;
  rtems_interrupt_level level;

  /*
   * Issue a reset.
   */
  *base = 0xf0;

  while (size)
  {
    rtems_interrupt_disable (level);
    *(base + 0xaaa) = 0xaa;
    *(base + 0x555) = 0x55;
    *(base + 0xaaa) = 0xa0;
    *seg = *data++;
    rtems_interrupt_enable (level);
    if (rtems_am29lv160_toggle_wait_8 (seg++) != 0)
      return EIO;
    size--;
  }

  /*
   * Issue a reset.
   */
  *base = 0xf0;

  return 0;
}

static int
rtems_am29lv160_write_data_16 (volatile uint16_t* base,
                               uint32_t           offset,
                               const uint16_t*    data,
                               uint32_t           size)
{
  volatile uint16_t*    seg = base + (offset / 2);
  rtems_interrupt_level level;

  size /= 2;

  /*
   * Issue a reset.
   */
  *base = 0xf0;

  while (size)
  {
    rtems_interrupt_disable (level);
    *(base + 0x555) = 0xaa;
    *(base + 0x2aa) = 0x55;
    *(base + 0x555) = 0xa0;
    *seg = *data++;
    rtems_interrupt_enable (level);
    if (rtems_am29lv160_toggle_wait_16 (seg++) != 0)
      return EIO;
    size--;
  }

  /*
   * Issue a reset.
   */
  *base = 0xf0;

  return 0;
}

static int
rtems_am29lv160_read (const rtems_fdisk_segment_desc* sd,
                      uint32_t                        device,
                      uint32_t                        segment,
                      uint32_t                        offset,
                      void*                           buffer,
                      uint32_t                        size)
{
  unsigned char* addr =
    rtems_am29lv160_configuration[device].base +
    sd->offset + ((segment - sd->segment) * sd->size) + offset;
  memcpy (buffer, addr, size);
  return 0;
}

/*
 * @todo Fix the odd alignment and odd sizes.
 */
static int
rtems_am29lv160_write (const rtems_fdisk_segment_desc* sd,
                       uint32_t                        device,
                       uint32_t                        segment,
                       uint32_t                        offset,
                       const void*                     buffer,
                       uint32_t                        size)
{
  int ret = rtems_am29lv160_verify (sd, device, segment, offset, buffer, size);

  if (ret != 0)
  {
    const rtems_am29lv160_config* ac = &rtems_am29lv160_configuration[device];
    uint32_t                      soffset;

    soffset = offset + sd->offset + ((segment - sd->segment) * sd->size);

    if (offset & 1)
      printf ("rtems_am29lv160_write: offset is odd\n");

    if (size & 1)
      printf ("rtems_am29lv160_write: size is odd\n");

    if (ac->bus_8bit)
      ret = rtems_am29lv160_write_data_8 (ac->base, soffset, buffer, size);
    else
      ret = rtems_am29lv160_write_data_16 (ac->base, soffset, buffer, size);

    /*
     * Verify the write worked.
     */
    if (ret == 0)
    {
      ret = rtems_am29lv160_verify (sd, device, segment, offset, buffer, size);
#if AM26LV160_ERROR_TRACE
      if (ret)
        printf ("AM26LV160: verify failed: %ld-%ld-%08lx: s=%ld\n",
                device, segment, offset, size);
#endif
    }
  }

  return ret;
}

static int
rtems_am29lv160_erase (const rtems_fdisk_segment_desc* sd,
                       uint32_t                        device,
                       uint32_t                        segment)
{
  int ret = rtems_am29lv160_blank (sd, device, segment, 0, sd->size);
  if (ret != 0)
  {
    const rtems_am29lv160_config* ac = &rtems_am29lv160_configuration[device];
    uint32_t                      offset;
    rtems_interrupt_level         level;

    offset = sd->offset + ((segment - sd->segment) * sd->size);

    if (ac->bus_8bit)
    {
      volatile uint8_t* base = ac->base;
      volatile uint8_t* seg  = base + offset;

      /*
       * Issue a reset.
       */
      rtems_interrupt_disable (level);
      *base = 0xf0;
      *(base + 0xaaa) = 0xaa;
      *(base + 0x555) = 0x55;
      *(base + 0xaaa) = 0x80;
      *(base + 0xaaa) = 0xaa;
      *(base + 0x555) = 0x55;
      *seg = 0x30;
      rtems_interrupt_enable (level);

      ret = rtems_am29lv160_toggle_wait_8 (seg);

      /*
       * Issue a reset.
       */
      *base = 0xf0;
    }
    else
    {
      volatile uint16_t* base = ac->base;
      volatile uint16_t* seg  = base + (offset / 2);

      /*
       * Issue a reset.
       */
      rtems_interrupt_disable (level);
      *base = 0xf0;
      *(base + 0x555) = 0xaa;
      *(base + 0x2aa) = 0x55;
      *(base + 0x555) = 0x80;
      *(base + 0x555) = 0xaa;
      *(base + 0x2aa) = 0x55;
      *seg = 0x30;
      rtems_interrupt_enable (level);

      ret = rtems_am29lv160_toggle_wait_16 (seg);

      /*
       * Issue a reset.
       */
      *base = 0xf0;
    }

    /*
     * Check the erase worked.
     */
    if (ret == 0)
    {
      ret = rtems_am29lv160_blank (sd, device, segment, 0, sd->size);
#if AM26LV160_ERROR_TRACE
      if (ret)
        printf ("AM26LV160: erase failed: %ld-%ld\n", device, segment);
#endif
    }
  }

  return ret;
}

static int
rtems_am29lv160_erase_device (const rtems_fdisk_device_desc* dd,
                              uint32_t                       device)
{
  uint32_t segment;

  for (segment = 0; segment < dd->segment_count; segment++)
  {
    uint32_t seg_segment;

    for (seg_segment = 0;
         seg_segment < dd->segments[segment].count;
         seg_segment++)
    {
      int ret = rtems_am29lv160_erase (&dd->segments[segment],
                                       device,
                                       segment + seg_segment);
      if (ret)
        return ret;
    }
  }

  return 0;
}

const rtems_fdisk_driver_handlers rtems_am29lv160_handlers =
{
  .read =         rtems_am29lv160_read,
  .write =        rtems_am29lv160_write,
  .blank =        rtems_am29lv160_blank,
  .verify =       rtems_am29lv160_verify,
  .erase =        rtems_am29lv160_erase,
  .erase_device = rtems_am29lv160_erase_device
};
