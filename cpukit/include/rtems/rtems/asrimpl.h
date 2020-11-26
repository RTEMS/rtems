/**
 * @file
 *
 * @ingroup RTEMSImplClassicASR
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicASR support.
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

#include <rtems/rtems/asrdata.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicASR Asynchronous Signal Routine (ASR)
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the implementation to support asynchronous signal
 *   routines.
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
  memset(asr, 0, sizeof(*asr));
}

RTEMS_INLINE_ROUTINE rtems_signal_set _ASR_Swap_signals( ASR_Information *asr )
{
  rtems_signal_set new_signals_posted;

  new_signals_posted   = asr->signals_pending;
  asr->signals_pending = asr->signals_posted;
  asr->signals_posted  = new_signals_posted;

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

  signal_set = asr->signals_posted;
  asr->signals_posted = 0;

  return signal_set;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
