/*
 *  ITRON Message Buffer Manager
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*
 *  snd_mbf - Send Message to MessageBuffer
 */

ER snd_mbf(
  ID  mbfid,
  VP  msg,
  INT msgsz
)
{
  return tsnd_mbf( mbfid, msg, msgsz, TMO_FEVR ); 
}
