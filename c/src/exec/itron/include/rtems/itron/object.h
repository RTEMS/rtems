/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_ITRON_OBJECT_h_
#define __RTEMS_ITRON_OBJECT_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>

typedef Objects_Control ITRON_Objects_Control;

/*
 *  Maximum length of an ITRON object name
 *
 *  NOTE:  Since ITRON objects do not have names, then then length is 0.
 */

#define ITRON_MAXIMUM_NAME_LENGTH 0

/*
 *  _ITRON_Objects_Open
 *
 *  Make this ITRON object visible to the system.
 *
 *  NOTE:  This macro hides the fact that ITRON objects don't have names.
 */

#define _ITRON_Objects_Open( _the_information, _the_object ) \
    _Objects_Open( (_the_information), (_the_object), NULL )

/*
 *  _ITRON_Objects_Close
 *
 *  Make this ITRON object invisible from the system.  Usually used as
 *  the first step of deleting an object.
 */

#define _ITRON_Objects_Close( _the_information, _the_object ) \
    _Objects_Close( (_the_information), (_the_object) )

/*
 *  _ITRON_Objects_Allocate_by_index
 *
 *  Allocate the ITRON object specified by "_id".  The ITRON id is the
 *  index portion of the traditional RTEMS ID.  The Classic and POSIX
 *  APIs do not require that a specific object be allocated.
 */

#define _ITRON_Objects_Allocate_by_index( _the_information, _id, _sizeof ) \
    _Objects_Allocate_by_index( (_the_information), (_id), (_sizeof) )

/*
 *  _ITRON_Objects_Clarify_allocation_id_error
 *
 *  This function is invoked when an object allocation ID error
 *  occurs to determine the specific ITRON error code to return.
 */

#define _ITRON_Objects_Clarify_allocation_id_error( _the_information, _id ) \
  (((_id) < -4) ? E_OACV : /* attempt to access a "system object" */ \
  ((_id) <= 0) ? E_ID :    /* bogus index of 0 - -3 */ \
  ((_id) <= (_the_information)->maximum) ? E_OBJ : /* object is in use */ \
   E_ID)  /* simply a bad id */

/*
 *  _ITRON_Objects_Clarify_get_id_error
 *
 *  This function is invoked when an object get ID error
 *  occurs to determine the specific ITRON error code to return.
 */
 
#define _ITRON_Objects_Clarify_get_id_error( _the_information, _id ) \
  (((_id) < -4) ? E_OACV : /* attempt to access a "system object" */ \
  ((_id) <= 0) ? E_ID :    /* bogus index of 0 - -3 */ \
  ((_id) <= (_the_information)->maximum) ? E_NOEXS : /* does not exist */ \
   E_ID)  /* simply a bad id */


/*
 *  _ITRON_Objects_Free
 *
 *  Free this ITRON object to the pool of inactive objects.  This
 *  operation is the same as for the Classic and POSIX APIs.
 */

#define _ITRON_Objects_Free( _the_information, _the_object ) \
    _Objects_Free( (_the_information), (_the_object) )


/*
 *  _ITRON_Objects_Get
 *
 *  Obtain (get) the pointer to the control block for the object
 *  specified by "id".  The ITRON id passed in here is simply
 *  the "index" portion of the traditional RTEMS ID.  This
 *  requires that this operation be slightly different
 *  from the object get used by the Classic and POSIX APIs.
 */

#define _ITRON_Objects_Get( _the_information, _id, _the_object ) \
    _Objects_Get_by_index( (_the_information), (_id), (_the_object) )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

