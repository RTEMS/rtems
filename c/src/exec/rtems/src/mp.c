/*
 *  Multiprocessing Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/mp.h>
#include <rtems/core/mpci.h>

/*PAGE
 *
 *  _Multiprocessing_Manager_initialization
 *
 */

void _Multiprocessing_Manager_initialization ( void )
{
}

/*PAGE
 *
 *  rtems_multiprocessing_announce
 *
 */

void rtems_multiprocessing_announce ( void )
{
  _MPCI_Announce();
}

/* end of file */
