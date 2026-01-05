/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief Mailbox Driver
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
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "bsp/mailbox.h"

#define MBOX_MAIL_CHANNEL_WIDTH 4
#define MBOX_MAIL_CHANNEL_MASK 0xF
#define MBOX_MAIL_DATA_MASK 0xFFFFFFF0
#define MBOX_REQUEST 0x00000000
#define MBOX_BUFFER_ALIGNMENT 16
#define MBOX_VALUE_ALIGNMENT 4

static inline size_t mbox_tag_total_size(uint32_t value_size) {
  size_t aligned_value =
      (value_size + (MBOX_VALUE_ALIGNMENT - 1)) & ~(MBOX_VALUE_ALIGNMENT - 1);

  return sizeof(mbox_property_tag) + aligned_value;
}

rtems_status_code rpi_mbox_property_buffer_prepare(void *buffer, size_t size) {
  if (((uintptr_t)buffer & (MBOX_BUFFER_ALIGNMENT - 1)) != 0) {
    return RTEMS_INVALID_ADDRESS;
  }

  memset(buffer, 0, size);
  return RTEMS_SUCCESSFUL;
}

void rpi_mbox_write(bcm2711_mbox_channel channel, uint32_t data) {
  while (BCM2835_REG(BCM2711_MBOX_STATUS) & BCM2711_MBOX_FULL) {
  }

  BCM2835_REG(BCM2711_MBOX_WRITE) =
      (data & MBOX_MAIL_DATA_MASK) | (channel & MBOX_MAIL_CHANNEL_MASK);
}

uint32_t rpi_mbox_read(bcm2711_mbox_channel channel) {
    uint32_t data;
    while (BCM2835_REG(BCM2711_MBOX_STATUS) & BCM2711_MBOX_EMPTY) {
    }
    data = BCM2835_REG(BCM2711_MBOX_READ);

    if ((data & MBOX_MAIL_CHANNEL_MASK) == channel) {
      return data & MBOX_MAIL_DATA_MASK;
    }
    return 0;
}

rtems_status_code rpi_mbox_process(mbox_property_message *msg) {
  rtems_cache_flush_multiple_data_lines(msg, msg->header.size);
  rpi_mbox_write(BCM2711_MBOX_CHANNEL_PROP_AVC, (uint32_t)(uintptr_t)msg);
  rpi_mbox_read(BCM2711_MBOX_CHANNEL_PROP_AVC);
  rtems_cache_invalidate_multiple_data_lines(msg, msg->header.size);
  return (msg->header.req_resp == BCM2711_MBOX_RESPONSE) ? RTEMS_SUCCESSFUL
                                                         : RTEMS_IO_ERROR;
}

rtems_status_code
rpi_mbox_property_message_init(mbox_property_message *msg, size_t buffer_size,
                               const mbox_property_tag_metadata *tags,
                               unsigned int tag_count) {
  uintptr_t head = (uintptr_t)msg->buffer;
  uintptr_t end = (uintptr_t)(msg) + buffer_size;

  for (unsigned int i = 0; i < tag_count; i++) {
    if (head + mbox_tag_total_size(tags[i].value_size) > end) {
      return RTEMS_INVALID_SIZE;
    }
    mbox_property_tag *tag = (mbox_property_tag *)head;
    tag->id = tags[i].tag_id;
    tag->buffer_size = tags[i].value_size;
    tag->val_len = 0;
    for (uint32_t j = 0; j < tags[i].value_size / sizeof(tag->value_buffer[0]);
         j++) {
      tag->value_buffer[j] = 0;
    }

    head += mbox_tag_total_size(tags[i].value_size);
  }
  if (head + sizeof(uint32_t) > end) {
    return RTEMS_INVALID_SIZE;
  }
  *(uint32_t *)head = BCM2711_TAG_END;
  head += sizeof(uint32_t);

  msg->header.size = head - (uintptr_t)msg;
  msg->header.req_resp = MBOX_REQUEST;
  return RTEMS_SUCCESSFUL;
}