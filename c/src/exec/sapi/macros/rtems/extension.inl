/*  extension.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the Extension Manager.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __EXTENSION_inl
#define __EXTENSION_inl

/*PAGE
 *
 *  _Extension_Allocate
 *
 */

#define _Extension_Allocate() \
  (Extension_Control *) _Objects_Allocate( &_Extension_Information )

/*PAGE
 *
 *  _Extension_Free
 *
 */

#define _Extension_Free( _the_extension ) \
  _Objects_Free( &_Extension_Information, &(_the_extension)->Object )

/*PAGE
 *
 *  _Extension_Get
 *
 */

#define _Extension_Get( _id, _location ) \
  (Extension_Control *) \
    _Objects_Get( &_Extension_Information, (_id), (_location) )

/*PAGE
 *
 *  _Extension_Is_null
 *
 */

#define _Extension_Is_null( _the_extension ) \
  ( (_the_extension) == NULL )

#endif
/* end of include file */
