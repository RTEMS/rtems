/**
 * @file
 *
 * @brief Inlined Routines in the Object Handler
 *
 * This include file contains the static inline implementation of all
 * of the inlined routines in the Object Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECTIMPL_H
#define _RTEMS_SCORE_OBJECTIMPL_H

#include <rtems/score/object.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/threaddispatch.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreObject
 *
 * @{
 */

/**
 *  Functions which compare names are prototyped like this.
 */
typedef bool    (*Objects_Name_comparators)(
  void       * /* name_1 */,
  void       * /* name_2 */,
  uint16_t     /* length */
);

/**
 *  This enumerated type is used in the class field of the object ID
 *  for RTEMS internal object classes.
 */
typedef enum {
  OBJECTS_INTERNAL_NO_CLASS =  0,
  OBJECTS_INTERNAL_THREADS  =  1,
  OBJECTS_INTERNAL_MUTEXES  =  2
} Objects_Internal_API;

/** This macro is used to generically specify the last API index. */
#define OBJECTS_INTERNAL_CLASSES_LAST OBJECTS_INTERNAL_MUTEXES

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

/** This macro is used to generically specify the last API index. */
#define OBJECTS_RTEMS_CLASSES_LAST OBJECTS_RTEMS_BARRIERS

/**
 *  This enumerated type is used in the class field of the object ID
 *  for the POSIX API.
 */
typedef enum {
  OBJECTS_POSIX_NO_CLASS            = 0,
  OBJECTS_POSIX_THREADS             = 1,
  OBJECTS_POSIX_KEYS                = 2,
  OBJECTS_POSIX_INTERRUPTS          = 3,
  OBJECTS_POSIX_MESSAGE_QUEUE_FDS   = 4,
  OBJECTS_POSIX_MESSAGE_QUEUES      = 5,
  OBJECTS_POSIX_MUTEXES             = 6,
  OBJECTS_POSIX_SEMAPHORES          = 7,
  OBJECTS_POSIX_CONDITION_VARIABLES = 8,
  OBJECTS_POSIX_TIMERS              = 9,
  OBJECTS_POSIX_BARRIERS            = 10,
  OBJECTS_POSIX_SPINLOCKS           = 11,
  OBJECTS_POSIX_RWLOCKS             = 12
} Objects_POSIX_API;

/** This macro is used to generically specify the last API index. */
#define OBJECTS_POSIX_CLASSES_LAST OBJECTS_POSIX_RWLOCKS

/*
 * For fake objects, which have an object identifier, but no objects
 * information block.
 */
typedef enum {
  OBJECTS_FAKE_OBJECTS_NO_CLASS   = 0,
  OBJECTS_FAKE_OBJECTS_SCHEDULERS = 1
} Objects_Fake_objects_API;

/**
 *  This enumerated type lists the locations which may be returned
 *  by _Objects_Get.  These codes indicate the success of locating
 *  an object with the specified ID.
 */
typedef enum {
#if defined(RTEMS_MULTIPROCESSING)
  OBJECTS_REMOTE = 2,         /* object is remote */
#endif
  OBJECTS_LOCAL  = 0,         /* object is local */
  OBJECTS_ERROR  = 1          /* id was invalid */
} Objects_Locations;

/**
 *  The following type defines the callout used when a local task
 *  is extracted from a remote thread queue (i.e. it's proxy must
 *  extracted from the remote queue).
 */
typedef void ( *Objects_Thread_queue_Extract_callout )( void * );

/**
 *  The following defines the structure for the information used to
 *  manage each class of objects.
 */
