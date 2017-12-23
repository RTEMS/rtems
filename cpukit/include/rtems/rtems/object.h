/**
 * @file rtems/rtems/object.h
 *
 * @defgroup ClassicClassInfo Object Class Information
 *
 * @ingroup ClassicRTEMS
 * @brief Classic API interfaces to Object Services
 *
 * This include file defines Classic API interfaces to Object Services.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_OBJECT_H
#define _RTEMS_RTEMS_OBJECT_H

#include <stdint.h>
#include <rtems/score/object.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicClassInfo Object Class Information
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the Classic API Object
 *  Class Services.
 */
/**@{*/

/**
 *  This structure is used to return information to the application
 *  about the objects configured for a specific API/Class combination.
 */
typedef struct {
  /** This field is the minimum valid object Id for this class. */
  rtems_id  minimum_id;
  /** This field is the maximum valid object Id for this class. */
  rtems_id  maximum_id;
  /** This field is the number of object instances configured for this class. */
  uint32_t  maximum;
  /** This field indicates if the class is configured for auto-extend. */
  bool      auto_extend;
  /** This field is the number of currently unallocated objects. */
  uint32_t  unallocated;
} rtems_object_api_class_information;

/**
 * @brief Build Object Id
 *
 * This function returns an object id composed of the
 * specified @a api, @a class, @a node,
 * and @a index.
 *
 * @param[in] _api indicates the api to use for the Id
 * @param[in] _class indicates the class to use for the Id
 * @param[in] _node indicates the node to use for the Id
 * @param[in] _index indicates the index to use for the Id
 *
 * @retval This method returns an object Id built from the
 *         specified values.
 *
 * @note A body is also provided.
 */
#define rtems_build_id( _api, _class, _node, _index ) \
  _Objects_Build_id( _api, _class, _node, _index )

/**
 * @brief Build Thirty-Two Bit Object Name
 *
 * RTEMS Object Helper -- Build an Object Id
 *
 * This function returns an object name composed of the four characters
 * C1, C2, C3, and C4.
 *
 * @param[in] _C1 is the first character of the name
 * @param[in] _C2 is the second character of the name
 * @param[in] _C3 is the third character of the name
 * @param[in] _C4 is the fourth character of the name
 *
 * @note This must be implemented as a macro for use in
 *       Configuration Tables. A body is also provided.
 *
 */
#define rtems_build_name( _C1, _C2, _C3, _C4 ) \
  _Objects_Build_name( _C1, _C2, _C3, _C4 )

/**
 * @brief Obtain Name of Object
 *
 * This directive returns the name associated with the specified
 * object ID.
 *
 * @param[in] id is the Id of the object to obtain the name of.
 * @param[out] name will be set to the name of the object
 *
 * @note The object must be have a name of the 32-bit form.
 *
 * @retval @a *name will contain user defined object name
 * @retval @a RTEMS_SUCCESSFUL - if successful
 * @retval error code - if unsuccessful
 */
rtems_status_code rtems_object_get_classic_name(
  rtems_id      id,
  rtems_name   *name
);

/**
 * @brief Obtain Object Name as String
 *
 * This directive returns the name associated with the specified
 * object ID.
 *
 * @param[in] id is the Id of the object to obtain the name of
 * @param[in] length is the length of the output name buffer
 * @param[out] name will be set to the name of the object
 *
 * @retval @a *name will contain user defined object name
 * @retval @a name - if successful
 * @retval @a NULL - if unsuccessful
 */
char *rtems_object_get_name(
  rtems_id       id,
  size_t         length,
  char          *name
);

/**
 * @brief Set Name of Object
 *
 * This method allows the caller to set the name of an
 * object. This can be used to set the name of objects
 * which do not have a naming scheme per their API.
 *
 * RTEMS Object Helper -- Set Name of Object as String
 *
 * @param[in] id is the Id of the object to obtain the name of
 * @param[out] name will be set to the name of the object
 *
 * @retval @a *name will contain user defined object name
 * @retval @a RTEMS_SUCCESSFUL - if successful
 * @retval error code - if unsuccessful
 */
rtems_status_code rtems_object_set_name(
  rtems_id       id,
  const char    *name
);

/**
 * @brief Get API Portion of Object Id
 *
 * RTEMS Object Helper -- Extract API From Id
 *
 * This function returns the API portion of the Id.
 *
 * @param[in] _id is the Id of the object to obtain the API from
 *
 * @retval This method returns the API portion of the provided
 *         @a _id.
 *
 * @note This method does NOT validate the @a _id provided.
 *
 * @note A body is also provided.
 */
#define rtems_object_id_get_api( _id ) \
  _Objects_Get_API( _id )

