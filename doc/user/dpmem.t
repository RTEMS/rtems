@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Dual-Ported Memory Manager, Dual-Ported Memory Manager Introduction, REGION_GET_SEGMENT_SIZE - Obtain size of a segment, Top
@end ifinfo
@chapter Dual-Ported Memory Manager
@ifinfo
@menu
* Dual-Ported Memory Manager Introduction::
* Dual-Ported Memory Manager Background::
* Dual-Ported Memory Manager Operations::
* Dual-Ported Memory Manager Directives::
@end menu
@end ifinfo

@ifinfo
@node Dual-Ported Memory Manager Introduction, Dual-Ported Memory Manager Background, Dual-Ported Memory Manager, Dual-Ported Memory Manager
@end ifinfo
@section Introduction

The dual-ported memory manager provides a mechanism
for converting addresses between internal and external
representations for multiple dual-ported memory areas (DPMA).
The directives provided by the dual-ported memory manager are:

@itemize @bullet
@item @code{port_create} - Create a port
@item @code{port_ident} - Get ID of a port
@item @code{port_delete} - Delete a port
@item @code{port_external_to_internal} - Convert external to internal address
@item @code{port_internal_to_external} - Convert internal to external address
@end itemize

@ifinfo
@node Dual-Ported Memory Manager Background, Dual-Ported Memory Manager Operations, Dual-Ported Memory Manager Introduction, Dual-Ported Memory Manager
@end ifinfo
@section Background

A dual-ported memory area (DPMA) is an contiguous
block of RAM owned by a particular processor but which can be
accessed by other processors in the system.  The owner accesses
the memory using internal addresses, while other processors must
use external addresses.  RTEMS defines a port as a particular
mapping of internal and external addresses.

There are two system configurations in which
dual-ported memory is commonly found.  The first is
tightly-coupled multiprocessor computer systems where the
dual-ported memory is shared between all nodes and is used for
inter-node communication.  The second configuration is computer
systems with intelligent peripheral controllers.  These
controllers typically utilize the DPMA for high-performance data
transfers.

@ifinfo
@node Dual-Ported Memory Manager Operations, Creating a Port, Dual-Ported Memory Manager Background, Dual-Ported Memory Manager
@end ifinfo
@section Operations
@ifinfo
@menu
* Creating a Port::
* Obtaining Port IDs::
* Converting an Address::
* Deleting a DPMA Port::
@end menu
@end ifinfo

@ifinfo
@node Creating a Port, Obtaining Port IDs, Dual-Ported Memory Manager Operations, Dual-Ported Memory Manager Operations
@end ifinfo
@subsection Creating a Port

The port_create directive creates a port into a DPMA
with the user-defined name.  The user specifies the association
between internal and external representations for the port being
created.  RTEMS allocates a Dual-Ported Memory Control Block
(DPCB) from the DPCB free list to maintain the newly created
DPMA.  RTEMS also generates a unique dual-ported memory port ID
which is returned to the calling task.  RTEMS does not
initialize the dual-ported memory area or access any memory
within it.

@ifinfo
@node Obtaining Port IDs, Converting an Address, Creating a Port, Dual-Ported Memory Manager Operations
@end ifinfo
@subsection Obtaining Port IDs

When a port is created, RTEMS generates a unique port
ID and assigns it to the created port until it is deleted.  The
port ID may be obtained by either of two methods.  First, as the
result of an invocation of the port_create directive, the task
ID is stored in a user provided location.  Second, the port ID
may be obtained later using the port_ident directive.  The port
ID is used by other dual-ported memory manager directives to
access this port.

@ifinfo
@node Converting an Address, Deleting a DPMA Port, Obtaining Port IDs, Dual-Ported Memory Manager Operations
@end ifinfo
@subsection Converting an Address

The port_external_to_internal directive is used to
convert an address from external to internal representation for
the specified port.  The port_internal_to_external directive is
used to convert an address from internal to external
representation for the specified port.  If an attempt is made to
convert an address which lies outside the specified DPMA, then
the address to be converted will be returned.

@ifinfo
@node Deleting a DPMA Port, Dual-Ported Memory Manager Directives, Converting an Address, Dual-Ported Memory Manager Operations
@end ifinfo
@subsection Deleting a DPMA Port

A port can be removed from the system and returned to
RTEMS with the port_delete directive.  When a port is deleted,
its control block is returned to the DPCB free list.

@ifinfo
@node Dual-Ported Memory Manager Directives, PORT_CREATE - Create a port, Deleting a DPMA Port, Dual-Ported Memory Manager
@end ifinfo
@section Directives
@ifinfo
@menu
* PORT_CREATE - Create a port::
* PORT_IDENT - Get ID of a port::
* PORT_DELETE - Delete a port::
* PORT_EXTERNAL_TO_INTERNAL - Convert external to internal address::
* PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address::
@end menu
@end ifinfo

