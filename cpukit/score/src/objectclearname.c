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
 *  _Objects_Clear_name
 *
 *  This method clears the specified name so that no caller can do a name to
 *  ID/object lookup past this point.
 */

void _Objects_Clear_name(
  void       *name,
  uint32_t    length
)
{
  uint32_t    index;
  uint32_t    maximum = length / OBJECTS_NAME_ALIGNMENT;
  uint32_t   *name_ptr = (uint32_t   *) name;

  for ( index=0 ; index < maximum ; index++ ) 
    *name_ptr++ = 0;
}
