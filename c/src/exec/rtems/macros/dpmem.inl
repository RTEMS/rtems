/*  macros/dpmem.h
 *
 *  This include file contains the inline routine used in conjunction
 *  with the Dual Ported Memory Manager
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
