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
#include <rtems/score/chainimpl.h>
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

/**
 *  This enumerated type is used in the class field of the object ID
 *  for RTEMS internal object classes.
 */
typedef enum {
  OBJECTS_INTERNAL_NO_CLASS =  0,
  OBJECTS_INTERNAL_THREADS  =  1
} Objects_Internal_API;

/**
 *  This enumerated type is used in the class field of the object ID
 *  for the RTEMS Classic API.
 */
typedef enum {
  OBJECTS_CLASSIC_NO_CLASS     = 0,
  OBJECTS_RTEMS_TASKS          = 1,
  OBJECTS_RTEMS_TIMERS         = 2,
  OBJECTS_RTEMS_SEMAPHORES     = 3,
  OBJECTS_RTEMS_MESSAGE_QUEUES = 4,
  OBJECTS_RTEMS_PARTITIONS     = 5,
  OBJECTS_RTEMS_REGIONS        = 6,
  OBJECTS_RTEMS_PORTS          = 7,
  OBJECTS_RTEMS_PERIODS        = 8,
  OBJECTS_RTEMS_EXTENSIONS     = 9,
  OBJECTS_RTEMS_BARRIERS       = 10
} Objects_Classic_API;

/**
 *  This enumerated type is used in the class field of the object ID
 *  for the POSIX API.
 */
typedef enum {
  OBJECTS_POSIX_NO_CLASS            = 0,
  OBJECTS_POSIX_THREADS             = 1,
  OBJECTS_POSIX_KEYS                = 2,
  OBJECTS_POSIX_INTERRUPTS          = 3,
  OBJECTS_POSIX_MESSAGE_QUEUES      = 5,
  OBJECTS_POSIX_SEMAPHORES          = 7,
  OBJECTS_POSIX_TIMERS              = 9,
  OBJECTS_POSIX_SHMS                = 12
} Objects_POSIX_API;

/**
 * @brief Constant for the object information string name length to indicate
 * that this object class has no string names.
 */
#define OBJECTS_NO_STRING_NAME 0

#if defined( RTEMS_MULTIPROCESSING )
struct _Thread_Control;

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

/**
 *  The following type defines the callout used when a local task
 *  is extracted from a remote thread queue (i.e. it's proxy must
 *  extracted from the remote queue).
 */
typedef void ( *Objects_Thread_queue_Extract_callout )(
  struct _Thread_Control *,
  Objects_Id
);
#endif

/**
 *  The following defines the structure for the information used to
 *  manage each class of objects.
 */
typedef struct {
  /** This is the maximum valid id of this object class. */
  Objects_Id        maximum_id;
  /** This points to the table of local objects. */
  Objects_Control **local_table;
  /** This is the number of objects on the Inactive list. */
  Objects_Maximum   inactive;
  /** This is the number of objects in a block. */
  Objects_Maximum   objects_per_block;
  /** This is the size in bytes of each object instance. */
  uint16_t          object_size;
  /**
   * @brief This is the maximum length of names.
   *
   * A length of zero indicates that this object has a no string name
   * (OBJECTS_NO_STRING_NAME).
   */
  uint16_t          name_length;
  /** This is the chain of inactive control blocks. */
  Chain_Control     Inactive;
  /** This is the number of inactive objects per block. */
  Objects_Maximum  *inactive_per_block;
  /** This is a table to the chain of inactive object memory blocks. */
  Objects_Control **object_blocks;
  Objects_Control  *initial_objects;
  #if defined(RTEMS_MULTIPROCESSING)
    /** This is this object class' method called when extracting a thread. */
    Objects_Thread_queue_Extract_callout extract;

    /**
     * @brief The global objects of this object information sorted by object
     * identifier.
     */
    RBTree_Control   Global_by_id;

    /**
     * @brief The global objects of this object information sorted by object
     * name.
     *
     * Objects with the same name are sorted according to their identifier.
     */
    RBTree_Control   Global_by_name;
  #endif
}   Objects_Information;

#if defined(RTEMS_MULTIPROCESSING)
#define OBJECTS_INFORMATION_MP( name, extract ) \
  , \
  extract, \
  RBTREE_INITIALIZER_EMPTY( name.Global_by_id ), \
  RBTREE_INITIALIZER_EMPTY( name.Global_by_name )
#else
#define OBJECTS_INFORMATION_MP( name, extract )
#endif

/**
 * @brief Statically initializes an objects information.
 *
 * The initialized objects information contains no objects.
 *
 * @param name The object class C designator namespace prefix, e.g. _Semaphore.
 * @param api The object API number, e.g. OBJECTS_CLASSIC_API.
 * @param cls The object class number, e.g. OBJECTS_RTEMS_SEMAPHORES.
 * @param nl The object name string length, use OBJECTS_NO_STRING_NAME for
 *   objects without a string name.
 */
#define OBJECTS_INFORMATION_DEFINE_ZERO( name, api, cls, nl ) \
Objects_Information name##_Information = { \
  _Objects_Build_id( api, cls, 1, 0 ), \
  NULL, \
  0, \
  0, \
  0, \
  nl, \
  CHAIN_INITIALIZER_EMPTY( name##_Information.Inactive ), \
  NULL, \
  NULL, \
  NULL \
  OBJECTS_INFORMATION_MP( name##_Information, NULL ) \
}

/**
 * @brief Statically initializes an objects information.
 *
 * The initialized objects information references a table with statically
 * allocated objects as specified by the object maximum parameter.  These
 * objects must be registered via a call to _Objects_Information().
 *
 * @param name The object class C designator namespace prefix, e.g. _Semaphore.
 * @param api The object API number, e.g. OBJECTS_CLASSIC_API.
 * @param cls The object class number, e.g. OBJECTS_RTEMS_SEMAPHORES.
 * @param type The object class type.
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 *   may be set).
 * @param nl The object name string length, use OBJECTS_NO_STRING_NAME for
 *   objects without a string name.
 * @param ex The optional object extraction method.  Used only if
 *   multiprocessing (RTEMS_MULTIPROCESSING) is enabled.
 */
#define OBJECTS_INFORMATION_DEFINE( name, api, cls, type, max, nl, ex ) \
static Objects_Control * \
name##_Local_table[ _Objects_Maximum_per_allocation( max ) ]; \
static type name##_Objects[ _Objects_Maximum_per_allocation( max ) ]; \
Objects_Information name##_Information = { \
  _Objects_Build_id( api, cls, 1, _Objects_Maximum_per_allocation( max ) ), \
  name##_Local_table, \
  0, \
  _Objects_Is_unlimited( max ) ? _Objects_Maximum_per_allocation( max ) : 0, \
  sizeof( type ), \
  nl, \
  CHAIN_INITIALIZER_EMPTY( name##_Information.Inactive ), \
  NULL, \
  NULL, \
  &name##_Objects[ 0 ].Object \
  OBJECTS_INFORMATION_MP( name##_Information, ex ) \
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
