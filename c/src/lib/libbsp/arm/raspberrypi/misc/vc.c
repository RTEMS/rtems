/**
 * @file
 *
 * @ingroup raspberrypi_vc
 *
 * @brief video core support.
 *
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */
#include <pthread.h>

#include <bsp.h>
#include <bsp/raspberrypi.h>
#include <bsp/mailbox.h>
#include <bsp/vc.h>
#include <libcpu/arm-cp15.h>
#include "vc_defines.h"

#if (RPI_L2_CACHE_ENABLE == 1)
  #define BCM2835_VC_MEMORY_MAPPING 0x40000000
#else
  #define BCM2835_VC_MEMORY_MAPPING 0xC0000000
#endif

static inline bool
bcm2835_mailbox_buffer_suceeded(const bcm2835_mbox_buf_hdr *hdr)
{
  return (hdr->buf_code == BCM2835_MBOX_BUF_CODE_REQUEST_SUCCEED);
}

static inline int
bcm2835_mailbox_send_read_buffer(void *buf)
{
  raspberrypi_mailbox_write(BCM2835_MBOX_CHANNEL_PROP_AVC,
    (unsigned int)buf + BCM2835_VC_MEMORY_MAPPING);
  raspberrypi_mailbox_read(BCM2835_MBOX_CHANNEL_PROP_AVC);
  return 0;
}

/*
 * When cache is enabled then content of buffer exchanged
 * with VideoCore has to be propagated through ARM11/Cortex-A7
 * caches
 */
static inline void
bcm2835_mailbox_buffer_flush_and_invalidate(void *buf, size_t size)
{
  uint32_t sctlr_val;

  sctlr_val = arm_cp15_get_control();

  arm_cp15_drain_write_buffer();
  if (sctlr_val & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M)) {
    arm_cp15_drain_write_buffer();
    rtems_cache_flush_multiple_data_lines(buf, size);
    rtems_cache_invalidate_multiple_data_lines(buf, size);
  }
}

#define BCM2835_MBOX_VAL_LENGTH_MASK(_val_len) \
  (_val_len&(~BCM2835_MBOX_TAG_VAL_LEN_RESPONSE))

int
bcm2835_mailbox_get_display_size(bcm2835_get_display_size_entries *_entries)
{
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_display_size get_display_size;
    uint32_t end_tag;
  }buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG_NO_REQ(&buffer.get_display_size,
    BCM2835_MAILBOX_TAG_GET_DISPLAY_SIZE);
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  _entries->width = buffer.get_display_size.body.resp.width;
  _entries->height = buffer.get_display_size.body.resp.height;
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}

int
bcm2835_mailbox_init_frame_buffer(bcm2835_init_frame_buffer_entries* _entries)
{
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_display_size set_display_size;
    bcm2835_mbox_tag_virtual_size set_virtual_size;
    bcm2835_mbox_tag_depth set_depth;
    bcm2835_mbox_tag_pixel_order set_pixel_order;
    bcm2835_mbox_tag_alpha_mode set_alpha_mode;
    bcm2835_mbox_tag_virtual_offset set_virtual_offset;
    bcm2835_mbox_tag_overscan set_overscan;
    bcm2835_mbox_tag_allocate_buffer allocate_buffer;
    bcm2835_mbox_tag_get_pitch get_pitch;
    uint32_t end_tag;
  } buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG(&buffer.set_display_size,
    BCM2835_MAILBOX_TAG_SET_DISPLAY_SIZE);
  buffer.set_display_size.body.req.width = _entries->xres;
  buffer.set_display_size.body.req.height = _entries->yres;
  BCM2835_MBOX_INIT_TAG(&buffer.set_virtual_size,
    BCM2835_MAILBOX_TAG_SET_VIRTUAL_SIZE);
  buffer.set_virtual_size.body.req.vwidth = _entries->xvirt;
  buffer.set_virtual_size.body.req.vheight = _entries->yvirt;
  BCM2835_MBOX_INIT_TAG(&buffer.set_depth,
    BCM2835_MAILBOX_TAG_SET_DEPTH);
  buffer.set_depth.body.req.depth = _entries->depth;
  BCM2835_MBOX_INIT_TAG(&buffer.set_pixel_order,
    BCM2835_MAILBOX_TAG_SET_PIXEL_ORDER);
  buffer.set_pixel_order.body.req.pixel_order = _entries->pixel_order;
  BCM2835_MBOX_INIT_TAG(&buffer.set_alpha_mode,
    BCM2835_MAILBOX_TAG_SET_ALPHA_MODE);
  buffer.set_alpha_mode.body.req.alpha_mode = _entries->alpha_mode;
  BCM2835_MBOX_INIT_TAG(&buffer.set_virtual_offset,
    BCM2835_MAILBOX_TAG_SET_VIRTUAL_OFFSET);
  buffer.set_virtual_offset.body.req.voffset_x = _entries->voffset_x;
  buffer.set_virtual_offset.body.req.voffset_x = _entries->voffset_y;
  BCM2835_MBOX_INIT_TAG(&buffer.set_overscan,
    BCM2835_MAILBOX_TAG_SET_OVERSCAN);
  buffer.set_overscan.body.req.overscan_top = _entries->overscan_top;
  buffer.set_overscan.body.req.overscan_bottom = _entries->overscan_bottom;
  buffer.set_overscan.body.req.overscan_left = _entries->overscan_left;
  buffer.set_overscan.body.req.overscan_right = _entries->overscan_right;
  BCM2835_MBOX_INIT_TAG(&buffer.allocate_buffer,
    BCM2835_MAILBOX_TAG_ALLOCATE_BUFFER);
  buffer.allocate_buffer.body.req.align = 0x100;
  BCM2835_MBOX_INIT_TAG_NO_REQ(&buffer.get_pitch,
    BCM2835_MAILBOX_TAG_GET_PITCH);
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  _entries->xres = buffer.set_display_size.body.resp.width;
  _entries->yres = buffer.set_display_size.body.resp.height;
  _entries->xvirt = buffer.set_virtual_size.body.resp.vwidth;
  _entries->yvirt = buffer.set_virtual_size.body.resp.vheight;
  _entries->depth = buffer.set_depth.body.resp.depth;
