/*  object.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Object Handler.  This Handler provides mechanisms which
 *  can be used to initialize and manipulate all objects which have 
 *  ids.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
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
  OBJECTS_NO_CLASS                  =  0,
  OBJECTS_INTERNAL_THREADS          =  1,
  OBJECTS_RTEMS_TASKS               =  2,
  OBJECTS_POSIX_THREADS             =  3,
  OBJECTS_RTEMS_TIMERS              =  4,
  OBJECTS_RTEMS_SEMAPHORES          =  5,
  OBJECTS_RTEMS_MESSAGE_QUEUES      =  6,
  OBJECTS_RTEMS_PARTITIONS          =  7,
  OBJECTS_RTEMS_REGIONS             =  8,
  OBJECTS_RTEMS_PORTS               =  9,
  OBJECTS_RTEMS_PERIODS             = 10,
  OBJECTS_RTEMS_EXTENSIONS          = 11,
  OBJECTS_POSIX_KEYS                = 12,
  OBJECTS_POSIX_INTERRUPTS          = 13,
  OBJECTS_POSIX_MESSAGE_QUEUES      = 14,
  OBJECTS_POSIX_MUTEXES             = 15,
  OBJECTS_POSIX_SEMAPHORES          = 16,
  OBJECTS_POSIX_CONDITION_VARIABLES = 17
} Objects_Classes;
 
#define OBJECTS_CLASSES_FIRST               OBJECTS_NO_CLASS
#define OBJECTS_CLASSES_LAST                OBJECTS_POSIX_CONDITION_VARIABLES
#define OBJECTS_CLASSES_FIRST_THREAD_CLASS  OBJECTS_INTERNAL_THREADS
#define OBJECTS_CLASSES_LAST_THREAD_CLASS   OBJECTS_POSIX_THREADS

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
  Chain_Node    Node;
  Objects_Id    id;
  Objects_Name  name;
}   Objects_Control;

/*
 *  The following defines the structure for the information used to
 *  manage each class of objects.
 */

typedef struct {
  Objects_Classes   the_class;       /* Class of this object */
  Objects_Id        minimum_id;      /* minimum valid id of this type */
  Objects_Id        maximum_id;      /* maximum valid id of this type */
  unsigned32        maximum;         /* maximum number of objects */
  Objects_Control **local_table;     /* table of local object pointers */
  Objects_Name     *name_table;      /* table of local object names */
  Chain_Control    *global_table;    /* pointer to global table */
  Chain_Control     Inactive;        /* chain of inactive ctl blocks */
  boolean           is_string;       /* TRUE if names are strings */
  unsigned32        name_length;     /* maximum length of names */
  boolean           is_thread;       /* TRUE if these are threads */
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

/*
 *  _Objects_Clear_name
 *
 *  DESCRIPTION:
 *
 *  XXX
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
 *  XXX
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
 *  XXX
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
 *  XXX
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
 *  XXX
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
 */

Objects_Control *_Objects_Get (
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
 *  _Objects_Get_information
 *
 *  DESCRIPTION:
 *
 *  Returns the information control block for the class of objects
 *  corresponding to this id.
 */

Objects_Information *_Objects_Get_information(
  Objects_Id  id
);
  
/*
 *  Pieces of object.inl are promoted out to the user
 */

#include <rtems/score/object.inl>
#include <rtems/score/objectmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
