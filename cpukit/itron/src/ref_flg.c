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

#include <rtems/itron/eventflags.h>

/*
 *  ref_flg - Reference Eventflag Status
 */

ER ref_flg(
  T_RFLG *pk_rflg __attribute__((unused)),
  ID      flgid __attribute__((unused))
)
{
  return E_OK;
}
