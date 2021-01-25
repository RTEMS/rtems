/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides parts of the application configuration
 *   information API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1989, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/config/if/header-2 */

#ifndef _RTEMS_RTEMS_CONFIG_H
#define _RTEMS_RTEMS_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <rtems/rtems/tasks.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/config/if/api-table */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief This structure contains a summary of the Classic API configuration.
 *
 * Use rtems_configuration_get_rtems_api_configuration() to get the
 * configuration table.
 */
typedef struct {
  /**
   * @brief This member contains the maximum number of Classic API Tasks
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_TASKS.
   */
  uint32_t maximum_tasks;

  /**
   * @brief This member is true, if the Classic API Notepads are enabled,
   *   otherwise it is false.
   */
  bool notepads_enabled;

  /**
   * @brief This member contains the maximum number of Classic API Timers
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_TIMERS.
   */
  uint32_t maximum_timers;

  /**
   * @brief This member contains the maximum number of Classic API Semaphores
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_SEMAPHORES.
   */
  uint32_t maximum_semaphores;

  /**
   * @brief This member contains the maximum number of Classic API Message Queues
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_MESSAGE_QUEUES.
   */
  uint32_t maximum_message_queues;

  /**
   * @brief This member contains the maximum number of Classic API Partitions
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_PARTITIONS.
   */
  uint32_t maximum_partitions;

  /**
   * @brief This member contains the maximum number of Classic API Regions
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_REGIONS.
   */
  uint32_t maximum_regions;

  /**
   * @brief This member contains the maximum number of Classic API Dual-Ported
   *   Memories configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_PORTS.
   */
  uint32_t maximum_ports;

  /**
   * @brief This member contains the maximum number of Classic API Rate Monotonic
   *   Periods configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_PERIODS.
   */
  uint32_t maximum_periods;

  /**
   * @brief This member contains the maximum number of Classic API Barriers
   *   configured for this application.
   *
   * See #CONFIGURE_MAXIMUM_BARRIERS.
   */
  uint32_t maximum_barriers;

  /**
   * @brief This member contains the number of Classic API Initialization Tasks
   *   configured for this application.
   *
   * See #CONFIGURE_RTEMS_INIT_TASKS_TABLE.
   */
  uint32_t number_of_initialization_tasks;

  /**
   * @brief This member contains the pointer to Classic API Initialization Tasks
   *   Table of this application.
   *
   * See #CONFIGURE_RTEMS_INIT_TASKS_TABLE.
   */
  const rtems_initialization_tasks_table *User_initialization_tasks_table;
} rtems_api_configuration_table;

/* Generated from spec:/rtems/config/if/get-api-configuration */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the Classic API Configuration Table of this application.
 *
 * @return Returns the pointer to the Classic API Configuration Table of this
 *   application.
 */
const rtems_api_configuration_table *
rtems_configuration_get_rtems_api_configuration( void );

/* Generated from spec:/rtems/config/if/get-maximum-barriers */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Barriers configured for this
 *   application.
 *
 * @return Returns the maximum number of Classic API Barriers configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_BARRIERS.
 */
uint32_t rtems_configuration_get_maximum_barriers( void );

/* Generated from spec:/rtems/config/if/get-maximum-message-queues */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Message Queues configured for
 *   this application.
 *
 * @return Returns the maximum number of Classic API Message Queues configured
 *   for this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_MESSAGE_QUEUES.
 */
uint32_t rtems_configuration_get_maximum_message_queues( void );

/* Generated from spec:/rtems/config/if/get-maximum-partitions */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Partitions configured for this
 *   application.
 *
 * @return Returns the maximum number of Classic API Partitions configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_PARTITIONS.
 */
uint32_t rtems_configuration_get_maximum_partitions( void );

/* Generated from spec:/rtems/config/if/get-maximum-periods */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Rate Monotonic Periods
 *   configured for this application.
 *
 * @return Returns the maximum number of Classic API Rate Monotonic Periods
 *   configured for this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_PERIODS.
 */
uint32_t rtems_configuration_get_maximum_periods( void );

/* Generated from spec:/rtems/config/if/get-maximum-ports */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Dual-Ported Memories
 *   configured for this application.
 *
 * @return Returns the maximum number of Classic API Dual-Ported Memories
 *   configured for this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_PORTS.
 */
uint32_t rtems_configuration_get_maximum_ports( void );

/* Generated from spec:/rtems/config/if/get-maximum-regions */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Regions configured for this
 *   application.
 *
 * @return Returns the maximum number of Classic API Regions configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_REGIONS.
 */
uint32_t rtems_configuration_get_maximum_regions( void );

/* Generated from spec:/rtems/config/if/get-maximum-semaphores */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Semaphores configured for this
 *   application.
 *
 * @return Returns the maximum number of Classic API Semaphores configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_SEMAPHORES.
 */
uint32_t rtems_configuration_get_maximum_semaphores( void );

/* Generated from spec:/rtems/config/if/get-maximum-tasks */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Tasks configured for this
 *   application.
 *
 * @return Returns the maximum number of Classic API Tasks configured for this
 *   application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_TASKS.
 */
uint32_t rtems_configuration_get_maximum_tasks( void );

/* Generated from spec:/rtems/config/if/get-maximum-timers */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API Timers configured for this
 *   application.
 *
 * @return Returns the maximum number of Classic API Timers configured for this
 *   application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_TIMERS.
 */
uint32_t rtems_configuration_get_maximum_timers( void );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_CONFIG_H */
