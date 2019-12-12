/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief Constants and Structures Associated with the Object Handler
 *
 * This include file contains all the constants and structures associated
 * with the Object Handler.  This Handler provides mechanisms which
 * can be used to initialize and manipulate all objects which have ids.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECT_H
#define _RTEMS_SCORE_OBJECT_H

#include <rtems/score/basedefs.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScore SuperCore
 *
 * @ingroup RTEMSInternal
 *
 * @brief Provides services for all APIs.
 *
 * @{
 */

/**
 * @defgroup RTEMSScoreCPU CPU Architecture Support
 *
 * @ingroup RTEMSScore
 *
 * @brief Provides CPU architecture dependent services.
 *
 * @{
 */

/**
 * @defgroup RTEMSScoreObject Object Handler
 *
 * @ingroup RTEMSScore
 *
 * @{
 */

/**
 *  The following type defines the control block used to manage
 *  object names.
 */
typedef union {
  /** This is a pointer to a string name. */
  const char *name_p;
  /** This is the actual 32-bit "raw" integer name. */
  uint32_t    name_u32;
} Objects_Name;

/**
 *  The following type defines the control block used to manage
 *  object IDs.  The format is as follows (0=LSB):
 *
 *     Bits  0 .. 15    = index  (up to 65535 objects of a type)
 *     Bits 16 .. 23    = node   (up to 255 nodes)
 *     Bits 24 .. 26    = API    (up to 7 API classes)
 *     Bits 27 .. 31    = class  (up to 31 object types per API)
 */
typedef uint32_t   Objects_Id;

/**
 * This type is used to store the maximum number of allowed objects
 * of each type.
 */
typedef uint16_t   Objects_Maximum;

/**
 *  This is the bit position of the starting bit of the index portion of
 *  the object Id.
 */
#define OBJECTS_INDEX_START_BIT  0U
/**
 *  This is the bit position of the starting bit of the node portion of
 *  the object Id.
 */
#define OBJECTS_NODE_START_BIT  16U

/**
 *  This is the bit position of the starting bit of the API portion of
 *  the object Id.
 */
#define OBJECTS_API_START_BIT   24U

/**
 *  This is the bit position of the starting bit of the class portion of
 *  the object Id.
 */
#define OBJECTS_CLASS_START_BIT 27U

/**
 *  This mask is used to extract the index portion of an object Id.
 */
#define OBJECTS_INDEX_MASK      (Objects_Id)0x0000ffffU

/**
 *  This mask is used to extract the node portion of an object Id.
 */
#define OBJECTS_NODE_MASK       (Objects_Id)0x00ff0000U

/**
 *  This mask is used to extract the API portion of an object Id.
 */
#define OBJECTS_API_MASK        (Objects_Id)0x07000000U

/**
 *  This mask is used to extract the class portion of an object Id.
 */
#define OBJECTS_CLASS_MASK      (Objects_Id)0xf8000000U

/**
 *  This mask represents the bits that is used to ensure no extra bits
 *  are set after shifting to extract the index portion of an object Id.
 */
#define OBJECTS_INDEX_VALID_BITS  (Objects_Id)0x0000ffffU

/**
 *  This mask represents the bits that is used to ensure no extra bits
 *  are set after shifting to extract the node portion of an object Id.
 */
#define OBJECTS_NODE_VALID_BITS   (Objects_Id)0x000000ffU

/**
 *  This mask represents the bits that is used to ensure no extra bits
 *  are set after shifting to extract the API portion of an object Id.
 */
#define OBJECTS_API_VALID_BITS    (Objects_Id)0x00000007U

/**
 *  This mask represents the bits that is used to ensure no extra bits
 *  are set after shifting to extract the class portion of an object Id.
 */
#define OBJECTS_CLASS_VALID_BITS  (Objects_Id)0x0000001fU

/**
 *  Mask to enable unlimited objects.  This is used in the configuration
 *  table when specifying the number of configured objects.
 */
#define OBJECTS_UNLIMITED_OBJECTS 0x80000000U

/**
 *  This is the lowest value for the index portion of an object Id.
 */
#define OBJECTS_ID_INITIAL_INDEX  (0)

/**
 *  This is the highest value for the index portion of an object Id.
 */
#define OBJECTS_ID_FINAL_INDEX    (0xffffU)

/**
 *  This enumerated type is used in the class field of the object ID.
 */
typedef enum {
  OBJECTS_NO_API       = 0,
  OBJECTS_INTERNAL_API = 1,
  OBJECTS_CLASSIC_API  = 2,
  OBJECTS_POSIX_API    = 3,
  OBJECTS_FAKE_OBJECTS_API = 7
} Objects_APIs;

/** This macro is used to generically specify the last API index. */
#define OBJECTS_APIS_LAST OBJECTS_POSIX_API

/**
 *  No object can have this ID.
 */
#define OBJECTS_ID_NONE 0

/**
 *  The following defines the constant which may be used
 *  to manipulate the calling task.
 */
#define OBJECTS_ID_OF_SELF ((Objects_Id) 0)

/**
 *  The following constant is used to specify that a name to ID search
 *  should search through all nodes.
 */
#define OBJECTS_SEARCH_ALL_NODES   0

/**
 *  The following constant is used to specify that a name to ID search
 *  should search through all nodes except the current node.
 */
#define OBJECTS_SEARCH_OTHER_NODES 0x7FFFFFFE

/**
 *  The following constant is used to specify that a name to ID search
 *  should search only on this node.
 */
#define OBJECTS_SEARCH_LOCAL_NODE  0x7FFFFFFF

/**
 *  The following constant is used to specify that a name to ID search
 *  is being asked for the ID of the currently executing task.
 */
#define OBJECTS_WHO_AM_I           0

/**
 *  This macros calculates the lowest ID for the specified api, class,
 *  and node.
 */
#define OBJECTS_ID_INITIAL(_api, _class, _node) \
  _Objects_Build_id( (_api), (_class), (_node), OBJECTS_ID_INITIAL_INDEX )

/**
 *  This macro specifies the highest object ID value
 */
#define OBJECTS_ID_FINAL           ((Objects_Id)~0)

/**
 *  This macro is used to build a thirty-two bit style name from
 *  four characters.  The most significant byte will be the
 *  character @a _C1.
 *
 *  @param[in] _C1 is the first character of the name
 *  @param[in] _C2 is the second character of the name
 *  @param[in] _C3 is the third character of the name
 *  @param[in] _C4 is the fourth character of the name
 */
#define  _Objects_Build_name( _C1, _C2, _C3, _C4 ) \
  ( (uint32_t)(_C1) << 24 | \
    (uint32_t)(_C2) << 16 | \
    (uint32_t)(_C3) << 8 | \
    (uint32_t)(_C4) )

/**
 * @brief Returns the API portion of the ID.
 *
 * @param id The object Id to be processed.
 *
 * @return An object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_APIs _Objects_Get_API(
  Objects_Id id
)
{
  return (Objects_APIs) ((id >> OBJECTS_API_START_BIT) & OBJECTS_API_VALID_BITS);
}

/**
 * @brief Returns the class portion of the ID.
 *
 * @param id The object Id to be processed.
 *
 * @return The class portion of the ID.
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_class(
  Objects_Id id
)
{
  return (uint32_t)
    ((id >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS);
}

/**
 * @brief Returns the node portion of the ID.
 *
 * @param id The object Id to be processed.
 *
 * @return Returns the node portion of an object ID.
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_node(
  Objects_Id id
)
{
  return (id >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS;
}

/**
 * @brief Returns the index portion of the ID.
 *
 * @param id is the Id to be processed.
 *
 * @return Returns the index portion of the specified object ID.
 */
RTEMS_INLINE_ROUTINE Objects_Maximum _Objects_Get_index(
  Objects_Id id
)
{
  return
    (Objects_Maximum)((id >> OBJECTS_INDEX_START_BIT) &
                                          OBJECTS_INDEX_VALID_BITS);
}

/**
 * @brief Builds an object ID from its components.
 *
 * @param the_api The object API.
 * @param the_class The object API class.
 * @param node The object node.
 * @param index The object index.
 *
 * @return Returns the object ID constructed from the arguments.
 */
#define _Objects_Build_id( the_api, the_class, node, index ) \
  ( (Objects_Id) ( (Objects_Id) the_api   << OBJECTS_API_START_BIT )   | \
                 ( (Objects_Id) the_class << OBJECTS_CLASS_START_BIT ) | \
                 ( (Objects_Id) node      << OBJECTS_NODE_START_BIT )  | \
                 ( (Objects_Id) index     << OBJECTS_INDEX_START_BIT ) )

/**
 * Returns if the object maximum specifies unlimited objects.
 *
 * @param[in] maximum The object maximum specification.
 *
 * @retval true Unlimited objects are available.
 * @retval false The object count is fixed.
 */
#define _Objects_Is_unlimited( maximum ) \
  ( ( ( maximum ) & OBJECTS_UNLIMITED_OBJECTS ) != 0 )

/*
 * We cannot use an inline function for this since it may be evaluated at
 * compile time.
 */
#define _Objects_Maximum_per_allocation( maximum ) \
  ((Objects_Maximum) ((maximum) & ~OBJECTS_UNLIMITED_OBJECTS))

/**
 * @brief The local MPCI node number.
 */
#if defined(RTEMS_MULTIPROCESSING)
extern uint16_t _Objects_Local_node;
#else
#define _Objects_Local_node ((uint16_t) 1)
#endif

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
