/*  Process_asr
 *
 *  This routine performs the processing for task 1's RTEMS_ASR.  It is called
 *  by an assembly routine which saves the necessary registers.
 *
 *  Input parameters:
 *    signal - signal set
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "tmacros.h"

rtems_asr Process_asr(
    rtems_signal_set signal
)
{
  if ( signal != expected_signal ) {
     printf(
       "ERROR: I was expecting signal 0x%.8" PRIxrtems_signal_set " got 0x%.8" PRIxrtems_signal_set "\n",
       expected_signal,
       signal
     );
     rtems_fatal_error_occurred( 0xf0000 );
  }
  signal_caught = 1;
}
