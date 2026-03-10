/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSGenericIODevAPI
 *
 * @brief Generic IO Device API
 */

/*
 * Copyright (C) 2026 Aaron Nyholm
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

#ifndef _DEV_IODEV_H
#define _DEV_IODEV_H

#include <time.h>

#include <rtems.h>
#include <rtems/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtems_iodev rtems_iodev;

/**
 * @defgroup RTEMSGenericIOSpaceAPI Generic IO Space API
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief Generic IO Space API to wrap IO memory regions.
 *
 * @{
 */

/* IOCTL Calls */

/**
 * @brief Obtains the iodev device.
 *
 * This command has no argument.
 */
#define RTEMS_IODEV_IOCTL_OBTAIN 0
/**
 * @brief Releases the iodev device.
 *
 * This command has no argument.
 */
#define RTEMS_IODEV_IOCTL_RELEASE 1

/**
 * @brief Get the device information, defined by the driver.
 *
 * @param[out] info pointer to struct containing device information.
 */
#define RTEMS_IODEV_IOCTL_DEVICE_INFO 2

/**
 * @brief Get number of events in iodev.
 *
 * @param[out] count size_t containing the number of regions.
 */
#define RTEMS_IODEV_IOCTL_REGION_COUNT 3

/**
 * @brief Get number of events in iodev.
 *
 * @param[in,out] arg Pointer to rtems_iodev_region struct
 * with offset and length for return values.
 */
#define RTEMS_IODEV_IOCTL_REGION_GET 4

/**
 * @brief Get number of events in iodev.
 *
 * @param[out] count size_t containing the number of events.
 */
#define RTEMS_IODEV_IOCTL_EVENT_COUNT 5

/**
 * @brief Get information for given event ID.
 *
 * @param[in,out] event_info Pointer to rtems_iodev_event_info struct
 * containing event index and returned information.
 */
#define RTEMS_IODEV_IOCTL_EVENT_INFO 6

/**
 * @brief Wait or poll on event. Set timeout to zero for indefinite wait.
 *
 * @param[in,out] event_args Pointer to rtems_iodev_event_args struct
 * containing event index, timeout length and iodev driver return value.
 */
#define RTEMS_IODEV_IOCTL_EVENT_WAIT 7

/**
 * @brief The maximum number of IO regions.
 */
#define RTEMS_IODEV_MAX_REGIONS 32

/**
 * @brief General definition for an iodev region.
 */
typedef struct rtems_iodev_region {
  /**
   * @brief Region index.
   */
  size_t      index;
  /**
   * @brief Region address.
   */
  void       *address;
  /**
   * @brief Length of region.
   */
  size_t      size;
  /**
   * @brief Name of region.
   */
  const char *name;
} rtems_iodev_region;

/**
 * @brief General description of iodev event.
 */
typedef struct rtems_iodev_event_info {
  /**
   * @brief Event index.
   */
  uint32_t    index;
  /**
   * @brief Name of event.
   */
  const char *name;
  /**
   * @brief Additional argument;
   */
  void       *args;
} rtems_iodev_event_info;

/**
 * @brief General definition of iodev event arguments.
 */
typedef struct rtems_iodev_event_args {
  /**
   * @brief Event index.
   */
  uint32_t        index;
  /**
   * @brief Timeout length.
   */
  struct timespec timeout;
  /**
   * @brief Returns if event timed out.
   */
  bool            timedout;
  /**
   * @brief Additional argument.
   */
  void           *args;
} rtems_iodev_event_args;

/**
 * @brief iodev device.
 */
struct rtems_iodev {
  /**
   * @brief Call to the device driver to get device information.
   *
   * @param[in] iodev Pointer to iodev device.
   * @param[out] info Pointer to device information.
   *
   * @retval 0 Successful operation.
   * @retval non-zero Failed operation.
   */
  int ( *get_device_info )( rtems_iodev *iodev, void *info );
  /**
   *
   * @brief Call to the device driver to get number of events.
   *
   * @param[in] iodev Pointer to iodev device.
   * @param[in,out] event_count Pointer to number of events.
   *
   * @retval 0 Successful operation.
   * @retval non-zero Failed operation.
   */
  int ( *get_event_count )( rtems_iodev *iodev, size_t *event_count );

