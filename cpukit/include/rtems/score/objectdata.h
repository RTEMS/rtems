/**
 * @file
 *
 * @ingroup ScoreObject
 *
 * @brief Object Handler Data Structures
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECTDATA_H
#define _RTEMS_SCORE_OBJECTDATA_H

#include <rtems/score/object.h>
#include <rtems/score/chain.h>
#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreObject
 *
 * @{
 */

/**
 *  The following defines the Object Control Block used to manage
 *  each object local to this node.
 */
typedef struct {
  /** This is the chain node portion of an object. */
  Chain_Node     Node;
  /** This is the object's ID. */
  Objects_Id     id;
  /** This is the object's name. */
  Objects_Name   name;
} Objects_Control;

#if defined( RTEMS_MULTIPROCESSING )
/**
 * @brief This defines the Global Object Control Block used to manage objects
 * resident on other nodes.
 */
typedef struct {
  /**
   * @brief Nodes to manage active and inactive global objects.
   */
  union {
    /**
     * @brief Inactive global objects reside on a chain.
     */
    Chain_Node Inactive;

    struct {
      /**
       * @brief Node to lookup an active global object by identifier.
       */
      RBTree_Node Id_lookup;

      /**
       * @brief Node to lookup an active global object by name.
       */
      RBTree_Node Name_lookup;
    } Active;
  } Nodes;

  /**
   * @brief The global object identifier.
   */
  Objects_Id id;

  /**
   * @brief The global object name.
   *
   * Using an unsigned thirty two bit value is broken but works.  If any API is
   * MP with variable length names .. BOOM!!!!
   */
  uint32_t name;
} Objects_MP_Control;
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
