/*
 *  ITRON Message Buffer Manager
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*
 *  prcv_mbf - Poll and Receive Message from MessageBuffer
 */

ER prcv_mbf(
  VP   msg,
  INT *p_msgsz,
  ID   mbfid
)
{
  return trcv_mbf(msg, p_msgsz, mbfid, TMO_POL);
}
