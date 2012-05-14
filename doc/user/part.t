@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Partition Manager

@cindex partitions

@section Introduction

The partition manager provides facilities to
dynamically allocate memory in fixed-size units.  The directives
provided by the partition manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}partition_create} - Create a partition
@item @code{@value{DIRPREFIX}partition_ident} - Get ID of a partition
@item @code{@value{DIRPREFIX}partition_delete} - Delete a partition
@item @code{@value{DIRPREFIX}partition_get_buffer} - Get buffer from a partition
@item @code{@value{DIRPREFIX}partition_return_buffer} - Return buffer to a partition
@end itemize

@section Background

@subsection Partition Manager Definitions

@cindex partition, definition

A partition is a physically contiguous memory area
divided into fixed-size buffers that can be dynamically
allocated and deallocated.

@cindex buffers, definition

Partitions are managed and maintained as a list of
buffers.  Buffers are obtained from the front of the partition's
free buffer chain and returned to the rear of the same chain.
When a buffer is on the free buffer chain, RTEMS uses two
pointers of memory from each buffer as the free buffer chain.  
When a buffer is allocated, the entire buffer is available for application use.
Therefore, modifying memory that is outside of an allocated
buffer could destroy the free buffer chain or the contents of an
adjacent allocated buffer.

@subsection Building a Partition Attribute Set

@cindex partition attribute set, building

In general, an attribute set is built by a bitwise OR
of the desired attribute components.  The set of valid partition
attributes is provided in the following table:

@itemize @bullet
@item @code{@value{RPREFIX}LOCAL} - local partition (default)
@item @code{@value{RPREFIX}GLOBAL} - global partition
@end itemize

Attribute values are specifically designed to be
mutually exclusive, therefore bitwise OR and addition operations
are equivalent as long as each attribute appears exactly once in
the component list.  An attribute listed as a default is not
required to appear in the attribute list, although it is a good
programming practice to specify default attributes.  If all
defaults are desired, the attribute
@code{@value{RPREFIX}DEFAULT_ATTRIBUTES} should be
specified on this call.  The attribute_set parameter should be
@code{@value{RPREFIX}GLOBAL} to indicate that the partition
is to be known globally.

@section Operations

@subsection Creating a Partition

The @code{@value{DIRPREFIX}partition_create} directive creates a partition
with a user-specified name.  The partition's name, starting
address, length and buffer size are all specified to the
@code{@value{DIRPREFIX}partition_create} directive.  
RTEMS allocates a Partition Control
Block (PTCB) from the PTCB free list.  This data structure is
used by RTEMS to manage the newly created partition.  The number
of buffers in the partition is calculated based upon the
specified partition length and buffer size. If successful,the
unique partition ID is returned to the calling task.

@subsection Obtaining Partition IDs

When a partition is created, RTEMS generates a unique
partition ID and assigned it to the created partition until it
is deleted.  The partition ID may be obtained by either of two
methods.  First, as the result of an invocation of the
@code{@value{DIRPREFIX}partition_create} directive, the partition
ID is stored in a user provided location.  Second, the partition
ID may be obtained later using the @code{@value{DIRPREFIX}partition_ident}
directive.  The partition ID is used by other partition manager directives
to access this partition.

@subsection Acquiring a Buffer

A buffer can be obtained by calling the
@code{@value{DIRPREFIX}partition_get_buffer} directive.  
If a buffer is available, then
it is returned immediately with a successful return code.
Otherwise, an unsuccessful return code is returned immediately
to the caller.  Tasks cannot block to wait for a buffer to
become available.

@subsection Releasing a Buffer

Buffers are returned to a partition's free buffer
chain with the @code{@value{DIRPREFIX}partition_return_buffer} directive.  This
directive returns an error status code if the returned buffer
was not previously allocated from this partition.

@subsection Deleting a Partition

The @code{@value{DIRPREFIX}partition_delete} directive allows a partition to
be removed and returned to RTEMS.  When a partition is deleted,
the PTCB for that partition is returned to the PTCB free list.
A partition with buffers still allocated cannot be deleted.  Any
task attempting to do so will be returned an error status code.

@section Directives

This section details the partition manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@c
@c
@c
@page
@subsection PARTITION_CREATE - Create a partition

