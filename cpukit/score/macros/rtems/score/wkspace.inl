/*  wkspace.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the RAM Workspace Handler.
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

#ifndef __WORKSPACE_inl
#define __WORKSPACE_inl

/*PAGE
 *
 *  _Workspace_Allocate
 *
 */

#define _Workspace_Allocate( _size ) \
   _Heap_Allocate( &_Workspace_Area, (_size) )

/*PAGE
 *
 *  _Workspace_Free
 *
 */

#define _Workspace_Free( _block ) \
   _Heap_Free( &_Workspace_Area, (_block) )

#endif
/* end of include file */
