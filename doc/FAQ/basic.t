@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Basic Information

The questions in this category are basic questions about RTEMS.  Where
did it come from, why is it, what is it, when should you use it, etc.?

@section What does RTEMS stand for?

RTEMS is an anacronym for the Real-Time Executive for Multiprocessor
Systems.

Initially RTEMS stood for the Real-Time Executive for Missile Systems
but as it became clear that the application domains that could use
RTEMS extended far beyond missiles, the "M" changed to mean Military.
At one point, there were both Ada and C implementations of RTEMS.  The
C version changed the "M" to mean Multiprocessor while the Ada version
remained with the "M" meaning Military.

@section What is RTEMS?

RTEMS is a real-time executive which provides a high performance
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

XXX This feature list is weak these days. :)

@section What standards does RTEMS support?

The original "Classic" RTEMS API is based on the Real-Time Executive
Interface Definition (RTEID) and the Open Real-Time Kernel Interface
Definition (ORKID).  RTEMS also includes support for POSIX threads
and real-time extensions.

With the addition of file system infrastructure, RTEMS supports
about 70% of the POSIX 1003.1b-1996 standard.  This standard
defines the programming interfaces of standard UNIX.   This means
that much source code that works on UNIX, also works on RTEMS.

@section What processors is RTEMS available for?

RTEMS is available for the following processor families:

@itemize @bullet
@item Motorola MC68xxx
@item Motorola MC683xx
@item Motorola ColdFire
@item Hitachi SH
@item Intel i386
@item Intel i960
@item MIPS
@item PowerPC
@item SPARC
@item AMD A29K
@item Hewlett-Packard PA-RISC
@end itemize

In addition, there is a port to UNIX which can be used as a prototyping
and simulation environment.

@section Executive vs. Kernel vs. Operating System (RTOS)

The developers of RTEMS developers use the terms executive and kernel
interchangeably.  In the embedded system community, the terms executive
or kernel are generally used to refer to small operating systems.
So we consider it proper to refer to RTEMS as an executive, a kernel,
or an operating system.

@section Where/why was it developed?

RTEMS was developed by On-Line Applications Research Corporation (OAR)
for the U.S. Army Missile Command.  The original goal of RTEMS was
to provide a portable, standards-based real-time executive for which
source code was available and royalties were paid.

In other words, RTEMS was open source before open source was cool.

@section Are there no similar commercial products?

Yes, but not all are based on standards and the open source philosophy.

@section How can I get RTEMS?

RTEMS is distributed by OAR Corporation via the Internet.  Point your
favorite browser at the following URL and following the link:

@set OAR-URL http://www.OARcorp.com
@ifset use-html
@href{@value{OAR-URL},,@value{OAR-URL}}
@end ifset
@ifclear use-html
@value{OAR-URL}
@end ifclear

@section What about support?

OAR Corporation provides numerous services for RTEMS including support,
training, and custom development.

@section Are there any mailing lists?

The primary RTEMS mailing list is @code{rtems-list@@oarcorp.com}.  You 
can subscribe by sending a message with the word @b{subscribe} as the 
body to @code{rtems-list-request@@oarcorp.com}.

@section Are there any license restrictions?

RTEMS is licensed under a modified version of the GNU General Public License
(GPL).  The modification places no restrictions on the applications which
use RTEMS but protects the interests of those who work on RTEMS.

The TCP/IP network stack included with RTEMS is a port of the FreeBSD
network stack and is licensed under different terms that also do not
place restrictions on the application.

@section Are there any export restrictions?

No.


