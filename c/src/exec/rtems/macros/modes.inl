/*  modes.inl
 *
 *  This include file contains the macro implementation of the
 *  inlined routines in the Mode Handler.
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

#ifndef __MODES_inl
#define __MODES_inl

/*PAGE
 *
 *  RTEMS_INTERRUPT_LEVEL
 */

#define RTEMS_INTERRUPT_LEVEL( _mode_set ) \
  ( (_mode_set) & RTEMS_INTERRUPT_MASK )

/*PAGE
 *
 *  _Modes_Mask_changed
 *
 */

#define _Modes_Mask_changed( _mode_set, _masks ) \
   ( (_mode_set) & (_masks) )

/*PAGE
 *
 *  _Modes_Is_asr_disabled
 *
 */

#define _Modes_Is_asr_disabled( _mode_set ) \
   ( (_mode_set) & RTEMS_ASR_MASK )

/*PAGE
 *
 *  _Modes_Is_preempt
 *
 */

#define _Modes_Is_preempt( _mode_set ) \
   ( ( (_mode_set) & RTEMS_PREEMPT_MASK ) == RTEMS_PREEMPT )

/*PAGE
 *
 *  _Modes_Is_timeslice
 *
 */

#define _Modes_Is_timeslice( _mode_set ) \
  (((_mode_set) & (RTEMS_TIMESLICE_MASK|RTEMS_PREEMPT_MASK)) == \
                  (RTEMS_TIMESLICE|RTEMS_PREEMPT) )

/*PAGE
 *
 *  _Modes_Get_interrupt_level
 *
 */

#define _Modes_Get_interrupt_level( _mode_set ) \
  ( (_mode_set) & RTEMS_INTERRUPT_MASK )

/*PAGE
 *
 *  _Modes_Set_interrupt_level
 *
 */

#define _Modes_Set_interrupt_level( _mode_set ) \
   _ISR_Set_level( _Modes_Get_interrupt_level( (_mode_set) ) )

/*PAGE
 *
 *  _Modes_Change
 *
 */

#define _Modes_Change( _old_mode_set, _new_mode_set, \
                       _mask, _out_mode_set, _changed ) \
  { Modes_Control _out_mode; \
    \
    _out_mode         =  (_old_mode_set); \
    _out_mode        &= ~(_mask); \
    _out_mode        |= (_new_mode_set) & (_mask); \
    *(_changed)       = _out_mode ^ (_old_mode_set); \
    *(_out_mode_set)  = _out_mode; \
  }

#endif
/* end of include file */
