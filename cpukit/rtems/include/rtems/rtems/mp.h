/*  mp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Manager.
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

#ifndef __RTEMS_MP_h
#define __RTEMS_MP_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 *  _Multiprocessing_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Multiprocessing_Manager_initialization ( void );

/*
 *
 *  rtems_multiprocessing_announce
 *
 *  DESCRIPTION:
 *
 *  This routine implements the MULTIPROCESSING_ANNOUNCE directive.
 *  It is invoked by the MPCI layer to indicate that an MPCI packet
 *  has been received.
 */

void rtems_multiprocessing_announce ( void );

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
