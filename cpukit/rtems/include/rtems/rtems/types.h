/**
 * @file rtems/rtems/types.h
 *
 *  This include file defines the types used by the RTEMS API.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_TYPES_H
#define _RTEMS_RTEMS_TYPES_H

/**
 *  @defgroup ClassicTypes Classic API Types
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

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

/** This type defines a single precision float. */
#ifdef RTEMS_DEPRECATED_TYPES
typedef single_precision rtems_single;

/** This type defines a double precision float. */
typedef double_precision rtems_double;

/** This type defines the RTEMS boolean type . */
typedef boolean          rtems_boolean;
#endif

/** This type defines is for Classic API object names. */
typedef uint32_t         rtems_name;

/**
 *  This type defines is for RTEMS object Id.  Although this
 *  type name is specific to the Classic API, the format of
 *  an object Id is the same across all APIs.
 */
typedef Objects_Id       rtems_id;

/**
 *  This defines a value that is an invalid object Id.
 */
#define RTEMS_ID_NONE OBJECTS_ID_NONE

/**
 * This type is the public name for task context area.
 */
typedef Context_Control            rtems_context;

#if (CPU_HARDWARE_FP == TRUE) || (CPU_SOFTWARE_FP == TRUE)
/**
 * This type is the public name for task floating point context area.
 */
typedef Context_Control_fp         rtems_context_fp;
#endif

/**
 * This type is the public name for the architecture specific
 * stack frame built as part of vectoring an interrupt.
 */
typedef CPU_Interrupt_frame        rtems_interrupt_frame;

/**
 *  This type defines the public name for the information
 *  structure returned by the Heap Handler via the Region
 *  Manager.
 */
typedef Heap_Information_block region_information_block;

/**
 *  This type defines the public name for the type that is
 *  used to manage intervals specified by clock ticks.
 */
typedef Watchdog_Interval rtems_interval;

/**
 *  This is the public type used to represent the CPU usage per thread.
 *
 *  @note When using nanosecond granularity timing, RTEMS may internally
 *        use a variety of represenations.
 */
#ifndef __RTEMS_USE_TICKS_CPU_USAGE_STATISTICS__
  typedef struct timespec rtems_thread_cpu_usage_t;
#else
  typedef uint32_t rtems_thread_cpu_usage_t;
#endif


/**
 *  The following record defines the time of control block.  This
 *  control block is used to maintain the current time of day.
 *
 *  @note This is an RTEID (a.k.a. Classic API) style time/date.
 */
typedef struct {
  /** This field is the year, A.D. */
  uint32_t   year;
  /** This field is the month, 1 -> 12 */
  uint32_t   month;
  /** This field is the day, 1 -> 31 */
  uint32_t   day;
  /** This field is the hour, 0 -> 23 */
  uint32_t   hour;
  /** This field is the minute, 0 -> 59 */
  uint32_t   minute;
  /** This field is the second, 0 -> 59 */
  uint32_t   second;
  /** This field is the elapsed ticks between secs */
  uint32_t   ticks;
}   rtems_time_of_day;

/**
 *  This defines the public name for an RTEMS API task mode type.
 */
typedef Modes_Control rtems_mode;

/*
 *  MPCI related entries
 */
#if defined(RTEMS_MULTIPROCESSING)
/**
 * This defines the public name for the set of MPCI packet
 * classes which are internally dispatched to the managers.
 */
typedef MP_packet_Classes          rtems_mp_packet_classes;

/**
 * This defines the public name for the common prefix
 * found at the beginning of each MPCI packet sent between
 * nodes. This can be thought of as an envelope.
 */
typedef MP_packet_Prefix           rtems_packet_prefix;

/**
 * This defines the public name for the type for an indirect pointer
 * to the initialization entry point for an MPCI handler.
 */
typedef MPCI_initialization_entry  rtems_mpci_initialization_entry;

/**
 * This defines the public name for the type for an indirect pointer
 * to the get_packet entry point for an MPCI handler.
 */
typedef MPCI_get_packet_entry      rtems_mpci_get_packet_entry;

/**
 * This defines the public name for the type for an indirect pointer
 * to the return_packet entry point for an MPCI handler.
 */
typedef MPCI_return_packet_entry   rtems_mpci_return_packet_entry;

/**
 * This defines the public name for the type for an indirect pointer
 * to the send_packet entry point for an MPCI handler.
 */
typedef MPCI_send_entry            rtems_mpci_send_packet_entry;

/**
 * This defines the public name for the type for an indirect pointer
 * to the receive entry point for an MPCI handler.
 */
typedef MPCI_receive_entry         rtems_mpci_receive_packet_entry;

/**
 * This defines the public name for the return type from every
 * MPCI handler routine.
 */
typedef MPCI_Entry                 rtems_mpci_entry;

/**
 * This defines the public name for the structure which is used to
 * configure an MPCI handler.
 */
typedef MPCI_Control               rtems_mpci_table;

#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
