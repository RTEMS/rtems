/*  object.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Object Handler.  This Handler provides mechanisms which
 *  can be used to initialize and manipulate all objects which have 
 *  ids.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __OBJECTS_h
#define __OBJECTS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/chain.h>
#include <rtems/score/isr.h>

/*
 *  Mask to enable unlimited objects
 */

#define OBJECTS_UNLIMITED_OBJECTS 0x80000000

/*
 *  The following type defines the control block used to manage
 *  object names.
 */

typedef void * Objects_Name;

/*
 *  Space for object names is allocated in multiples of this.
 *
 *  NOTE:  Must be a power of 2.  Matches the name manipulation routines.
 */

#define OBJECTS_NAME_ALIGNMENT     sizeof( unsigned32 )

/*
 *  Functions which compare names are prototyped like this.
 */

typedef boolean (*Objects_Name_comparators)(
  void       * /* name_1 */,
  void       * /* name_2 */,
  unsigned32   /* length */
);

/*
 *  The following type defines the control block used to manage
 *  object IDs.  The format is as follows (0=LSB):
 *
 *     Bits  0 .. 15    = index
 *     Bits 16 .. 25    = node
 *     Bits 26 .. 31    = class
 */

typedef unsigned32 Objects_Id;

#define OBJECTS_INDEX_START_BIT  0
#define OBJECTS_NODE_START_BIT  16
#define OBJECTS_CLASS_START_BIT 26

#define OBJECTS_INDEX_MASK      0x0000ffff
#define OBJECTS_NODE_MASK       0x03ff0000
#define OBJECTS_CLASS_MASK      0xfc000000

#define OBJECTS_INDEX_VALID_BITS  0x0000ffff
#define OBJECTS_NODE_VALID_BITS   0x000003ff
#define OBJECTS_CLASS_VALID_BITS  0x000000cf

/*
 *  This enumerated type is used in the class field of the object ID.
 */

typedef enum {
  OBJECTS_NO_CLASS                    =  0,
  OBJECTS_INTERNAL_THREADS            =  1,
  OBJECTS_RTEMS_TASKS                 =  2,
  OBJECTS_POSIX_THREADS               =  3,
  OBJECTS_ITRON_TASKS                 =  4,
  OBJECTS_RTEMS_TIMERS                =  5,
  OBJECTS_RTEMS_SEMAPHORES            =  6,
  OBJECTS_RTEMS_MESSAGE_QUEUES        =  7,
  OBJECTS_RTEMS_PARTITIONS            =  8,
  OBJECTS_RTEMS_REGIONS               =  9,
  OBJECTS_RTEMS_PORTS                 = 10,
  OBJECTS_RTEMS_PERIODS               = 11,
  OBJECTS_RTEMS_EXTENSIONS            = 12,
  OBJECTS_POSIX_KEYS                  = 13,
  OBJECTS_POSIX_INTERRUPTS            = 14,
  OBJECTS_POSIX_MESSAGE_QUEUES        = 15,
  OBJECTS_POSIX_MUTEXES               = 16,
  OBJECTS_POSIX_SEMAPHORES            = 17,
  OBJECTS_POSIX_CONDITION_VARIABLES   = 18,
  OBJECTS_ITRON_EVENTFLAGS            = 19,
  OBJECTS_ITRON_MAILBOXES             = 20,
  OBJECTS_ITRON_MESSAGE_BUFFERS       = 21,
  OBJECTS_ITRON_PORTS                 = 22,
  OBJECTS_ITRON_SEMAPHORES            = 23,
  OBJECTS_ITRON_VARIABLE_MEMORY_POOLS = 24,
  OBJECTS_ITRON_FIXED_MEMORY_POOLS    = 25
} Objects_Classes;
 
#define OBJECTS_CLASSES_FIRST               OBJECTS_NO_CLASS
#define OBJECTS_CLASSES_LAST                OBJECTS_ITRON_FIXED_MEMORY_POOLS
#define OBJECTS_CLASSES_FIRST_THREAD_CLASS  OBJECTS_INTERNAL_THREADS
#define OBJECTS_CLASSES_LAST_THREAD_CLASS   OBJECTS_ITRON_TASKS

/*
 *  This enumerated type lists the locations which may be returned
 *  by _Objects_Get.  These codes indicate the success of locating
 *  an object with the specified ID.
 */

