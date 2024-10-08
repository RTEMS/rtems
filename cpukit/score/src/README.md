Unlimited Local Node Objects
============================

This document explains how the unlimited objects support works.  This was
written by Chris Johns <ccj@acm.org> of Objective Design Systems as a
design document.  This was submitted as part of the patch which added
this capability.  


1. Why ?

This patch changes the way RTEMS allocates, frees, and manages the
'Objects_Control' structure.

The 'Objects_Control' structure is at the root of all objects in
RTEMS. The RTEMS and POSIX API allows users to create tasks, message
queues, semaphores and other resources. These are all a type of
Object. The POSIX API allow similar operations. These also map to
Objects.

Currently the number of objects that can be created is a static value
loaded into the Configuration table before starting the kernel. The
application cannot exceed these limits. Various means are used to tune
this value. During development the value is usually set large. This
saves having to change it everytime a developer adds a new
resource. With a large team of developers the configuration table file
can cycle through a large number of revisions. The wasted memory is
only recovered when memory runs short. The issue of the configuration
table parameters become more important the less memory you have.

The Configuration table requires a calculation to occur at compile
time to set the size of the Workspace. The calculation is an
estimate. You need to specify an overhead value for memory that can
not be calculated. An example of memory that cannot be calculated is
stack sizes. This issue is not directly related to allowing unlimited
objects how-ever the need to calculate the memory usage for a system
in this manner is prone to error.

I would like to see download support added to RTEMS. The kernel
configuration being set at boot time means a download application can
be limited. This can defeat one of the purposes of using downloaded
code, no need to change ROMs. In a system I worked on the cost to
change ROMS in a complete system was high and could take a week. This
change is the first phase of supporting downloaded applications.

1.1 How do Objects work ?

All applications interact with the super core (c/src/exec/score) via
an API. The central structure used in the super core is the
`object'. Two application interfaces exist. They are RTEMS and
POSIX. Both map to the super core using objects.

An object in RTEMS is a resource which the user (through the API)
creates. The different types of objects are referred to as classes of
objects. An object is referenced by an id. This is of type `rtems_id'
and is a 32bit unsigned integer. The id is unique for each object no
matter what class.

Objects are anchored by the `_Object_Information' structure. There is
one per type or class of object. A global table of pointers to each
information structure for a class of objects is held in
`Objects_Information_table'.

Objects consist of 6 main structures. The `_Object_Information' is the
root structure. It contains pointers to the `local_table',
`name_table', `global_table', the Inactive chain, and the object
memory. It also contains the various variables which describe the
object. We are only concerned with the `local_table', `name_table',
Inactive chain, and the object memory to support unlimited objects.

The `local_table' holds the pointers to open objects. A `local_table
entry which is null is free and the object will be sitting on the
Inactive chain. The index into the table is based on part of the
id. Given an id the you can find the index into the `local_table', and
therefore the object. The `local_table' has the entries for the
indexes below the minimum_id's index. The minimum_id is always set to
1 (the change allows another value to be selected if require). The
index of 0 is reserved and never used. This allows any actions using
an id of zero to fail or map to a special case.

The `name_table' holds the names of the objects. Each entry in this
table is the maximum size the name of the object can be. The size of
names is not constrained by the object code (but is by the MP object
code, and the API and should be fixed).

The `global_table' and code that uses it has not changed. I did not
look at the this code, and I am not farmilar with it.

The Inactive chain stores objects which are free or not
allocated. This design saves searching for a free object when
allocating therefore providing a deterministic allocation scheme. When
the chain is empty a null is returned.

The change documented below basically extends the `local_table' and
`name_table' structures at run-time. The memory used be these table
is not large compared to the memory for the objects, and so are never
reduced in size once extended. The object's memory grows and shrinks
depending of the user's usage.

Currently, the user specifies the total number of objects in the
Configuration table. The change alters the function of the values in
the Configuration table. A flag can be masked on to the value which
selects the extending mode. If the user does not set the flag the
object code operates with an object ceiling. A small performance
overhead will be incurred as the allocate and free routines are now
not inlined and a check of the auto_extend flag is made. The remaining
value field of the Configuration table entry is total number of
objects that can be allocated when not in unlimited mode.