@cindex create a partition

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_partition_create
@example
rtems_status_code rtems_partition_create(
  rtems_name       name,
  void            *starting_address,
  uint32_t         length,
  uint32_t         buffer_size,
  rtems_attribute  attribute_set,
  rtems_id        *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Partition_Create (
   Name             : in     RTEMS.Name;
   Starting_Address : in     RTEMS.Address;
   Length           : in     RTEMS.Unsigned32;
   Buffer_Size      : in     RTEMS.Unsigned32;
   Attribute_Set    : in     RTEMS.Attribute;
   ID               :    out RTEMS.ID;
   Result           :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - partition created successfully@*
@code{@value{RPREFIX}INVALID_NAME} - invalid partition name@*
@code{@value{RPREFIX}TOO_MANY} - too many partitions created@*
@code{@value{RPREFIX}INVALID_ADDRESS} - address not on four byte boundary@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{starting_address} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_SIZE} - length or buffer size is 0@*
@code{@value{RPREFIX}INVALID_SIZE} - length is less than the buffer size@*
@code{@value{RPREFIX}INVALID_SIZE} - buffer size not a multiple of 4@*
@code{@value{RPREFIX}MP_NOT_CONFIGURED} - multiprocessing not configured@*
@code{@value{RPREFIX}TOO_MANY} - too many global objects

@subheading DESCRIPTION:

This directive creates a partition of fixed size
buffers from a physically contiguous memory space which starts
at starting_address and is length bytes in size.  Each allocated
buffer is to be of @code{buffer_size} in bytes.  The assigned
partition id is returned in @code{id}.  This partition id is used to
access the partition with other partition related directives.
For control and maintenance of the partition, RTEMS allocates a
PTCB from the local PTCB free pool and initializes it.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

The @code{starting_address} must be properly aligned for the
target architecture.

The @code{buffer_size} parameter must be a multiple of
the CPU alignment factor.  Additionally, @code{buffer_size}
must be large enough to hold two pointers on the target
architecture.  This is required for RTEMS to manage the
buffers when they are free.

Memory from the partition is not used by RTEMS to
store the Partition Control Block.

The following partition attribute constants are
defined by RTEMS:

@itemize @bullet
@item @code{@value{RPREFIX}LOCAL} - local partition (default)
@item @code{@value{RPREFIX}GLOBAL} - global partition
@end itemize

The PTCB for a global partition is allocated on the
local node.  The memory space used for the partition must reside
in shared memory. Partitions should not be made global unless
remote tasks must interact with the partition.  This is to avoid
the overhead incurred by the creation of a global partition.
When a global partition is created, the partition's name and id
must be transmitted to every node in the system for insertion in
the local copy of the global object table.

The total number of global objects, including
partitions, is limited by the maximum_global_objects field in
the Configuration Table.

@c
@c
@c
@page
@subsection PARTITION_IDENT - Get ID of a partition

@cindex get ID of a partition
@cindex obtain ID of a partition

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_partition_ident
@example
rtems_status_code rtems_partition_ident(
  rtems_name  name,
  uint32_t    node,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Partition_Ident (
   Name   : in     RTEMS.Name;
   Node   : in     RTEMS.Unsigned32;
   ID     :    out RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - partition identified successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_NAME} - partition name not found@*
@code{@value{RPREFIX}INVALID_NODE} - invalid node id

@subheading DESCRIPTION:

This directive obtains the partition id associated
with the partition name.  If the partition name is not unique,
then the partition id will match one of the partitions with that
name.  However, this partition id is not guaranteed to
correspond to the desired partition.  The partition id is used
with other partition related directives to access the partition.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

If node is @code{@value{RPREFIX}SEARCH_ALL_NODES}, all nodes are searched
with the local node being searched first.  All other nodes are
searched with the lowest numbered node searched first.

If node is a valid node number which does not
represent the local node, then only the partitions exported by
the designated node are searched.

This directive does not generate activity on remote
nodes.  It accesses only the local copy of the global object
table.

@c
@c
@c
@page
@subsection PARTITION_DELETE - Delete a partition

@cindex delete a partition

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_partition_delete
@example
rtems_status_code rtems_partition_delete(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Partition_Delete (
   ID     : in     RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - partition deleted successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid partition id@*
@code{@value{RPREFIX}RESOURCE_IN_USE} - buffers still in use@*
@code{@value{RPREFIX}ILLEGAL_ON_REMOTE_OBJECT} - cannot delete remote partition

@subheading DESCRIPTION:

This directive deletes the partition specified by id.
The partition cannot be deleted if any of its buffers are still
allocated.  The PTCB for the deleted partition is reclaimed by
RTEMS.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

The calling task does not have to be the task that
created the partition.  Any local task that knows the partition
id can delete the partition.

When a global partition is deleted, the partition id
must be transmitted to every node in the system for deletion
from the local copy of the global object table.

The partition must reside on the local node, even if
the partition was created with the @code{@value{RPREFIX}GLOBAL} option.

@c
@c
@c
@page
@subsection PARTITION_GET_BUFFER - Get buffer from a partition

@cindex get buffer from partition
@cindex obtain buffer from partition

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_partition_get_buffer
@example
rtems_status_code rtems_partition_get_buffer(
  rtems_id   id,
  void     **buffer
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Partition_Get_Buffer (
   ID     : in     RTEMS.ID;
   Buffer :    out RTEMS.Address;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - buffer obtained successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{buffer} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid partition id@*
@code{@value{RPREFIX}UNSATISFIED} - all buffers are allocated

@subheading DESCRIPTION:

This directive allows a buffer to be obtained from
the partition specified in id.  The address of the allocated
buffer is returned in buffer.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

All buffers begin on a four byte boundary.

A task cannot wait on a buffer to become available.

Getting a buffer from a global partition which does
not reside on the local node will generate a request telling the
remote node to allocate a buffer from the specified partition.

@c
@c
@c
@page
@subsection PARTITION_RETURN_BUFFER - Return buffer to a partition

@cindex return buffer to partitition

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_partition_return_buffer
@example
rtems_status_code rtems_partition_return_buffer(
  rtems_id  id,
  void     *buffer
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Partition_Return_Buffer (
   ID     : in     RTEMS.ID;
   Buffer : in     RTEMS.Address;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - buffer returned successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{buffer} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid partition id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - buffer address not in partition

@subheading DESCRIPTION:

This directive returns the buffer specified by buffer
to the partition specified by id.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

Returning a buffer to a global partition which does
not reside on the local node will generate a request telling the
remote node to return the buffer to the specified partition.

Returning a buffer multiple times is an error.  It will corrupt the internal
state of the partition.
