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
 *  fwd_por - Forward Rendezvous to Other Port
 */

ER fwd_por(
  ID    porid __attribute__((unused)),
  UINT  calptn __attribute__((unused)),
  RNO   rdvno __attribute__((unused)),
  VP    msg __attribute__((unused)),
  INT   cmsgsz __attribute__((unused))
)
{
  return E_OK;
}
