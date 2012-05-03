/**
 * @file rtems/rtems/asr.inl
 *
 *  This include file contains the implemenation of all routines
 *  associated with the asynchronous signal handler which are inlined.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_ASR_H
# error "Never use <rtems/rtems/asr.inl> directly; include <rtems/rtems/asr.h> instead."
#endif

#ifndef _RTEMS_RTEMS_ASR_INL
#define _RTEMS_RTEMS_ASR_INL

#include <rtems/score/isr.h>

/**
 *  @addtogroup ClassicASR
 *  @{
 */

/**
 *  @brief ASR_Initialize
 *
 *  This routine initializes the given RTEMS_ASR information record.
 */
RTEMS_INLINE_ROUTINE void _ASR_Initialize (
  ASR_Information *information
)
{
  information->is_enabled      = false;
  information->handler         = NULL;
  information->mode_set        = RTEMS_DEFAULT_MODES;
  information->signals_posted  = 0;
  information->signals_pending = 0;
  information->nest_level      = 0;
}

/**
 *  @brief ASR_Swap_signals
 *
 *  This routine atomically swaps the pending and posted signal
 *  sets.  This is done when the thread alters its mode in such a
 *  way that the RTEMS_ASR disable/enable flag changes.
 */
RTEMS_INLINE_ROUTINE void _ASR_Swap_signals (
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

/**
 *  @brief ASR_Is_null_handler
 *
 *  This function returns TRUE if the given asr_handler is NULL and
 *  FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _ASR_Is_null_handler (
  rtems_asr_entry asr_handler
)
{
  return asr_handler == NULL;
}

/**
 *  @brief ASR_Are_signals_pending
 *
 *  This function returns TRUE if there are signals pending in the
 *  given RTEMS_ASR information record and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _ASR_Are_signals_pending (
  ASR_Information *information
)
{
  return information->signals_posted != 0;
}

/**
 *  @brief ASR_Post_signals
 *
 *  This routine posts the given signals into the signal_set
 *  passed in.  The result is returned to the user in signal_set.
 *
 *  NOTE:  This must be implemented as a macro.
 */
RTEMS_INLINE_ROUTINE void _ASR_Post_signals(
  rtems_signal_set  signals,
  rtems_signal_set *signal_set
)
{
  ISR_Level              _level;

  _ISR_Disable( _level );
    *signal_set |= signals;
  _ISR_Enable( _level );
}

/**@}*/

#endif
/* end of include file */
