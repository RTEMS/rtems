/*  macros/coresem.h
 *
 *  This include file contains all of the inlined routines associated
 *  with core semaphores.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __MACROS_CORE_SEMAPHORE_h
#define __MACROS_CORE_SEMAPHORE_h

/*PAGE
 *
 *  _CORE_semaphore_Is_priority
 *
 */

#define _CORE_semaphore_Is_priority( _the_attribute ) \
  ( (_the_attribute)->discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY )

/*PAGE
 *
 *  _Core_semaphore_Get_count
 *
 */
 
#define _Core_semaphore_Get_count( _the_semaphore ) \
  ( (_the_semaphore)->count )


#endif
/* end of include file */
