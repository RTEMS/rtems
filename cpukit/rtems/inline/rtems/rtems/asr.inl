/*  inline/asr.inl
 *
 *  This include file contains the implemenation of all routines
 *  associated with the asynchronous signal handler which are inlined.
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

#ifndef __INLINE_ASR_inl
#define __INLINE_ASR_inl

#include <rtems/core/isr.h>

/*PAGE
 *
 *  _ASR_Initialize
 *
 */

STATIC INLINE void _ASR_Initialize (
  ASR_Information *information
)
{
  information->is_enabled      = TRUE;
  information->handler         = NULL;
  information->mode_set        = RTEMS_DEFAULT_MODES;
  information->signals_posted  = 0;
  information->signals_pending = 0;
  information->nest_level      = 0;
}

/*PAGE
 *
 *  _ASR_Swap_signals
 *
 */

STATIC INLINE void _ASR_Swap_signals (
  ASR_Information *information
)
{
  rtems_signal_set _signals;
  ISR_Level        _level;

  _ISR_Disable( _level );
    _signals                     = information->signals_pending;
    information->signals_pending = information->signals_posted;
    information->signals_posted  = _signals;
  _ISR_Enable( _level );
}

/*PAGE
 *
 *  _ASR_Is_null_handler
 *
 */

STATIC INLINE boolean _ASR_Is_null_handler (
  rtems_asr_entry asr_handler
)
{
  return asr_handler == NULL;
}

/*PAGE
 *
 *  _ASR_Are_signals_pending
 *
 */

STATIC INLINE boolean _ASR_Are_signals_pending (
  ASR_Information *information
)
{
  return information->signals_posted != 0;
}

/*PAGE
 *
 *  _ASR_Post_signals
 *
 */

STATIC INLINE void _ASR_Post_signals(
  rtems_signal_set  signals,
  rtems_signal_set *signal_set
)
{
  ISR_Level              _level;

  _ISR_Disable( _level );
    *signal_set |= signals;
  _ISR_Enable( _level );
}


#endif
/* end of include file */
