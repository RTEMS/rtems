/**
 *  @file
 *
 *  @brief MP Support
 *  @ingroup ClassicMP
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/mp.h>
#include <rtems/score/mpciimpl.h>

void _Multiprocessing_Manager_initialization ( void )
{
}

void rtems_multiprocessing_announce ( void )
{
  _MPCI_Announce();
}

/* end of file */
