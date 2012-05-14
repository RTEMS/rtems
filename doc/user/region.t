@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Region Manager

@cindex regions

@section Introduction

The region manager provides facilities to dynamically
allocate memory in variable sized units.  The directives
provided by the region manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}region_create} - Create a region
@item @code{@value{DIRPREFIX}region_ident} - Get ID of a region
@item @code{@value{DIRPREFIX}region_delete} - Delete a region
@item @code{@value{DIRPREFIX}region_extend} - Add memory to a region
@item @code{@value{DIRPREFIX}region_get_segment} - Get segment from a region
@item @code{@value{DIRPREFIX}region_return_segment} - Return segment to a region
@item @code{@value{DIRPREFIX}region_get_segment_size} - Obtain size of a segment
@item @code{@value{DIRPREFIX}region_resize_segment} - Change size of a segment
@end itemize

@section Background

@subsection Region Manager Definitions

@cindex region, definition
@cindex segment, definition

A region makes up a physically contiguous memory
space with user-defined boundaries from which variable-sized
segments are dynamically allocated and deallocated.  A segment
is a variable size section of memory which is allocated in
multiples of a user-defined page size.  This page size is
required to be a multiple of four greater than or equal to four.
For example, if a request for a 350-byte segment is made in a
region with 256-byte pages, then a 512-byte segment is allocated.

Regions are organized as doubly linked chains of
variable sized memory blocks.  Memory requests are allocated
using a first-fit algorithm.  If available, the requester
receives the number of bytes requested (rounded up to the next
page size).  RTEMS requires some overhead from the region's
memory for each segment that is allocated.  Therefore, an
application should only modify the memory of a segment that has
been obtained from the region.  The application should NOT
modify the memory outside of any obtained segments and within
the region's boundaries while the region is currently active in
the system.

Upon return to the region, the free block is
coalesced with its neighbors (if free) on both sides to produce
the largest possible unused block.

@subsection Building an Attribute Set

@cindex region attribute set, building

In general, an attribute set is built by a bitwise OR
of the desired attribute components.  The set of valid region
attributes is provided in the following table:

@itemize @bullet
@item @code{@value{RPREFIX}FIFO} - tasks wait by FIFO (default)
@item @code{@value{RPREFIX}PRIORITY} - tasks wait by priority
@end itemize

Attribute values are specifically designed to be
mutually exclusive, therefore bitwise OR and addition operations
are equivalent as long as each attribute appears exactly once in
the component list.  An attribute listed as a default is not
required to appear in the attribute list, although it is a good
programming practice to specify default attributes.  If all
defaults are desired, the attribute
@code{@value{RPREFIX}DEFAULT_ATTRIBUTES} should be
specified on this call.

This example demonstrates the attribute_set parameter
needed to create a region with the task priority waiting queue
discipline.  The attribute_set parameter to the
@code{@value{DIRPREFIX}region_create}
directive should be @code{@value{RPREFIX}PRIORITY}.

@subsection Building an Option Set

In general, an option is built by a bitwise OR of the
desired option components.  The set of valid options for the
@code{@value{DIRPREFIX}region_get_segment} directive are
listed in the following table:

@itemize @bullet
@item @code{@value{RPREFIX}WAIT} - task will wait for segment (default)
@item @code{@value{RPREFIX}NO_WAIT} - task should not wait
@end itemize

Option values are specifically designed to be
mutually exclusive, therefore bitwise OR and addition operations
are equivalent as long as each option appears exactly once in
the component list.  An option listed as a default is not
required to appear in the option list, although it is a good
programming practice to specify default options.  If all
defaults are desired, the option
@code{@value{RPREFIX}DEFAULT_OPTIONS} should be
specified on this call.

This example demonstrates the option parameter needed
to poll for a segment.  The option parameter passed to the
@code{@value{DIRPREFIX}region_get_segment} directive should
be @code{@value{RPREFIX}NO_WAIT}.

@section Operations

@subsection Creating a Region

The @code{@value{DIRPREFIX}region_create} directive creates a region with the
user-defined name.  The user may select FIFO or task priority as
the method for placing waiting tasks in the task wait queue.
RTEMS allocates a Region Control Block (RNCB) from the RNCB free
list to maintain the newly created region.  RTEMS also generates
a unique region ID which is returned to the calling task.

