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

#include "system.h"

rtems_asr Process_asr(
    rtems_signal_set signal
)
{
  if ( signal != expected_signal ) {
     printf(
       "ERROR: I was expecting signal 0x%.8x got 0x%.8x\n",
       expected_signal,
       signal
     );
     rtems_fatal_error_occurred( 0xf0000 );
  }
  signal_caught = 1;
}
