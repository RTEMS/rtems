/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief Raspberry Pi specific Mailbox Definitions
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

#ifndef LIBBSP_AARCH64_RASPBERRYPI_4_MBOX_H
#define LIBBSP_AARCH64_RASPBERRYPI_4_MBOX_H

#include "bsp/raspberrypi.h"
#include <rtems/rtems/cache.h>
#include <rtems/rtems/status.h>
#include <rtems/score/assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name VideoCore Power Device IDs
 * @brief Device identifiers for power management operations via mailbox
 * interface
 * @{
 */
#define RPI_POWER_DEVICE_SD_CARD 0x00000000  /**< SD Card */
#define RPI_POWER_DEVICE_UART0 0x00000001    /**< UART0 */
#define RPI_POWER_DEVICE_UART1 0x00000002    /**< UART1 */
#define RPI_POWER_DEVICE_USB_HCD 0x00000003  /**< USB Host Controller */
#define RPI_POWER_DEVICE_I2C0 0x00000004     /**< I2C0 */
#define RPI_POWER_DEVICE_I2C1 0x00000005     /**< I2C1 */
#define RPI_POWER_DEVICE_I2C2 0x00000006     /**< I2C2 */
#define RPI_POWER_DEVICE_SPI 0x00000007      /**< SPI */
#define RPI_POWER_DEVICE_CCP2TX 0x00000008   /**< CCP2TX */
#define RPI_POWER_DEVICE_UNKNOWN1 0x00000009 /**< Unknown device (RPi4) */
#define RPI_POWER_DEVICE_UNKNOWN2 0x0000000a /**< Unknown device (RPi4) */
/** @} */

/**
 * @name VideoCore Clock IDs
 * @brief Clock identifiers for clock management operations via mailbox
 * interface
 * @{
 */
#define RPI_CLOCK_RESERVED 0x00000000  /**< Reserved */
#define RPI_CLOCK_EMMC 0x00000001      /**< EMMC clock */
#define RPI_CLOCK_UART 0x00000002      /**< UART clock */
#define RPI_CLOCK_ARM 0x00000003       /**< ARM core clock */
#define RPI_CLOCK_CORE 0x00000004      /**< VideoCore clock */
#define RPI_CLOCK_V3D 0x00000005       /**< V3D (GPU) clock */
#define RPI_CLOCK_H264 0x00000006      /**< H264 encoder clock */
#define RPI_CLOCK_ISP 0x00000007       /**< Image Sensor Pipeline clock */
#define RPI_CLOCK_SDRAM 0x00000008     /**< SDRAM clock */
#define RPI_CLOCK_PIXEL 0x00000009     /**< Pixel clock */
#define RPI_CLOCK_PWM 0x0000000a       /**< PWM clock */
#define RPI_CLOCK_HEVC 0x0000000b      /**< HEVC encoder clock */
#define RPI_CLOCK_EMMC2 0x0000000c     /**< EMMC2 clock */
#define RPI_CLOCK_M2MC 0x0000000d      /**< M2MC clock */
#define RPI_CLOCK_PIXEL_BVB 0x0000000e /**< Pixel BVB clock */
/** @} */

/**
 * @brief Mailbox Channel IDs
 *
 * Mailbox channels used for communication between ARM and VideoCore.
 * Each channel serves a specific purpose in the communication protocol.
 */
typedef enum {
  BCM2711_MBOX_CHANNEL_PM = 0,        /**< Power management */
  BCM2711_MBOX_CHANNEL_FB = 1,  /**< Framebuffer */
  BCM2711_MBOX_CHANNEL_VUART = 2, /**< Virtual UART */
  BCM2711_MBOX_CHANNEL_VCHIQ = 3,        /**< VCHIQ */
  BCM2711_MBOX_CHANNEL_LED = 4,         /**< LEDs */
  BCM2711_MBOX_CHANNEL_BUTTON = 5,      /**< Buttons */
  BCM2711_MBOX_CHANNEL_TOUCHS = 6,  /**< Touch screen */
  BCM2711_MBOX_CHANNEL_COUNT = 7,     /**< Reserved */
  BCM2711_MBOX_CHANNEL_PROP_AVC = 8,  /**< Property tags (ARM -> VC) */
  BCM2711_MBOX_CHANNEL_PROP_VCA = 9   /**< Property tags (VC -> ARM) */
} bcm2711_mbox_channel;

/**
 * @brief Mailbox Property Tag IDs
 */
typedef enum {
  BCM2711_TAG_GET_FIRMWARE_REV = 0x00000001,   /**< Get firmware revision */
  BCM2711_TAG_GET_BOARD_MODEL = 0x00010001,    /**< Get board model */
  BCM2711_TAG_GET_BOARD_REVISION = 0x00010002, /**< Get board revision */
  BCM2711_TAG_GET_BOARD_MAC = 0x00010003,      /**< Get board MAC address */
  BCM2711_TAG_GET_BOARD_SERIAL = 0x00010004,   /**< Get board serial number */
  BCM2711_TAG_GET_ARM_MEMORY = 0x00010005, /**< Get ARM memory base and size */
  BCM2711_TAG_GET_VC_MEMORY =
      0x00010006, /**< Get VideoCore memory base and size */
  BCM2711_TAG_GET_CLOCKS = 0x00010007, /**< Get available clocks */

  BCM2711_TAG_GET_COMMAND_LINE = 0x00050001, /**< Get boot command line */

  BCM2711_TAG_GET_DMA_CHANNELS = 0x00060001, /**< Get usable DMA channels */

  BCM2711_TAG_GET_POWER_STATE = 0x00020001, /**< Get power state of a device */
  BCM2711_TAG_GET_TIMING = 0x00020002,      /**< Get power timing information */
  BCM2711_TAG_SET_POWER_STATE = 0x00028001, /**< Set power state of a device */

  BCM2711_TAG_GET_CLOCK_STATE = 0x00030001,    /**< Get clock state */
  BCM2711_TAG_SET_CLOCK_STATE = 0x00038001,    /**< Set clock state */
  BCM2711_TAG_GET_CLOCK_RATE = 0x00030002,     /**< Get clock rate */
  BCM2711_TAG_SET_CLOCK_RATE = 0x00038002,     /**< Set clock rate */
  BCM2711_TAG_GET_MAX_CLOCK_RATE = 0x00030004, /**< Get maximum clock rate */
  BCM2711_TAG_GET_MIN_CLOCK_RATE = 0x00030007, /**< Get minimum clock rate */
  BCM2711_TAG_GET_TURBO = 0x00030009,          /**< Get turbo state */
  BCM2711_TAG_SET_TURBO = 0x00038009,          /**< Set turbo state */
  BCM2711_TAG_GET_CLOCK_RATE_MEASURED =
      0x00030047, /**< Get measured clock rate */

  BCM2711_TAG_GET_VOLTAGE = 0x00030003,     /**< Get voltage */
  BCM2711_TAG_SET_VOLTAGE = 0x00038003,     /**< Set voltage */
  BCM2711_TAG_GET_MAX_VOLTAGE = 0x00030005, /**< Get maximum voltage */
  BCM2711_TAG_GET_MIN_VOLTAGE = 0x00030008, /**< Get minimum voltage */
  BCM2711_TAG_GET_TEMPERATURE = 0x00030006, /**< Get temperature */
  BCM2711_TAG_GET_MAX_TEMPERATURE =
      0x0003000a, /**< Get maximum safe temperature */

  BCM2711_TAG_GET_LED_STATE = 0x00030041,  /**< Get LED state */
  BCM2711_TAG_TEST_LED_STATE = 0x00034041, /**< Test/set LED state */
  BCM2711_TAG_SET_LED_STATE = 0x00038041,  /**< Set LED state */

  BCM2711_TAG_ALLOCATE_MEMORY = 0x0003000c, /**< Allocate VideoCore memory */
  BCM2711_TAG_LOCK_MEMORY = 0x0003000d,     /**< Lock allocated memory */
  BCM2711_TAG_UNLOCK_MEMORY = 0x0003000e,   /**< Unlock allocated memory */
  BCM2711_TAG_RELEASE_MEMORY = 0x0003000f,  /**< Release allocated memory */
  BCM2711_TAG_EXECUTE_CODE = 0x00030010,    /**< Execute code on Videocore */

  BCM2711_TAG_GET_EDID_BLOCK = 0x00030020, /**< Get EDID block */
  BCM2711_TAG_END = 0x00000000             /**< End Tag */
} bcm2711_mbox_property_tag;

/**
 * @brief Mailbox Property Interface Tag structure
 * Represents a single property tag in the mailbox message.
 */
typedef struct mbox_property_tag {
  bcm2711_mbox_property_tag id;               /**< Property Tag ID */
  uint32_t buffer_size;      /**< Size of value_buffer in bytes */
  volatile uint32_t val_len; /**< Request: 0, Response: length of valid data */
  volatile uint32_t value_buffer[]; /**< Tag-specific data buffer */
} mbox_property_tag;

/**
 * @brief Mailbox Property Message Header
 */
typedef struct mbox_property_header {
  uint32_t size;     /**< Total message size in bytes */
  uint32_t req_resp; /**< Request: 0, Response: status code */
} mbox_property_header;

/**
 * @brief Metadata for initialising a property tag
 */
typedef struct mbox_property_tag_metadata {
  bcm2711_mbox_property_tag tag_id; /**< Property Tag ID */
  uint32_t value_size;              /**< Size of value buffer in bytes */
} mbox_property_tag_metadata;

/**
 * @brief Mailbox Property Message Structure
 *
 * Must be allocated with 16-byte alignment
 */
typedef struct __attribute__((aligned(16))) mbox_property_message {
  mbox_property_header header; /**< Property Message Header */
  mbox_property_tag buffer[];  /**< Buffer property to be populated with tags */
} mbox_property_message;

/**
 * @brief Prepare the mbox_buffer for rpi_mbox_property_message_init
 *
 * Verifies the alignment of the buffer
 * Sets the bytes of the buffer to 0
 * @param buffer Pointer to the mailbox buffer
 * @param size Size of the buffer
 * @return RTEMS_INVALID_ADDRESS if the buffer is not aligned
 * @return RTEMS_SUCCESSFUL if the buffer is aigned and all bytes are set to 0
 */
rtems_status_code rpi_mbox_property_buffer_prepare(void *buffer, size_t size);

/**
 * @brief Write data to the mailbox write register
 *
 * Packs the data and channel fields in the mailbox write register
 * @param channel 0 to 9 defined Mailbox Channels
 * @param data The value to store in the data section of the register
 */
void rpi_mbox_write(bcm2711_mbox_channel channel, uint32_t data);

/**
 * @brief Read data from the mailbox read register
 *
 * Reads data from the mailbox read register and validates against the channel
 * @param channel 0 to 9 defined Mailbox Channels
 * @return The data after verifying the channel is matching
 */
uint32_t rpi_mbox_read(bcm2711_mbox_channel channel);

/**
 * @brief Process a mailbox property message
 *
 * Sends the initialized message to the VideoCore GPU via the mailbox interface,
 * waits for the response, and validates the result. Handles cache operations
 * to ensure data coherency between CPU and VideoCore.
 *
 * The message buffer must be 16-byte aligned and initialized with
 * rpi_mbox_property_message_init() before calling this function.
 *
 * @param msg Pointer to 16-byte aligned and initialized message buffer
 * @return RTEMS_SUCCESSFUL if the message was processed successfully by
 * VideoCore
 * @return RTEMS_IO_ERROR if the VideoCore returned an error response
 */
rtems_status_code rpi_mbox_process(mbox_property_message *msg);

/**
 * @brief Initialize a mailbox property message
 *
 * Prepares a message buffer with the specified property tags for communication
 * with the VideoCore GPU. This function constructs the message by populating
 * the header and creating tags based on the provided metadata array.
 *
 * The message buffer must be 16-byte aligned (e.g., allocated with
 * aligned_alloc()). After initialization, call rpi_mbox_process() to send the
 * message to VideoCore.
 *
 * @param msg Pointer to 16-byte aligned message buffer
 * @param buffer_size Total size of the message buffer in bytes
 * @param tags Array of tag metadata describing the properties to query/set
 * @param tag_count Number of tags in the tags array
 *
 * @return RTEMS_SUCCESSFUL if initialization succeeded
 * @return RTEMS_INVALID_SIZE if buffer_size is too small for the specified tags
 */
rtems_status_code
rpi_mbox_property_message_init(mbox_property_message *msg, size_t buffer_size,
                               const mbox_property_tag_metadata *tags,
                               unsigned int tag_count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AARCH64_RASPBERRYPI_4_MBOX_H */