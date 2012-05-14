@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Introduction

The purpose of this document is to guide you through the process of 
installing a GNU cross development environment to use with RTEMS.

If you are already familiar with the concepts behind a cross compiler and
have a background in Unix, these instructions should provide the bare
essentials for performing a setup of the following items: 

@itemize @bullet
@item GNU Cross Compilation Tools for RTEMS on your build-host system
@item RTEMS OS for the target
@item GNU Debugger (GDB)
@end itemize

The remainder of this chapter provides background information on real-time
embedded systems and cross development and an overview of other 
resources of interest on the Internet.  If you are not familiar with 
real-time embedded systems or the other areas, please read those sections.  
These sections will help familiarize you with the
types of systems RTEMS is designed to be used in and the cross development
process used when developing RTEMS applications.

@section Real-Time Embedded Systems

Real-time embedded systems are found in practically every facet of our
everyday lives.  Today's systems range from the common telephone, automobile
control systems, and kitchen appliances to complex air traffic control
systems, military weapon systems, and production line control including
robotics and automation. However, in the current climate of rapidly changing
technology, it is difficult to reach a consensus on the definition of a
real-time embedded system. Hardware costs are continuing to rapidly decline 
while at the same time the hardware is increasing in power and functionality.
As a result, embedded systems that were not considered viable two years ago
are suddenly a cost effective solution. In this domain, it is not uncommon
for a single hardware configuration to employ a variety of architectures and
technologies. Therefore, we shall define an embedded system as any computer
system that is built into a larger system consisting of multiple technologies
such as digital and analog electronics,  mechanical devices, and sensors.

Even as hardware platforms become more powerful, most embedded systems are
critically dependent on the real-time software embedded in the systems
themselves.  Regardless of how efficiently the hardware operates, the
performance of the embedded real-time software determines the success of the
system.  As the complexity of the embedded hardware platform grows, so does
the size and complexity of the embedded software. Software systems must
routinely perform activities which were only dreamed of a short time ago.
These large, complex, real-time embedded applications now commonly contain
one million lines of code or more.

Real-time embedded systems have a complex set of characteristics that
distinguish them from other software applications.  Real-time embedded
systems are driven by and must respond to real world events while adhering to
rigorous requirements imposed by the environment with which they interact. 
The correctness of the system depends not only on the results of
computations, but also on the time at which the results are produced.  The
most important and complex characteristic of real-time application systems is
that they must receive and respond to a set of external stimuli within rigid
and critical time constraints. 

A single real-time application can be composed of both soft and hard
real-time components. A typical example of a hard real-time system is a
nuclear reactor control system that must not only detect failures, but must
also respond quickly enough to prevent a meltdown. This application also has
soft real-time requirements because it may involve a man-machine interface.
Providing an interactive input to the control system is not as critical as
setting off an alarm to indicate a failure condition. However, the
interactive system component must respond within an acceptable time limit to
allow the operator to interact efficiently with the control system. 

@section Cross Development

Today almost all real-time embedded software systems are developed in a 
@b{cross development} environment using cross development tools. In the cross
development environment, software development activities are typically
performed on one computer system, the @b{build-host} system, while the result of the
development effort (produced by the cross tools) is a software system that
executes on the @b{target} platform. The requirements for the target platform are
usually incompatible and quite often in direct conflict with the requirements
for the build-host.  Moreover, the target hardware is often custom designed for a
particular project.  This means that the cross development toolset must allow
the developer to customize the tools to address target specific run-time
issues.  The toolset must have provisions for board dependent initialization
code, device drivers, and error handling code. 

The build-host computer is optimized to support the code development cycle with
support for code editors, compilers, and linkers requiring large disk drives,
user development windows, and multiple developer connections.  Thus the build-host
computer is typically a traditional UNIX workstation such as those available
from SUN or Silicon Graphics, or a PC running either a version of MS-Windows
or UNIX.  The build-host system may also be required to execute
office productivity applications to allow the software developer
to write  documentation, make presentations, or track the project's
progress using a project management tool.  This necessitates that the
build-host computer be general purpose with resources such as a
thirty-two or sixty-four bit processor, large amounts of
RAM, a  monitor, mouse, keyboard, hard and floppy disk drives, CD-ROM drive,
and a graphics card.  It is likely that the system will be multimedia capable
and have some networking capability. 

Conversely, the target platform generally has limited traditional computer
resources.  The hardware is designed for the particular functionality and
requirements of the embedded system and optimized to perform those tasks
effectively.  Instead of hard drives and keyboards, it is composed of
sensors, relays, and stepper motors. The per-unit cost of the target platform
is typically a critical concern.  No hardware component is included without
being cost justified.  As a result, the processor of the target system is
often from a different processor family than that of the build-host system and
usually has lower performance.  In addition to the processor families
designed only for use in embedded systems, there are versions of nearly every
general-purpose processor specifically tailored for real-time embedded
systems.  For example, many of the processors targeting the embedded market
do not include hardware floating point units, but do include peripherals such
as timers, serial controllers, or network interfaces. 

@section Resources on the Internet

This section describes various resources on the Internet which are of
use to RTEMS users.

@c
@c  Online Tool Documentation
@c

@subsection Online Tool Documentation

Each of the tools in the GNU development suite comes with documentation.
It is in the reader's and tool maintainers' interest that one read the
documentation before posting a problem to a mailing list or news group.
The RTEMS Project provides formatted documentation for the primary
tools in the cross development toolset including BINUTILS, GCC,
NEWLIB, and GDB with the pre-built versions of those tools.

Much of the documentation is available at other sites on the Internet.
The following is a list of URLs where one can find HTML versions of
the GNU manuals:

@table @b

@item Free Software Foundation
@uref{http://www.gnu.org/manual/manual.html,
http://www.gnu.org/manual/manual.html}

@item Delorie Software
@uref{http://www.delorie.com/gnu/docs, http://www.delorie.com/gnu/docs}

@end table


@subsection RTEMS Mailing List

@uref{mailto:@value{RTEMSUSERS},@value{RTEMSUSERS}}

This is the primary  mailing list for the discussion of issues
related to RTEMS, including GNAT/RTEMS.  If you have questions
about RTEMS, wish to make suggestions, track development efforts,
or just want to pick up hints, this is a good list to monitor.
If you would like to browse the thousands of messages in the fifteen
year archive of the mailing list or subscribe to it, please visit
@uref{http://www.rtems.org/mailman,http://www.rtems.org/mailman} for
more information,

@subsection GCC Mailing Lists

The GCC Project is hosted at @uref{http://gcc.gnu.org,http://gcc.gnu.org}.
They maintain multiple mailing lists that are described at the web site
along with subscription information.