typedef struct {
  /** This field indicates the API of this object class. */
  Objects_APIs      the_api;
  /** This is the class of this object set. */
  uint16_t          the_class;
  /** This is the minimum valid id of this object class. */
  Objects_Id        minimum_id;
  /** This is the maximum valid id of this object class. */
  Objects_Id        maximum_id;
  /** This is the maximum number of objects in this class. */
  Objects_Maximum   maximum;
  /** This is the true if unlimited objects in this class. */
  bool              auto_extend;
  /** This is the number of objects in a block. */
  Objects_Maximum   allocation_size;
  /** This is the size in bytes of each object instance. */
  size_t            size;
  /** This points to the table of local objects. */
  Objects_Control **local_table;
  /** This is the chain of inactive control blocks. */
  Chain_Control     Inactive;
  /** This is the number of objects on the Inactive list. */
  Objects_Maximum   inactive;
  /** This is the number of inactive objects per block. */
  uint32_t         *inactive_per_block;
  /** This is a table to the chain of inactive object memory blocks. */
  void            **object_blocks;
  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    /** This is true if names are strings. */
    bool              is_string;
  #endif
  /** This is the maximum length of names. */
  uint16_t          name_length;
  #if defined(RTEMS_MULTIPROCESSING)
    /** This is this object class' method called when extracting a thread. */
    Objects_Thread_queue_Extract_callout extract;
    /** This is this object class' pointer to the global name table */
    Chain_Control    *global_table;
  #endif
}   Objects_Information;

/**
 *  The following is referenced to the node number of the local node.
 */
#if defined(RTEMS_MULTIPROCESSING)
SCORE_EXTERN uint16_t       _Objects_Local_node;
#else
#define _Objects_Local_node ((uint16_t)1)
#endif

/**
 *  The following is referenced to the number of nodes in the system.
 */
#if defined(RTEMS_MULTIPROCESSING)
SCORE_EXTERN uint16_t    _Objects_Maximum_nodes;
#else
#define _Objects_Maximum_nodes 1
#endif

/**
 *  The following is the list of information blocks per API for each object
 *  class.  From the ID, we can go to one of these information blocks,
 *  and obtain a pointer to the appropriate object control block.
 */
SCORE_EXTERN Objects_Information
    **_Objects_Information_table[OBJECTS_APIS_LAST + 1];

/**
 *  This function extends an object class information record.
 *
 *  @param[in] information points to an object class information block.
 */
void _Objects_Extend_information(
  Objects_Information *information
);

/**
 *  @brief Shrink an object class information record
 *
 *  This function shrink an object class information record.
 *  The object's name and object space are released. The local_table
 *  etc block does not shrink. The InActive list needs to be scanned
 *  to find the objects are remove them.
 *
 *  @param[in] information points to an object class information block.
 */
void _Objects_Shrink_information(
  Objects_Information *information
);

/**
 *  @brief Initialize object Information
 *
 *  This function initializes an object class information record.
 *  SUPPORTS_GLOBAL is true if the object class supports global
 *  objects, and false otherwise.  Maximum indicates the number
 *  of objects required in this class and size indicates the size
 *  in bytes of each control block for this object class.  The
 *  name length and string designator are also set.  In addition,
 *  the class may be a task, therefore this information is also included.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] the_api indicates the API associated with this information block.
 *  @param[in] the_class indicates the class of object being managed
 *             by this information block.  It is specific to @a the_api.
 *  @param[in] maximum is the maximum number of instances of this object
 *             class which may be concurrently active.
 *  @param[in] size is the size of the data structure for this class.
 *  @param[in] is_string is true if this object uses string style names.
 *  @param[in] maximum_name_length is the maximum length of object names.
 */
void _Objects_Initialize_information (
  Objects_Information *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  uint16_t             size,
  bool                 is_string,
  uint32_t             maximum_name_length
#if defined(RTEMS_MULTIPROCESSING)
  ,
  bool                 supports_global,
  Objects_Thread_queue_Extract_callout extract
#endif
);

/**
 *  @brief Object API Maximum Class
 *
 *  This function returns the highest numeric value of a valid
 *  API for the specified @a api.
 *
 *  @param[in] api is the API of interest
 *
 *  @retval A positive integer on success and 0 otherwise.
 */
unsigned int _Objects_API_maximum_class(
  uint32_t api
);

