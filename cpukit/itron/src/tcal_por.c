/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/itron.h>

#include <rtems/itron/port.h>

/*
 *  tcal_por - Call Port for Rendezvous with Timeout
 */

ER tcal_por(
  VP    msg,
  INT  *p_rmsgsz,
  ID    porid,
  UINT  calptn,
  INT   cmsgsz,
  TMO   tmout
)
{
  return E_OK;
}
