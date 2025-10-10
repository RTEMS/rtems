/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreObject which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_OBJECTIMPL_H
#define _RTEMS_SCORE_OBJECTIMPL_H

#include <rtems/score/objectdata.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/assert.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/status.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreObject
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

/** This macro is used to generically specify the last API index. */
#define OBJECTS_INTERNAL_CLASSES_LAST OBJECTS_INTERNAL_THREADS

/** This macro is used to generically specify the last API index. */
#define OBJECTS_RTEMS_CLASSES_LAST OBJECTS_RTEMS_BARRIERS

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

/**
 *  The following is referenced to the number of nodes in the system.
 */
#if defined(RTEMS_MULTIPROCESSING)
extern uint16_t _Objects_Maximum_nodes;
#else
#define _Objects_Maximum_nodes 1
#endif

/**
 * This is the minimum object ID index associated with an object.
 */
#define OBJECTS_INDEX_MINIMUM 1U

/**
 *  The following is the list of information blocks per API for each object
 *  class.  From the ID, we can go to one of these information blocks,
 *  and obtain a pointer to the appropriate object control block.
 */
extern Objects_Information ** const
_Objects_Information_table[ OBJECTS_APIS_LAST + 1 ];

/**
 * @brief Extends an object class information record.
 *
 * @param information Points to an object class information block.
 *
 * @retval 0 The extend operation failed.
 * @retval block The block index of the new objects block.
 */
Objects_Maximum _Objects_Extend_information(
  Objects_Information *information
);

/**
 * @brief Free the objects block with the specified index.
 *
 * @param information The objects information.
 * @param block The block index.
 */
void _Objects_Free_objects_block(
  Objects_Information *information,
  Objects_Maximum      block
);

/**
 * @brief Shrinks an object class information record.
 *
 * This function shrinks an object class information record.
 * The object's name and object space are released. The local_table
 * etc block does not shrink. The InActive list needs to be scanned
 * to find the objects are remove them.
 *
 * @param information Points to an object class information block.
 */
void _Objects_Shrink_information(
  Objects_Information *information
);

/**
 * @brief Initializes the specified objects information.
 *
 * The objects information must be statically pre-initialized with the
 * OBJECTS_INFORMATION_DEFINE() macro before this function is called.
 *
 * @param information The object information to be initialized.
 */
void _Objects_Initialize_information( Objects_Information *information );

/**
 * @brief Returns highest numeric value of a valid API for the specified API.
 *
 * This function returns the highest numeric value of a valid
 * API for the specified @a api.
 *
 * @param api The API of interest.
 *
 * @retval some_value Positive integer on success.
 * @retval 0 The method failed.
 */
