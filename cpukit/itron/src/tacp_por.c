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
 *  tacp_por - Accept Port for Rendezvous with Timeout
 */

ER tacp_por(
  RNO  *p_rdvno,
  VP    msg,
  INT  *p_cmsgsz,
  ID    porid,
  UINT  acpptn,
  TMO   tmout
)
{
  return E_OK;
}
