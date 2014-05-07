/**
 * @file
 *
 * @ingroup ClassicASRImpl
 *
 * @brief Classic ASR Implementation
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_ASRIMPL_H
#define _RTEMS_RTEMS_ASRIMPL_H

#include <rtems/rtems/asr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicASRImpl Classic ASR Implementation
 *
 * @ingroup ClassicASR
 *
 * @{
 */

/**
 *  @brief ASR_Initialize
 *
 *  This routine initializes the given RTEMS_ASR information record.
 */
RTEMS_INLINE_ROUTINE void _ASR_Initialize (
  ASR_Information *asr
)
{
  asr->is_enabled      = false;
  asr->handler         = NULL;
  asr->mode_set        = RTEMS_DEFAULT_MODES;
  asr->signals_posted  = 0;
  asr->signals_pending = 0;
  asr->nest_level      = 0;
}

RTEMS_INLINE_ROUTINE void _ASR_Create( ASR_Information *asr )
{
  _ISR_lock_Initialize( &asr->Lock, "ASR" );
  _ASR_Initialize( asr );
}

RTEMS_INLINE_ROUTINE void _ASR_Destroy( ASR_Information *asr )
{
  _ISR_lock_Destroy( &asr->Lock );
}

/**
 *  @brief ASR_Swap_signals
 *
 *  This routine atomically swaps the pending and posted signal
 *  sets.  This is done when the thread alters its mode in such a
 *  way that the RTEMS_ASR disable/enable flag changes.
 */
RTEMS_INLINE_ROUTINE void _ASR_Swap_signals (
  ASR_Information *asr
)
{
  rtems_signal_set _signals;
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &asr->Lock, &lock_context );
    _signals             = asr->signals_pending;
    asr->signals_pending = asr->signals_posted;
    asr->signals_posted  = _signals;
  _ISR_lock_Release_and_ISR_enable( &asr->Lock, &lock_context );
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
  ASR_Information *asr
)
{
  return asr->signals_posted != 0;
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
  ASR_Information  *asr,
  rtems_signal_set  signals,
  rtems_signal_set *signal_set
)
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &asr->Lock, &lock_context );
    *signal_set |= signals;
  _ISR_lock_Release_and_ISR_enable( &asr->Lock, &lock_context );
}

RTEMS_INLINE_ROUTINE rtems_signal_set _ASR_Get_posted_signals(
  ASR_Information *asr
)
{
  rtems_signal_set signal_set;
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &asr->Lock, &lock_context );
    signal_set = asr->signals_posted;
    asr->signals_posted = 0;
  _ISR_lock_Release_and_ISR_enable( &asr->Lock, &lock_context );

  return signal_set;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
