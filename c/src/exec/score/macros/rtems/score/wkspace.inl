/*  wkspace.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the RAM Workspace Handler.
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