unsigned int _Objects_API_maximum_class(
  uint32_t api
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
 * @param[in, out] information The object information block.
 *
 * @retval object The allocated object.
 * @retval NULL No object available.
 *
 * @see _Objects_Free().
 */
Objects_Control *_Objects_Allocate( Objects_Information *information );

/**
 * @brief Searches an object of the specified class with the specified name on
 *   the specified set of nodes.
 *
 * This method converts an object name to an identifier.  It performs a look up
 * using the object information block for this object class.
 *
 * @param name is the name of the object to find.
 * @param node is the set of nodes to search.
 * @param[out] id is the pointer to an object identifier variable or NULL.  The
 *   object identifier will be stored in the referenced variable, if the
 *   operation was successful.
 * @param information is the pointer to an object class information block.
 *
 * @retval STATUS_SUCCESSFUL The operations was successful.
 * @retval STATUS_INVALID_ADDRESS The id parameter was NULL.
 * @retval STATUS_INVALID_NAME No object exists with the specified name on the
 *   specified node set.
 */
Status_Control _Objects_Name_to_id_u32(
  uint32_t                   name,
  uint32_t                   node,
  Objects_Id                *id,
  const Objects_Information *information
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
 * @param[out] name_length_p Optional parameter to return the name length.
 * @param[out] error The error indication in case of failure.  Must not be NULL.
 *
 * @retval pointer The first object according to object index associated with
 *      this name.
 * @retval NULL No object exists for this name or invalid parameters.
 */
Objects_Control *_Objects_Get_by_name(
  const Objects_Information *information,
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
);

/**
 * @brief Returns the name associated with object id.
 *
 * This function implements the common portion of the object Id
 * to name directives.  This function returns the name
 * associated with object id.
 *
 * @param id is the Id of the object whose name we are locating.
 * @param[out] name will contain the name of the object, if found.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.  @a name
 *      contains the name of the object.
 * @retval STATUS_INVALID_ID The id is invalid, the operation failed.
 *
 * @note This function currently does not support string names.
 */
Status_Control _Objects_Id_to_name (
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
 *      callers get the object identifier as the first parameter themself.
 * @param lock_context The interrupt lock context.  This is the second
 *      parameter since usual callers get the interrupt lock context as the second
 *      parameter themself.
 * @param information The object class information block.
 *
 * @retval pointer The pointer to the associated object control block.
 *      Interrupts are now disabled and must be restored using the specified lock
 *      context via _ISR_lock_ISR_enable() or _ISR_lock_Release_and_ISR_enable().
 * @retval NULL No associated object exists.
 */
Objects_Control *_Objects_Get(
  Objects_Id                 id,
  ISR_lock_Context          *lock_context,
  const Objects_Information *information
);

/**
 * @brief  Maps object ids to object control blocks.
 *
 * This function maps object ids to object control blocks.
 * If id corresponds to a local object, then it returns
 * the_object control pointer which maps to id and location
 * is set to OBJECTS_LOCAL.  If the object class supports global
 * objects and the object id is global and resides on a remote
 * node, then location is set to OBJECTS_REMOTE, and the_object
 * is undefined.  Otherwise, location is set to OBJECTS_ERROR
 * and the_object is undefined.
 *
 * @param id The Id of the object whose name we are locating.
 *   This is the first parameter since usual callers get the object identifier
 *   as the first parameter themself.
 * @param information Points to an object class information block.
 *
 * @retval pointer The local object corresponding to the given id.
 * @retval NULL The object to the given id was not found locally.
 */
Objects_Control *_Objects_Get_no_protection(
  Objects_Id                 id,
  const Objects_Information *information
);

/**
 * @brief Gets the next open object after the specified object identifier.
 *
 * Locks the object allocator mutex in case a next object exists.
 *
 * @param id The Id of the object whose name we are locating.
 *   This is the first parameter since usual callers get the object identifier
 *   as the first parameter themself.
 * @param information Points to an object class information block.
 * @param[in, out] next_id_p The Id of the next object we will look at.
 *
 * @retval pointer Pointer to the located object.
 * @retval NULL An error occurred.
 */
Objects_Control *_Objects_Get_next(
  Objects_Id                 id,
  const Objects_Information *information,
  Objects_Id                *next_id_p
);

/**
 * @brief Gets object information.
 *
 * This function returns the information structure given
 * an API and Class.  This can be done independent of the
 * existence of any objects created by the API.
 *
 * @param the_api Indicates the API for the information we want
 * @param the_class Indicates the Class for the information we want
 *
 * @retval pointer Pointer to the Object Information Table
 *         for the class of objects which corresponds to this object ID.
 * @retval NULL An error occurred.
 */
Objects_Information *_Objects_Get_information(
  Objects_APIs   the_api,
  uint16_t       the_class
);

/**
 * @brief Gets information of an object from an ID.
 *
 * This function returns the information structure given
 * an @a id of an object.
 *
 * @param id The object ID to get the information from.
 *
 * @retval pointer Pointer to the Object Information Table
 *         for the class of objects which corresponds to this object ID.
 * @retval NULL An error occurred.
 */
Objects_Information *_Objects_Get_information_id(
  Objects_Id  id
);

/**
 * @brief Returns if the object has a string name.
 *
 * @param information The object information table.
 *
 * @retval true The object has a string name.
 * @retval false The object does not have a string name.
 */
static inline bool _Objects_Has_string_name(
  const Objects_Information *information
)
{
  return information->name_length > 0;
}

/**
 * @brief Gets object name in the form of a C string.
 *
 * This method gets the name of an object and returns its name
 * in the form of a C string.  It attempts to be careful about
 * overflowing the user's string and about returning unprintable characters.
 *
 * @param id The object to obtain the name of.
 * @param length Indicates the length of the caller's buffer.
 * @param[out] name A string which will be filled in.
 *
 * @retval @a name The operation was succeeded and the string was correctly filled in.
 * @retval NULL An error occurred.
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
 * @param name The object name.
 * @param is_string Indicates if the object name is a string or a four
 *   character array (32-bit unsigned integer).
 * @param[out] buffer The string buffer for the text representation.
 * @param buffer_size The buffer size in characters.
 *
 * @return The length of the text representation.  May be greater than or equal
 * to the buffer size if truncation occurred.
 */
size_t _Objects_Name_to_string(
  Objects_Name  name,
  bool          is_string,
  char         *buffer,
  size_t        buffer_size
);

/**
 * @brief Sets objects name.
 *
 * This method sets the object name to either a copy of a string
 * or up to the first four characters of the string based upon
 * whether this object class uses strings for names.
 *
 * @param information points to the object information.
 * @param[out] the_object is the object to operate upon.
 * @param name is a pointer to the name to use.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 *
 * @retval STATUS_NO_MEMORY There was no memory available to duplicate the name.
 */
Status_Control _Objects_Set_name(
  const Objects_Information *information,
  Objects_Control           *the_object,
  const char                *name
);

/**
 * @brief Removes object with a 32-bit integer name from its namespace.
 *
 * @param information The corresponding object information table.
 * @param[out] the_object The object to operate upon.
 */
static inline void _Objects_Namespace_remove_u32(
  const Objects_Information *information,
  Objects_Control           *the_object
)
{
  (void) information;

  _Assert( !_Objects_Has_string_name( information ) );
  the_object->name.name_u32 = 0;
}

/**
 * @brief Removes object with a string name from its namespace.
 *
 * @param information The corresponding object information table.
 * @param[out] the_object The object the object to operate upon.
 */
void _Objects_Namespace_remove_string(
  const Objects_Information *information,
  Objects_Control           *the_object
);

/**
 * @brief Closes object.
 *
 * This function removes the_object control pointer and object name
 * in the Local Pointer and Local Name Tables.
 *
 * @param information Points to an Object Information Table.
 * @param[out] the_object A pointer to an object.
 */
void _Objects_Close(
  const Objects_Information *information,
  Objects_Control           *the_object
);

/**
 * @brief Returns the count of active objects.
 *
 * @param information The object information table.
 *
 * @return The count of active objects.
 */
Objects_Maximum _Objects_Active_count(
  const Objects_Information *information
);

/**
 * @brief Returns the object's objects per block.
 *
 * @param information The object information table.
 *
 * @return The number of objects per block of @a information.
 */
static inline Objects_Maximum _Objects_Extend_size(
  const Objects_Information *information
)
{
  return information->objects_per_block;
}

/**
 * @brief Checks if the api is valid.
 *
 * @param the_api is the api portion of an object ID.
 *
 * @retval true The specified api value is valid.
 * @retval false The specified api value is not valid.
 */
static inline bool _Objects_Is_api_valid(
  uint32_t   the_api
)
{
  return ( 1 <= the_api && the_api <= OBJECTS_APIS_LAST );
}

/**
 * @brief Checks if the node is of the local object.
 *
 * @param node The node number and corresponds to the node number
 *        portion of an object ID.
 *
 * @retval true The specified node is the local node.
 * @retval false The specified node is not the local node.
 */
static inline bool _Objects_Is_local_node(
  uint32_t   node
)
{
  return ( node == _Objects_Local_node );
}

/**
 * @brief Checks if the id is of a local object.
 *
 * @param id The object ID.
 *
 * @retval true The specified object Id is local.
 * @retval false The specified object Id is not local.
 *
 * @note On a single processor configuration, this always returns true.
 */
static inline bool _Objects_Is_local_id(
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
 * @brief Checks if two object IDs are equal.
 *
 * @param left The Id on the left hand side of the comparison.
 * @param right The Id on the right hand side of the comparison.
 *
 * @retval true The specified object IDs are equal.
 * @retval false The specified object IDs are not equal.
 */
static inline bool _Objects_Are_ids_equal(
  Objects_Id left,
  Objects_Id right
)
{
  return ( left == right );
}

/**
 * @brief Returns the identifier with the minimum index for the specified identifier.
 *
 * The specified identifier must have valid API, class and node fields.
 *
 * @param id The identifier to be processed.
 *
 * @return The corresponding ID with the minimum index.
 */
static inline Objects_Id _Objects_Get_minimum_id( Objects_Id id )
{
  id &= ~OBJECTS_INDEX_MASK;
  id += (Objects_Id) OBJECTS_INDEX_MINIMUM << OBJECTS_INDEX_START_BIT;
  return id;
}

/**
 * @brief Returns the maximum index of the specified object class.
 *
 * @param information The object information.
 *
 * @return The maximum index of the specified object class.
 */
static inline Objects_Maximum _Objects_Get_maximum_index(
  const Objects_Information *information
)
{
  return _Objects_Get_index( information->maximum_id );
}

/**
 * @brief Get an inactive object or NULL.
 *
 * @retval NULL No inactive object is available.
 * @retval object An inactive object.
 */
static inline Objects_Control *_Objects_Get_inactive(
  Objects_Information *information
)
{
  return (Objects_Control *) _Chain_Get_unprotected( &information->Inactive );
}

/**
 * @brief Checks if the automatic object extension (unlimited objects) is
 * enabled.
 *
 * @param information The object information.
 *
 * @retval true The automatic object extension (unlimited objects) is enabled.
 * @retval false The automatic object extension (unlimited objects) is not enabled.
 */
static inline Objects_Maximum _Objects_Is_auto_extend(
  const Objects_Information *information
)
{
  return information->objects_per_block != 0;
}

/**
 * @brief Sets the pointer to the local_table object
 * referenced by the index.
 *
 * @param[in, out] information Points to an Object Information Table.
 * @param index The index of the object the caller wants to access.
 * @param the_object The local object pointer.
 *
 * @note This routine is ONLY to be called in places where the
 *       index portion of the Id is known to be good.  This is
 *       OK since it is normally called from object create/init
 *       or delete/destroy operations.
 */

static inline void _Objects_Set_local_object(
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
  _Assert( index >= OBJECTS_INDEX_MINIMUM );
  _Assert( index <= _Objects_Get_maximum_index( information ) );

  information->local_table[ index - OBJECTS_INDEX_MINIMUM ] = the_object;
}

/**
 * @brief Invalidates an object Id.
 *
 * This function sets the pointer to the local_table object
 * referenced by the index to NULL so the object Id is invalid
 * after this call.
 *
 * @param[in, out] information points to an Object Information Table.
 * @param the_object The local object pointer.
 *
 * @note This routine is ONLY to be called in places where the
 *       index portion of the Id is known to be good.  This is
 *       OK since it is normally called from object create/init
 *       or delete/destroy operations.
 */

static inline void _Objects_Invalidate_Id(
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
 * @brief Assigns the 32-bit unsigned integer name to the object and places the
 *   object in the local object table.
 *
 * @param information is the object information.
 *
 * @param[in, out] the_object is the object to open.
 *
 * @param name is the name of the object to open.
 *
 * @return Returns the identifier of the object which is now valid.
 */
static inline Objects_Id _Objects_Open_u32(
  const Objects_Information *information,
  Objects_Control           *the_object,
  uint32_t                   name
)
{
  _Assert( information != NULL );
  _Assert( !_Objects_Has_string_name( information ) );
  _Assert( the_object != NULL );

  the_object->name.name_u32 = name;

  _Objects_Set_local_object(
    information,
    _Objects_Get_index( the_object->id ),
    the_object
  );

  return the_object->id;
}

/**
 * @brief Assigns the string name to the object and places the object in the
 *   local object table.
 *
 * @param information is the object information.
 *
 * @param[in, out] the_object is the object to open.
 *
 * @param name is the name of the object to open.
 */
static inline void _Objects_Open_string(
  const Objects_Information *information,
  Objects_Control           *the_object,
  const char                *name
)
{
  _Assert( information != NULL );
  _Assert( _Objects_Has_string_name( information ) );
  _Assert( the_object != NULL );

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
static inline void _Objects_Allocator_lock( void )
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
static inline void _Objects_Allocator_unlock( void )
{
  _RTEMS_Unlock_allocator();
}

/**
 * @brief Checks if the allocator is the owner of the object allocator mutex
 *
 * @retval true The allocator is the owner of the object allocator mutex.
 * @retval false The allocato is not the owner of the object allocator mutex.
 */
static inline bool _Objects_Allocator_is_owner( void )
{
  return _RTEMS_Allocator_is_owner();
}

/**
 * @brief Allocates an object without locking the allocator mutex.
 *
 * This function can be called in two contexts
 * - the executing thread is the owner of the object allocator mutex, or
 * - in case the system state is not up, e.g. during sequential system
 *   initialization.
 *
 * @param[in, out] information The object information block.
 *
 * @retval object The allocated object.
 * @retval NULL No object available.
 *
 * @see _Objects_Allocate() and _Objects_Free().
 */
static inline Objects_Control *_Objects_Allocate_unprotected(
  Objects_Information *information
)
{
  _Assert(
    _Objects_Allocator_is_owner()
      || !_System_state_Is_up( _System_state_Get() )
  );

  return ( *information->allocate )( information );
}

/**
 * @brief Frees an object.
 *
 * Appends the object to the chain of inactive objects.
 *
 * @param information The object information block.
 * @param[out] the_object The object to free.
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
static inline void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  _Assert( _Objects_Allocator_is_owner() );
  _Assert( information->deallocate != NULL );
  ( *information->deallocate )( information, the_object );
}

/**
 * @brief Returns true, if the object associated with the zero-based index is
 *   contained in an allocated block of objects, otherwise false.
 *
 * @param index is the zero-based object index.
 * @param objects_per_block is the object count per block.
 *
 * @retval true The object associated with the zero-based index is in an
 *   allocated block of objects.
 * @retval false Otherwise.
 */
static inline bool _Objects_Is_in_allocated_block(
  Objects_Maximum index,
  Objects_Maximum objects_per_block
)
{
  return index >= objects_per_block;
}

/**
 * @brief Activate the object.
 *
 * This function must be only used in case this objects information supports
 * unlimited objects.
 *
 * @param information The object information block.
 * @param the_object The object to activate.
 */
static inline void _Objects_Activate_unlimited(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  Objects_Maximum objects_per_block;
  Objects_Maximum index;

  _Assert( _Objects_Is_auto_extend( information ) );

  objects_per_block = information->objects_per_block;
  index = _Objects_Get_index( the_object->id ) - OBJECTS_INDEX_MINIMUM;

  if ( _Objects_Is_in_allocated_block( index, objects_per_block ) ) {
    Objects_Maximum block;

    block = index / objects_per_block;

    information->inactive_per_block[ block ]--;
    information->inactive--;
  }
}

/**
 * @brief Allocate an object and extend the objects information on demand.
 *
 * This function must be only used in case this objects information supports
 * unlimited objects.
 *
 * @param information The object information block.
 * @param extend The object information extend handler.
 */
static inline Objects_Control *_Objects_Allocate_with_extend(
  Objects_Information   *information,
  void                ( *extend )( Objects_Information * )
)
{
  Objects_Control *the_object;

  _Assert( _Objects_Is_auto_extend( information ) );

  the_object = _Objects_Get_inactive( information );

  if ( the_object == NULL ) {
    ( *extend )( information );
    the_object = _Objects_Get_inactive( information );
  }

  if ( the_object != NULL ) {
    _Objects_Activate_unlimited( information, the_object );
  }

  return the_object;
}

/**
 * @brief This function does nothing.
 *
 * @param ptr is not used.
 */
void _Objects_Free_nothing( void *ptr );

/** @} */

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif


#endif
/* end of include file */
