/*  _Shm_isr()
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include "shm.h"

rtems_isr Shm_isr(
  rtems_vector_number vector
)
{
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
}
