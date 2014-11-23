/**
 * @file  rtems/score/object.h
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
#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Score SuperCore
 *
 * @brief Provides services for all APIs.
 */
/**@{*/

#if defined(RTEMS_POSIX_API)
  /**
   *  This macro is defined when an API is enabled that requires the
   *  use of strings for object names.  Since the Classic API uses
   *  32-bit unsigned integers and not strings, this allows us to
   *  disable this in the smallest RTEMS configuratinos.
   */
  #define RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES
#endif

/**
 * @defgroup ScoreCPU CPU Architecture Support
 *
 * @ingroup Score
 *
 * @brief Provides CPU architecture dependent services.
 */
/**@{*/

/**
 *  @defgroup ScoreObject Object Handler
 *
 *  @ingroup Score
 */
/**@{*/

/**
 *  The following type defines the control block used to manage
 *  object names.
 */
typedef union {
  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    /** This is a pointer to a string name. */
    const char *name_p;
  #endif
  /** This is the actual 32-bit "raw" integer name. */
  uint32_t    name_u32;
} Objects_Name;

#if defined(RTEMS_USE_16_BIT_OBJECT)
/**
 *  The following type defines the control block used to manage
 *  object IDs.  The format is as follows (0=LSB):
 *
 *     Bits  0 ..  7    = index  (up to 254 objects of a type)
 *     Bits  8 .. 10    = API    (up to 7 API classes)
 *     Bits 11 .. 15    = class  (up to 31 object types per API)
 */
typedef uint16_t   Objects_Id;

/**
 * This type is used to store the maximum number of allowed objects
 * of each type.
 */
typedef uint8_t    Objects_Maximum;

#define OBJECTS_INDEX_START_BIT  0U
#define OBJECTS_API_START_BIT    8U
#define OBJECTS_CLASS_START_BIT 11U

#define OBJECTS_INDEX_MASK      (Objects_Id)0x00ffU
#define OBJECTS_API_MASK        (Objects_Id)0x0700U
#define OBJECTS_CLASS_MASK      (Objects_Id)0xF800U

#define OBJECTS_INDEX_VALID_BITS  (Objects_Id)0x00ffU
#define OBJECTS_API_VALID_BITS    (Objects_Id)0x0007U
/* OBJECTS_NODE_VALID_BITS should not be used with 16 bit Ids */
#define OBJECTS_CLASS_VALID_BITS  (Objects_Id)0x001fU

#define OBJECTS_UNLIMITED_OBJECTS 0x8000U

#define OBJECTS_ID_INITIAL_INDEX  (0)
#define OBJECTS_ID_FINAL_INDEX    (0xff)

#else
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
#endif

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
#endif

/**
 *  No object can have this ID.
 */
#define OBJECTS_ID_NONE 0

/**
 *  The following defines the constant which may be used
 *  with _Objects_Get to manipulate the calling task.
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
 * This function returns the API portion of the ID.
 *
 * @param[in] id is the object Id to be processed.
 *
 * @return This method returns an object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_APIs _Objects_Get_API(
  Objects_Id id
)
{
  return (Objects_APIs) ((id >> OBJECTS_API_START_BIT) & OBJECTS_API_VALID_BITS);
}

/**
 * This function returns the class portion of the ID.
 *
 * @param[in] id is the object Id to be processed
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_class(
  Objects_Id id
)
{
  return (uint32_t)
    ((id >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS);
}

/**
 * This function returns the node portion of the ID.
 *
 * @param[in] id is the object Id to be processed
 *
 * @return This method returns the node portion of an object ID.
 */
RTEMS_INLINE_ROUTINE uint32_t _Objects_Get_node(
  Objects_Id id
)
{
  /*
   * If using 16-bit Ids, then there is no node field and it MUST
   * be a single processor system.
   */
  #if defined(RTEMS_USE_16_BIT_OBJECT)
    return 1;
  #else
    return (id >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS;
  #endif
}

/**
 * This function returns the index portion of the ID.
 *
 * @param[in] id is the Id to be processed
 *
 * @return This method returns the class portion of the specified object ID.
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
 * This function builds an object's id from the processor node and index
 * values specified.
 *
 * @param[in] the_api indicates the API associated with this Id.
 * @param[in] the_class indicates the class of object.
 *            It is specific to @a the_api.
 * @param[in] node is the node where this object resides.
 * @param[in] index is the instance number of this object.
 *
 * @return This method returns an object Id constructed from the arguments.
 */
RTEMS_INLINE_ROUTINE Objects_Id _Objects_Build_id(
  Objects_APIs     the_api,
  uint16_t         the_class,
  uint8_t          node,
  uint16_t         index
)
{
  return (( (Objects_Id) the_api )   << OBJECTS_API_START_BIT)   |
         (( (Objects_Id) the_class ) << OBJECTS_CLASS_START_BIT) |
         #if !defined(RTEMS_USE_16_BIT_OBJECT)
           (( (Objects_Id) node )    << OBJECTS_NODE_START_BIT)  |
         #endif
         (( (Objects_Id) index )     << OBJECTS_INDEX_START_BIT);
}

/**
 * Returns if the object maximum specifies unlimited objects.
 *
 * @param[in] maximum The object maximum specification.
 *
 * @retval true Unlimited objects are available.
 * @retval false The object count is fixed.
 */
RTEMS_INLINE_ROUTINE bool _Objects_Is_unlimited( uint32_t maximum )
{
  return (maximum & OBJECTS_UNLIMITED_OBJECTS) != 0;
}

/*
 * We cannot use an inline function for this since it may be evaluated at
 * compile time.
 */
#define _Objects_Maximum_per_allocation( maximum ) \
  ((Objects_Maximum) ((maximum) & ~OBJECTS_UNLIMITED_OBJECTS))

/**@}*/
/**@}*/
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