If the user masks the flag on to a value on the Configuration table
auto-exdending mode is selected for that class of object. The value
becomes the allocation unit size. If there are no free objects the
object's tables are extended by the allocation unit number of
objects. The object table is shrunk when the user frees objects. The
table must have one free allocation block, and at least half the
allocation size of another block before the object memory of the free
allocation block is returned to the heap. This stops threshold
thrashing when objects around the allocation unit size and created and
destroyed.

At least one allocation block size of objects is created and never
destroyed.

The change to support unlimited objects has extended the object
information structure.

The flag, `auto_extend' controls if the object can be automatically
extended. The user masks the flag RTEMS_UNLIMITED_FLAGS onto the
Configuration table number to select the auto-extend mode. This is
passed to the `_Objects_Initialize_information' function in the
parameter maximum. The flag is tested for and the auto_extend flag
updated to reflect the state of the flag before being stipped from the
maximum.

The `allocation_size' is set to the parameter maxium in the function
`_Objects_Initialize_information' if `auto_extend' is true. Making the
allocation size small causes the memory to be allocated and freed more
often. This only effects the performance times for creating a resource
such as a task. It does how-ever give you fine grain memory
control. If the performance of creating resources is not a problem
make the size small.

The size of the object is required to be stored. It is used when
extending the object information.

A count of the object on the Inactive list is maintained. This is used
during freeing objects. If the count is above 1.5 times the
`allocation_size' an attempt is made to shrink the object
informtation. Shrinking might not always succeed as a single
allocation block might not be free. Random freeing of objects can
result in some fragmentation. Any further allocations will use the
free objects before extending the object's information tables.

A table of inactive objects per block is maintained. This table, like
the `local_table' and `name_table' grows as more blocks are
allocated. A check is made of a blocks inactive count when an object
which is part of that block is freed. If the total inactive count
exceeds 1.5 times the allocation size, and the block's inactive count
is the allocation_size, the objects data block is returnd to the
workspace heap.

The `objects_blocks' is a table of pointers. The object_block's pointers
point to the object's data block. The object's data block is a single
allocation of the name space and object space. This was two separate
allocations but is now one. The objects_block's table is use to
determine if a block is allocated, and the address of the memory block
to be returned to the workspace heap when the object informtation
space is shrunk.

2.0 Detail Of the Auto-Extend Patch to rtems-4.0.0, Snapshot 19990302

o Configuration table support.

  Added a flag OBJECTS_UNLIMITED_OBJECTS to score/headers/object.h
  header file. This is referenced in the file sapi/headers/config.h to
  create the flag RTEMS_UNLIMITED_OBJECTS. A macro is provided to take
  a resource count and apply the flag. The macro is called
  `rtems_resource_unlimited'. The user uses this macro when building a
  configuration table. It can be used with the condefs.h header file.

o Object Information Structure

  The object information structure, Objects_Information, has been
  extended with the follow fields :

    boolean auto_extend -
    
      When true the object's information tables can be extended untill
      all memory is used. When false the current functionallity is
      maintained.

    uint32_t   allocation_size -
    
      When auto_extend is true, it is the value in the Configuration
      table and is the number of objects the object's information
      tables are extended or shrunk.

   uint32_t   size -
   
      The size of the object. It is used to calculate the size of
      memory required to be allocated when extending the table.

   uint32_t   inactive -
   
      The number of elements on the Inactive chain.

   uint32_t   *inactive_per_block -
   
      Pointer to a table of counts of the inactive objects from a
      block on the Inactive chain. It is used to know which blocks are
      all free and therefore can be returned to the heap.

   void **object_blocks -
   
      Pointer to a table of pointers to the object data. The table
      holds the pointer used to return a block to the heap when
      shrinking the object's information tables.

o Changes to Existing Object Functions

  Two functions prototypes are added. They are :

   _Objects_Extend_information,
   _Objects_Shrink_information
   _Object_Allocate, and 
   _Object_Free

  The last were inlined, how-ever now they are not as they are too
  complex to implement as macros now.

