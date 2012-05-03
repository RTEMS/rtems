@c
@c  COPYRIGHT (c) 1988-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@c
@c  The following figure was replaced with an ASCII equivalent.
@c    Figure 2-1 Object ID Composition
@c

@chapter Key Concepts

@section Introduction

The facilities provided by RTEMS are built upon a
foundation of very powerful concepts.  These concepts must be
understood before the application developer can efficiently
utilize RTEMS.  The purpose of this chapter is to familiarize
one with these concepts.

@section Objects

@cindex objects

RTEMS provides directives which can be used to
dynamically create, delete, and manipulate a set of predefined
object types.  These types include tasks, message queues,
semaphores, memory regions, memory partitions, timers, ports,
and rate monotonic periods.  The object-oriented nature of RTEMS
encourages the creation of modular applications built upon
re-usable "building block" routines.

All objects are created on the local node as required
by the application and have an RTEMS assigned ID.  All objects
have a user-assigned name.  Although a relationship exists
between an object's name and its RTEMS assigned ID, the name and
ID are not identical.  Object names are completely arbitrary and
selected by the user as a meaningful "tag" which may commonly
reflect the object's use in the application.  Conversely, object
IDs are designed to facilitate efficient object manipulation by
the executive.

@subsection Object Names

@cindex object name
@findex rtems_object_name

An object name is an unsigned thirty-two bit entity
associated with the object by the user.  The data type
@code{@value{DIRPREFIX}name} is used to store object names.

@findex rtems_build_name

Although not required by RTEMS, object names are often
composed of four ASCII characters which help identify that object.
For example, a task which causes a light to blink might be
called "LITE".  The @code{@value{DIRPREFIX}build_name} routine
is provided to build an object name from four ASCII characters.  
The following example illustrates this:

@ifset is-C
@example
rtems_object_name my_name;

my_name = rtems_build_name( 'L', 'I', 'T', 'E' );
@end example
@end ifset

@ifset is-Ada
@example
My_Name : RTEMS.Name;

My_Name = RTEMS.Build_Name( 'L', 'I', 'T', 'E' );
@end example
@end ifset

However, it is not required that the application use ASCII
characters to build object names.  For example, if an
application requires one-hundred tasks, it would be difficult to
assign meaningful ASCII names to each task.  A more convenient
approach would be to name them the binary values one through
one-hundred, respectively.

@ifset is-C
@findex rtems_object_get_name

RTEMS provides a helper routine, @code{@value{DIRPREFIX}object_get_name},
which can be used to obtain the name of any RTEMS object using just
its ID.  This routine attempts to convert the name into a printable string.

The following example illustrates the use of this method to print
an object name:

@example
#include <rtems.h>
#include <rtems/bspIo.h>

void print_name(rtems_id id)
@{
  char  buffer[10];   /* name assumed to be 10 characters or less */
  char *result;

  result = rtems_object_get_name( id, sizeof(buffer), buffer ); 
  printk( "ID=0x%08x name=%s\n", id, ((result) ? result : "no name") );
@}
@end example
@end ifset


@subsection Object IDs

@cindex object ID
@cindex object ID composition
@findex rtems_id

@need 3000

An object ID is a unique unsigned integer value which uniquely identifies
an object instance.  Object IDs are passed as arguments to many directives
in RTEMS and RTEMS translates the ID to an internal object pointer. The
efficient manipulation of object IDs is critical to the performance
of RTEMS services.  Because of this, there are two object Id formats
defined.  Each target architecture specifies which format it will use.
There is a thirty-two bit format which is used for most of the supported
architectures and supports multiprocessor configurations.  There is also
a simpler sixteen bit format which is appropriate for smaller target
architectures and does not support multiprocessor configurations.

@subsubsection Thirty-Two Object ID Format

The thirty-two bit format for an object ID is composed of four parts: API,
object class, node, and index.  The data type @code{@value{DIRPREFIX}id}
is used to store object IDs.


@float Figure,fig:Object-Id-32
@caption{Thirty-Two Bit Object Id}

