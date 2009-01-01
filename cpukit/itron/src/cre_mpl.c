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
 *  cre_mpl - Create Variable-Size Memorypool
 */

ER cre_mpl(
  ID      mplid __attribute__((unused)),
  T_CMPL *pk_cmpl __attribute__((unused))
)
{
  return E_OK;
}
