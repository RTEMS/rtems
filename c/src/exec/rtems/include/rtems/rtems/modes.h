/*  modes.h
 *
 *  This include file contains all constants and structures associated
 *  with the RTEMS thread and RTEMS_ASR modes.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

/*
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

STATIC INLINE unsigned32 RTEMS_INTERRUPT_LEVEL (
  Modes_Control mode_set
);

/*
 *  _Modes_Mask_changed
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if any of the mode flags in mask
 *  are set in mode_set, and FALSE otherwise.
 */

STATIC INLINE boolean _Modes_Mask_changed (
  Modes_Control mode_set,
  Modes_Control masks
);

/*
 *  _Modes_Is_asr_disabled
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if mode_set indicates that Asynchronous
 *  Signal Processing is disabled, and FALSE otherwise.
 */

STATIC INLINE boolean _Modes_Is_asr_disabled (
  Modes_Control mode_set
);

/*
 *  _Modes_Is_preempt
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if mode_set indicates that preemption
 *  is enabled, and FALSE otherwise.
 */

STATIC INLINE boolean _Modes_Is_preempt (
  Modes_Control mode_set
);

/*
 *  _Modes_Is_timeslice
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if mode_set indicates that timeslicing
 *  is enabled, and FALSE otherwise.
 */

STATIC INLINE boolean _Modes_Is_timeslice (
  Modes_Control mode_set
);

/*
 *  _Modes_Get_interrupt_level
 *
 *  DESCRIPTION:
 *
 *  This function returns the interrupt level portion of the mode_set.
 */

STATIC INLINE ISR_Level _Modes_Get_interrupt_level (
  Modes_Control mode_set
);

/*
 *  _Modes_Set_interrupt_level
 *
 *  DESCRIPTION:
 *
 *  This routine sets the current interrupt level to that specified
 *  in the mode_set.
 */

STATIC INLINE void _Modes_Set_interrupt_level (
  Modes_Control mode_set
);

/*
 *  _Modes_Change
 *
 *  DESCRIPTION:
 *
 *  This routine changes the modes in old_mode_set indicated by
 *  mask to the requested values in new_mode_set.  The resulting
 *  mode set is returned in out_mode_set and the modes that changed
 *  is returned in changed.
 */

STATIC INLINE void _Modes_Change (
  Modes_Control  old_mode_set,
  Modes_Control  new_mode_set,
  Modes_Control  mask,
  Modes_Control *out_mode_set,
  Modes_Control *changed
);

#include <rtems/rtems/modes.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
