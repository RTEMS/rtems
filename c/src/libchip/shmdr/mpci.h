/*  mpci.h
 *
 *  This include file contains all the renaming necessary to
 *  have an application use the Shared Memory Driver as its
 *  sole mechanism for MPCI.
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

#ifndef __MPCI_h
#define __MPCI_h

#ifdef __cplusplus
extern "C" {
#endif

#include "shm.h"

#define MPCI_Initialization( _configuration ) \
  Shm_Initialization( _configuration )

#define MPCI_Get_packet( _the_packet ) \
  Shm_Get_packet( _the_packet )

#define MPCI_Return_packet( _the_packet ) \
  Shm_Return_packet( _the_packet )

#define MPCI_Receive_packet( _the_packet ) \
  Shm_Receive_packet( _the_packet )

#define MPCI_Send_packet( _destination, _the_packet ) \
  Shm_Send_packet( _destination, _the_packet )

/* Unnecessary... mapped in shm.h
#define MPCI_Fatal( _the_error ) \
  Shm_Fatal( _the_error )
*/

#define MPCI_Enable_statistics()

#define MPCI_Print_statistics() \
  Shm_Print_statistics()

/* no need to rename the MPCI_Table either */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
