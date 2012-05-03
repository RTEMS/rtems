/**
 * @file  rtems/score/object.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OBJECT_H
#define _RTEMS_SCORE_OBJECT_H

#include <rtems/score/chain.h>
#include <rtems/score/isr.h>

#if defined(RTEMS_POSIX_API)
  /**
   *  This macro is defined when an API is enabled that requires the
   *  use of strings for object names.  Since the Classic API uses
   *  32-bit unsigned integers and not strings, this allows us to
   *  disable this in the smallest RTEMS configuratinos.
   */
  #define RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Score SuperCore
 *
 * @brief Provides services for all APIs.
 */

/**
 * @defgroup ScoreCPU CPU Architecture Support
 *
 * @ingroup Score
 *
 * @brief Provides CPU architecture dependent services.
 */

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

/**
 *  Functions which compare names are prototyped like this.
 */
typedef bool    (*Objects_Name_comparators)(
  void       * /* name_1 */,
  void       * /* name_2 */,
  uint16_t     /* length */
);

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
  OBJECTS_POSIX_API    = 3
} Objects_APIs;

/** This macro is used to generically specify the last API index. */
#define OBJECTS_APIS_LAST OBJECTS_POSIX_API

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
  /** This is this object class' method called when extracting a thread. */
  Objects_Thread_queue_Extract_callout extract;
  #if defined(RTEMS_MULTIPROCESSING)
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
 *  This function extends an object class information record.
 *
 *  @param[in] information points to an object class information block.
 */
void _Objects_Extend_information(
  Objects_Information *information
);

/**
 *  This function shrink an object class information record.
 *
 *  @param[in] information points to an object class information block.
 */
void _Objects_Shrink_information(
  Objects_Information *information
);

/**
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
 *  This function returns the highest numeric value of a valid
 *  API for the specified @a api.
 *
 *  @param[in] api is the API of interest
 *
 *  @return A positive integer on success and 0 otherwise.
 */
unsigned int _Objects_API_maximum_class(
  uint32_t api
);

/**
 *  This function allocates a object control block from
 *  the inactive chain of free object control blocks.
 *
 *  @param[in] information points to an object class information block.
 */
Objects_Control *_Objects_Allocate(
  Objects_Information *information
);

/**
 *
 *  This function frees a object control block to the
 *  inactive chain of free object control blocks.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] the_object points to the object to deallocate.
 */
void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
);

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
 *  This method converts an object name to an Id.  It performs a look up
 *  using the object information block for this object class.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] name is the name of the object to find.
 *  @param[in] node is the set of nodes to search.
 *  @param[in] id will contain the Id if the search is successful.
 *
 *  @return This method returns one of the values from the
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
 *  This method converts an object name to an Id.  It performs a look up
 *  using the object information block for this object class.
 *
 *  @param[in] information points to an object class information block.
 *  @param[in] name is the name of the object to find.
 *  @param[in] id will contain the Id if the search is successful.
 *
 *  @return This method returns one of the values from the
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
 *  This function implements the common portion of the object Id
 *  to name directives.  This function returns the name
 *  associated with object id.
 *
 *  @param[in] id is the Id of the object whose name we are locating.
 *  @param[in] name will contain the name of the object, if found.
 *
 *  @return This method returns one of the values from the
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
 *  @return This method returns one of the values from the
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
 *  @param[in] level is the interrupt level being turned.
 *
 *  @return This method returns one of the values from the
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
  ISR_Level           *level
);

/**
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
 *  @return This method returns one of the values from the
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
 *  @return This method returns the pointer to the object located or
 *          NULL on error.
 */
Objects_Control *_Objects_Get_next(
    Objects_Information *information,
    Objects_Id           id,
    Objects_Locations   *location_p,
    Objects_Id          *next_id_p
);

/**
 *  This function return the information structure given
 *  an the API and Class.  This can be done independent of
 *  the existence of any objects created by the API.
 *
 *  @param[in] the_api indicates the API for the information we want
 *  @param[in] the_class indicates the Class for the information we want
 *
 *  @return This method returns a pointer to the Object Information Table
 *          for the class of objects which corresponds to this object ID.
 */
Objects_Information *_Objects_Get_information(
  Objects_APIs   the_api,
  uint16_t       the_class
);

/**
 *  This function return the information structure given
 *  an id of an object.
 *
 *  @param[in] id is an object ID
 *
 *  @return This method returns a pointer to the Object Information Table
 *          for the class of objects which corresponds to this object ID.
 */
Objects_Information *_Objects_Get_information_id(
  Objects_Id  id
);

/**
 *  This method objects the name of an object and returns its name
 *  in the form of a C string.  It attempts to be careful about
 *  overflowing the user's string and about returning unprintable characters.
 *
 *  @param[in] id is the object to obtain the name of
 *  @param[in] length indicates the length of the caller's buffer
 *  @param[in] name points a string which will be filled in.
 *
 *  @return This method returns @a name or NULL on error. @a *name will
 *          contain the name if successful.
 */
char *_Objects_Get_name_as_string(
  Objects_Id   id,
  size_t       length,
  char        *name
);

/**
 *  This method sets the object name to either a copy of a string
 *  or up to the first four characters of the string based upon
 *  whether this object class uses strings for names.
 *
 *  @param[in] information points to the object information structure
 *  @param[in] the_object is the object to operate upon
 *  @param[in] name is a pointer to the name to use
 *
 *  @return If successful, true is returned.  Otherwise false is returned.
 */
bool _Objects_Set_name(
  Objects_Information *information,
  Objects_Control     *the_object,
  const char          *name
);

/**
 *  This function removes the_object from the namespace.
 *
 *  @param[in] information points to an Object Information Table
 *  @param[in] the_object is a pointer to an object
 */
void _Objects_Namespace_remove(
  Objects_Information  *information,
  Objects_Control      *the_object
);

/**
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

/*
 *  Pieces of object.inl are promoted out to the user
 */

#include <rtems/score/object.inl>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
