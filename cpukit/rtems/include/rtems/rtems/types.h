/*  types.h
 *
 *  This include file defines the types used by the RTEMS API.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_RTEMS_TYPES_h
#define __RTEMS_RTEMS_TYPES_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  RTEMS basic type definitions
 */

#include <stdint.h>

/* backward compatibility types */
typedef uint8_t        rtems_unsigned8;
typedef uint16_t       rtems_unsigned16;
typedef uint32_t       rtems_unsigned32;
typedef uint64_t       rtems_unsigned64;

typedef int8_t         rtems_signed8;
typedef int16_t        rtems_signed16;
typedef int32_t        rtems_signed32;
typedef int64_t        rtems_signed64;

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

typedef single_precision rtems_single;    /* single precision float */
typedef double_precision rtems_double;    /* double precision float */

typedef boolean          rtems_boolean;

typedef uint32_t       rtems_name;
typedef Objects_Id       rtems_id;

typedef Context_Control            rtems_context;
typedef Context_Control_fp         rtems_context_fp;
typedef CPU_Interrupt_frame        rtems_interrupt_frame;

/*
 *  Region information block
 */

typedef Heap_Information_block region_information_block;

/*
 *  Time related 
 */

typedef Watchdog_Interval rtems_interval;
typedef TOD_Control       rtems_time_of_day;

/*
 *  Define the type for an RTEMS API task mode.
 */
 
typedef Modes_Control rtems_mode;

/*
 *  MPCI related entries
 */

#if defined(RTEMS_MULTIPROCESSING)
typedef MP_packet_Classes          rtems_mp_packet_classes;
typedef MP_packet_Prefix           rtems_packet_prefix;

typedef MPCI_initialization_entry  rtems_mpci_initialization_entry;
typedef MPCI_get_packet_entry      rtems_mpci_get_packet_entry;
typedef MPCI_return_packet_entry   rtems_mpci_return_packet_entry;
typedef MPCI_send_entry            rtems_mpci_send_packet_entry;
typedef MPCI_receive_entry         rtems_mpci_receive_packet_entry;

typedef MPCI_Entry rtems_mpci_entry;

typedef MPCI_Control rtems_mpci_table;
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
