/*  copyrt.h
 *
 *  This include file contains the copyright notice for RTEMS
 *  which is included in every binary copy of the executive.
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

#ifndef __RTEMS_COPYRIGHT_h
#define __RTEMS_COPYRIGHT_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef INIT

const char _Copyright_Notice[] =
"COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.\n\
On-Line Applications Research Corporation (OAR).\n\
All rights assigned to U.S. Government, 1994.\n";

#else

extern const char _Copyright_Notice[];

#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