/**
 * @brief Allocates an object without locking the allocator mutex.
 *
 * This function can be called in two contexts
 * - the executing thread is the owner of the object allocator mutex, or
 * - in case the system state is not up, e.g. during sequential system
 *   initialization.
 *
 * @param[in] information The object information block.
 *
 * @retval NULL No object available.
 * @retval object The allocated object.
 *
 * @see _Objects_Allocate() and _Objects_Free().
 */
Objects_Control *_Objects_Allocate_unprotected(
  Objects_Information *information
);

/**
 * @brief Allocates an object.
 *
 * This function locks the object allocator mutex via
 * _Objects_Allocator_lock().  The caller must later unlock the object
 * allocator mutex via _Objects_Allocator_unlock().  The caller must unlock the
 * mutex in any case, even if the allocation failed due to resource shortage.
 *
 * A typical object allocation code looks like this:
 * @code
 * rtems_status_code some_create( rtems_id *id )
 * {
 *   rtems_status_code  sc;
 *   Some_Control      *some;
 *
 *   // The object allocator mutex protects the executing thread from
 *   // asynchronous thread restart and deletion.
 *   some = (Some_Control *) _Objects_Allocate( &_Some_Information );
 *
 *   if ( some != NULL ) {
 *     _Some_Initialize( some );
 *     sc = RTEMS_SUCCESSFUL;
 *   } else {
 *     sc = RTEMS_TOO_MANY;
 *   }
 *
 *   _Objects_Allocator_unlock();
 *
 *   return sc;
 * }
 * @endcode
 *
 * @param[in] information The object information block.
 *
 * @retval NULL No object available.
 * @retval object The allocated object.
 *
 * @see _Objects_Free().
 */
Objects_Control *_Objects_Allocate( Objects_Information *information );

/**
 * @brief Frees an object.
 *
 * Appends the object to the chain of inactive objects.
 *
 * @param[in] information The object information block.
 * @param[in] the_object The object to free.
 *
 * @see _Objects_Allocate().
 *
 * A typical object deletion code looks like this:
 * @code
 * rtems_status_code some_delete( rtems_id id )
 * {
 *   rtems_status_code  sc;
 *   Some_Control      *some;
 *   Objects_Locations  location;
 *
 *   // The object allocator mutex protects the executing thread from
 *   // asynchronous thread restart and deletion.
 *   _Objects_Allocator_lock();
 *
 *   // This will disable thread dispatching, so this starts a thread dispatch
 *   // critical section.
 *   some = (Semaphore_Control *)
 *     _Objects_Get( &_Some_Information, id, &location );
 *
 *   switch ( location ) {
 *     case OBJECTS_LOCAL:
 *       // After the object close an object get with this identifier will
 *       // fail.
 *       _Objects_Close( &_Some_Information, &some->Object );
 *
 *       _Some_Delete( some );
 *
 *       // This enables thread dispatching, so the thread dispatch critical
 *       // section ends here.
 *       _Objects_Put( &some->Object );
 *
 *       // Thread dispatching is enabled.  The object free is only protected
 *       // by the object allocator mutex.
 *       _Objects_Free( &_Some_Information, &some->Object );
 *
 *       sc = RTEMS_SUCCESSFUL;
 *       break;
 *     default:
 *       sc = RTEMS_INVALID_ID;
 *       break;
 *   }
 *
 *   _Objects_Allocator_unlock();
 *
 *   return sc;
 * }
 * @endcode
 */
void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
);

/**
 *  This function implements the common portion of the object
 *  identification directives.  This directive returns the object
 *  id associated with name.  If more than one object of this class
 *  is named name, then the object to which the id belongs is
 *  arbitrary.  Node indicates the extent of the search for the
 *  id of the object named name.  If the object class supports global
 *  objects, then the search can be limited to a particular node
 *  or allowed to encompass all nodes.
 */
typedef enum {
  OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL,
  OBJECTS_INVALID_NAME,
  OBJECTS_INVALID_ADDRESS,
  OBJECTS_INVALID_ID,
  OBJECTS_INVALID_NODE
} Objects_Name_or_id_lookup_errors;