#if (BSP_IS_RPI2 == 1)
  _entries->base = buffer.allocate_buffer.body.resp.base;
#else
  _entries->base = buffer.allocate_buffer.body.resp.base
                    - BCM2835_VC_MEMORY_MAPPING;
#endif
  _entries->size = buffer.allocate_buffer.body.resp.size;
  _entries->pixel_order = buffer.set_pixel_order.body.resp.pixel_order;
  _entries->alpha_mode = buffer.set_alpha_mode.body.resp.alpha_mode;
  _entries->voffset_x = buffer.set_virtual_offset.body.resp.voffset_x;
  _entries->voffset_y = buffer.set_virtual_offset.body.resp.voffset_y;
  _entries->overscan_left = buffer.set_overscan.body.resp.overscan_left;
  _entries->overscan_right = buffer.set_overscan.body.resp.overscan_right;
  _entries->overscan_top = buffer.set_overscan.body.resp.overscan_top;
  _entries->overscan_bottom = buffer.set_overscan.body.resp.overscan_bottom;
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}

int
bcm2835_mailbox_get_pitch(bcm2835_get_pitch_entries* _entries)
{
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_get_pitch get_pitch;
    uint32_t end_tag;
  }buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG_NO_REQ(&buffer.get_pitch,
    BCM2835_MAILBOX_TAG_GET_PITCH);
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  _entries->pitch = buffer.get_pitch.body.resp.pitch;
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}

int
bcm2835_mailbox_get_cmdline(bcm2835_get_cmdline_entries* _entries)
{
  int i;
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_get_cmd_line get_cmd_line;
    uint32_t end_tag;
  }buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG_NO_REQ(&buffer.get_cmd_line,
    BCM2835_MAILBOX_TAG_GET_CMD_LINE);
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  for (i = 0;
        i < BCM2835_MBOX_VAL_LENGTH_MASK(buffer.get_cmd_line.tag_hdr.val_len);
        ++i)
  {
    _entries->cmdline[i] = buffer.get_cmd_line.body.resp.cmdline[i];
  }
  _entries->cmdline[i] = '\0';
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}

int
bcm2835_mailbox_set_power_state(bcm2835_set_power_state_entries *_entries)
{
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_power_state set_power_state;
    uint32_t end_tag;
  }buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG(&buffer.set_power_state,
    BCM2835_MAILBOX_TAG_SET_POWER_STATE);
  buffer.set_power_state.body.req.dev_id = _entries->dev_id;
  buffer.set_power_state.body.req.state = _entries->state;
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  _entries->dev_id = buffer.set_power_state.body.resp.dev_id;
  _entries->state  = buffer.set_power_state.body.resp.state;
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}

int
bcm2835_mailbox_get_arm_memory(bcm2835_get_arm_memory_entries* _entries)
{
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_get_arm_memory get_arm_memory;
    uint32_t end_tag;
  }buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG_NO_REQ(&buffer.get_arm_memory,
    BCM2835_MAILBOX_TAG_GET_ARM_MEMORY);
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  _entries->base = buffer.get_arm_memory.body.resp.base;
  _entries->size = buffer.get_arm_memory.body.resp.size;
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}

int
bcm2835_mailbox_get_vc_memory(bcm2835_get_vc_memory_entries* _entries)
{
  struct {
    bcm2835_mbox_buf_hdr hdr;
    bcm2835_mbox_tag_get_vc_memory get_vc_memory;
    uint32_t end_tag;
  }buffer BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE;
  BCM2835_MBOX_INIT_BUF(&buffer);
  BCM2835_MBOX_INIT_TAG_NO_REQ(&buffer.get_vc_memory,
    BCM2835_MAILBOX_TAG_GET_VC_MEMORY);
  bcm2835_mailbox_buffer_flush_and_invalidate(&buffer, sizeof(&buffer));
  if (bcm2835_mailbox_send_read_buffer(&buffer))
    return -1;
  _entries->base = buffer.get_vc_memory.body.resp.base;
  _entries->size = buffer.get_vc_memory.body.resp.size;
  if( !bcm2835_mailbox_buffer_suceeded(&buffer.hdr) )
    return -2;
  return 0;
}
