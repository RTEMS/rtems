/*
 *  ITRON Message Buffer Manager
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*
 *  psnd_mbf - Poll and Send Message to MessageBuffer
 */

ER psnd_mbf(
  ID  mbfid,
  VP  msg,
  INT msgsz
)
{
  return tsnd_mbf( mbfid, msg, msgsz, TMO_POL ); 
}