/**
 *  This macro defines the first entry in the
 *  @ref Objects_Name_or_id_lookup_errors enumerated list.
 */
#define OBJECTS_NAME_ERRORS_FIRST OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL

/**
 *  This macro defines the last entry in the
 *  @ref Objects_Name_or_id_lookup_errors enumerated list.
 */
#define OBJECTS_NAME_ERRORS_LAST  OBJECTS_INVALID_NODE

/**
 *  @brief Converts an object name to an Id.
 *
 *  This method converts an object name to an Id.  It performs a look up
 *  using the object information block for this object class.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] name is the name of the object to find.
 *  @param[in] node is the set of nodes to search.
 *  @param[in] id will contain the Id if the search is successful.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a id will contain the Id of
 *          the requested object.
 */
Objects_Name_or_id_lookup_errors _Objects_Name_to_id_u32(
  Objects_Information *information,
  uint32_t             name,
  uint32_t             node,
  Objects_Id          *id
);

#if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
/**
 *  @brief Converts an object name to an Id.
 *
 *  This method converts an object name to an Id.  It performs a look up
 *  using the object information block for this object class.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] name is the name of the object to find.
 *  @param[in] id will contain the Id if the search is successful.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a id will contain the Id of
 *          the requested object.
 */
Objects_Name_or_id_lookup_errors _Objects_Name_to_id_string(
  Objects_Information *information,
  const char          *name,
  Objects_Id          *id
);
#endif

/**
 *  @brief Implements the common portion of the object Id to name directives.
 *
 *  This function implements the common portion of the object Id
 *  to name directives.  This function returns the name
 *  associated with object id.
 *
 *  @param[in] id is the Id of the object whose name we are locating.
 *  @param[in] name will contain the name of the object, if found.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a name will contain the name of
 *          the requested object.
 *
 *  @note This function currently does not support string names.
 */
Objects_Name_or_id_lookup_errors _Objects_Id_to_name (
  Objects_Id      id,
  Objects_Name   *name
);

/**
 *  @brief Maps object ids to object control blocks.
 *
 *  This function maps object ids to object control blocks.
 *  If id corresponds to a local object, then it returns
 *  the_object control pointer which maps to id and location
 *  is set to OBJECTS_LOCAL.  If the object class supports global
 *  objects and the object id is global and resides on a remote
 *  node, then location is set to OBJECTS_REMOTE, and the_object
 *  is undefined.  Otherwise, location is set to OBJECTS_ERROR
 *  and the_object is undefined.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] id is the Id of the object whose name we are locating.
 *  @param[in] location will contain an indication of success or failure.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a id will contain the Id of
 *          the requested object.
 *
 *  @note _Objects_Get returns with dispatching disabled for
 *  local and remote objects.  _Objects_Get_isr_disable returns with
 *  dispatching disabled for remote objects and interrupts for local
 *  objects.
 */
Objects_Control *_Objects_Get (
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
);

/**
 *  @brief Maps object ids to object control blocks.
 *
 *  This function maps object ids to object control blocks.
 *  If id corresponds to a local object, then it returns
 *  the_object control pointer which maps to id and location
 *  is set to OBJECTS_LOCAL.  If the object class supports global
 *  objects and the object id is global and resides on a remote
 *  node, then location is set to OBJECTS_REMOTE, and the_object
 *  is undefined.  Otherwise, location is set to OBJECTS_ERROR
 *  and the_object is undefined.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] id is the Id of the object whose name we are locating.
 *  @param[in] location will contain an indication of success or failure.
 *  @param[in] lock_context is the previous interrupt state being turned.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a name will contain the name of
 *          the requested object.
 *
 *  @note _Objects_Get returns with dispatching disabled for
 *  local and remote objects.  _Objects_Get_isr_disable returns with
 *  dispatchng disabled for remote objects and interrupts for local
 *  objects.
 */
