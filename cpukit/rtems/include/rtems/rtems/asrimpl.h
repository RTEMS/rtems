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
  RTEMS_STATIC_ASSERT( RTEMS_DEFAULT_MODES == 0, _ASR_Create_mode_set );
}

RTEMS_INLINE_ROUTINE void _ASR_Destroy( ASR_Information *asr )
{
  _ISR_lock_Destroy( &asr->Lock );
}

RTEMS_INLINE_ROUTINE void _ASR_Acquire_critical(
  ASR_Information  *asr,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &asr->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _ASR_Acquire(
  ASR_Information  *asr,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable( lock_context );
  _ASR_Acquire_critical( asr, lock_context );
}

RTEMS_INLINE_ROUTINE void _ASR_Release(
  ASR_Information  *asr,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &asr->Lock, lock_context );
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

RTEMS_INLINE_ROUTINE rtems_signal_set _ASR_Swap_signals( ASR_Information *asr )
{
  rtems_signal_set new_signals_posted;
  ISR_lock_Context lock_context;

  _ASR_Acquire( asr, &lock_context );
    new_signals_posted   = asr->signals_pending;
    asr->signals_pending = asr->signals_posted;
    asr->signals_posted  = new_signals_posted;
  _ASR_Release( asr, &lock_context );

  return new_signals_posted;
}

RTEMS_INLINE_ROUTINE void _ASR_Post_signals(
  rtems_signal_set  signals,
  rtems_signal_set *signal_set
)
{
  *signal_set |= signals;
}

RTEMS_INLINE_ROUTINE rtems_signal_set _ASR_Get_posted_signals(
  ASR_Information *asr
)
{
  rtems_signal_set signal_set;
  ISR_lock_Context lock_context;

  _ASR_Acquire( asr, &lock_context );
    signal_set = asr->signals_posted;
    asr->signals_posted = 0;
  _ASR_Release( asr, &lock_context );

  return signal_set;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
