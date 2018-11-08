/**
 * @file
 *
 * @ingroup ClassicASRImpl
 *
 * @brief Classic ASR Data Structures
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_ASRDATA_H
#define _RTEMS_RTEMS_ASRDATA_H

#include <rtems/rtems/asr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicASRImpl
 *
 * @{
 */

/**
 *  The following defines the control structure used to manage
 *  signals.  Each thread has a copy of this record.
 */
typedef struct {
  /** This field indicates if are ASRs enabled currently. */
  bool              is_enabled;
  /** This field indicates if address of the signal handler function. */
  rtems_asr_entry   handler;
  /** This field indicates if the task mode the signal will run with. */
  rtems_mode        mode_set;
  /** This field indicates the signal set that is posted. */
  rtems_signal_set  signals_posted;
  /** This field indicates the signal set that is pending. */
  rtems_signal_set  signals_pending;
  /** This field indicates if nest level of signals being processed */
  uint32_t          nest_level;
}   ASR_Information;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