Objects_Control *_Objects_Get_isr_disable(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location,
  ISR_lock_Context    *lock_context
);

/**
 *  @brief  Maps object ids to object control blocks.
 *
 *  This function maps object ids to object control blocks.
 *  If id corresponds to a local object, then it returns
 *  the_object control pointer which maps to id and location
 *  is set to OBJECTS_LOCAL.  If the object class supports global
 *  objects and the object id is global and resides on a remote
 *  node, then location is set to OBJECTS_REMOTE, and the_object
 *  is undefined.  Otherwise, location is set to OBJECTS_ERROR
 *  and the_object is undefined.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] id is the Id of the object whose name we are locating.
 *  @param[in] location will contain an indication of success or failure.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a id will contain the Id of
 *          the requested object.
 *
 *  @note _Objects_Get returns with dispatching disabled for
 *  local and remote objects.  _Objects_Get_isr_disable returns with
 *  dispatching disabled for remote objects and interrupts for local
 *  objects.
 */
Objects_Control *_Objects_Get_no_protection(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
);

/**
 *  Like @ref _Objects_Get, but is used to find "next" open object.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] id is the Id of the object whose name we are locating.
 *  @param[in] location_p will contain an indication of success or failure.
 *  @param[in] next_id_p is the Id of the next object we will look at.
 *
 *  @retval This method returns the pointer to the object located or
 *          NULL on error.
 */
Objects_Control *_Objects_Get_next(
    Objects_Information *information,
    Objects_Id           id,
    Objects_Locations   *location_p,
    Objects_Id          *next_id_p
);

/**
 *  @brief Get object information.
 *
 *  This function return the information structure given
 *  an the API and Class.  This can be done independent of
 *  the existence of any objects created by the API.
 *
 *  @param[in] the_api indicates the API for the information we want
 *  @param[in] the_class indicates the Class for the information we want
 *
 *  @retval This method returns a pointer to the Object Information Table
 *          for the class of objects which corresponds to this object ID.
 */
Objects_Information *_Objects_Get_information(
  Objects_APIs   the_api,
  uint16_t       the_class
);

/**
 *  @brief Get information of an object from an ID.
 *
 *  This function return the information structure given
 *  an @a id of an object.
 *
 *  @param[in] id is the object ID to get the information from
 *
 *  @retval This method returns a pointer to the Object Information Table
 *          for the class of objects which corresponds to this object ID.
 */
Objects_Information *_Objects_Get_information_id(
  Objects_Id  id
);

/**
 *  @brief Gets object name in the form of a C string.
 *
 *  This method objects the name of an object and returns its name
 *  in the form of a C string.  It attempts to be careful about
 *  overflowing the user's string and about returning unprintable characters.
 *
 *  @param[in] id is the object to obtain the name of
 *  @param[in] length indicates the length of the caller's buffer
 *  @param[in] name points a string which will be filled in.
 *
 *  @retval This method returns @a name or NULL on error. @a *name will
 *          contain the name if successful.
 */
char *_Objects_Get_name_as_string(
  Objects_Id   id,
  size_t       length,
  char        *name
);

/**
 *  @brief Set objects name.
 *
 *  This method sets the object name to either a copy of a string
 *  or up to the first four characters of the string based upon
 *  whether this object class uses strings for names.
 *
 *  @param[in] information points to the object information structure
 *  @param[in] the_object is the object to operate upon
 *  @param[in] name is a pointer to the name to use
 *
 *  @retval If successful, true is returned.  Otherwise false is returned.
 */
bool _Objects_Set_name(
  Objects_Information *information,
  Objects_Control     *the_object,
  const char          *name
);

/**
 *  @brief Removes object from namespace.
 *
 *  This function removes @a the_object from the namespace.
 *
 *  @param[in] information points to an Object Information Table.
 *  @param[in] the_object is a pointer to an object.
 */
void _Objects_Namespace_remove(
  Objects_Information  *information,
  Objects_Control      *the_object
);

