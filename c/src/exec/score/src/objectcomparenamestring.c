/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
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

#include <string.h>

/*PAGE
 *
 *  _Objects_Compare_name_string
 *
 * This routine compares the name of an object with the specified string.
 *
 * Input parameters:
 *   name_1 - one name
 *   name_2 - other name
 *   length - maximum length to compare
 *
 * Output parameters:
 *   returns  - TRUE on a match
 */
 
boolean _Objects_Compare_name_string(
  void       *name_1,
  void       *name_2,
  unsigned32  length
)
{
  if ( !strncmp( name_1, name_2, length ) )
    return TRUE;
  return FALSE;
}