/**
 * @brief Get Class Portion of Object Id
 *
 * This function returns the class portion of the @a _id ID.
 *
 * @param[in] _id is the Id of the object to obtain the class from
 *
 * @retval This method returns the class portion of the provided
 *         @a _id.
 *
 * @note This method does NOT validate the @a _id provided.
 *
 * @note A body is also provided.
 */
#define rtems_object_id_get_class( _id ) \
  _Objects_Get_class( _id )

/**
 * @brief Get Node Portion of Object Id
 *
 * This function returns the node portion of the ID.
 *
 * @param[in] _id is the Id of the object to obtain the node from
 *
 * @retval This method returns the node portion of the provided
 *          @a _id.
 *
 * @note This method does NOT validate the @a _id provided.
 *
 * @note A body is also provided.
 */
#define rtems_object_id_get_node( _id ) \
  _Objects_Get_node( _id )

/**
 * @brief Get Index Portion of Object Id
 *
 * This function returns the index portion of the ID.
 *
 * @param[in] _id is the Id of the object to obtain the index from
 *
 * @retval This method returns the index portion of the provided
 *         @a _id.
 *
 * @note This method does NOT validate the @a _id provided.
 *
 * @note A body is also provided.
 */
#define rtems_object_id_get_index( _id ) \
  _Objects_Get_index( _id )

/**
 * @brief Get Lowest Valid API Index
 *
 * This method returns the lowest valid value for the API
 * portion of an RTEMS object Id.
 *
 * @retval This method returns the least valid value for
 *         the API portion of an RTEMS object Id.
 *
 * @note A body is also provided.
 */
#define rtems_object_id_api_minimum() \
  OBJECTS_INTERNAL_API

/**
 * @brief Get Highest Valid API Index
 *
 * This method returns the highest valid value for the API
 * portion of an RTEMS object Id.
 *
 * @retval This method returns the greatest valid value for
 *         the API portion of an RTEMS object Id.
 *
 * @note A body is also provided.
 */
#define rtems_object_id_api_maximum() \
  OBJECTS_APIS_LAST

/**
 * @brief Get Lowest Valid Class Value
 *
 * This method returns the lowest valid value Class for the
 * specified @a api. Each API supports a different number
 * of object classes.
 *
 * @param[in] api is the API to obtain the minimum class of
 *
 * @retval This method returns the least valid value for
 *         class number for the specified @a api.
 * RTEMS Object Helper -- Get Least Valid Class for an API
 */
int rtems_object_api_minimum_class(
  int api
);

/**
 * @brief Get Highest Valid Class Value
 *
 * This method returns the highest valid value Class for the
 * specified @a api. Each API supports a different number
 * of object classes.
 *
 * @param[in] api is the API to obtain the maximum class of
 *
 * @retval This method returns the greatet valid value for
 *         class number for the specified @a api.
 */
int rtems_object_api_maximum_class(
  int api
);


/**
 * @brief Get Highest Valid Class Value
 *
 * This method returns the lowest valid value Class for the
 * specified @a api. Each API supports a different number
 * of object classes.
 *
 * @param[in] api is the API to obtain the maximum class of
 *
 * @retval This method returns the least valid value for
 *         class number for the specified @a api.
 */
int rtems_object_id_api_maximum_class(
  int api
);

/**
 * @brief Get API Name
 *
 * This method returns a string containing the name of the
 * specified @a api.
 *
 * @param[in] api is the API to obtain the name of
 *
 * @retval If successful, this method returns the name of
 *         the specified @a api. Otherwise, it returns
 *         the string "BAD API"
 */
const char *rtems_object_get_api_name(
  int api
);

/**
 * @brief Get Class Name
 *
 * This method returns a string containing the name of the
 * @a class from the specified @a api.
 *
 * @param[in] the_api is the API for the class
 * @param[in] the_class is the class to obtain the name of
 *
 * @retval If successful, this method returns the name of
 *         the specified @a class. Otherwise, it returns
 *         the string "BAD CLASS"
 */
const char *rtems_object_get_api_class_name(
  int the_api,
  int the_class
);

/**
 * @brief Get Class Information
 *
 * This method returns a string containing the name of the
 * @a the_class from the specified @a api.
 *
 * @param[in] the_api is the API for the class
 * @param[in] the_class is the class to obtain information about
 * @param[in] info points to the information structure to fill in
 *
 * @retval If successful, this method returns the name of
 *         RTEMS_SUCCESSFUL with @a *info filled in. Otherwise,
 *         a status is returned to indicate the error.
 *
 */
rtems_status_code rtems_object_get_class_information(
  int                                 the_api,
  int                                 the_class,
  rtems_object_api_class_information *info
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