It is not possible to calculate the exact number of
bytes available to the user since RTEMS requires overhead for
each segment allocated.  For example, a region with one segment
that is the size of the entire region has more available bytes
than a region with two segments that collectively are the size
of the entire region.  This is because the region with one
segment requires only the overhead for one segment, while the
other region requires the overhead for two segments.

Due to automatic coalescing, the number of segments
in the region dynamically changes.  Therefore, the total
overhead required by RTEMS dynamically changes.

@subsection Obtaining Region IDs

When a region is created, RTEMS generates a unique
region ID and assigns it to the created region until it is
deleted.  The region ID may be obtained by either of two
methods.  First, as the result of an invocation of the
@code{@value{DIRPREFIX}region_create} directive,
the region ID is stored in a user
provided location.  Second, the region ID may be obtained later
using the @code{@value{DIRPREFIX}region_ident} directive.  
The region ID is used by other region manager directives to
access this region.

@subsection Adding Memory to a Region

The @code{@value{DIRPREFIX}region_extend} directive may be used to add memory
to an existing region.  The caller specifies the size in bytes
and starting address of the memory being added.

NOTE:  Please see the release notes or RTEMS source
code for information regarding restrictions on the location of
the memory being added in relation to memory already in the
region.

@subsection Acquiring a Segment

The @code{@value{DIRPREFIX}region_get_segment} directive attempts to acquire
a segment from a specified region.  If the region has enough
available free memory, then a segment is returned successfully
to the caller.  When the segment cannot be allocated, one of the
following situations applies:

@itemize @bullet
@item By default, the calling task will wait forever to acquire the segment.

@item Specifying the @code{@value{RPREFIX}NO_WAIT} option forces
an immediate return with an error status code.

@item Specifying a timeout limits the interval the task will
wait before returning with an error status code.
@end itemize

If the task waits for the segment, then it is placed
in the region's task wait queue in either FIFO or task priority
order.  All tasks waiting on a region are returned an error when
the message queue is deleted.

@subsection Releasing a Segment

When a segment is returned to a region by the
@code{@value{DIRPREFIX}region_return_segment} directive, it is merged with its
unallocated neighbors to form the largest possible segment.  The
first task on the wait queue is examined to determine if its
segment request can now be satisfied.  If so, it is given a
segment and unblocked.  This process is repeated until the first
task's segment request cannot be satisfied.

@subsection Obtaining the Size of a Segment

The @code{@value{DIRPREFIX}region_get_segment_size} directive returns the
size in bytes of the specified segment.  The size returned
includes any "extra" memory included in the segment because of
rounding up to a page size boundary.

@subsection Changing the Size of a Segment

The @code{@value{DIRPREFIX}region_resize_segment} directive is used
to change the size in bytes of the specified segment.  The size may be
increased or decreased.  When increasing the size of a segment, it is
possible that the request cannot be satisfied.  This directive provides
functionality similar to the @code{realloc()} function in the Standard
C Library.

@subsection Deleting a Region

A region can be removed from the system and returned
to RTEMS with the @code{@value{DIRPREFIX}region_delete}
directive.  When a region is
deleted, its control block is returned to the RNCB free list.  A
region with segments still allocated is not allowed to be
deleted.  Any task attempting to do so will be returned an
error.  As a result of this directive, all tasks blocked waiting
to obtain a segment from the region will be readied and returned
a status code which indicates that the region was deleted.

@section Directives

This section details the region manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection REGION_CREATE - Create a region

