/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 *  _Objects_Copy_name_raw
 *
 *  XXX
 */
 
void _Objects_Copy_name_raw(
  void       *source,
  void       *destination,
  unsigned32  length
)
{
  unsigned32 *source_p = (unsigned32 *) source;
  unsigned32 *destination_p = (unsigned32 *) destination;
  unsigned32  tmp_length = length / OBJECTS_NAME_ALIGNMENT;
 
  while ( tmp_length-- )
    *destination_p++ = *source_p++;
}