This section details the dual-ported memory manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@page
@ifinfo
@node PORT_CREATE - Create a port, PORT_IDENT - Get ID of a port, Dual-Ported Memory Manager Directives, Dual-Ported Memory Manager Directives
@end ifinfo
@subsection PORT_CREATE - Create a port

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_port_create(
  rtems_name        name,
  void             *internal_start,
  void             *external_start,
  rtems_unsigned32  length,
  rtems_id         *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Port_Create (
   Name           : in     RTEMS.Name;
   Internal_Start : in     RTEMS.Address;
   External_Start : in     RTEMS.Address;
   Length         : in     RTEMS.Unsigned32;
   ID             :    out RTEMS.ID;
   Result         :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - port created successfully@*
@code{INVALID_NAME} - invalid task name@*
@code{INVALID_ADDRESS} - address not on four byte boundary@*
@code{TOO_MANY} - too many DP memory areas created

@subheading DESCRIPTION:

This directive creates a port which resides on the
local node for the specified DPMA.  The assigned port id is
returned in id.  This port id is used as an argument to other
dual-ported memory manager directives to convert addresses
within this DPMA.

For control and maintenance of the port, RTEMS
allocates and initializes an DPCB from the DPCB free pool.  Thus
memory from the dual-ported memory area is not used to store the
DPCB.

@subheading NOTES:

The internal_address and external_address parameters
must be on a four byte boundary.

This directive will not cause the calling task to be
preempted.

@page
@ifinfo
@node PORT_IDENT - Get ID of a port, PORT_DELETE - Delete a port, PORT_CREATE - Create a port, Dual-Ported Memory Manager Directives
@end ifinfo
@subsection PORT_IDENT - Get ID of a port

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_port_ident(
  rtems_name  name,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Port_Ident (
   Name   : in     RTEMS.Name;
   ID     :    out RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - port identified successfully@*
@code{INVALID_NAME} - port name not found

@subheading DESCRIPTION:

This directive obtains the port id associated with
the specified name to be acquired.  If the port name is not
unique, then the port id will match one of the DPMAs with that
name.  However, this port id is not guaranteed to correspond to
the desired DPMA.  The port id is used to access this DPMA in
other dual-ported memory area related directives.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

@page
@ifinfo
@node PORT_DELETE - Delete a port, PORT_EXTERNAL_TO_INTERNAL - Convert external to internal address, PORT_IDENT - Get ID of a port, Dual-Ported Memory Manager Directives
@end ifinfo
@subsection PORT_DELETE - Delete a port

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_port_delete(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Port_Delete (
   ID     : in     RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - port deleted successfully@*
@code{INVALID_ID} - invalid port id

@subheading DESCRIPTION:

This directive deletes the dual-ported memory area
specified by id.  The DPCB for the deleted dual-ported memory
area is reclaimed by RTEMS.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

The calling task does not have to be the task that
created the port.  Any local task that knows the port id can
delete the port.

@page
@ifinfo
@node PORT_EXTERNAL_TO_INTERNAL - Convert external to internal address, PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address, PORT_DELETE - Delete a port, Dual-Ported Memory Manager Directives
@end ifinfo
@subsection PORT_EXTERNAL_TO_INTERNAL - Convert external to internal address

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_port_external_to_internal(
  rtems_id   id,
  void      *external,
  void     **internal
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Port_External_To_Internal (
   ID       : in     RTEMS.ID;
   External : in     RTEMS.Address;
   Internal :    out RTEMS.Address;
   Result   :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} -  always successful

@subheading DESCRIPTION:

This directive converts a dual-ported memory address
from external to internal representation for the specified port.
If the given external address is invalid for the specified
port, then the internal address is set to the given external
address.

@subheading NOTES:

This directive is callable from an ISR.

This directive will not cause the calling task to be
preempted.

@page
@ifinfo
@node PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address, I/O Manager, PORT_EXTERNAL_TO_INTERNAL - Convert external to internal address, Dual-Ported Memory Manager Directives
@end ifinfo
@subsection PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_port_internal_to_external(
  rtems_id   id,
  void      *internal,
  void     **external
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Port_Internal_To_External (
   ID       : in     RTEMS.ID;
   Internal : in     RTEMS.Address;
   External :    out RTEMS.Address;
   Result   :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} -  always successful

@subheading DESCRIPTION:

This directive converts a dual-ported memory address
from internal to external representation so that it can be
passed to owner of the DPMA represented by the specified port.
If the given internal address is an invalid dual-ported address,
then the external address is set to the given internal address.

@subheading NOTES:

This directive is callable from an ISR.

This directive will not cause the calling task to be
preempted.

