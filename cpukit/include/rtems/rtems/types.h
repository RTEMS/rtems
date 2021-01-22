/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides types used by the Classic API.
 */

/*
 * Copyright (C) 2009, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 2006, 2008 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/type/if/header */

#ifndef _RTEMS_RTEMS_TYPES_H
#define _RTEMS_RTEMS_TYPES_H

#include <stdint.h>
#include <sys/_timespec.h>
#include <sys/_timeval.h>
#include <sys/cpuset.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/watchdogticks.h>

#if defined(RTEMS_MULTIPROCESSING)
  #include <rtems/score/mpci.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/req/group */

/**
 * @defgroup RTEMSImplClassic Classic API
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains the Classic API implementation.
 */

/* Generated from spec:/rtems/type/if/group */

/**
 * @defgroup RTEMSAPIClassicTypes Basic Types
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief This group contains basic Classic API types.
 */

/* Generated from spec:/rtems/type/if/id */

/**
 * @ingroup RTEMSAPIClassicTypes
 *
 * @brief This type represents RTEMS object identifiers.
 */
typedef Objects_Id rtems_id;

/* Generated from spec:/rtems/type/if/id-none */

/**
 * @ingroup RTEMSAPIClassicTypes
 *
 * @brief This constant represents an invalid RTEMS object identifier.
 *
 * No RTEMS object can have this identifier.
 */
#define RTEMS_ID_NONE OBJECTS_ID_NONE

/* Generated from spec:/rtems/type/if/interval */

/**
 * @ingroup RTEMSAPIClassicTypes
 *
 * @brief This type represents clock tick intervals.
 */
typedef Watchdog_Interval rtems_interval;

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mp-packet-classes */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief This enumeration defines the MPCI packet classes.
   */
  typedef MP_packet_Classes rtems_mp_packet_classes;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-entry */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief MPCI handler routines shall have this return type.
   */
  typedef MPCI_Entry rtems_mpci_entry;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-get-packet-entry */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief MPCI get packet routines shall have this type.
   */
  typedef MPCI_get_packet_entry rtems_mpci_get_packet_entry;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-initialization-entry */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief MPCI initialization routines shall have this type.
   */
  typedef MPCI_initialization_entry rtems_mpci_initialization_entry;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-receive-packet-entry */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief MPCI receive packet routines shall have this type.
   */
  typedef MPCI_receive_entry rtems_mpci_receive_packet_entry;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-return-packet-entry */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief MPCI return packet routines shall have this type.
   */
  typedef MPCI_return_packet_entry rtems_mpci_return_packet_entry;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-send-packet-entry */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief MPCI send packet routines shall have this type.
   */
  typedef MPCI_send_entry rtems_mpci_send_packet_entry;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/mpci-table */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief This type represents the user-provided MPCI control.
   */
  typedef MPCI_Control rtems_mpci_table;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/multiprocessing-table */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief This type represents the user-provided MPCI configuration.
   */
  typedef MPCI_Configuration rtems_multiprocessing_table;
#endif

/* Generated from spec:/rtems/type/if/name */

/**
 * @ingroup RTEMSAPIClassicTypes
 *
 * @brief This type represents Classic API object names.
 *
 * It is an unsigned 32-bit integer which can be treated as a numeric value or
 * initialized using rtems_build_name() to encode four ASCII characters.  A
 * value of zero may have a special meaning in some directives.
 */
typedef uint32_t rtems_name;

/* Generated from spec:/rtems/type/if/no-timeout */

/**
 * @ingroup RTEMSAPIClassicTypes
 *
 * @brief This clock tick interval constant indicates that the calling task is
 *   willing to wait potentially forever on a resource.
 */
#define RTEMS_NO_TIMEOUT ( (rtems_interval) WATCHDOG_NO_TIMEOUT )

#if defined(RTEMS_MULTIPROCESSING)
  /* Generated from spec:/rtems/type/if/packet-prefix */

  /**
   * @ingroup RTEMSAPIClassicTypes
   *
   * @brief This type represents the prefix found at the beginning of each MPCI
   *   packet sent between nodes.
   */
  typedef MP_packet_Prefix rtems_packet_prefix;
#endif

/* Generated from spec:/rtems/type/if/time-of-day */

/**
 * @ingroup RTEMSAPIClassicTypes
 *
 * @brief This type represents Classic API calendar times.
 */
typedef struct {
  /**
   * @brief This member contains the year A.D.
   */
  uint32_t year;

  /**
   * @brief This member contains the month of the year with values from 1 to 12.
   */
  uint32_t month;

  /**
   * @brief This member contains the day of the month with values from 1 to 31.
   */
  uint32_t day;

  /**
   * @brief This member contains the hour of the day with values from 0 to 23.
   */
  uint32_t hour;

  /**
   * @brief This member contains the minute of the hour with values from 0 to 59.
   */
  uint32_t minute;

  /**
   * @brief This member contains the second of the minute with values from 0 to
   *   59.
   */
  uint32_t second;

  /**
   * @brief This member contains the clock tick of the second with values from 0
   *   to rtems_clock_get_ticks_per_second() minus one.
   */
  uint32_t ticks;
} rtems_time_of_day;

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_TYPES_H */
