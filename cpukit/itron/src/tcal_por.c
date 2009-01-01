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
  VP    msg __attribute__((unused)),
  INT  *p_rmsgsz __attribute__((unused)),
  ID    porid __attribute__((unused)),
  UINT  calptn __attribute__((unused)),
  INT   cmsgsz __attribute__((unused)),
  TMO   tmout __attribute__((unused))
)
{
  return E_OK;
}
