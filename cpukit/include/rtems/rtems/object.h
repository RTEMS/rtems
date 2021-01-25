/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides the Object Services API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2009 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/object/if/header */

#ifndef _RTEMS_RTEMS_OBJECT_H
#define _RTEMS_RTEMS_OBJECT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/object/if/group */

/**
 * @defgroup RTEMSAPIClassicObject Object Services
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief RTEMS provides a collection of services to assist in the management
 *   and usage of the objects created and utilized via other managers.  These
 *   services assist in the manipulation of RTEMS objects independent of the
 *   API used to create them.
 */

/* Generated from spec:/rtems/object/if/api-class-information */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This structure is used to return information to the application about
 *   the objects configured for a specific API/Class combination.
 */
typedef struct {
  /**
   * @brief This member contains the minimum valid object identifier for this
   *   class.
   */
  rtems_id minimum_id;

  /**
   * @brief This member contains the maximum valid object identifier for this
   *   class.
   */
  rtems_id maximum_id;

  /**
   * @brief This member contains the maximum number of active objects configured
   *   for this class.
   */
  uint32_t maximum;

  /**
   * @brief This member is true, if this class is configured for automatic object
   *   extension, otherwise it is false.
   */
  bool auto_extend;

  /**
   * @brief This member contains the number of currently inactive objects of this
   *   class.
   */
  uint32_t unallocated;
} rtems_object_api_class_information;

/* Generated from spec:/rtems/object/if/id-final */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant represents the highest object identifier value.
 */
#define RTEMS_OBJECT_ID_FINAL OBJECTS_ID_FINAL

/* Generated from spec:/rtems/object/if/id-final-index */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant represents the highest value for the index component of
 *   an object identifier.
 */
#define RTEMS_OBJECT_ID_FINAL_INDEX OBJECTS_ID_FINAL_INDEX

/* Generated from spec:/rtems/object/if/id-initial */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Builds the object identifier with the lowest index from the API,
 *   class, and MPCI node components.
 *
 * @param _api is the API of the object identifier to build.
 *
 * @param _class is the class of the object identifier to build.
 *
 * @param _node is the MPCI node of the object identifier to build.
 *
 * @return Returns the object identifier with the lowest index built from the
 *   API, class, and MPCI node components.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
#define RTEMS_OBJECT_ID_INITIAL( _api, _class, _node ) \
  OBJECTS_ID_INITIAL( _api, _class, _node )

/* Generated from spec:/rtems/object/if/id-initial-index */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant represents the lowest value for the index component of
 *   an object identifier.
 */
#define RTEMS_OBJECT_ID_INITIAL_INDEX OBJECTS_ID_INITIAL_INDEX

/* Generated from spec:/rtems/object/if/search-all-nodes */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant indicates that an object name to identifier search
 *   should search through all MPCI nodes of the system.
 */
#define RTEMS_SEARCH_ALL_NODES OBJECTS_SEARCH_ALL_NODES

/* Generated from spec:/rtems/object/if/search-local-node */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant indicates that an object name to identifier search
 *   should search only the local MPCI node of the system.
 */
#define RTEMS_SEARCH_LOCAL_NODE OBJECTS_SEARCH_LOCAL_NODE

/* Generated from spec:/rtems/object/if/search-other-nodes */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant indicates that an object name to identifier search
 *   should search through all MPCI nodes of the system except the local node.
 */
#define RTEMS_SEARCH_OTHER_NODES OBJECTS_SEARCH_OTHER_NODES

/* Generated from spec:/rtems/object/if/who-am-i */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief This constant indicates that an object name to identifier search is
 *   being asked for the identifier of the currently executing task.
 */
#define RTEMS_WHO_AM_I OBJECTS_WHO_AM_I

/* Generated from spec:/rtems/object/if/build-id */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Builds the object identifier from the API, class, MPCI node, and
 *   index components.
 *
 * @param _api is the API of the object identifier to build.
 *
 * @param _class is the class of the object identifier to build.
 *
 * @param _node is the MPCI node of the object identifier to build.
 *
 * @param _index is the index of the object identifier to build.
 *
 * @return Returns the object identifier built from the API, class, MPCI node,
 *   and index components.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
#define rtems_build_id( _api, _class, _node, _index ) \
  _Objects_Build_id( _api, _class, _node, _index )

