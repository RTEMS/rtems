/*  macros/attr.h
 *
 *  This include file contains all of the inlined routines associated
 *  with attributes.
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

#if defined(RTEMS_MULTIPROCESSING)
#define _Attributes_Is_global( _attribute_set ) \
  ( (_attribute_set) & RTEMS_GLOBAL )
#endif

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
  (((_attribute_set) & RTEMS_SEMAPHORE_CLASS) == RTEMS_BINARY_SEMAPHORE)

/*PAGE
 *
 *  _Attributes_Is_simple_binary_semaphore
 *
 */

#define _Attributes_Is_simple_binary_semaphore( _attribute_set ) \
  (((_attribute_set) & RTEMS_SEMAPHORE_CLASS) == RTEMS_SIMPLE_BINARY_SEMAPHORE)

/*PAGE
 *
 *  _Attributes_Is_counting_semaphore
 *
 */

#define _Attributes_Is_counting_semaphore( _attribute_set ) \
  (((_attribute_set) & RTEMS_SEMAPHORE_CLASS) == RTEMS_COUNTING_SEMAPHORE)

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

/*PAGE
 *
 *  _Attributes_Is_system_task
 *
 */
 
#define _Attributes_Is_system_task( _attribute_set ) \
  ( (_attribute_set) & RTEMS_SYSTEM_TASK )

#endif
/* end of include file */
