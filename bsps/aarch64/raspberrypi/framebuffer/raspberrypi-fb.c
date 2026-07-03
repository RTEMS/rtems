/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief Raspberry Pi 4 Framebuffer Driver
 */

/*
 * Copyright (C) 2026 Shaunak Datar
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
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SUBSTITUTE, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/framebuffer.h>
#include <rtems/libio.h>
#include <bsp/aarch64-mmu.h>
#include <bsp/mailbox.h>
#include <rtems/fb.h>
#include <rtems/rtems/cache.h>
#include <rtems/thread.h>


#define RPI_FB_TAG_COUNT 5
#define RPI_FB_ALIGNMENT 16
#define VC2ARM_MASK 0xC0000000
#define RPI_FB_MSG_BUFFER_SIZE 128
#define RPI_FB_MSG_ALIGNMENT 64

static rtems_mutex fb_mutex = RTEMS_MUTEX_INITIALIZER("FrameBuffer Lock");
static bool fb_open = false;
static uint8_t fb_msg_buffer[RPI_FB_MSG_BUFFER_SIZE]
  __attribute__((aligned(RPI_FB_MSG_ALIGNMENT)));

typedef struct {
  struct fb_var_screeninfo var;
  struct fb_fix_screeninfo fix;

  bool initialised;
} rpi_fb_device;

static rpi_fb_device rpi_fb;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t pixel_order;
} rpi_fb_display_config;

typedef struct {
  mbox_property_tag *phys;
  mbox_property_tag *virt;
  mbox_property_tag *depth;
  mbox_property_tag *alloc;
  mbox_property_tag *pitch;
} rpi_fb_tags;

static rtems_status_code rpi_fb_init(
  const rpi_fb_display_config *cfg
)
{
  static const mbox_property_tag_metadata fb_tags[RPI_FB_TAG_COUNT] = {
    { BCM2711_TAG_SET_PHYSICAL_WIDTH_HEIGHT, 8 },
    { BCM2711_TAG_SET_VIRTUAL_WIDTH_HEIGHT,  8 },
    { BCM2711_TAG_SET_DEPTH,                 4 },
    { BCM2711_TAG_ALLOCATE_BUFFER,           8 },
    { BCM2711_TAG_GET_PITCH,                 4 },
  };

  rpi_fb_tags tag;
  uint32_t fb_bus;
  uint32_t fb_size;
  uint32_t pitch;
  uintptr_t fb_phys;
  mbox_property_tag *current;

  mbox_property_message *msg =
    (mbox_property_message *)fb_msg_buffer;

  rtems_status_code sc;

  sc = rpi_mbox_property_buffer_prepare(
    fb_msg_buffer,
    sizeof(fb_msg_buffer)
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = rpi_mbox_property_message_init(
    msg,
    sizeof(fb_msg_buffer),
    fb_tags,
    RPI_FB_TAG_COUNT
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Resolve tag pointers by walking the initialised message once. */
  current =
    (mbox_property_tag *)((uint8_t *)msg + sizeof(mbox_property_header));

  tag.phys = current;
  current = rpi_get_next_tag(current);

  tag.virt = current;
  current = rpi_get_next_tag(current);

  tag.depth = current;
  current = rpi_get_next_tag(current);

  tag.alloc = current;
  current = rpi_get_next_tag(current);

  tag.pitch = current;

  tag.phys->value_buffer[0] = cfg->width;
  tag.phys->value_buffer[1] = cfg->height;

  tag.virt->value_buffer[0] = cfg->width;
  tag.virt->value_buffer[1] = cfg->height;

  tag.depth->value_buffer[0] = cfg->depth;

  /* requested alignment in bytes */
  tag.alloc->value_buffer[0] = RPI_FB_ALIGNMENT;
  tag.alloc->value_buffer[1] = 0;

  sc = rpi_mbox_process(msg);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  fb_bus  = tag.alloc->value_buffer[0];
  fb_size = tag.alloc->value_buffer[1];
  pitch   = tag.pitch->value_buffer[0];

  if (fb_bus == 0 || fb_size == 0 || pitch == 0) {
    return RTEMS_IO_ERROR;
  }

  /* Convert the VideoCore address to ARM address */
  fb_phys = fb_bus & ~VC2ARM_MASK;

  rpi_fb.var.xres           = cfg->width;
  rpi_fb.var.yres           = cfg->height;
  rpi_fb.var.bits_per_pixel = cfg->depth;

  rpi_fb.fix.line_length = pitch;
  rpi_fb.fix.smem_start  = (volatile char *)(uintptr_t)fb_phys;
  rpi_fb.fix.smem_len    = fb_size;

  /*
   * The framebuffer lives in the GPU memory carve-out above RAM_TOP, which
   * the boot MMU tables do not cover, so it must be mapped here once its
   * address is known.
   */
  sc = aarch64_mmu_map(
    (uintptr_t)rpi_fb.fix.smem_start,
    rpi_fb.fix.smem_len,
    AARCH64_MMU_DATA_RW_CACHED
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  rpi_fb.initialised = true;
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code rpi_fb_get_display_config(
  rpi_fb_display_config *cfg
)
{
  static const mbox_property_tag_metadata tags[] = {
    { BCM2711_TAG_GET_PHYSICAL_WIDTH_HEIGHT, 8 },
    { BCM2711_TAG_GET_DEPTH,                 4 },
    { BCM2711_TAG_GET_PIXEL_ORDER,           4 },
  };

  mbox_property_message *msg =
    (mbox_property_message *)fb_msg_buffer;

  mbox_property_tag *current;
  mbox_property_tag *phys;
  mbox_property_tag *depth;
  mbox_property_tag *pixel;

  rtems_status_code sc;

  sc = rpi_mbox_property_buffer_prepare(
    fb_msg_buffer,
    sizeof(fb_msg_buffer)
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = rpi_mbox_property_message_init(
    msg,
    sizeof(fb_msg_buffer),
    tags,
    RTEMS_ARRAY_SIZE(tags)
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = rpi_mbox_process(msg);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  current = (mbox_property_tag *)
    ((uint8_t *)msg + sizeof(mbox_property_header));

  phys = current;
  current = rpi_get_next_tag(current);

  depth = current;
  current = rpi_get_next_tag(current);

  pixel = current;

  cfg->width  = phys->value_buffer[0];
  cfg->height = phys->value_buffer[1];
  cfg->depth  = depth->value_buffer[0];
  cfg->pixel_order = pixel->value_buffer[0];

  if (cfg->width == 0 || cfg->height == 0) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) minor;
  (void) arg;

  rtems_status_code sc;

  sc = rtems_io_register_name(FRAMEBUFFER_DEVICE_0_NAME, major, 0);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(sc);
  }

  memset(&rpi_fb, 0, sizeof(rpi_fb));
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  rtems_status_code sc;
  rpi_fb_display_config cfg;

  rtems_mutex_lock(&fb_mutex);
  if (fb_open) {
    rtems_mutex_unlock(&fb_mutex);
    return RTEMS_RESOURCE_IN_USE;
  }

  if (!rpi_fb.initialised) {
    sc = rpi_fb_get_display_config(&cfg);
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_mutex_unlock(&fb_mutex);
      return sc;
    }
    sc = rpi_fb_init(&cfg);
    if (sc != RTEMS_SUCCESSFUL) {
      rtems_mutex_unlock(&fb_mutex);
      return sc;
    }
  }
  fb_open = true;
  rtems_mutex_unlock(&fb_mutex);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  rtems_mutex_lock(&fb_mutex);
  fb_open = false;
  rtems_mutex_unlock(&fb_mutex);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;

  rtems_libio_rw_args_t *rw = (rtems_libio_rw_args_t *)arg;

  if (rw->offset >= rpi_fb.fix.smem_len) {
    rw->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  rw->bytes_moved =
    ((rw->offset + rw->count) > rpi_fb.fix.smem_len)
    ? (rpi_fb.fix.smem_len - rw->offset)
    : rw->count;

  memcpy(
    rw->buffer,
    (const void *)(rpi_fb.fix.smem_start + rw->offset),
    rw->bytes_moved
  );
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;

  rtems_libio_rw_args_t *rw = (rtems_libio_rw_args_t *)arg;

  if (rw->offset >= rpi_fb.fix.smem_len) {
    rw->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  rw->bytes_moved =
    ((rw->offset + rw->count) > rpi_fb.fix.smem_len)
    ? (rpi_fb.fix.smem_len - rw->offset)
    : rw->count;

  memcpy(
    (void *)(rpi_fb.fix.smem_start + rw->offset),
    rw->buffer,
    rw->bytes_moved
  );

  rtems_cache_flush_multiple_data_lines(
    (void *)(rpi_fb.fix.smem_start + rw->offset),
    rw->bytes_moved
  );

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;

  rtems_libio_ioctl_args_t *ioctl = (rtems_libio_ioctl_args_t *)arg;

  switch (ioctl->command) {
    case FBIOGET_VSCREENINFO:
      memcpy(ioctl->buffer, &rpi_fb.var, sizeof(rpi_fb.var));
      ioctl->ioctl_return = 0;
      break;
    case FBIOGET_FSCREENINFO:
      memcpy(ioctl->buffer, &rpi_fb.fix, sizeof(rpi_fb.fix));
      ioctl->ioctl_return = 0;
      break;
    case FBIOGETCMAP:
    case FBIOPUTCMAP:
      ioctl->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
    default:
      ioctl->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}