/* Generated from spec:/rtems/object/if/build-name */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Builds the object name composed of the four characters.
 *
 * @param _c1 is the first character of the name.
 *
 * @param _c2 is the second character of the name.
 *
 * @param _c3 is the third character of the name.
 *
 * @param _c4 is the fourth character of the name.
 *
 * This directive takes the four characters provided as arguments and composes
 * a 32-bit object name with ``_c1`` in the most significant 8-bits and ``_c4``
 * in the least significant 8-bits.
 *
 * @return Returns the object name composed of the four characters.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
#define rtems_build_name( _c1, _c2, _c3, _c4 ) \
  _Objects_Build_name( _c1, _c2, _c3, _c4 )

/* Generated from spec:/rtems/object/if/get-classic-name */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the object name associated with the object identifier.
 *
 * @param id is the object identifier to get the name.
 *
 * @param[out] name is the pointer to an object name variable.  The object name
 *   associated with the object identifier will be stored in this variable, in
 *   case of a successful operation.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``name`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no object information available for the
 *   object identifier.
 *
 * @retval ::RTEMS_INVALID_ID The object name associated with the object
 *   identifier was a string.
 *
 * @retval ::RTEMS_INVALID_ID There was no object associated with the object
 *   identifier.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
rtems_status_code rtems_object_get_classic_name(
  rtems_id    id,
  rtems_name *name
);

/* Generated from spec:/rtems/object/if/get-name */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the object name associated with the object identifier as a
 *   string.
 *
 * @param id is the object identifier to get the name.
 *
 * @param length is the buffer length in bytes.
 *
 * @param[out] name is the pointer to a buffer of the specified length.
 *
 * The object name is stored in the name buffer.  If the name buffer length is
 * greater than zero, then the stored object name will be ``NUL`` terminated.
 * The stored object name may be truncated to fit the length.  There is no
 * indication if a truncation occurred.  Every attempt is made to return name
 * as a printable string even if the object has the Classic API 32-bit integer
 * style name.
 *
 * @retval NULL The ``length`` parameter was 0.
 *
 * @retval NULL The ``name`` parameter was NULL.
 *
 * @retval NULL There was no object information available for the object
 *   identifier.
 *
 * @retval NULL There was no object associated with the object identifier.
 *
 * @return Returns the ``name`` parameter value, if there is an object name
 *   associated with the object identifier.
 *
 * @par Notes
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 */
char *rtems_object_get_name( rtems_id id, size_t length, char *name );

/* Generated from spec:/rtems/object/if/set-name */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Sets the object name of the object associated with the object
 *   identifier.
 *
 * @param id is the object identifier of the object to set the name.
 *
 * @param name is the object name to set.
 *
 * This directive will set the object name based upon the user string.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``name`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no object information available for the
 *   object identifier.
 *
 * @retval ::RTEMS_INVALID_ID There was no object associated with the object
 *   identifier.
 *
 * @retval ::RTEMS_NO_MEMORY There was no memory available to duplicate the
 *   name.
 *
 * @par Notes
 * @parblock
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 *
 * This directive can be used to set the name of objects which do not have a
 * naming scheme per their API.
 *
 * If the object specified by ``id`` is of a class that has a string name, this
 * directive will free the existing name to the RTEMS Workspace and allocate
 * enough memory from the RTEMS Workspace to make a copy of the string located
 * at ``name``.
 *
 * If the object specified by ``id`` is of a class that has a 32-bit integer
 * style name, then the first four characters in ``name`` will be used to
 * construct the name.
 * @endparblock
 */
rtems_status_code rtems_object_set_name( rtems_id id, const char *name );

/* Generated from spec:/rtems/object/if/id-get-api */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the API component of the object identifier.
 *
 * @param _id is the object identifier with the API component to get.
 *
 * @return Returns the API component of the object identifier.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * This directive does not validate the object identifier provided in ``_id``.
 *
 * A body is also provided.
 * @endparblock
 */
#define rtems_object_id_get_api( _id ) _Objects_Get_API( _id )

/* Generated from spec:/rtems/object/if/id-get-class */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the class component of the object identifier.
 *
 * @param _id is the object identifier with the class component to get.
 *
 * @return Returns the class component of the object identifier.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * This directive does not validate the object identifier provided in ``_id``.
 *
 * A body is also provided.
 * @endparblock
 */
