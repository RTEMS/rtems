/*  macros/coresem.h
 *
 *  This include file contains all of the inlined routines associated
 *  with core semaphores.
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
