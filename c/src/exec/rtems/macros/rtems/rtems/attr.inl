/*  macros/attr.h
 *
 *  This include file contains all of the inlined routines associated
 *  with attributes.
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

#ifndef __MACROS_ATTRIBUTES_h
#define __MACROS_ATTRIBUTES_h

/*PAGE
 *
 *  _Attributes_Set
 */

#define _Attributes_Set( _new_attributes, _attribute_set ) \
  ( (_attribute_set) | (_new_attributes) )

/*PAGE
 *
 *  _Attributes_Clear
 */

#define _Attributes_Clear( _attribute_set, _mask ) \
  ( (_attribute_set) & ~(_mask) )

/*PAGE
 *
 *  _Attributes_Is_floating_point
 *
 */

#define _Attributes_Is_floating_point( _attribute_set ) \
  ( (_attribute_set) & RTEMS_FLOATING_POINT )

/*PAGE
 *
 *  _Attributes_Is_global
 *
 */

#define _Attributes_Is_global( _attribute_set ) \
  ( (_attribute_set) & RTEMS_GLOBAL )

/*PAGE
 *
 *  _Attributes_Is_priority
 *
 */

#define _Attributes_Is_priority( _attribute_set ) \
  ( (_attribute_set) & RTEMS_PRIORITY )

/*PAGE
 *
 *  _Attributes_Is_binary_semaphore
 *
 */

#define _Attributes_Is_binary_semaphore( _attribute_set ) \
  ( (_attribute_set) & RTEMS_BINARY_SEMAPHORE )

/*PAGE
 *
 *  _Attributes_Is_inherit_priority
 *
 */

#define _Attributes_Is_inherit_priority( _attribute_set ) \
  ( (_attribute_set) & RTEMS_INHERIT_PRIORITY )

/*PAGE
 *
 *  _Attributes_Is_priority_ceiling
 *
 */
 
#define _Attributes_Is_priority_ceiling( _attribute_set ) \
  ( (_attribute_set) & RTEMS_PRIORITY_CEILING )

#endif
/* end of include file */
