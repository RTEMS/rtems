/*  modes.h
 *
 *  This include file contains all constants and structures associated
 *  with the RTEMS thread and RTEMS_ASR modes.
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

#ifndef __RTEMS_MODES_h
#define __RTEMS_MODES_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/isr.h>

/*
 *  The following type defines the control block used to manage
 *  each a mode set.
 */

typedef unsigned32 Modes_Control;

/*
 *  The following constants define the individual modes and masks
 *  which may be used to compose a mode set and to alter modes.
 */

#define RTEMS_ALL_MODE_MASKS     0x0000ffff

#define RTEMS_DEFAULT_MODES     0x00000000
#define RTEMS_CURRENT_MODE      0

#define RTEMS_PREEMPT_MASK    0x00000100  /* preemption bit           */
#define RTEMS_TIMESLICE_MASK  0x00000200  /* timeslice bit            */
#define RTEMS_ASR_MASK        0x00000400  /* RTEMS_ASR enable bit           */
#define RTEMS_INTERRUPT_MASK  CPU_MODES_INTERRUPT_MASK

#define RTEMS_PREEMPT      0x00000000     /* enable preemption        */
#define RTEMS_NO_PREEMPT   0x00000100     /* disable preemption       */

#define RTEMS_NO_TIMESLICE 0x00000000     /* disable timeslicing      */
#define RTEMS_TIMESLICE    0x00000200     /* enable timeslicing       */

#define RTEMS_ASR          0x00000000     /* enable RTEMS_ASR               */
#define RTEMS_NO_ASR       0x00000400     /* disable RTEMS_ASR              */

/*
 *  The number of bits for interrupt levels is CPU dependent.
 *  RTEMS supports 0 to 256 levels in bits 0-7 of the mode.
 */

/*PAGE
 *
 *  RTEMS_INTERRUPT_LEVEL
 *
 *  DESCRIPTION:
 *
 *  This function returns the processor dependent interrupt
 *  level which corresponds to the requested interrupt level.
 *
 * NOTE: RTEMS supports 256 interrupt levels using the least
 *       significant eight bits of MODES.CONTROL.  On any
 *       particular CPU, fewer than 256 levels may be supported.
 */

#define RTEMS_INTERRUPT_LEVEL( _mode_set ) \
  ( (_mode_set) & RTEMS_INTERRUPT_MASK )
 

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/modes.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
