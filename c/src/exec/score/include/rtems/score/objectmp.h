/*  objectmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of Global RTEMS Objects.
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

#ifndef __RTEMS_OBJECTS_MP_h
#define __RTEMS_OBJECTS_MP_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This defines the Global Object Control Block used to manage
 *  objects resident on other nodes.
 */

typedef struct {
  Objects_Control Object;
  unsigned32      name;     /* XXX broken but works */
  /* XXX If any API is MP with variable length names .. BOOM!!!! */
}   Objects_MP_Control;

/*
 *  _Objects_MP_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine intializes the inactive global object chain
 *  based on the maximum number of global objects configured.
 */

void _Objects_MP_Handler_initialization (
  unsigned32 node,
  unsigned32 maximum_nodes,
  unsigned32 maximum_global_objects
);

/*
 *  _Objects_MP_Allocate_global_object
 *
 *  DESCRIPTION:
 *
 *  This function allocates a Global Object control block.
 */

STATIC INLINE Objects_MP_Control *_Objects_MP_Allocate_global_object (
  void
);

/*
 *  _Objects_MP_Free_global_object
 *
 *  DESCRIPTION:
 *
 *  This routine deallocates a Global Object control block.
 */

STATIC INLINE void _Objects_MP_Free_global_object (
  Objects_MP_Control *the_object
);

/*
 *  _Objects_MP_Is_null_global_object
 *
 *  DESCRIPTION:
 *
 *  This function returns whether the global object is NULL or not.
 */

STATIC INLINE boolean _Objects_MP_Is_null_global_object (
  Objects_MP_Control *the_object
);

/*PAGE
 *
 *  _Objects_MP_Open
 *
 *  DESCRIPTION:
 *
 *  This routine place the specified global object in the 
 *  specified information table.  
 */
 
void _Objects_MP_Open (
  Objects_Information *information,
  Objects_MP_Control  *the_global_object,
  unsigned32           the_name,      /* XXX -- wrong for variable */
  Objects_Id           the_id
);

/*
 *  _Objects_MP_Allocate_and_open
 *
 *  DESCRIPTION:
 *
 *  This routine allocates a global object control block
 *  and places it in the specified information table.  If the
 *  allocation fails, then is_fatal_error determines the
 *  error processing actions taken.
 */

boolean _Objects_MP_Allocate_and_open (
  Objects_Information *information,
  unsigned32           the_name,     /* XXX -- wrong for variable length */
  Objects_Id           the_id,
  boolean              is_fatal_error
);

/*
 *  _Objects_MP_Close
 *
 *  DESCRIPTION:
 *
 *  This routine removes a global object from the specified
 *  information table and deallocates the global object control block.
 */

void _Objects_MP_Close (
  Objects_Information *information,
  Objects_Id           the_id
);

/*
 *  _Objects_MP_Global_name_search
 *
 *  DESCRIPTION:
 *
 *  This routine looks for the object with the_name in the global
 *  object tables indicated by information.  It returns the ID of the
 *  object with that name if one is found.
 */

Objects_Name_to_id_errors _Objects_MP_Global_name_search (
  Objects_Information *information,
  Objects_Name         the_name,
  unsigned32           nodes_to_search,
  Objects_Id          *the_id
);

/*
 *  _Objects_MP_Is_remote
 *
 *  DESCRIPTION:
 *
 *  This function searches the Global Object Table managed
 *  by information for the object indicated by ID.  If the object
 *  is found, then location is set to objects_remote, otherwise
 *  location is set to objects_error.  In both cases, the_object
 *  is undefined.
 */

void _Objects_MP_Is_remote (
  Objects_Information  *information,
  Objects_Id            the_id,
  Objects_Locations    *location,
  Objects_Control     **the_object
);

/*
 *  The following chain header is used to manage the set of
 *  inactive global object control blocks.
 */

EXTERN unsigned32     _Objects_MP_Maximum_global_objects;
EXTERN Chain_Control  _Objects_MP_Inactive_global_objects;

#include <rtems/score/objectmp.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
