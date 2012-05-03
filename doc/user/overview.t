@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@c
@c  This chapter is missing the following figures:
@c
@c     Figure 1-1  RTEMS Application Architecture
@c     Figure 1-2  RTEMS Internal Architecture
@c

@chapter Overview

@section Introduction

RTEMS, Real-Time Executive for Multiprocessor Systems, is a
real-time executive (kernel) which provides a high performance
environment for embedded military applications including the
following features:

@itemize @bullet
@item multitasking capabilities
@item homogeneous and heterogeneous multiprocessor systems
@item event-driven, priority-based, preemptive scheduling
@item optional rate monotonic scheduling
@item intertask communication and synchronization
@item priority inheritance
@item responsive interrupt management
@item dynamic memory allocation
@item high level of user configurability
@end itemize

This manual describes the usage of RTEMS for
applications written in the @value{LANGUAGE} programming language.  Those
implementation details that are processor dependent are provided
in the Applications Supplement documents.  A supplement
document which addresses specific architectural issues that
affect RTEMS is provided for each processor type that is
supported.

@section Real-time Application Systems

Real-time application systems are a special class of
computer applications.  They have a complex set of
characteristics that distinguish them from other software
problems.  Generally, they must adhere to more rigorous
requirements.  The correctness of the system depends not only on
the results of computations, but also on the time at which the
results are produced.  The most important and complex
characteristic of real-time application systems is that they
must receive and respond to a set of external stimuli within
rigid and critical time constraints referred to as deadlines.
Systems can be buried by an avalanche of interdependent,
asynchronous or cyclical event streams.

Deadlines can be further characterized as either hard
or soft based upon the value of the results when produced after
the deadline has passed.  A deadline is hard if the results have
no value or if their use will result in a catastrophic event.
In contrast, results which are produced after a soft deadline
may have some value.

Another distinguishing requirement of real-time
application systems is the ability to coordinate or manage a
large number of concurrent activities. Since software is a
synchronous entity, this presents special problems.  One
instruction follows another in a repeating synchronous cycle.
Even though mechanisms have been developed to allow for the
processing of external asynchronous events, the software design
efforts required to process and manage these events and tasks
are growing more complicated.

The design process is complicated further by
spreading this activity over a set of processors instead of a
single processor. The challenges associated with designing and
building real-time application systems become very complex when
multiple processors are involved.  New requirements such as
interprocessor communication channels and global resources that
must be shared between competing processors are introduced.  The
ramifications of multiple processors complicate each and every
characteristic of a real-time system.

@section Real-time Executive

Fortunately, real-time operating systems or real-time
executives serve as a cornerstone on which to build the
application system.  A real-time multitasking executive allows
an application to be cast into a set of logical, autonomous
processes or tasks which become quite manageable.  Each task is
internally synchronous, but different tasks execute
independently, resulting in an asynchronous processing stream.
Tasks can be dynamically paused for many reasons resulting in a
different task being allowed to execute for a period of time.
The executive also provides an interface to other system
components such as interrupt handlers and device drivers.
System components may request the executive to allocate and
coordinate resources, and to wait for and trigger synchronizing
conditions.  The executive system calls effectively extend the
CPU instruction set to support efficient multitasking.  By
causing tasks to travel through well-defined state transitions,
system calls permit an application to demand-switch between
tasks in response to real-time events.

By proper grouping of responses to stimuli into
separate tasks, a system can now asynchronously switch between
independent streams of execution, directly responding to
external stimuli as they occur.  This allows the system design
to meet critical performance specifications which are typically
measured by guaranteed response time and transaction throughput.
The multiprocessor extensions of RTEMS provide the features
necessary to manage the extra requirements introduced by a
system distributed across several processors.  It removes the
physical barriers of processor boundaries from the world of the
system designer, enabling more critical aspects of the system to
receive the required attention. Such a system, based on an
efficient real-time, multiprocessor executive, is a more
realistic model of the outside world or environment for which it
is designed.  As a result, the system will always be more
logical, efficient, and reliable.

