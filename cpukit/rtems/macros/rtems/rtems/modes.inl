/*  modes.inl
 *
 *  This include file contains the macro implementation of the
 *  inlined routines in the Mode Handler.
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

#ifndef __MODES_inl
#define __MODES_inl

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
   (((_mode_set) & RTEMS_ASR_MASK) == RTEMS_NO_ASR)

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
  (((_mode_set) & RTEMS_TIMESLICE_MASK) == RTEMS_TIMESLICE)

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