  /**
   * @brief Call to the device driver to get info on event.
   *
   * @param[in] iodev Pointer to iodev device.
   * @param[in,out] event_args Arguments for wait operation.
   *
   * @retval 0 Successful operation.
   * @retval non-zero Failed operation.
   */
  int ( *get_event_info )(
    rtems_iodev            *iodev,
    rtems_iodev_event_info *event_info
  );

  /**
   * @brief Call to the device driver to wait on event.
   *
   * @param[in] iodev Pointer to iodev device.
   * @param[in,out] event_args Arguments for wait operation.
   *
   * @retval 0 Successful operation.
   * @retval non-zero Failed operation.
   */
  int ( *event_wait )(
    rtems_iodev            *iodev,
    rtems_iodev_event_args *event_args
  );

  /**
   * @brief Call to the device driver to get array of regions.
   *
   * @param[in] iodev Pointer to iodev device.
   * @param[in,out] regions Pointer to array of regions.
   *
   * @retval 0 Successful operation.
   * @retval non-zero Failed operation.
   */
  int ( *get_regions )( rtems_iodev *iodev, rtems_iodev_region **regions );

  /**
   * @brief Call to the device driver to get number of regions.
   *
   * @param[in] iodev Pointer to iodev device.
   * @param[in,out] region_count Pointer to number of regions.
   *
   * @retval 0 Successful operation.
   * @retval non-zero Failed operation.
   */
  int ( *get_region_count )( rtems_iodev *iodev, size_t *region_count );

  /**
   * @brief Destroys the iodev device.
   *
   * @param[in] iodev Pointer to iodev device.
   */
  void ( *destroy )( rtems_iodev *iodev );

  /**
   * @brief Callback to destroy private data not owned directly by the iodev
   * framework.
   *
   * @param[in] iodev Pointer to iodev device.
   */
  void ( *priv_destroy )( rtems_iodev *iodev );

  /**
   * @brief Pointer to device driver.
   */
  void *driver;

  /**
   * @brief Mutex to protect the iodev device access.
   */
  rtems_recursive_mutex mutex;
};

/**
 * @brief Allocate and initialize the iodev device.
 *
 * After a successful allocation and initialization of the iodev device
 * it must be destroyed via rtems_iodev_destroy_unregistered().
 *
 * @param[in] size The number of bytes to allocate.
 *
 * @retval NULL Failed to set up iodev device.
 * @retval non-NULL The new iodev device.
 */
rtems_iodev *rtems_iodev_alloc_and_init( size_t size );

/**
 * @brief Initialize the iodev device.
 *
 * After a successful initialization of the iodev device it must be
 * destroyed via rtems_iodev_destroy_unregistered().
 *
 * @param[in] iodev The iodev device to initialize.
 *
 * @retval non-zero Failed to set up iodev device.
 * @retval 0 Successful set up of iodev device.
 */
int rtems_iodev_init( rtems_iodev *iodev );

/**
 * @brief Register the iodev device.
 *
 * This function always claims ownership of the iodev device passed to it. Once
 * successfully registered, the iodev device can only be destroyed by
 * unregistering it.
 *
 * After initialization and before registration rtems_iodev device driver
 * functions need to be set.
 *
 * @param[in] iodev The iodev device.
 * @param[in] iodev_path The path to the iodev device file.
 *
 * @retval 0 Successful operation.
 * @retval non-zero Failed operation.
 */
int rtems_iodev_register( rtems_iodev *iodev, const char *iodev_path );

/**
 * @brief Unregister and destroy the iodev device.
 *
 * @param[in] iodev The iodev device.
 *
 * @retval 0 Successful operation.
 * @retval non-zero Failed operation.
 */
int rtems_iodev_unregister_and_destroy( const char *iodev_path );

/**
 * @brief Destroys the iodev device. Frees the iodev if initialised with
 * rtems_iodev_alloc_and_init.
 *
 * This function must only be used on rtems_iodev instances that have not been
 * successfully registered with rtems_iodev_register.
 *
 * @param[in] iodev The iodev device.
 */
void rtems_iodev_destroy_unregistered( rtems_iodev *iodev );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _DEV_IODEV_H */