By using the directives provided by RTEMS, the
real-time applications developer is freed from the problem of
controlling and synchronizing multiple tasks and processors.  In
addition, one need not develop, test, debug, and document
routines to manage memory, pass messages, or provide mutual
exclusion.  The developer is then able to concentrate solely on
the application.  By using standard software components, the
time and cost required to develop sophisticated real-time
applications is significantly reduced.

@section RTEMS Application Architecture

One important design goal of RTEMS was to provide a
bridge between two critical layers of typical real-time systems.
As shown in the following figure, RTEMS serves as a buffer between the
project dependent application code and the target hardware.
Most hardware dependencies for real-time applications can be
localized to the low level device drivers.

@float Figure,fig:RTEMS-App-Arch
@caption{RTEMS Application Architecture}

@ifset use-ascii
@example
@group
      +-----------------------------------------------------------+
      |             Application Dependent Software                |
      |        +----------------------------------------+         |
      |        |    Standard Application Components     |         |
      |        |                          +-------------+---+     |
      |    +---+-----------+              |                 |     |
      |    | Board Support |              |      RTEMS      |     |
      |    |    Package    |              |                 |     |
      +----+---------------+--------------+-----------------+-----|
      |                      Target Hardware                      |
      +-----------------------------------------------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule#&
\hbox to 0.50in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.50in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.50in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.50in{\enskip\hfil#\hfil}&
\vrule#\cr
\multispan{17}\hrulefill\cr
% to force all columns to desired width
& \enskip && \enskip && \enskip && \enskip &&
    \enskip && \enskip &&\enskip &&\enskip &\cr
% For debugging columns
%& \enskip 0&& \enskip 1&& \enskip 2&& \enskip 3&&
%    \enskip 4&& \enskip 5&&\enskip 6&&\enskip 7&\cr
\strut&\multispan{15}&\cr
&\multispan{15}\hfil Application Dependent Software\hfil&\cr
\strut&\multispan{15}&\cr
&\multispan{2}&&\multispan{8}\hrulefill &\multispan{2}&\cr
\strut&\multispan{2}&&&\multispan{7}&&\multispan{2}&&\cr
&\multispan{2}&&&\multispan{7}\hfil Standard Application Components\hfil&
    &\multispan{2}&&\cr
\strut&\multispan{2}&&&\multispan{7}&&\multispan{2}&&\cr
&&\multispan{5}\hrulefill&&\multispan{7}\hrulefill&&\cr
\strut&&&\multispan{3}                  &&&&\multispan{5}&&&\cr
&&&\multispan{3}\hfil Device\hfil&&&&\multispan{5}\hfil RTEMS\hfil&&&\cr
&&&\multispan{3}\hfil Drivers\hfil&&&&\multispan{5}&&&\cr
\strut&&&\multispan{3}                  &&&&\multispan{5}&&&\cr
\multispan{17}\hrulefill\cr
\strut&\multispan{15}&\cr
&\multispan{15}\hfil Target Hardware\hfil&\cr
\strut&\multispan{15}&\cr
\multispan{17}\hrulefill\cr
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<IMG SRC="rtemsarc.png" WIDTH=500 HEIGHT=300 ALT="RTEMS Application Architecture">
@end html
@end ifset
@end float

The RTEMS I/O interface manager provides an efficient tool for incorporating
these hardware dependencies into the system while simultaneously
providing a general mechanism to the application code that
accesses them.  A well designed real-time system can benefit
from this architecture by building a rich library of standard
application components which can be used repeatedly in other
real-time projects.

@section RTEMS Internal Architecture

RTEMS can be viewed as a set of layered components that work in
harmony to provide a set of services to a real-time application
system.  The executive interface presented to the application is
formed by grouping directives into logical sets called resource managers.
Functions utilized by multiple managers such as scheduling,
dispatching, and object management are provided in the executive
core.  The executive core depends on a small set of CPU dependent routines.
Together these components provide a powerful run time
environment that promotes the development of efficient real-time
application systems.  The following figure illustrates this organization:

@float Figure,fig:rtems-layers
@caption{RTEMS Layered Architecture}

@ifset use-ascii
@example
@group
           +-----------------------------------------------+
           |          RTEMS Executive Interface            |
           +-----------------------------------------------+
           |                 RTEMS Core                    |
           +-----------------------------------------------+
           |              CPU Dependent Code               |
           +-----------------------------------------------+
@end group
@end example
@end ifset

@ifset use-tex
@center{@image{rtemspie,4in,3in, RTEMS Layered Architecture}}
@tex
@end tex
@end ifset

@ifset use-html
@html
<IMG SRC="rtemspie.png" WIDTH=500 HEIGHT=300 ALT="RTEMS Layered Architecture">
@end html
@end ifset
@end float

Subsequent chapters present a detailed description of the capabilities
provided by each of the following RTEMS managers:

@itemize @bullet
@item initialization
@item task
@item interrupt
@item clock
@item timer
@item semaphore
@item message
@item event
@item signal
@item partition
@item region
@item dual ported memory
@item I/O
@item fatal error
@item rate monotonic
@item user extensions
@item multiprocessing
@end itemize

@section User Customization and Extensibility

As thirty-two bit microprocessors have decreased in
cost, they have become increasingly common in a variety of
embedded systems.  A wide range of custom and general-purpose
processor boards are based on various thirty-two bit processors.
RTEMS was designed to make no assumptions concerning the
characteristics of individual microprocessor families or of
specific support hardware.  In addition, RTEMS allows the system
developer a high degree of freedom in customizing and extending
its features.

RTEMS assumes the existence of a supported
microprocessor and sufficient memory for both RTEMS and the
real-time application.  Board dependent components such as
clocks, interrupt controllers, or I/O devices can be easily
integrated with RTEMS.  The customization and extensibility
features allow RTEMS to efficiently support as many environments
as possible.

@section Portability

The issue of portability was the major factor in the
creation of RTEMS.  Since RTEMS is designed to isolate the
hardware dependencies in the specific board support packages,
the real-time application should be easily ported to any other
processor.  The use of RTEMS allows the development of real-time
applications which can be completely independent of a particular
microprocessor architecture.

@section Memory Requirements

Since memory is a critical resource in many real-time
embedded systems, RTEMS was specifically designed to automatically
leave out all services that are not required from the run-time
environment.  Features such as networking, various fileystems,
and many other features are completely optional.  This allows
the application designer the flexibility to tailor RTEMS to most
efficiently meet system requirements while still satisfying even
the most stringent memory constraints.  As a result, the size
of the RTEMS executive is application dependent.

RTEMS requires RAM to manage each instance of an RTEMS object
that is created.  Thus the more RTEMS objects an application
needs, the more memory that must be reserved.  See
@ref{Configuring a System Determining Memory Requirements} for
more details.

RTEMS utilizes memory for both code and data space.
Although RTEMS' data space must be in RAM, its code space can be
located in either ROM or RAM.

@section Audience

This manual was written for experienced real-time
software developers.  Although some background is provided, it
is assumed that the reader is familiar with the concepts of task
management as well as intertask communication and
synchronization.  Since directives, user related data
structures, and examples are presented in @value{LANGUAGE}, a basic
understanding of the @value{LANGUAGE} programming language
is required to fully
understand the material presented.  However, because of the
similarity of the Ada and C RTEMS implementations, users will
find that the use and behavior of the two implementations is
very similar.  A working knowledge of the target processor is
helpful in understanding some of RTEMS' features.  A thorough
understanding of the executive cannot be obtained without
studying the entire manual because many of RTEMS' concepts and
features are interrelated.  Experienced RTEMS users will find
that the manual organization facilitates its use as a reference
document.

@section Conventions

The following conventions are used in this manual:

@itemize @bullet
@item Significant words or phrases as well as all directive
names are printed in bold type.

@item Items in bold capital letters are constants defined by
RTEMS.  Each language interface provided by RTEMS includes a
file containing the standard set of constants, data types, and
@value{STRUCTURE} definitions which can be incorporated into the user
application.

@item A number of type definitions are provided by RTEMS and
can be found in rtems.h.

@item The characters "0x" preceding a number indicates that
the number is in hexadecimal format.  Any other numbers are
assumed to be in decimal format.
@end itemize

@section Manual Organization

This first chapter has presented the introductory and
background material for the RTEMS executive.  The remaining
chapters of this manual present a detailed description of RTEMS
and the environment, including run time behavior, it creates for
the user.

A chapter is dedicated to each manager and provides a
detailed discussion of each RTEMS manager and the directives
which it provides.  The presentation format for each directive
includes the following sections:

@itemize @bullet
@item Calling sequence
@item Directive status codes
@item Description
@item Notes
@end itemize

The following provides an overview of the remainder
of this manual:

@table @asis
@item Chapter 2
Key Concepts: presents an
introduction to the ideas which are common across multiple RTEMS
managers.

@item Chapter 3:
RTEMS Data Types: describes the fundamental data types shared
by the services in the RTEMS Classic API.

@item Chapter 4:
Initialization Manager: describes the functionality and directives
provided by the Initialization Manager.

@item Chapter 5:
Task Manager: describes the functionality and directives provided
by the Task Manager.

@item Chapter 6:
Interrupt Manager: describes the functionality and directives
provided by the Interrupt Manager.

@item Chapter 7:
Clock Manager: describes the functionality and directives
provided by the Clock Manager.

@item Chapter 8:
Timer Manager: describes the functionality and directives provided
by the Timer Manager.

@item Chapter 9:
Semaphore Manager: describes the functionality and directives
provided by the Semaphore Manager.

@item Chapter 10:
Message Manager: describes the functionality and directives
provided by the Message Manager.

@item Chapter 11:
Event Manager: describes the
functionality and directives provided by the Event Manager.

@item Chapter 12:
Signal Manager: describes the
functionality and directives provided by the Signal Manager.

@item Chapter 13:
Partition Manager: describes the
functionality and directives provided by the Partition Manager.

@item Chapter 14:
Region Manager: describes the
functionality and directives provided by the Region Manager.

@item Chapter 15:
Dual-Ported Memory Manager: describes
the functionality and directives provided by the Dual-Ported
Memory Manager.

@item Chapter 16:
I/O Manager: describes the
functionality and directives provided by the I/O Manager.

@item Chapter 17:
Fatal Error Manager: describes the functionality and directives
provided by the Fatal Error Manager.

@item Chapter 18:
Scheduling Concepts: details the RTEMS scheduling algorithm and
task state transitions.

@item Chapter 19:
Rate Monotonic Manager: describes the functionality and directives
provided by the Rate Monotonic Manager.

@item Chapter 20:
Board Support Packages: defines the
functionality required of user-supplied board support packages.

@item Chapter 21:
User Extensions: shows the user how to
extend RTEMS to incorporate custom features.

@item Chapter 22:
Configuring a System: details the process by which one tailors RTEMS
for a particular single-processor or multiprocessor application.

@item Chapter 23:
Multiprocessing Manager: presents a
conceptual overview of the multiprocessing capabilities provided
by RTEMS as well as describing the Multiprocessing
Communications Interface Layer and Multiprocessing Manager
directives.

@item Chapter 24:
Directive Status Codes: provides a definition of each of the
directive status codes referenced in this manual.

@item Chapter 25:
Example Application: provides a template for simple RTEMS applications.

@item Chapter 26:
Glossary: defines terms used throughout this manual.

@end table