typedef enum {
  OBJECTS_LOCAL  = 0,         /* object is local */
  OBJECTS_REMOTE = 1,         /* object is remote */
  OBJECTS_ERROR  = 2          /* id was invalid */
}  Objects_Locations;

/*
 *  The following defines the Object Control Block used to manage
 *  each object local to this node.
 */

typedef struct {
  Chain_Node     Node;
  Objects_Id     id;
  Objects_Name   name;
}   Objects_Control;

/*
 *  The following defines the structure for the information used to
 *  manage each class of objects.
 */

typedef struct {
  Objects_Classes   the_class;          /* Class of this object */
  Objects_Id        minimum_id;         /* minimum valid id of this type */
  Objects_Id        maximum_id;         /* maximum valid id of this type */
  unsigned32        maximum;            /* maximum number of objects */
  boolean           auto_extend;        /* TRUE if unlimited objects */
  unsigned32        allocation_size;    /* number of objects in a block */
  unsigned32        size;               /* size of the objects */
  Objects_Control **local_table;
  Objects_Name     *name_table;
  Chain_Control    *global_table;       /* pointer to global table */
  Chain_Control     Inactive;           /* chain of inactive ctl blocks */
  unsigned32        inactive;           /* number of objects on the InActive list */
  unsigned32       *inactive_per_block; /* used to release a block */
  void            **object_blocks;      /* the object memory to remove */
  boolean           is_string;          /* TRUE if names are strings */
  unsigned32        name_length;        /* maximum length of names */
  boolean           is_thread;          /* TRUE if these are threads */
                                        /*   irregardless of API */
}   Objects_Information;

/*
 *  The following defines the data storage which contains the
 *  node number of the local node.
 */

SCORE_EXTERN unsigned32  _Objects_Local_node;
SCORE_EXTERN unsigned32  _Objects_Maximum_nodes;

/*
 *  The following is the list of information blocks for each object
 *  class.  From the ID, we can go to one of these information blocks,
 *  and obtain a pointer to the appropriate object control block.
 */

SCORE_EXTERN Objects_Information 
    *_Objects_Information_table[OBJECTS_CLASSES_LAST + 1];

/*
 *  The following defines the constant which may be used
 *  with _Objects_Get to manipulate the calling task.
 *
 */

#define OBJECTS_ID_OF_SELF ((Objects_Id) 0)

/*
 *  The following define the constants which may be used in name searches.
 */

#define OBJECTS_SEARCH_ALL_NODES   0
#define OBJECTS_SEARCH_OTHER_NODES 0x7FFFFFFE
#define OBJECTS_SEARCH_LOCAL_NODE  0x7FFFFFFF
#define OBJECTS_WHO_AM_I           0

/*
 * Parameters and return id's for _Objects_Get_next
 */

#define OBJECTS_ID_INITIAL_INDEX   (0)
#define OBJECTS_ID_FINAL_INDEX     (0xffff)

#define OBJECTS_ID_INITIAL(_class, _node) \
  _Objects_Build_id( (_class), (_node), OBJECTS_ID_INITIAL_INDEX )

#define OBJECTS_ID_FINAL           ((Objects_Id)~0)

/*
 *  _Objects_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This function performs the initialization necessary for this handler.
 *
 */

void _Objects_Handler_initialization(
  unsigned32 node,
  unsigned32 maximum_nodes,
  unsigned32 maximum_global_objects
);

/*
 *  _Objects_Extend_information
 *
 *  DESCRIPTION:
 *
 *  This function extends an object class information record.
 */

void _Objects_Extend_information(
  Objects_Information *information
);

/*
 *  _Objects_Shrink_information
 *
 *  DESCRIPTION:
 *
 *  This function shrink an object class information record.
 */

void _Objects_Shrink_information(
  Objects_Information *information
);

/*
 *  _Objects_Initialize_information
 *
 *  DESCRIPTION:
 *
 *  This function initializes an object class information record.
 *  SUPPORTS_GLOBAL is TRUE if the object class supports global
 *  objects, and FALSE otherwise.  Maximum indicates the number
 *  of objects required in this class and size indicates the size
 *  in bytes of each control block for this object class.  The
 *  name length and string designator are also set.  In addition,
 *  the class may be a task, therefore this information is also included.
 */

