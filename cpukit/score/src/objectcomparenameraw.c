/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 *  _Objects_Compare_name_raw
 *
 *  XXX
 */
 
boolean _Objects_Compare_name_raw(
  void       *name_1,
  void       *name_2,
  unsigned32  length
)
{
  unsigned32 *name_1_p = (unsigned32 *) name_1;
  unsigned32 *name_2_p = (unsigned32 *) name_2;
  unsigned32  tmp_length = length / OBJECTS_NAME_ALIGNMENT;
 
  while ( tmp_length-- )
    if ( *name_1_p++ != *name_2_p++ )
      return FALSE;

  return TRUE;
}
