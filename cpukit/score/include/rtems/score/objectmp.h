/** 
 *  @file  rtems/score/objectmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of Global RTEMS Objects.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_OBJECTMP_H
#define _RTEMS_SCORE_OBJECTMP_H

/**
 *  @defgroup ScoreObjectMP Object Handler Multiprocessing Support
 *
 *  This group contains functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This defines the Global Object Control Block used to manage
 *  objects resident on other nodes.
 */
typedef struct {
  Objects_Control Object;
  uint32_t        name;     /* XXX broken but works */
  /* XXX If any API is MP with variable length names .. BOOM!!!! */
}   Objects_MP_Control;

/** @brief  Objects MP Handler initialization
 *
 *  This routine intializes the inactive global object chain
 *  based on the maximum number of global objects configured.
 */
void _Objects_MP_Handler_initialization (
  uint32_t   node,
  uint32_t   maximum_nodes,
  uint32_t   maximum_global_objects
);

/** @brief Objects MP Open
 *
 *  This routine place the specified global object in the
 *  specified information table.
 */

void _Objects_MP_Open (
  Objects_Information *information,
  Objects_MP_Control  *the_global_object,
  uint32_t             the_name,      /* XXX -- wrong for variable */
  Objects_Id           the_id
);

/** @brief  Objects MP Allocate and open
 *
 *  This routine allocates a global object control block
 *  and places it in the specified information table.  If the
 *  allocation fails, then is_fatal_error determines the
 *  error processing actions taken.
 */
boolean _Objects_MP_Allocate_and_open (
  Objects_Information *information,
  uint32_t             the_name,     /* XXX -- wrong for variable length */
  Objects_Id           the_id,
  boolean              is_fatal_error
);

/** @brief  Objects MP Close
 *
 *  This routine removes a global object from the specified
 *  information table and deallocates the global object control block.
 */
void _Objects_MP_Close (
  Objects_Information *information,
  Objects_Id           the_id
);

/** @brief  Objects MP Global name search
 *
 *  This routine looks for the object with the_name in the global
 *  object tables indicated by information.  It returns the ID of the
 *  object with that name if one is found.
 */
Objects_Name_or_id_lookup_errors _Objects_MP_Global_name_search (
  Objects_Information *information,
  Objects_Name         the_name,
  uint32_t             nodes_to_search,
  Objects_Id          *the_id
);

/** @brief  Objects MP Is remote
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
SCORE_EXTERN uint32_t       _Objects_MP_Maximum_global_objects;
SCORE_EXTERN Chain_Control  _Objects_MP_Inactive_global_objects;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/objectmp.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
