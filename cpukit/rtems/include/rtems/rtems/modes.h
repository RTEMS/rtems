/**
 * @file rtems/rtems/modes.h
 *
 *  This include file contains all constants and structures associated
 *  with the RTEMS thread and RTEMS_ASR modes.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MODES_H
#define _RTEMS_RTEMS_MODES_H

/**
 *  @defgroup ClassicModes Modes
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/isr.h>

/**
 *  The following type defines the control block used to manage
 *  each a mode set.
 */
typedef uint32_t   Modes_Control;

/**
 *  The following constants define the individual modes and masks
 *  which may be used to compose a mode set and to alter modes.
 */
#define RTEMS_ALL_MODE_MASKS     0x0000ffff

/**
 *  This mode constant is the default mode set.
 */
#define RTEMS_DEFAULT_MODES     0x00000000

/**
 *  This mode constant is used when the user wishes to obtain their
 *  current execution mode.
 */
#define RTEMS_CURRENT_MODE      0

/** This mode constant corresponds to the timeslice enable/disable bit. */
#define RTEMS_TIMESLICE_MASK  0x00000200

/** This mode constant corresponds to the preemption enable/disable bit. */
#define RTEMS_PREEMPT_MASK    0x00000100

/** This mode constant corresponds to the signal enable/disable bit. */
#define RTEMS_ASR_MASK        0x00000400

/** This mode constant corresponds to the interrupt enable/disable bits. */
#define RTEMS_INTERRUPT_MASK  CPU_MODES_INTERRUPT_MASK

/** This mode constant is used to indicate preemption is enabled. */
#define RTEMS_PREEMPT      0x00000000
/** This mode constant is used to indicate preemption is disabled. */
#define RTEMS_NO_PREEMPT   0x00000100

/** This mode constant is used to indicate timeslicing is disabled. */
#define RTEMS_NO_TIMESLICE 0x00000000
/** This mode constant is used to indicate timeslicing is enabled. */
#define RTEMS_TIMESLICE    0x00000200

/** This mode constant is used to indicate signal processing is enabled. */
#define RTEMS_ASR          0x00000000
/** This mode constant is used to indicate signal processing is disabled. */
#define RTEMS_NO_ASR       0x00000400

/**
 *  @brief RTEMS_INTERRUPT_LEVEL
 *
 *  This function returns the processor dependent interrupt
 *  level which corresponds to the requested interrupt level.
 *
 *  @note RTEMS supports 256 interrupt levels using the least
 *        significant eight bits of MODES.CONTROL.  On any
 *        particular CPU, fewer than 256 levels may be supported.
 */
#define RTEMS_INTERRUPT_LEVEL( _mode_set ) \
  ( (_mode_set) & RTEMS_INTERRUPT_MASK )

/**
 *  @brief Interrupt Mask Variable
 *
 *  This variable is used by bindings from languages other than C and C++.
 */
extern const uint32_t rtems_interrupt_mask;

/**
 *  @brief Body for RTEMS_INTERRUPT_LEVEL macro.
 *
 *  @param[in] level is the desired interrupt level
 *
 *  @return This methods returns a mode with the desired interrupt
 *          @a level in the proper bitfield location.
 *
 *  @note This variable is used by bindings from languages other than
 *        C and C++.
 */
Modes_Control rtems_interrupt_level_body(
  uint32_t   level
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/modes.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