@ifset use-ascii
@example
@group
     31      27 26   24 23          16 15                             0
     +---------+-------+--------------+-------------------------------+
     |         |       |              |                               |
     |  Class  |  API  |     Node     |             Index             |
     |         |       |              |                               |
     +---------+-------+--------------+-------------------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp1
@center{@image{ObjectId-32Bits,,2in,Thirty-Two Bit Object Id}}
@end ifset

@ifset use-html
@html
<P ALIGN="center"><IMG SRC="ObjectId-32Bits.png"
     WIDTH=550 HEIGHT=400 ALT="Thirty-Two Bit Object Id"></P>
@end html
@end ifset
@end float

The most significant five bits are the object class.  The next
three bits indicate the API to which the object class belongs.
The next eight bits (16-23) are the number of the node on which
this object was created.  The node number is always one (1) in a single
processor system.  The least significant sixteen bits form an
identifier within a particular object type.  This identifier,
called the object index, ranges in value from 1 to the maximum
number of objects configured for this object type.

@subsubsection Sixteen Bit Object ID Format

The sixteen bit format for an object ID is composed of three parts: API,
object class, and index.  The data type @code{@value{DIRPREFIX}id}
is used to store object IDs.

@float Figure,fig:Object-Id-16
@caption{Sixteen Bit Object Id}


@ifset use-ascii
@example
@group
     15      11 10    8 7            0
     +---------+-------+--------------+
     |         |       |              |
     |  Class  |  API  |    Index     |
     |         |       |              |
     +---------+-------+--------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp1
@center{@image{ObjectId-16Bits,,2in,Sixteen Bit Object Id}}
@end ifset

@ifset use-html
@html
<P ALIGN="center"><IMG SRC="ObjectId-16Bits.png"
     WIDTH=550 HEIGHT=400 ALT="16 Bit Object Id"></P>
@end html
@end ifset
@end float

The sixteen-bit format is designed to be as similar as possible to the
thrity-two bit format.  The differences are limited to the eliminatation
of the node field and reduction of the index field from sixteen-bits
to 8-bits.  Thus the sixteen bit format only supports up to 255 object
instances per API/Class combination and single processor systems.
As this format is typically utilized by sixteen-bit processors with
limited address space, this is more than enough object instances.

@subsection Object ID Description

The components of an object ID make it possible
to quickly locate any object in even the most complicated
multiprocessor system.  Object ID's are associated with an
object by RTEMS when the object is created and the corresponding
ID is returned by the appropriate object create directive.  The
object ID is required as input to all directives involving
objects, except those which create an object or obtain the ID of
an object.

The object identification directives can be used to
dynamically obtain a particular object's ID given its name.
This mapping is accomplished by searching the name table
associated with this object type.  If the name is non-unique,
then the ID associated with the first occurrence of the name
will be returned to the application.  Since object IDs are
returned when the object is created, the object identification
directives are not necessary in a properly designed single
processor application.

In addition, services are provided to portably examine the
subcomponents of an RTEMS ID.  These services are
described in detail later in this manual but are prototyped
as follows:

@cindex obtaining class from object ID
@cindex obtaining node from object ID
@cindex obtaining index from object ID
@cindex get class from object ID
@cindex get node from object ID
@cindex get index from object ID
@findex rtems_object_id_get_api
@findex rtems_object_id_get_class
@findex rtems_object_id_get_node
@findex rtems_object_id_get_index

@example
uint32_t rtems_object_id_get_api( rtems_id );
uint32_t rtems_object_id_get_class( rtems_id );
uint32_t rtems_object_id_get_node( rtems_id );
uint32_t rtems_object_id_get_index( rtems_id );
@end example

An object control block is a data structure defined
by RTEMS which contains the information necessary to manage a
particular object type.  For efficiency reasons, the format of
each object type's control block is different.  However, many of
the fields are similar in function.  The number of each type of
control block is application dependent and determined by the
values specified in the user's Configuration Table.  An object
control block is allocated at object create time and freed when
the object is deleted.  With the exception of user extension
routines, object control blocks are not directly manipulated by
user applications.

@section Communication and Synchronization

@cindex communication and synchronization

In real-time multitasking applications, the ability
for cooperating execution threads to communicate and synchronize
with each other is imperative.  A real-time executive should
provide an application with the following capabilities:

@itemize @bullet
@item Data transfer between cooperating tasks
@item Data transfer between tasks and ISRs
@item Synchronization of cooperating tasks
@item Synchronization of tasks and ISRs
@end itemize

Most RTEMS managers can be used to provide some form
of communication and/or synchronization.  However, managers
dedicated specifically to communication and synchronization
provide well established mechanisms which directly map to the
application's varying needs.  This level of flexibility allows
the application designer to match the features of a particular
manager with the complexity of communication and synchronization
required.  The following managers were specifically designed for
communication and synchronization:

@itemize @bullet
@item Semaphore
@item Message Queue
@item Event
@item Signal
@end itemize

The semaphore manager supports mutual exclusion
involving the synchronization of access to one or more shared
user resources.  Binary semaphores may utilize the optional
priority inheritance algorithm to avoid the problem of priority
inversion.  The message manager supports both communication and
synchronization, while the event manager primarily provides a
high performance synchronization mechanism.  The signal manager
supports only asynchronous communication and is typically used
for exception handling.

@section Time

@cindex time

The development of responsive real-time applications
requires an understanding of how RTEMS maintains and supports
time-related operations.  The basic unit of time in RTEMS is
known as a tick.  The frequency of clock ticks is completely
application dependent and determines the granularity and
accuracy of all interval and calendar time operations.

@findex rtems_interval

By tracking time in units of ticks, RTEMS is capable
of supporting interval timing functions such as task delays,
timeouts, timeslicing, the delayed execution of timer service
routines, and the rate monotonic scheduling of tasks.  An
interval is defined as a number of ticks relative to the current
time.  For example, when a task delays for an interval of ten
ticks, it is implied that the task will not execute until ten
clock ticks have occurred.
All intervals are specified using data type
@code{@value{DIRPREFIX}interval}.

A characteristic of interval timing is that the
actual interval period may be a fraction of a tick less than the
interval requested.  This occurs because the time at which the
delay timer is set up occurs at some time between two clock
ticks.  Therefore, the first countdown tick occurs in less than
the complete time interval for a tick.  This can be a problem if
the clock granularity is large.

The rate monotonic scheduling algorithm is a hard
real-time scheduling methodology.  This methodology provides
rules which allows one to guarantee that a set of independent
periodic tasks will always meet their deadlines -- even under
transient overload conditions.  The rate monotonic manager
provides directives built upon the Clock Manager's interval
timer support routines.

Interval timing is not sufficient for the many
applications which require that time be kept in wall time or
true calendar form.  Consequently, RTEMS maintains the current
date and time.  This allows selected time operations to be
scheduled at an actual calendar date and time.  For example, a
task could request to delay until midnight on New Year's Eve
before lowering the ball at Times Square.
The data type @code{@value{DIRPREFIX}time_of_day} is used to specify
calendar time in RTEMS services.  
@xref{Clock Manager Time and Date Data Structures, , Time and Date Data Structures}.
@findex rtems_time_of_day

Obviously, the directives which use intervals or wall
time cannot operate without some external mechanism which
provides a periodic clock tick.  This clock tick is typically
provided by a real time clock or counter/timer device.

@section Memory Management

@cindex memory management

RTEMS memory management facilities can be grouped
into two classes: dynamic memory allocation and address
translation.  Dynamic memory allocation is required by
applications whose memory requirements vary through the
application's course of execution.  Address translation is
needed by applications which share memory with another CPU or an
intelligent Input/Output processor.  The following RTEMS
managers provide facilities to manage memory:

@itemize @bullet
@item Region

@item Partition

@item Dual Ported Memory
@end itemize

RTEMS memory management features allow an application
to create simple memory pools of fixed size buffers and/or more
complex memory pools of variable size segments.  The partition
manager provides directives to manage and maintain pools of
fixed size entities such as resource control blocks.
Alternatively, the region manager provides a more general
purpose memory allocation scheme that supports variable size
blocks of memory which are dynamically obtained and freed by the
application.  The dual-ported memory manager provides executive
support for address translation between internal and external
dual-ported RAM address space.