void _Objects_Initialize_information (
  Objects_Information *information,
  Objects_Classes      the_class,
  boolean              supports_global,
  unsigned32           maximum,
  unsigned32           size,
  boolean              is_string,
  unsigned32           maximum_name_length,
  boolean              is_task
);

/*PAGE
 *
 *  _Objects_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a object control block from
 *  the inactive chain of free object control blocks.
 */

Objects_Control *_Objects_Allocate(
  Objects_Information *information
);

/*
 *  _Objects_Allocate_by_index
 *
 *  DESCRIPTION:
 *
 *  This function allocates the object control block
 *  specified by the index from the inactive chain of
 *  free object control blocks.
 */

Objects_Control *_Objects_Allocate_by_index(
  Objects_Information *information,
  unsigned32           index,
  unsigned32           sizeof_control
);

/*PAGE
 *
 *  _Objects_Free
 *
 *  DESCRIPTION:
 *
 *  This function frees a object control block to the
 *  inactive chain of free object control blocks.
 */

void _Objects_Free(
  Objects_Information *information,
  Objects_Control     *the_object
);

/*
 *  _Objects_Clear_name
 *
 *  DESCRIPTION:
 *
 *  This method zeroes out the name.
 */
 
void _Objects_Clear_name(
  void       *name,
  unsigned32  length
);

/*
 *  _Objects_Copy_name_string
 *
 *  DESCRIPTION:
 *
 *  This method copies a string style object name from source to destination.
 */

void _Objects_Copy_name_string(
  void       *source,
  void       *destination
);

/*
 *  _Objects_Copy_name_raw
 *
 *  DESCRIPTION:
 *
 *  This method copies a raw style object name from source to destination.
 */

void _Objects_Copy_name_raw(
  void       *source,
  void       *destination,
  unsigned32  length
);

/*
 *  _Objects_Compare_name_string
 *
 *  DESCRIPTION:
 *
 *  This method compares two string style object names.
 */

boolean _Objects_Compare_name_string(
  void       *name_1,
  void       *name_2,
  unsigned32  length
);

/*
 *  _Objects_Compare_name_raw
 *
 *  DESCRIPTION:
 *
 *  This method compares two raw style object names.
 */

boolean _Objects_Compare_name_raw(
  void       *name_1,
  void       *name_2,
  unsigned32  length
);
/*
 *  _Objects_Name_to_id
 *
 *  DESCRIPTION:
 *
 *  This function implements the common portion of the object
 *  identification directives.  This directive returns the object
 *  id associated with name.  If more than one object of this class
 *  is named name, then the object to which the id belongs is
 *  arbitrary.  Node indicates the extent of the search for the
 *  id of the object named name.  If the object class supports global
 *  objects, then the search can be limited to a particular node
 *  or allowed to encompass all nodes.
 *
 */

typedef enum {
  OBJECTS_SUCCESSFUL,
  OBJECTS_INVALID_NAME,
  OBJECTS_INVALID_NODE
} Objects_Name_to_id_errors;

#define OBJECTS_NAME_ERRORS_FIRST OBJECTS_SUCCESSFUL
#define OBJECTS_NAME_ERRORS_LAST  OBJECTS_INVALID_NODE

Objects_Name_to_id_errors _Objects_Name_to_id(
  Objects_Information *information,
  Objects_Name         name,
  unsigned32           node,
  Objects_Id          *id
);

/*
 *  _Objects_Get
 *
 *  DESCRIPTION:
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
 *  NOTE: _Objects_Get returns with dispatching disabled for 
 *        local and remote objects.
 *        _Objects_Get_isr_disable returns with dispatching
 *        disabled for remote objects and interrupts for local
 *        objects.
 */

Objects_Control *_Objects_Get (
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
);

Objects_Control *_Objects_Get_isr_disable(
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location,
  ISR_Level           *level
);

Objects_Control *_Objects_Get_by_index (
  Objects_Information *information,
  Objects_Id           id,
  Objects_Locations   *location
);
/*
 *  _Objects_Get_next
 *
 *  DESCRIPTION:
 *
 *  Like _Objects_Get, but is used to find "next" open object.
 *
 */

Objects_Control *_Objects_Get_next(
    Objects_Information *information,
    Objects_Id           id,
    Objects_Locations   *location_p,
    Objects_Id          *next_id_p
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
