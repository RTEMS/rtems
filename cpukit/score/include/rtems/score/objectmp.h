/**
 *  @file  rtems/score/objectmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of Global RTEMS Objects.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECTMP_H
#define _RTEMS_SCORE_OBJECTMP_H

/**
 *  @defgroup ScoreObjectMP Object Handler Multiprocessing Support
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which is used to manage
 *  objects which have been declared to be globally visible.  This handler
 *  knows objects from all of the nodes in the system.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This defines the Global Object Control Block used to manage
 *  objects resident on other nodes.  It is derived from Object.
 */
typedef struct {
  /** This is an object control structure. */
  Objects_Control Object;
  /** This is the name of the object.  Using an unsigned thirty two
   *  bit value is broken but works.  If any API is MP with variable
   *  length names .. BOOM!!!!
   */
  uint32_t        name;
}   Objects_MP_Control;

/**
 *  @brief  Objects MP Handler initialization
 *
 *  This routine intializes the inactive global object chain
 *  based on the maximum number of global objects configured.
 */
void _Objects_MP_Handler_initialization(void);

/**
 *  @brief  Objects MP Handler Early initialization
 *
 *  This routine intializes the global object node number
 *  used in the ID field of all objects.
 */
void _Objects_MP_Handler_early_initialization(void);

/**
 *  @brief Objects MP Open
 *
 *  This routine place the specified global object in the
 *  specified information table.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_global_object points to the object being opened.
 *  @param[in] the_name is the name of the object being opened.
 *  @param[in] the_id is the Id of the object being opened.
 *
 *  @todo This method only works for object types with 4 byte object names.
 *        It does not support variable length object names.
 */
void _Objects_MP_Open (
  Objects_Information *information,
  Objects_MP_Control  *the_global_object,
  uint32_t             the_name,
  Objects_Id           the_id
);

/**
 *  @brief  Objects MP Allocate and open
 *
 *  This routine allocates a global object control block
 *  and places it in the specified information table.  If the
 *  allocation fails, then is_fatal_error determines the
 *  error processing actions taken.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_name is the name of the object being opened.
 *  @param[in] the_id is the Id of the object being opened.
 *  @param[in] is_fatal_error is true if not being able to allocate the
 *             object is considered a fatal error.
 *
 *  @todo This method only works for object types with 4 byte object names.
 *        It does not support variable length object names.
 */
bool _Objects_MP_Allocate_and_open (
  Objects_Information *information,
  uint32_t             the_name,
  Objects_Id           the_id,
  bool                 is_fatal_error
);

/**
 *  @brief  Objects MP Close
 *
 *  This routine removes a global object from the specified
 *  information table and deallocates the global object control block.
 */
void _Objects_MP_Close (
  Objects_Information *information,
  Objects_Id           the_id
);

/**
 *  @brief  Objects MP Global name search
 *
 *  This routine looks for the object with the_name in the global
 *  object tables indicated by information.  It returns the ID of the
 *  object with that name if one is found.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_name is the name of the object being searched for.
 *  @param[in] nodes_to_search indicates the set of nodes to search.
 *  @param[in] the_id will contain the Id of the object if found.
 *
 *  @return This method returns one of the
 *          @ref Objects_Name_or_id_lookup_errors.  If successful, @a the_id
 *          will contain the Id of the object.
 */
Objects_Name_or_id_lookup_errors _Objects_MP_Global_name_search (
  Objects_Information *information,
  Objects_Name         the_name,
  uint32_t             nodes_to_search,
  Objects_Id          *the_id
);

/**
 *  @brief  Objects MP Is remote
 *
 *  This function searches the Global Object Table managed
 *  by information for the object indicated by ID.  If the object
 *  is found, then location is set to objects_remote, otherwise
 *  location is set to objects_error.  In both cases, the_object
 *  is undefined.
 *
 *  @param[in] information points to the object information table for this
 *             object class.
 *  @param[in] the_id is the Id of the object being opened.
 *  @param[in] location will contain the location of the object.
 *  @param[in] the_object will contain a pointer to the object.
 *
 *  @return This method fills in @a location to indicate successful location
 *          of the object or error.  On success, @a the_object will be
 *          filled in.
 */
void _Objects_MP_Is_remote (
  Objects_Information  *information,
  Objects_Id            the_id,
  Objects_Locations    *location,
  Objects_Control     **the_object
);

/**
 *  This is the maximum number of global objects configured.
 */
SCORE_EXTERN uint32_t       _Objects_MP_Maximum_global_objects;

/**
 *  The following chain header is used to manage the set of
 *  inactive global object control blocks.
 */
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
