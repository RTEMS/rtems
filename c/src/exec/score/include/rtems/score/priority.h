/*  priority.h
 *
 *  This include file contains all thread priority manipulation routines.
 *  This Handler provides mechanisms which can be used to
 *  initialize and manipulate thread priorities.
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

#ifndef __PRIORITY_h
#define __PRIORITY_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following type defines the control block used to manage
 *  thread priorities.
 *
 *  NOTE: Priority 0 is reserved for internal threads only.
 */

typedef unsigned32 Priority_Control;

#define PRIORITY_MINIMUM      0         /* highest thread priority */
#define PRIORITY_MAXIMUM      255       /* lowest thread priority */

/*
 *  The following record defines the information associated with
 *  each thread to manage its interaction with the priority bit maps.
 */

typedef struct {
  Priority_Bit_map_control *minor;        /* addr of minor bit map slot */
  Priority_Bit_map_control  ready_major;  /* priority bit map ready mask */
  Priority_Bit_map_control  ready_minor;  /* priority bit map ready mask */
  Priority_Bit_map_control  block_major;  /* priority bit map block mask */
  Priority_Bit_map_control  block_minor;  /* priority bit map block mask */
}   Priority_Information;

/*
 *  The following data items are the priority bit map.
 *  Each of the sixteen bits used in the _Priority_Major_bit_map is
 *  associated with one of the sixteen entries in the _Priority_Bit_map.
 *  Each bit in the _Priority_Bit_map indicates whether or not there are
 *  threads ready at a particular priority.  The mapping of
 *  individual priority levels to particular bits is processor
 *  dependent as is the value of each bit used to indicate that
 *  threads are ready at that priority.
 */

EXTERN volatile Priority_Bit_map_control _Priority_Major_bit_map;
EXTERN Priority_Bit_map_control _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

/*
 *  The definition of the Priority_Bit_map_control type is CPU dependent.
 *
 */

/*
 *  _Priority_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

STATIC INLINE void _Priority_Handler_initialization( void );

/*
 *  _Priority_Is_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_priority if valid for a
 *  user task, and FALSE otherwise.
 */

STATIC INLINE boolean _Priority_Is_valid (
  Priority_Control the_priority
);

/*
 *  _Priority_Major
 *
 *  DESCRIPTION:
 *
 *  This function returns the major portion of the_priority.
 */

STATIC INLINE unsigned32 _Priority_Major (
  Priority_Control the_priority
);

/*
 *  _Priority_Minor
 *
 *  DESCRIPTION:
 *
 *  This function returns the minor portion of the_priority.
 */

STATIC INLINE unsigned32 _Priority_Minor (
  Priority_Control the_priority
);

/*
 *  _Priority_Add_to_bit_map
 *
 *  DESCRIPTION:
 *
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been readied.
 */

STATIC INLINE void _Priority_Add_to_bit_map (
  Priority_Information *the_priority_map
);

/*
 *  _Priority_Remove_from_bit_map
 *
 *  DESCRIPTION:
 *
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been removed from the
 *  ready state.
 */

STATIC INLINE void _Priority_Remove_from_bit_map (
  Priority_Information *the_priority_map
);

/*
 *  _Priority_Get_highest
 *
 *  DESCRIPTION:
 *
 *  This function returns the priority of the highest priority
 *  ready thread.
 */

STATIC INLINE Priority_Control _Priority_Get_highest( void );

/*
 *  _Priority_Initialize_information
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_priority_map so that it
 *  contains the information necessary to manage a thread
 *  at new_priority.
 */

STATIC INLINE void _Priority_Initialize_information(
  Priority_Information *the_priority_map,
  Priority_Control      new_priority
);

/*
 *  _Priority_Is_group_empty
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority GROUP is empty, and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Priority_Is_group_empty (
  Priority_Control      the_priority
);

#include <rtems/core/priority.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
