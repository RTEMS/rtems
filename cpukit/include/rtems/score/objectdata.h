/**
 * @file
 *
 * @ingroup RTEMSScoreObject
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
 * @addtogroup RTEMSScoreObject
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
  OBJECTS_INTERNAL_NO_CLASS = 0,

  /* Must be one, see __Thread_Get_objects_information() */
  OBJECTS_INTERNAL_THREADS = 1
} Objects_Internal_API;

/**
 *  This enumerated type is used in the class field of the object ID
 *  for the RTEMS Classic API.
 */
typedef enum {
  OBJECTS_CLASSIC_NO_CLASS = 0,

  /* Must be one, see __Thread_Get_objects_information() */
  OBJECTS_RTEMS_TASKS = 1,

  OBJECTS_RTEMS_TIMERS,
  OBJECTS_RTEMS_SEMAPHORES,
  OBJECTS_RTEMS_MESSAGE_QUEUES,
  OBJECTS_RTEMS_PARTITIONS,
  OBJECTS_RTEMS_REGIONS,
  OBJECTS_RTEMS_PORTS,
  OBJECTS_RTEMS_PERIODS,
  OBJECTS_RTEMS_EXTENSIONS,
  OBJECTS_RTEMS_BARRIERS
} Objects_Classic_API;

/**
 *  This enumerated type is used in the class field of the object ID
 *  for the POSIX API.
 */
typedef enum {
  OBJECTS_POSIX_NO_CLASS = 0,

  /* Must be one, see __Thread_Get_objects_information() */
  OBJECTS_POSIX_THREADS = 1,

  OBJECTS_POSIX_KEYS,
  OBJECTS_POSIX_MESSAGE_QUEUES,
  OBJECTS_POSIX_SEMAPHORES,
  OBJECTS_POSIX_TIMERS,
  OBJECTS_POSIX_SHMS
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
 * @brief The information structure used to manage each API class of objects.
 *
 * If objects for the API class are configured, an instance of this structure
 * is statically allocated and pre-initialized by OBJECTS_INFORMATION_DEFINE()
 * through <rtems/confdefs.h>.  The RTEMS library contains a statically
 * allocated and pre-initialized instance for each API class providing zero
 * objects, see OBJECTS_INFORMATION_DEFINE_ZERO().
 */
typedef struct {
  /**
   * @brief This is the maximum valid ID of this object API class.
   *
   * This member is statically initialized and provides also the object API,
   * class and multiprocessing node information.
   *
   * It is used by _Objects_Get() to validate an object ID.
   */
  Objects_Id maximum_id;

  /**
   * @brief This points to the table of local object control blocks.
   *
   * This member is statically initialized.  In case objects for this API class
   * are configured, it initially points to a statically allocated table
   * defined by <rtems/confdefs.h>.  _Objects_Extend_information() may replace
   * the table with a larger one on demand.
   */
  Objects_Control **local_table;

  /**
   * @brief This is the number of object control blocks on the inactive chain.
   *
   * This member is only used if unlimited objects are configured for this API
   * class.  It is used to trigger calls to _Objects_Shrink_information() in
   * _Objects_Free().
   */
  Objects_Maximum inactive;

  /**
   * @brief This is the number of object control blocks in an allocation block.
   *
   * This member is statically initialized and read-only.  It is only used if
   * unlimited objects are configured for this API class.  It defines the count
   * of object control blocks used to extend and shrink this API class.
   */
  Objects_Maximum objects_per_block;

  /**
   * @brief This is the size in bytes of each object control block.
   *
   * This member is statically initialized and read-only.
   */
  uint16_t object_size;

  /**
   * @brief This is the maximum length of names.
   *
   * This member is statically initialized and read-only.  A length of zero
   * indicates that this API class has a no string name
   * (OBJECTS_NO_STRING_NAME).
   */
  uint16_t name_length;

  /**
   * @brief This is the chain of inactive object control blocks.
   *
   * This member is statically initialized to an empty chain.  The
   * _Objects_Initialize_information() will populate this chain with the
   * object control blocks initially configured.
   */
  Chain_Control Inactive;

  /**
   * @brief This is the number of inactive object control blocks per allocation
   * block.
   *
   * It is only used if unlimited objects are configured for this API class.
   */
  Objects_Maximum *inactive_per_block;

  /**
   * @brief This is a table to allocation blocks of object control blocks.
   *
   * It is only used if unlimited objects are configured for this API class.
   * The object control blocks extend and shrink by these allocation blocks.
   */
  Objects_Control **object_blocks;

  /**
   * @brief This points to the object control blocks initially available.
   *
   * This member is statically initialized and read-only.  In case objects for
   * this API class are configured, it points to a statically allocated table
   * of object control blocks defined by <rtems/confdefs.h>, otherwise this
   * member is NULL.
   */
  Objects_Control *initial_objects;

#if defined(RTEMS_MULTIPROCESSING)
  /**
   * @brief This method is used by _Thread_queue_Extract_with_proxy().
   *
   * This member is statically initialized and read-only.
   */
  Objects_Thread_queue_Extract_callout extract;

  /**
   * @brief The global objects of this object information sorted by object ID.
   *
   * This member is statically initialized to an empty tree.  The
   * _Objects_MP_Open() and _Objects_MP_Close() functions alter this tree.
   */
  RBTree_Control Global_by_id;

  /**
   * @brief The global objects of this object information sorted by object
   * name.
   *
   * This member is statically initialized to an empty tree.  The
   * _Objects_MP_Open() and _Objects_MP_Close() functions alter this tree.
   *
   * Objects with the same name are sorted according to their ID.
   */
  RBTree_Control Global_by_name;
#endif
} Objects_Information;

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
