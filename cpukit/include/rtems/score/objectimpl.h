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

#include <rtems/score/objectdata.h>
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
  OBJECTS_INTERNAL_THREADS  =  1
} Objects_Internal_API;

/** This macro is used to generically specify the last API index. */
#define OBJECTS_INTERNAL_CLASSES_LAST OBJECTS_INTERNAL_THREADS

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
  OBJECTS_POSIX_MESSAGE_QUEUES      = 5,
  OBJECTS_POSIX_SEMAPHORES          = 7,
  OBJECTS_POSIX_TIMERS              = 9,
  OBJECTS_POSIX_SHMS                = 12
} Objects_POSIX_API;

/** This macro is used to generically specify the last API index. */
#define OBJECTS_POSIX_CLASSES_LAST OBJECTS_POSIX_SHMS

/*
 * For fake objects, which have an object identifier, but no objects
 * information block.
 */
typedef enum {
  OBJECTS_FAKE_OBJECTS_NO_CLASS   = 0,
  OBJECTS_FAKE_OBJECTS_SCHEDULERS = 1
} Objects_Fake_objects_API;

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  The following type defines the callout used when a local task
 *  is extracted from a remote thread queue (i.e. it's proxy must
 *  extracted from the remote queue).
 */
typedef void ( *Objects_Thread_queue_Extract_callout )(
  Thread_Control *,
  Objects_Id
);
#endif

/**
 *  The following defines the structure for the information used to
 *  manage each class of objects.
 */
typedef struct {
  /** This is the minimum valid id of this object class. */
  Objects_Id        minimum_id;
  /** This is the maximum valid id of this object class. */
  Objects_Id        maximum_id;
  /** This points to the table of local objects. */
  Objects_Control **local_table;
  /** This is the maximum number of objects in this class. */
  Objects_Maximum   maximum;
  /** This is the number of objects on the Inactive list. */
  Objects_Maximum   inactive;
  /** This is the number of objects in a block. */
  Objects_Maximum   allocation_size;
  /** This is the maximum length of names. */
  uint16_t          name_length;
  /** This field indicates the API of this object class. */
  uint8_t           the_api;
  /** This is the class of this object set. */
  uint8_t           the_class;
  /** This is true if names are strings. */
  bool              is_string;
  /** This is the true if unlimited objects in this class. */
  bool              auto_extend;
  /** This is the size in bytes of each object instance. */
  size_t            size;
  /** This is the chain of inactive control blocks. */
  Chain_Control     Inactive;
  /** This is the number of inactive objects per block. */
  Objects_Maximum  *inactive_per_block;
  /** This is a table to the chain of inactive object memory blocks. */
  Objects_Control **object_blocks;
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

/**
 *  The following is referenced to the node number of the local node.
 */
#if defined(RTEMS_MULTIPROCESSING)
extern uint16_t _Objects_Local_node;
#else
#define _Objects_Local_node ((uint16_t)1)
#endif

/**
 *  The following is referenced to the number of nodes in the system.
 */
#if defined(RTEMS_MULTIPROCESSING)
extern uint16_t _Objects_Maximum_nodes;
#else
#define _Objects_Maximum_nodes 1
#endif

/**
 *  The following is the list of information blocks per API for each object
 *  class.  From the ID, we can go to one of these information blocks,
 *  and obtain a pointer to the appropriate object control block.
 */
extern Objects_Information ** const
_Objects_Information_table[ OBJECTS_APIS_LAST + 1 ];

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

void _Objects_Do_initialize_information(
  Objects_Information *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  uint16_t             size,
  bool                 is_string,
  uint32_t             maximum_name_length
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Objects_Thread_queue_Extract_callout extract
#endif
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
#if defined(RTEMS_MULTIPROCESSING)
  #define _Objects_Initialize_information( \
    information, \
    the_api, \
    the_class, \
    maximum, \
    size, \
    is_string, \
    maximum_name_length, \
    extract \
  ) \
    _Objects_Do_initialize_information( \
      information, \
      the_api, \
      the_class, \
      maximum, \
      size, \
      is_string, \
      maximum_name_length, \
      extract \
    )
#else
  #define _Objects_Initialize_information( \
    information, \
    the_api, \
    the_class, \
    maximum, \
    size, \
    is_string, \
    maximum_name_length, \
    extract \
  ) \
    _Objects_Do_initialize_information( \
      information, \
      the_api, \
      the_class, \
      maximum, \
      size, \
      is_string, \
      maximum_name_length \
    )
#endif

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
 *   Some_Control      *some;
 *
 *   // The object allocator mutex protects the executing thread from
 *   // asynchronous thread restart and deletion.
 *   _Objects_Allocator_lock();
 *
 *   // Get the object under protection of the object allocator mutex.
 *   some = (Semaphore_Control *)
 *     _Objects_Get_no_protection( id, &_Some_Information );
 *
 *   if ( some == NULL ) {
 *     _Objects_Allocator_unlock();
 *     return RTEMS_INVALID_ID;
 *   }
 *
 *   // After the object close an object get with this identifier will
 *   // fail.
 *   _Objects_Close( &_Some_Information, &some->Object );
 *
 *   _Some_Delete( some );
 *
 *   // Thread dispatching is enabled.  The object free is only protected
 *   // by the object allocator mutex.
 *   _Objects_Free( &_Some_Information, &some->Object );
 *
 *   _Objects_Allocator_unlock();
 *   return RTEMS_SUCCESSFUL;
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