/**
 *  @brief Close object.
 *
 *  This function removes the_object control pointer and object name
 *  in the Local Pointer and Local Name Tables.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 */
void _Objects_Close(
  Objects_Information  *information,
  Objects_Control      *the_object
);

/**
 * @brief Returns the count of active objects.
 *
 * @param[in] information The object information table.
 *
 * @retval The count of active objects.
 */
Objects_Maximum _Objects_Active_count(
  const Objects_Information *information
);

/**
 * This function returns true if the api is valid.
 *
 * @param[in] the_api is the api portion of an object ID.
 *
 * @return This method returns true if the specified api value is valid
 *         and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_api_valid(
  uint32_t   the_api
)
{
  if ( !the_api || the_api > OBJECTS_APIS_LAST )
   return false;
  return true;
}

/**
 * This function returns true if the node is of the local object, and
 * false otherwise.
 *
 * @param[in] node is the node number and corresponds to the node number
 *        portion of an object ID.
 *
 * @return This method returns true if the specified node is the local node
 *         and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_local_node(
  uint32_t   node
)
{
  return ( node == _Objects_Local_node );
}

/**
 * This function returns true if the id is of a local object, and
 * false otherwise.
 *
 * @param[in] id is an object ID
 *
 * @return This method returns true if the specified object Id is local
 *         and false otherwise.
 *
 * @note On a single processor configuration, this always returns true.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_local_id(
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Id id
#else
  Objects_Id id __attribute__((unused))
#endif
)
{
#if defined(RTEMS_MULTIPROCESSING)
  return _Objects_Is_local_node( _Objects_Get_node(id) );
#else
  return true;
#endif
}

/**
 * This function returns true if left and right are equal,
 * and false otherwise.
 *
 * @param[in] left is the Id on the left hand side of the comparison
 * @param[in] right is the Id on the right hand side of the comparison
 *
 * @return This method returns true if the specified object IDs are equal
 *         and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Are_ids_equal(
  Objects_Id left,
  Objects_Id right
)
{
  return ( left == right );
}

/**
 * This function returns a pointer to the local_table object
 * referenced by the index.
 *
 * @param[in] information points to an Object Information Table
 * @param[in] index is the index of the object the caller wants to access
 *
 * @return This method returns a pointer to a local object or NULL if the
 *         index is invalid and RTEMS_DEBUG is enabled.
 */
RTEMS_INLINE_ROUTINE Objects_Control *_Objects_Get_local_object(
  Objects_Information *information,
  uint16_t             index
)
{
  /*
   * This routine is ONLY to be called from places in the code
   * where the Id is known to be good.  Therefore, this should NOT
   * occur in normal situations.
   */
  #if defined(RTEMS_DEBUG)
    if ( index > information->maximum )
      return NULL;
  #endif
  return information->local_table[ index ];
}

/**
 * This function sets the pointer to the local_table object
 * referenced by the index.
 *
 * @param[in] information points to an Object Information Table
 * @param[in] index is the index of the object the caller wants to access
 * @param[in] the_object is the local object pointer
 *
 * @note This routine is ONLY to be called in places where the
 *       index portion of the Id is known to be good.  This is
 *       OK since it is normally called from object create/init
 *       or delete/destroy operations.
 */

RTEMS_INLINE_ROUTINE void _Objects_Set_local_object(
  Objects_Information *information,
  uint32_t             index,
  Objects_Control     *the_object
)
{
  /*
   *  This routine is ONLY to be called from places in the code
   *  where the Id is known to be good.  Therefore, this should NOT
   *  occur in normal situations.
   */
  #if defined(RTEMS_DEBUG)
    if ( index > information->maximum )
      return;
  #endif

  information->local_table[ index ] = the_object;
}

/**
 * This function sets the pointer to the local_table object
 * referenced by the index to a NULL so the object Id is invalid
 * after this call.
 *
 * @param[in] information points to an Object Information Table
 * @param[in] the_object is the local object pointer
 *
 * @note This routine is ONLY to be called in places where the
 *       index portion of the Id is known to be good.  This is
 *       OK since it is normally called from object create/init
 *       or delete/destroy operations.
 */

