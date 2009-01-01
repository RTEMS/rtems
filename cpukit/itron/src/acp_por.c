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
 *  acp_por - Accept Port for Rendezvous Poll
 */

ER acp_por(
  RNO  *p_rdvno __attribute__((unused)),
  VP    msg __attribute__((unused)),
  INT  *p_cmsgsz __attribute__((unused)),
  ID    porid __attribute__((unused)),
  UINT  acpptn __attribute__((unused))
)
{
  return E_OK;
}
