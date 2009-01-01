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

#include <rtems/itron/vmempool.h>

/*
 *  ref_mpl - Reference Variable-Size Memorypool Status
 */

ER ref_mpl(
  T_RMPL *pk_rmpl __attribute__((unused)),
  ID      mplid __attribute__((unused))
)
{
  return E_OK;
}