RTEMS_INLINE_ROUTINE void _Objects_Invalidate_Id(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  _Assert( information != NULL );
  _Assert( the_object != NULL );

  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    NULL
  );
}

/**
 * This function places the_object control pointer and object name
 * in the Local Pointer and Local Name Tables, respectively.
 *
 * @param[in] information points to an Object Information Table
 * @param[in] the_object is a pointer to an object
 * @param[in] name is the name of the object to make accessible
 */
RTEMS_INLINE_ROUTINE void _Objects_Open(
  Objects_Information *information,
  Objects_Control     *the_object,
  Objects_Name         name
)
{
  _Assert( information != NULL );
  _Assert( the_object != NULL );

  the_object->name = name;

  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );
}

/**
 * This function places the_object control pointer and object name
 * in the Local Pointer and Local Name Tables, respectively.
 *
 * @param[in] information points to an Object Information Table
 * @param[in] the_object is a pointer to an object
 * @param[in] name is the name of the object to make accessible
 */
RTEMS_INLINE_ROUTINE void _Objects_Open_u32(
  Objects_Information *information,
  Objects_Control     *the_object,
  uint32_t             name
)
{
  /* ASSERT: information->is_string == false */
  the_object->name.name_u32 = name;

  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );
}

/**
 * This function places the_object control pointer and object name
 * in the Local Pointer and Local Name Tables, respectively.
 *
 * @param[in] information points to an Object Information Table
 * @param[in] the_object is a pointer to an object
 * @param[in] name is the name of the object to make accessible
 */
RTEMS_INLINE_ROUTINE void _Objects_Open_string(
  Objects_Information *information,
  Objects_Control     *the_object,
  const char          *name
)
{
  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    /* ASSERT: information->is_string */
    the_object->name.name_p = name;
  #endif

  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );
}

/**
 * @brief Puts back an object obtained with _Objects_Get().
 *
 * This function decrements the thread dispatch disable level.  The
 * _Thread_Dispatch() is called if the level reaches zero.
 */
RTEMS_INLINE_ROUTINE void _Objects_Put(
  Objects_Control *the_object
)
{
  (void) the_object;
  _Thread_Enable_dispatch();
}

/**
 * @brief Puts back an object obtained with _Objects_Get().
 *
 * This function decrements the thread dispatch disable level.  The
 * _Thread_Dispatch() is not called if the level reaches zero, thus a thread
 * dispatch will not take place immediately on the current processor.
 */
RTEMS_INLINE_ROUTINE void _Objects_Put_without_thread_dispatch(
  Objects_Control *the_object
)
{
  (void) the_object;
  _Thread_Unnest_dispatch();
}

/**
 * @brief Locks the object allocator mutex.
 *
 * While holding the allocator mutex the executing thread is protected from
 * asynchronous thread restart and deletion.
 *
 * The usage of the object allocator mutex with the thread life protection
 * makes it possible to allocate and free objects without thread dispatching
 * disabled.  The usage of a unified workspace and unlimited objects may lead
 * to heap fragmentation.  Thus the execution time of the _Objects_Allocate()
 * function may increase during system run-time.
 *
 * @see _Objects_Allocator_unlock() and _Objects_Allocate().
 */
RTEMS_INLINE_ROUTINE void _Objects_Allocator_lock( void )
{
  _RTEMS_Lock_allocator();
}

/**
 * @brief Unlocks the object allocator mutex.
 *
 * In case the mutex is fully unlocked, then this function restores the
 * previous thread life protection state and thus may not return if the
 * executing thread was restarted or deleted in the mean-time.
 */
RTEMS_INLINE_ROUTINE void _Objects_Allocator_unlock( void )
{
  _RTEMS_Unlock_allocator();
}

/** @} */

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif


#endif
/* end of include file */