typedef enum {
  OBJECTS_GET_BY_NAME_INVALID_NAME,
  OBJECTS_GET_BY_NAME_NAME_TOO_LONG,
  OBJECTS_GET_BY_NAME_NO_OBJECT
} Objects_Get_by_name_error;

/**
 * @brief Gets an object control block identified by its name.
 *
 * The object information must use string names.
 *
 * @param information The object information.  Must not be NULL.
 * @param name The object name.
 * @param name_length_p Optional parameter to return the name length.
 * @param error The error indication in case of failure.  Must not be NULL.
 *
 * @retval NULL No object exists for this name or invalid parameters.
 * @retval other The first object according to object index associated with
 * this name.
 */
Objects_Control *_Objects_Get_by_name(
  const Objects_Information *information,
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
);

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
 * @brief Maps the specified object identifier to the associated local object
 * control block.
 *
 * In this function interrupts are disabled during the object lookup.  In case
 * an associated object exists, then interrupts remain disabled, otherwise the
 * previous interrupt state is restored.
 *
 * @param id The object identifier.  This is the first parameter since usual
 *   callers get the object identifier as the first parameter themself.
 * @param lock_context The interrupt lock context.  This is the second
 *   parameter since usual callers get the interrupt lock context as the second
 *   parameter themself.
 * @param information The object class information block.
 *
 * @retval NULL No associated object exists.
 * @retval other The pointer to the associated object control block.
 * Interrupts are now disabled and must be restored using the specified lock
 * context via _ISR_lock_ISR_enable() or _ISR_lock_Release_and_ISR_enable().
 */
Objects_Control *_Objects_Get(
  Objects_Id                 id,
  ISR_lock_Context          *lock_context,
  const Objects_Information *information
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
 *  @param[in] id is the Id of the object whose name we are locating.
 *    This is the first parameter since usual callers get the object identifier
 *    as the first parameter themself.
 *  @param[in] information points to an object class information block.
 *
 *  @retval This method returns one of the values from the
 *          @ref Objects_Name_or_id_lookup_errors enumeration to indicate
 *          successful or failure.  On success @a id will contain the Id of
 *          the requested object.
 */
Objects_Control *_Objects_Get_no_protection(
  Objects_Id                 id,
  const Objects_Information *information
);

/**
 *  Gets the next open object after the specified object identifier.
 *
 *  Locks the object allocator mutex in case a next object exists.
 *
 *  @param[in] id is the Id of the object whose name we are locating.
 *    This is the first parameter since usual callers get the object identifier
 *    as the first parameter themself.
 *  @param[in] information points to an object class information block.
 *  @param[in] next_id_p is the Id of the next object we will look at.
 *
 *  @retval This method returns the pointer to the object located or
 *          NULL on error.
 */
Objects_Control *_Objects_Get_next(
  Objects_Id                 id,
  const Objects_Information *information,
  Objects_Id                *next_id_p
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
 * @brief Converts the specified object name to a text representation.
 *
 * Non-printable characters according to isprint() are converted to '*'.
 *
 * @param[in] name The object name.
 * @param[in] is_string Indicates if the object name is a string or a four
 *   character array (32-bit unsigned integer).
 * @param[in] buffer The string buffer for the text representation.
 * @param[in] buffer_size The buffer size in characters.
 *
 * @retval The length of the text representation.  May be greater than or equal
 * to the buffer size if truncation occurred.
 */
size_t _Objects_Name_to_string(
  Objects_Name  name,
  bool          is_string,
  char         *buffer,
  size_t        buffer_size
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
  const Objects_Information *information,
  Objects_Control           *the_object,
  const char                *name
);

/**
 * @brief Removes object with a 32-bit integer name from its namespace.
 *
 * @param[in] information The corresponding object information table.
 * @param[in] the_object The object.
 */
void _Objects_Namespace_remove_u32(
  const Objects_Information *information,
  Objects_Control           *the_object
);

/**
 * @brief Removes object with a string name from its namespace.
 *
 * @param[in] information The corresponding object information table.
 * @param[in] the_object The object.
 */
void _Objects_Namespace_remove_string(
  const Objects_Information *information,
  Objects_Control           *the_object
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
  const Objects_Information *information,
  Objects_Control           *the_object
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

RTEMS_INLINE_ROUTINE Objects_Maximum _Objects_Extend_size(
  const Objects_Information *information
)
{
  return information->auto_extend ? information->allocation_size : 0;
}

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
  Objects_Id id RTEMS_UNUSED
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
  const Objects_Information *information,
  uint32_t                   index,
  Objects_Control           *the_object
)
{
  /*
   *  This routine is ONLY to be called from places in the code
   *  where the Id is known to be good.  Therefore, this should NOT
   *  occur in normal situations.
   */
  _Assert( index <= information->maximum );

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
  const Objects_Information *information,
  Objects_Control           *the_object
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
  const Objects_Information *information,
  Objects_Control           *the_object,
  uint32_t                   name
)
{
  _Assert( !information->is_string );
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
  const Objects_Information *information,
  Objects_Control           *the_object,
  const char                *name
)
{
  _Assert( information->is_string );
  the_object->name.name_p = name;

  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );
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

RTEMS_INLINE_ROUTINE bool _Objects_Allocator_is_owner( void )
{
  return _RTEMS_Allocator_is_owner();
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
