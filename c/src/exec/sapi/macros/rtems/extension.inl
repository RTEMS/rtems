/*  extension.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the Extension Manager.
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