#define rtems_object_id_get_class( _id ) _Objects_Get_class( _id )

/* Generated from spec:/rtems/object/if/id-get-node */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the MPCI node component of the object identifier.
 *
 * @param _id is the object identifier with the MPCI node component to get.
 *
 * @return Returns the MPCI node component of the object identifier.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * This directive does not validate the object identifier provided in ``_id``.
 *
 * A body is also provided.
 * @endparblock
 */
#define rtems_object_id_get_node( _id ) _Objects_Get_node( _id )

/* Generated from spec:/rtems/object/if/id-get-index */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the index component of the object identifier.
 *
 * @param _id is the object identifier with the index component to get.
 *
 * @return Returns the index component of the object identifier.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * This directive does not validate the object identifier provided in ``_id``.
 *
 * A body is also provided.
 * @endparblock
 */
#define rtems_object_id_get_index( _id ) _Objects_Get_index( _id )

/* Generated from spec:/rtems/object/if/id-api-minimum */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the lowest valid value for the API component of an object
 *   identifier.
 *
 * @return Returns the lowest valid value for the API component of an object
 *   identifier.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * A body is also provided.
 * @endparblock
 */
#define rtems_object_id_api_minimum() OBJECTS_INTERNAL_API

/* Generated from spec:/rtems/object/if/id-api-maximum */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the highest valid value for the API component of an object
 *   identifier.
 *
 * @return Returns the highest valid value for the API component of an object
 *   identifier.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * A body is also provided.
 * @endparblock
 */
#define rtems_object_id_api_maximum() OBJECTS_APIS_LAST

/* Generated from spec:/rtems/object/if/api-minimum-class */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the lowest valid class value of the object API.
 *
 * @param api is the object API to get the lowest valid class value.
 *
 * @retval -1 The object API was invalid.
 *
 * @return Returns the lowest valid class value of the object API.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
int rtems_object_api_minimum_class( int api );

/* Generated from spec:/rtems/object/if/api-maximum-class */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the highest valid class value of the object API.
 *
 * @param api is the object API to get the highest valid class value.
 *
 * @retval 0 The object API was invalid.
 *
 * @return Returns the highest valid class value of the object API.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
int rtems_object_api_maximum_class( int api );

/* Generated from spec:/rtems/object/if/get-api-name */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets a descriptive name of the object API.
 *
 * @param api is the object API to get the name.
 *
 * @retval "BAD API" The API was invalid.
 *
 * @return Returns a descriptive name of the API, if the API was valid.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * The string returned is from constant space.  Do not modify or free it.
 * @endparblock
 */
const char *rtems_object_get_api_name( int api );

/* Generated from spec:/rtems/object/if/get-api-class-name */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets a descriptive name of the object class of the object API.
 *
 * @param the_api is the object API of the object class.
 *
 * @param the_class is the object class of the object API to get the name.
 *
 * @retval "BAD API" The API was invalid.
 *
 * @retval "BAD CLASS" The class of the API was invalid.
 *
 * @return Returns a descriptive name of the class of the API, if the class of
 *   the API and the API were valid.
 *
 * @par Notes
 * @parblock
 * This directive is strictly local and does not impact task scheduling.
 *
 * The string returned is from constant space.  Do not modify or free it.
 * @endparblock
 */
const char *rtems_object_get_api_class_name( int the_api, int the_class );

/* Generated from spec:/rtems/object/if/get-class-information */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the object class information of the object class of the object
 *   API.
 *
 * @param the_api is the object API of the object class.
 *
 * @param the_class is the object class of the object API to get the class
 *   information.
 *
 * @param info is the pointer to an object class information variable.  The
 *   object class information of the class of the API will be stored in this
 *   variable, in case of a successful operation.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``info`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NUMBER The class of the API or the API was invalid.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
rtems_status_code rtems_object_get_class_information(
  int                                 the_api,
  int                                 the_class,
  rtems_object_api_class_information *info
);

/* Generated from spec:/rtems/object/if/get-local-node */

/**
 * @ingroup RTEMSAPIClassicObject
 *
 * @brief Gets the local MPCI node number.
 *
 * @return Returns the local MPCI node number.
 *
 * @par Notes
 * This directive is strictly local and does not impact task scheduling.
 */
static inline uint16_t rtems_object_get_local_node( void )
{
  return _Objects_Local_node;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_OBJECT_H */
