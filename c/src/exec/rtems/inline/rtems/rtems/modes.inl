/*  modes.inl
 *
 *  This include file contains the static inline implementation of the
 *  inlined routines in the Mode Handler
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

STATIC INLINE unsigned32 RTEMS_INTERRUPT_LEVEL (
  Modes_Control mode_set
)
{
  return mode_set & RTEMS_INTERRUPT_MASK;
}

/*PAGE
 *
 *  _Modes_Mask_changed
 *
 */

STATIC INLINE boolean _Modes_Mask_changed (
  Modes_Control mode_set,
  Modes_Control masks
)
{
   return ( mode_set & masks );
}

/*PAGE
 *
 *  _Modes_Is_asr_disabled
 *
 */

STATIC INLINE boolean _Modes_Is_asr_disabled (
  Modes_Control mode_set
)
{
   return (mode_set & RTEMS_ASR_MASK) == RTEMS_NO_ASR;
}

/*PAGE
 *
 *  _Modes_Is_preempt
 *
 */

STATIC INLINE boolean _Modes_Is_preempt (
  Modes_Control mode_set
)
{
   return (mode_set & RTEMS_PREEMPT_MASK) == RTEMS_PREEMPT;
}

/*PAGE
 *
 *  _Modes_Is_timeslice
 *
 */

STATIC INLINE boolean _Modes_Is_timeslice (
  Modes_Control mode_set
)
{
  return (mode_set & RTEMS_TIMESLICE_MASK) == RTEMS_TIMESLICE;
}

/*PAGE
 *
 *  _Modes_Get_interrupt_level
 *
 */

STATIC INLINE ISR_Level _Modes_Get_interrupt_level (
  Modes_Control mode_set
)
{
  return ( mode_set & RTEMS_INTERRUPT_MASK );
}

/*PAGE
 *
 *  _Modes_Set_interrupt_level
 *
 */

STATIC INLINE void _Modes_Set_interrupt_level (
  Modes_Control mode_set
)
{
  _ISR_Set_level( _Modes_Get_interrupt_level( mode_set ) );
}

/*PAGE
 *
 *  _Modes_Change
 *
 */

STATIC INLINE void _Modes_Change (
  Modes_Control  old_mode_set,
  Modes_Control  new_mode_set,
  Modes_Control  mask,
  Modes_Control *out_mode_set,
  Modes_Control *changed
)
{
  Modes_Control _out_mode;

  _out_mode      =  old_mode_set;
  _out_mode     &= ~mask;
  _out_mode     |= new_mode_set & mask;
  *changed       = _out_mode ^ old_mode_set;
  *out_mode_set  = _out_mode;
}

#endif
/* end of include file */
