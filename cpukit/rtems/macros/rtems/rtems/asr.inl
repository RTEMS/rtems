/*  macros/asr.h
 *
 *  This include file contains the implemenation of all routines
 *  associated with the asynchronous signal handler which are inlined.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __MACROS_ASR_h
#define __MACROS_ASR_h

#include <rtems/score/isr.h>

/*PAGE
 *
 *  _ASR_Initialize
 *
 */

#define _ASR_Initialize( _information ) \
{ \
  (_information)->is_enabled      = TRUE; \
  (_information)->handler         = NULL; \
  (_information)->mode_set        = RTEMS_DEFAULT_MODES; \
  (_information)->signals_posted  = 0; \
  (_information)->signals_pending = 0; \
  (_information)->nest_level      = 0; \
}

/*PAGE
 *
 *  _ASR_Swap_signals
 *
 */

#define _ASR_Swap_signals( _information ) \
{ \
  rtems_signal_set _signals; \
  ISR_Level              _level; \
   \
  _ISR_Disable( _level ); \
    _signals                        = (_information)->signals_pending; \
    (_information)->signals_pending = (_information)->signals_posted; \
    (_information)->signals_posted  = _signals; \
  _ISR_Enable( _level ); \
}

/*PAGE
 *
 *  _ASR_Is_null_handler
 *
 */

#define _ASR_Is_null_handler( _asr_handler ) \
  ( (_asr_handler) == NULL )

/*PAGE
 *
 *  _ASR_Are_signals_pending
 *
 */

#define _ASR_Are_signals_pending( _information ) \
 ( (_information)->signals_posted != 0 )

/*PAGE
 *
 *  _ASR_Post_signals
 *
 */

#define _ASR_Post_signals( _signals, _signal_set ) \
  do { \
     ISR_Level _level; \
     \
     _ISR_Disable( _level ); \
       *(_signal_set) |= (_signals); \
     _ISR_Enable( _level ); \
  } while ( 0 )

#endif
/* end of include file */
