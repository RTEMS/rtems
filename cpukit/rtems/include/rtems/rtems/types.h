/**
 * @file
 *
 * @ingroup ClassicRTEMS
 *
 * @brief Types used by the Classic API.
 */

/*  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TYPES_H
#define _RTEMS_RTEMS_TYPES_H

/*
 *  RTEMS basic type definitions
 */

#include <stdint.h>
#include <rtems/score/heap.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/modes.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicRTEMS
 *
 * @{
 */

#ifdef RTEMS_DEPRECATED_TYPES
/**
 * @brief Single precision float type.
 *
 * @deprecated Use @c float instead.
 */
typedef single_precision rtems_single;

/**
 * @brief Double precision float type.
 *
 * @deprecated Use @c double instead.
 */
typedef double_precision rtems_double;

/**
 * @brief RTEMS boolean type.
 *
 * @deprecated Use @c bool instead
 */
typedef boolean          rtems_boolean;
#endif

/**
 * @brief Classic API @ref ClassicRTEMSSubSecObjectNames "object name" type.
 *
 * Contains the name of a Classic API object. It is an unsigned 32-bit integer
 * which can be treated as a numeric value or initialized using
 * rtems_build_name() to contain four ASCII characters.
 */
typedef uint32_t         rtems_name;

/**
 * @brief Used to manage and manipulate
 * @ref ClassicRTEMSSubSecObjectIdentifiers "RTEMS object identifiers".
 */
typedef Objects_Id       rtems_id;

/**
 * @brief Invalid object identifier value.
 *
 * No object can have this identifier value.
 */
#define RTEMS_ID_NONE OBJECTS_ID_NONE

/**
 * @brief Public name for task context area.
 */
typedef Context_Control            rtems_context;

#if (CPU_HARDWARE_FP == TRUE) || (CPU_SOFTWARE_FP == TRUE)
/**
 * @brief Public name for task floating point context area.
 */
typedef Context_Control_fp         rtems_context_fp;
#endif

/**
 * @brief Defines the format of the interrupt stack frame as it appears to a
 * user ISR.
 *
 * This data structure may not be defined on all ports
 */
typedef CPU_Interrupt_frame        rtems_interrupt_frame;

/**
 * @brief Information structure returned by the Heap Handler via the Region
 * Manager.
 */
typedef Heap_Information_block region_information_block;

/**
 * @brief Used to manage and manipulate intervals specified by
 * @ref ClassicRTEMSSecTime "clock ticks".
 */
typedef Watchdog_Interval rtems_interval;

/**
 * @brief Represents the CPU usage per thread.
 *
 * When using nanoseconds granularity timing, RTEMS may internally use a
 * variety of representations.
 */
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  typedef struct timespec rtems_thread_cpu_usage_t;
#else
  typedef uint32_t rtems_thread_cpu_usage_t;
#endif

/**
 * @brief Data structure to manage and manipulate calendar
 * @ref ClassicRTEMSSecTime "time".
 */
typedef struct {
  /**
   * @brief Year, A.D.
   */
  uint32_t   year;
  /**
   * @brief Month, 1 .. 12.
   */
  uint32_t   month;
  /**
   * @brief Day, 1 .. 31.
   */
  uint32_t   day;
  /**
   * @brief Hour, 0 .. 23.
   */
  uint32_t   hour;
  /**
   * @brief Minute, 0 .. 59.
   */
  uint32_t   minute;
  /**
   * @brief Second, 0 .. 59.
   */
  uint32_t   second;
  /**
   * @brief Elapsed ticks between seconds.
   */
  uint32_t   ticks;
}   rtems_time_of_day;

/**
 * @brief Task mode type.
 */
typedef Modes_Control rtems_mode;

/*
 *  MPCI related entries
 */
#if defined(RTEMS_MULTIPROCESSING)
/**
 * @brief Set of MPCI packet classes which are internally dispatched to the
 * managers.
 */
typedef MP_packet_Classes          rtems_mp_packet_classes;

/**
 * @brief Prefix found at the beginning of each MPCI packet sent between nodes.
 */
typedef MP_packet_Prefix           rtems_packet_prefix;

/**
 * @brief Indirect pointer to the initialization entry point for an MPCI
 * handler.
 */
typedef MPCI_initialization_entry  rtems_mpci_initialization_entry;

/**
 * @brief Indirect pointer to the get_packet entry point for an MPCI handler.
 */
typedef MPCI_get_packet_entry      rtems_mpci_get_packet_entry;

/**
 * @brief Indirect pointer to the return_packet entry point for an MPCI
 * handler.
 */
typedef MPCI_return_packet_entry   rtems_mpci_return_packet_entry;

/**
 * @brief Indirect pointer to the send_packet entry point for an MPCI handler.
 */
typedef MPCI_send_entry            rtems_mpci_send_packet_entry;

/**
 * @brief Indirect pointer to the receive entry point for an MPCI handler.
 */
typedef MPCI_receive_entry         rtems_mpci_receive_packet_entry;

/**
 * @brief Return type from every MPCI handler routine.
 */
typedef MPCI_Entry                 rtems_mpci_entry;

/**
 * @brief Structure which is used to configure an MPCI handler.
 */
typedef MPCI_Control               rtems_mpci_table;

#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
