/*  macros/attr.h
 *
 *  This include file contains all of the inlined routines associated
 *  with attributes.
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
