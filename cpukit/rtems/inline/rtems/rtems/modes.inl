/*  modes.inl
 *
 *  This include file contains the static inline implementation of the
 *  inlined routines in the Mode Handler
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
 *  DESCRIPTION:
 *
 *  This function returns TRUE if any of the mode flags in mask
 *  are set in mode_set, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Modes_Mask_changed (
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
 *  DESCRIPTION:
 *
 *  This function returns TRUE if mode_set indicates that Asynchronous
 *  Signal Processing is disabled, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Modes_Is_asr_disabled (
  Modes_Control mode_set
)
{
   return (mode_set & RTEMS_ASR_MASK) == RTEMS_NO_ASR;
}

/*PAGE
 *
 *  _Modes_Is_preempt
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if mode_set indicates that preemption
 *  is enabled, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Modes_Is_preempt (
  Modes_Control mode_set
)
{
   return (mode_set & RTEMS_PREEMPT_MASK) == RTEMS_PREEMPT;
}

/*PAGE
 *
 *  _Modes_Is_timeslice
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if mode_set indicates that timeslicing
 *  is enabled, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Modes_Is_timeslice (
  Modes_Control mode_set
)
{
  return (mode_set & RTEMS_TIMESLICE_MASK) == RTEMS_TIMESLICE;
}

/*PAGE
 *
 *  _Modes_Get_interrupt_level
 *
 *  DESCRIPTION:
 *
 *  This function returns the interrupt level portion of the mode_set.
 */

RTEMS_INLINE_ROUTINE ISR_Level _Modes_Get_interrupt_level (
  Modes_Control mode_set
)
{
  return ( mode_set & RTEMS_INTERRUPT_MASK );
}

/*PAGE
 *
 *  _Modes_Set_interrupt_level
 *
 *  DESCRIPTION:
 *
 *  This routine sets the current interrupt level to that specified
 *  in the mode_set.
 */

RTEMS_INLINE_ROUTINE void _Modes_Set_interrupt_level (
  Modes_Control mode_set
)
{
  _ISR_Set_level( _Modes_Get_interrupt_level( mode_set ) );
}

/*PAGE
 *
 *  _Modes_Change
 *
 *  DESCRIPTION:
 *
 *  This routine changes the modes in old_mode_set indicated by
 *  mask to the requested values in new_mode_set.  The resulting
 *  mode set is returned in out_mode_set and the modes that changed
 *  is returned in changed.
 */

RTEMS_INLINE_ROUTINE void _Modes_Change (
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
