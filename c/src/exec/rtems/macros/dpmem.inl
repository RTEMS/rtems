/*  macros/dpmem.h
 *
 *  This include file contains the inline routine used in conjunction
 *  with the Dual Ported Memory Manager
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

#ifndef __MACROS_DPMEM_h
#define __MACROS_DPMEM_h

/*PAGE
 *
 *  _Dual_ported_memory_Allocate
 *
 */

#define _Dual_ported_memory_Allocate() \
  (Dual_ported_memory_Control *) \
     _Objects_Allocate( &_Dual_ported_memory_Information )

/*PAGE
 *
 *  _Dual_ported_memory_Free
 *
 */

#define _Dual_ported_memory_Free( _the_port ) \
  _Objects_Free( &_Dual_ported_memory_Information, &(_the_port)->Object )

/*PAGE
 *
 *  _Dual_ported_memory_Get
 *
 */

#define _Dual_ported_memory_Get( _id, _location ) \
  (Dual_ported_memory_Control *) \
     _Objects_Get( &_Dual_ported_memory_Information, (_id), (_location) )


/*PAGE
 *
 *  _Dual_ported_memory_Is_null
 */

#define _Dual_ported_memory_Is_null( _the_port ) \
  ( (_the_port) == NULL )

#endif
/* end of include file */