@cindex create a region

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_create
@example
rtems_status_code rtems_region_create(
  rtems_name       name,
  void            *starting_address,
  intptr_t         length,
  uint32_t         page_size,
  rtems_attribute  attribute_set,
  rtems_id        *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Create (
   Name             : in     RTEMS.Name;
   Starting_Address : in     RTEMS.Address;
   Length           : in     RTEMS.Unsigned32;
   Page_Size        : in     RTEMS.Unsigned32;
   Attribute_Set    : in     RTEMS.Attribute;
   ID               :    out RTEMS.ID;
   Result           :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - region created successfully@*
@code{@value{RPREFIX}INVALID_NAME} - invalid region name@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{starting_address} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - address not on four byte boundary@*
@code{@value{RPREFIX}TOO_MANY} - too many regions created@*
@code{@value{RPREFIX}INVALID_SIZE} - invalid page size

@subheading DESCRIPTION:

This directive creates a region from a physically
contiguous memory space which starts at starting_address and is
length bytes long.  Segments allocated from the region will be a
multiple of page_size bytes in length.  The assigned region id
is returned in id.  This region id is used as an argument to
other region related directives to access the region.

For control and maintenance of the region, RTEMS
allocates and initializes an RNCB from the RNCB free pool.  Thus
memory from the region is not used to store the RNCB.  However,
some overhead within the region is required by RTEMS each time a
segment is constructed in the region.

Specifying @code{@value{RPREFIX}PRIORITY} in attribute_set causes tasks
waiting for a segment to be serviced according to task priority.
Specifying @code{@value{RPREFIX}FIFO} in attribute_set or selecting
@code{@value{RPREFIX}DEFAULT_ATTRIBUTES} will cause waiting tasks to
be serviced in First In-First Out order.

The @code{starting_address} parameter must be aligned on a
four byte boundary.  The @code{page_size} parameter must be a multiple
of four greater than or equal to eight.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

The following region attribute constants are defined
by RTEMS:

@itemize @bullet
@item @code{@value{RPREFIX}FIFO} - tasks wait by FIFO (default)
@item @code{@value{RPREFIX}PRIORITY} - tasks wait by priority
@end itemize

@c
@c
@c
@page
@subsection REGION_IDENT - Get ID of a region

@cindex get ID of a region
@cindex obtain ID of a region

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_ident
@example
rtems_status_code rtems_region_ident(
  rtems_name  name,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Ident (
   Name   : in     RTEMS.Name;
   ID     :    out RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - region identified successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_NAME} - region name not found

@subheading DESCRIPTION:

This directive obtains the region id associated with
the region name to be acquired.  If the region name is not
unique, then the region id will match one of the regions with
that name.  However, this region id is not guaranteed to
correspond to the desired region.  The region id is used to
access this region in other region manager directives.

@subheading NOTES:

This directive will not cause the running task to be preempted.

@c
@c
@c
@page
@subsection REGION_DELETE - Delete a region

@cindex delete a region

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_delete
@example
rtems_status_code rtems_region_delete(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Delete (
   ID     : in     RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - region deleted successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid region id@*
@code{@value{RPREFIX}RESOURCE_IN_USE} - segments still in use

@subheading DESCRIPTION:

This directive deletes the region specified by id.
The region cannot be deleted if any of its segments are still
allocated.  The RNCB for the deleted region is reclaimed by
RTEMS.

@subheading NOTES:

This directive will not cause the calling task to be preempted.

The calling task does not have to be the task that
created the region.  Any local task that knows the region id can
delete the region.

@c
@c
@c
@page
@subsection REGION_EXTEND - Add memory to a region

@cindex add memory to a region
@cindex region, add memory

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_extend
@example
rtems_status_code rtems_region_extend(
  rtems_id  id,
  void     *starting_address,
  intptr_t  length
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Extend (
   ID               : in     RTEMS.ID;
   Starting_Address : in     RTEMS.Address;
   Length           : in     RTEMS.Unsigned32;
   Result           :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - region extended successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{starting_address} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid region id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - invalid address of area to add

@subheading DESCRIPTION:

This directive adds the memory which starts at
starting_address for length bytes to the region specified by id.

@subheading NOTES:

This directive will not cause the calling task to be preempted.

The calling task does not have to be the task that
created the region.  Any local task that knows the region id can
extend the region.

@c
@c
@c
@page
@subsection REGION_GET_SEGMENT - Get segment from a region

@cindex get segment from region

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_get_segment
@example
rtems_status_code rtems_region_get_segment(
  rtems_id         id,
  intptr_t         size,
  rtems_option     option_set,
  rtems_interval   timeout,
  void           **segment
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Get_Segment (
   ID         : in     RTEMS.ID;
   Size       : in     RTEMS.Unsigned32;
   Option_Set : in     RTEMS.Option;
   Timeout    : in     RTEMS.Interval;
   Segment    :    out RTEMS.Address;
   Result     :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - segment obtained successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{segment} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid region id@*
@code{@value{RPREFIX}INVALID_SIZE} - request is for zero bytes or exceeds
the size of maximum segment which is possible for this region@*
@code{@value{RPREFIX}UNSATISFIED} - segment of requested size not available@*
@code{@value{RPREFIX}TIMEOUT} - timed out waiting for segment@*
@code{@value{RPREFIX}OBJECT_WAS_DELETED} - region deleted while waiting

@subheading DESCRIPTION:

This directive obtains a variable size segment from
the region specified by id.  The address of the allocated
segment is returned in segment.  The @code{@value{RPREFIX}WAIT}
and @code{@value{RPREFIX}NO_WAIT} components
of the options parameter are used to specify whether the calling
tasks wish to wait for a segment to become available or return
immediately if no segment is available.  For either option, if a
sufficiently sized segment is available, then the segment is
successfully acquired by returning immediately with  the
@code{@value{RPREFIX}SUCCESSFUL} status code.

If the calling task chooses to return immediately and
a segment large enough is not available, then an error code
indicating this fact is returned.  If the calling task chooses
to wait for the segment and a segment large enough is not
available, then the calling task is placed on the region's
segment wait queue and blocked.  If the region was created with
the @code{@value{RPREFIX}PRIORITY} option, then the calling
task is inserted into the
wait queue according to its priority.  However, if the region
was created with the @code{@value{RPREFIX}FIFO} option, then the calling
task is placed at the rear of the wait queue.

The timeout parameter specifies the maximum interval
that a task is willing to wait to obtain a segment.  If timeout
is set to @code{@value{RPREFIX}NO_TIMEOUT}, then the
calling task will wait forever.

@subheading NOTES:

The actual length of the allocated segment may be
larger than the requested size because a segment size is always
a multiple of the region's page size.

The following segment acquisition option constants
are defined by RTEMS:

@itemize @bullet
@item @code{@value{RPREFIX}WAIT} - task will wait for segment (default)
@item @code{@value{RPREFIX}NO_WAIT} - task should not wait
@end itemize

A clock tick is required to support the timeout functionality of
this directive.

@c
@c
@c
@page
@subsection REGION_RETURN_SEGMENT - Return segment to a region

@cindex return segment to region

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_return_segment
@example
rtems_status_code rtems_region_return_segment(
  rtems_id  id,
  void     *segment
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Return_Segment (
   ID      : in     RTEMS.ID;
   Segment : in     RTEMS.Address;
   Result  :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - segment returned successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{segment} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid region id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - segment address not in region

@subheading DESCRIPTION:

This directive returns the segment specified by
segment to the region specified by id.  The returned segment is
merged with its neighbors to form the largest possible segment.
The first task on the wait queue is examined to determine if its
segment request can now be satisfied.  If so, it is given a
segment and unblocked.  This process is repeated until the first
task's segment request cannot be satisfied.

@subheading NOTES:

This directive will cause the calling task to be
preempted if one or more local tasks are waiting for a segment
and the following conditions exist:

@itemize @bullet
@item a waiting task has a higher priority than the calling task

@item the size of the segment required by the waiting task
is less than or equal to the size of the segment returned.
@end itemize

@c
@c
@c
@page
@subsection REGION_GET_SEGMENT_SIZE - Obtain size of a segment

@cindex get size of segment

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_get_segment_size
@example
rtems_status_code rtems_region_get_segment_size(
  rtems_id  id,
  void     *segment,
  ssize_t  *size
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Get_Segment_Size (
   ID         : in     RTEMS.ID;
   Segment    : in     RTEMS.Address;
   Size       :    out RTEMS.Unsigned32;
   Result     :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - segment obtained successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{segment} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{size} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid region id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - segment address not in region

@subheading DESCRIPTION:

This directive obtains the size in bytes of the specified segment.

@subheading NOTES:

The actual length of the allocated segment may be
larger than the requested size because a segment size is always
a multiple of the region's page size.

@c
@c
@c
@page
@subsection REGION_RESIZE_SEGMENT - Change size of a segment

@cindex resize segment

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_region_resize_segment
@example
rtems_status_code rtems_region_resize_segment(
  rtems_id     id,
  void        *segment,
  ssize_t      size,
  ssize_t     *old_size
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Region_Resize_Segment (
   ID         : in     RTEMS.ID;
   Segment    : in     RTEMS.Address;
   Size       : in     RTEMS.Unsigned32;
   Old_Size   :    out RTEMS.Unsigned32;
   Result     :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - segment obtained successfully@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{segment} is NULL@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{old_size} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid region id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - segment address not in region
@code{@value{RPREFIX}UNSATISFIED} - unable to make segment larger

@subheading DESCRIPTION:

This directive is used to increase or decrease the size of 
a segment.  When increasing the size of a segment, it
is possible that there is not memory available contiguous
to the segment.  In this case, the request is unsatisfied.

@subheading NOTES:

If an attempt to increase the size of a segment fails, then
the application may want to allocate a new segment of the desired
size, copy the contents of the original segment to the new, larger
segment and then return the original segment.

