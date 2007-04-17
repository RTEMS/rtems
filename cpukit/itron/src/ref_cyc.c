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

#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/itron/time.h>

/*
 *  ref_cyc - Reference Cyclic Handler Status
 */

ER ref_cyc(
  T_RCYC *pk_rcyc,
  HNO     cycno
)
{
#if 0
  int timeElapse_since_actCyclic;
#endif
  T_DCYC *pk_dcyc;
#if 0
  Watchdog_Control *object;
#endif

/* XXX */ pk_dcyc = 0;

/* XXX will to use a "get" routine to map from id to object pointer */
/* XXX and the object pointer should be of type specific to this manager */
#if 0
  if( object->Object_ID == cycno))
#else
  if ( 1 )
#endif
  {
     pk_rcyc->exinf = pk_dcyc->exinf;
     pk_rcyc->cycact = pk_dcyc->cycact;
#if 0
     pk_rcyc->lfttim = pk_dcyc->cyctim - timeElapse_since_actCyclic;
#endif

     return E_OK;
  }
  else
     return E_NOEXS;
}