o Object Initialisation

  The function _Objects_Initialize_information has been changed to
  initialisation of the information structure's fields then call the
  new function _Objects_Extend_information.
  
  The first block of objects is always allocated and never
  released. This means with the auto-extend flag set to true the user
  still sees the same behaviour expected without this change. That is
  the number objects specified in the Configuration table is the
  number of object allocated during RTEMS initialisation. If not
  enough memory is found during this initial extend a fatal error
  occurs. The fatal error only occurs for this case of extending the
  object's information tables.

o Object Information Extend

  The _Object_Information_Extend is a new function. It takes some of
  the code form the old _Object_Initialize_information function. The
  function extends an object's information base.

  Extending the first time is a special case. The function assumes the
  maximum index will be less than the minimum index. This means the
  minimum index must be greater than 0 at initialisation. The other
  special case made is coping the tables from the old location to the
  new location. The first block case is trapped and tables are
  initialised instead. Workspace allocation for the first block is
  tested for an if the first block the allocate or fatal error call is
  made. This traps an RTEMS initialise allocation error.

  The remainder of the code deals with all cases of extending the
  object's information.

  The current block count is first determined, then a scan of the
  object_block table is made to locate a free slot. Blocks can be
  freed in any order. The index base for the block is also determined.

  If the index base is greater than the maximum index, the tables must
  grow. To grow the tables, a new larger memory block is allocated and
  the tables copied. The object's information structure is then
  updated to point to the new tables. The tables are allocated in one
  memory block from the work-space heap. The single block is then
  broken down in the required tables.

  Once the tables are copied, and the new extended parts initialised
  the table pointers in the object's information structure are
  updated. This is protected by masking interrupts.

  The old table's memory block is returned to the heap.

  The names table and object is allocated. This again is a single
  block which is divided.

  The objects are initialised onto a local Inactive chain. They are
  then copied to the object's Inactive chain to complete the
  initialisation.

o Object Informtation Shrink

  The _Object_Shrink_information function is new. It is required to
  scan all the blocks to see which one has no objects allocated. The
  last object freed might not belong to a block which is completely
  free.

  Once a block is located, the Inactive chain is interated down
  looking for objects which belong to the block of object being
  released.

  Once the Inactive chain scan is complete the names table and object
  memory is returned to the work-space heap and the table references cleared.

  XXX - I am not sure if this should occur if better protection or
  different code to provide better protection.

  The information tables do not change size. Once extended they never
  shrink.

o Object Allocation

  The _Objects_Allocate attempts to get an object from the Inactive
  chain. If auto-extend mode is not enabled no further processing
  occurs. The extra overhead for this implemetation is the function is
  not inlined and check of a boolean occurs. It should effect the
  timing figures.

  If auto-extend is enabled, a further check is made to see if the get
  from the Inactive chain suceeded in getting an object. If it failed
  a call is made to extend the object's information tables.

  The get from the Inactive chain is retried. The result of this is
  returned to the user. A failure here is the users problem.

o Object Free

  The _Objects_Free puts the object back onto the Inactive
  chain. Again if auto-extend mode is not enabled no further
  processing occurs and performance overhead will low.

  If auto-extend mode is enabled, a check is to see if the number of
  Inactive objects is one and a half times the allocation size. If
  there are that many free objects an attempt is made to shrink the
  object's information.

o Object Index and the Get Function

  The existing code allocates the number of object specified in the
  configuration table, how-ever it makes the local_table have one more
  element. This is the slot for an id of 0. The 0 slot is always a
  NULL providing a simple check for a 0 id for object classes.

  The existing _Objects_Get code removes the minimum id, which I think
  could only be 1 from the index, then adds one for the 0 slot.

  This change removes this index adjustment code in _Objects_Get.

  The extend information starts the index count when scanning for free
  blocks at the minumun index. This means the base index for a block
  will always be adjusted by the minimum index. The extend information
  function only ever allocates the allocation size of
  objects. Finially the object's local_table size is the maximum plus
  the minumum index size. The maximum is really the maximum index.

  This means the values in the object's information structure and
  tables do not need the index adjustments which existed before.

o The Test

  A new sample test, unlimited is provided. It attempts to test this
  change.
