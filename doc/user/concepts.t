@c
@c  COPYRIGHT (c) 1996.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@c
@c  The following figure was replaced with an ASCII equivalent.
@c    Figure 2-1 Object ID Composition
@c
@ifinfo
@node Key Concepts, Key Concepts Introduction, Overview Manual Organization, Top
@end ifinfo
@chapter Key Concepts
@ifinfo
@menu
* Key Concepts Introduction::
* Key Concepts Objects::
* Key Concepts Communication and Synchronization::
* Key Concepts Time::
* Key Concepts Memory Management::
@end menu
@end ifinfo

@ifinfo
@node Key Concepts Introduction, Key Concepts Objects, Key Concepts, Key Concepts
@end ifinfo
@section Introduction

The facilities provided by RTEMS are built upon a
foundation of very powerful concepts.  These concepts must be
understood before the application developer can efficiently
utilize RTEMS.  The purpose of this chapter is to familiarize
one with these concepts.

@ifinfo
@node Key Concepts Objects, Key Concepts Communication and Synchronization, Key Concepts Introduction, Key Concepts
@end ifinfo
@section Objects

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

An object name is an unsigned thirty-two bit entity
associated with the object by the user.  Although not required
by RTEMS, object names are typically composed of four ASCII
characters which help identify that object.  For example, a task
which causes a light to blink might be called "LITE".  Utilities
are provided to build an object name from four ASCII characters
and to decompose an object name into four ASCII characters.
However, it is not required that the application use ASCII
characters to build object names.  For example, if an
application requires one-hundred tasks, it would be difficult to
assign meaningful ASCII names to each task.  A more convenient
approach would be to name them the binary values one through
one-hundred, respectively.

@need 3000

An object ID is a unique unsigned thirty-two bit
entity composed of three parts: object class, node, and index.
The most significant six bits are the object class.  The next
ten bits are the number of the node on which this object was
created.  The node number is always one (1) in a single
processor system.  The least significant sixteen bits form an
identifier within a particular object type.  This identifier,
called the object index, ranges in value from 1 to the maximum
number of objects configured for this object type.

@ifset use-ascii
@example
@group
     31        26 25              16 15                             0
     +-----------+------------------+-------------------------------+
     |           |                  |                               |
     |   Class   |       Node       |             Index             |
     |           |                  |                               |
     +-----------+------------------+-------------------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\strut#&
\hbox to 0.50in{\enskip#}&
\hbox to 0.50in{\enskip#}&
#&
\hbox to 0.50in{\enskip#}&
\hbox to 0.50in{\enskip#}&
#&
\hbox to 1.00in{\enskip#}&
\hbox to 1.00in{\enskip#}&
#\cr
\multispan{9}\cr
\multispan{2}31\hfil&\multispan{2}\hfil26\enskip&
 \multispan{1}\enskip25\hfil&\multispan{2}\hfil16\enskip&
 \multispan{1}\enskip15\hfil&\multispan{2}\hfil0\cr
&&&&&&&&&\cr
}}\hfil}
\centerline{\vbox{\offinterlineskip\halign{
\strut\vrule#&
\hbox to 0.50in{\enskip#}&
\hbox to 0.50in{\enskip#}&
\vrule#&
\hbox to 0.50in{\enskip#}&
\hbox to 0.50in{\enskip#}&
\vrule#&
\hbox to 0.50in{\enskip#}&
\hbox to 0.50in{\enskip#}&
\vrule#\cr
\multispan{9}\cr
\noalign{\hrule}
&&&&&&&&&\cr
&\multispan{2}\hfil Class\hfil&&
 \multispan{2}\hfil Node\hfil&&
 \multispan{2}\hfil Index\hfil&\cr
&&&&&&&&&\cr
\noalign{\hrule}
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=6 WIDTH="60%" BORDER=0>
<TR><TD ALIGN=left><STRONG>31</STRONG></TD>
    <TD ALIGN=right><STRONG>26</STRONG></TD>
    <TD ALIGN=left><STRONG>25</STRONG></TD>
    <TD ALIGN=right><STRONG>16</STRONG></TD>
    <TD ALIGN=left><STRONG>15</STRONG></TD>
    <TD ALIGN=right><STRONG>0</STRONG></TD></TR>
  </TABLE>
</CENTER>
<CENTER>
  <TABLE COLS=6 WIDTH="60%" BORDER=2>
<TR><TD ALIGN=center COLSPAN=2>Class</TD>
    <TD ALIGN=center COLSPAN=2>Node</TD>
    <TD ALIGN=center COLSPAN=2>Index</TD></TD>
  </TABLE>
</CENTER>
@end html
@end ifset


The three components of an object ID make it possible
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

@ifinfo
@node Key Concepts Communication and Synchronization, Key Concepts Time, Key Concepts Objects, Key Concepts
@end ifinfo
@section Communication and Synchronization

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

@ifinfo
@node Key Concepts Time, Key Concepts Memory Management, Key Concepts Communication and Synchronization, Key Concepts
@end ifinfo
@section Time

The development of responsive real-time applications
requires an understanding of how RTEMS maintains and supports
time-related operations.  The basic unit of time in RTEMS is
known as a tick.  The frequency of clock ticks is completely
application dependent and determines the granularity and
accuracy of all interval and calendar time operations.

By tracking time in units of ticks, RTEMS is capable
of supporting interval timing functions such as task delays,
timeouts, timeslicing, the delayed execution of timer service
routines, and the rate monotonic scheduling of tasks.  An
interval is defined as a number of ticks relative to the current
time.  For example, when a task delays for an interval of ten
ticks, it is implied that the task will not execute until ten
clock ticks have occurred.

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

Obviously, the directives which use intervals or wall
time cannot operate without some external mechanism which
provides a periodic clock tick.  This clock tick is typically
provided by a real time clock or counter/timer device.

@ifinfo
@node Key Concepts Memory Management, Initialization Manager, Key Concepts Time, Key Concepts
@end ifinfo
@section Memory Management

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
