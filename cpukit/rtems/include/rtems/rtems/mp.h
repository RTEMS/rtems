/*  mp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
