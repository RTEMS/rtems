@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Basic Information

The questions in this category are basic questions about RTEMS.  Where
did it come from, why is it, what is it, when should you use it, etc.?

@section What does RTEMS stand for?

RTEMS is an an acronym for the Real-Time Executive for Multiprocessor
Systems.

Initially RTEMS stood for the Real-Time Executive for Missile Systems
but as it became clear that the application domains that could use
RTEMS extended far beyond missiles, the "M" changed to mean Military.
At one point, there were both Ada and C implementations of RTEMS.  The
C version changed the "M" to mean Multiprocessor while the Ada version
remained with the "M" meaning Military.

@section What is RTEMS?

RTEMS is a real-time executive which provides a high performance
environment for embedded military applications including many
features. The following is just a short list of the features
available in RTEMS.  If you are interested in something that
is not on this list, please contact the RTEMS Team.  Features
are being added continuously.

@itemize @bullet

@item Standards Compliant
@itemize @bullet
@item POSIX 1003.1b API including threads
@item RTEID/ORKID based Classic API
@end itemize

@item TCP/IP Stack
@itemize @bullet
@item high performance port of FreeBSD TCP/IP stack
@item UDP, TCP
@item ICMP, DHCP, RARP
@item TFTP
@item RPC
@item FTPD
@item HTTPD
@item CORBA
@end itemize

@item Debugging
@itemize @bullet
@item GNU debugger (gdb)
@item DDD GUI interface to GDB
@item thread aware
@item debug over Ethernet 
@item debug over Serial Port
@end itemize

@item Filesystem Support
@itemize @bullet
@item In-Memory Filesystem (IMFS)
@item TFTP Client Filesystem
@end itemize

@item Basic Kernel Features
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

@end itemize


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
@item ARM
@item Hitachi H8/300
@item Hitachi SH
@item Intel i386
@item Intel i960
@item MIPS
@item PowerPC
@item SPARC
@item Hewlett-Packard PA-RISC
@item Texas Instruments C3x/C4x
@item OpenCores OR32
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
for the U.S. Army Missile Command prior to that organizations merger
with the Aviation Command that resulted in the new command, U. S. Army
Aviation and Missile command (AMCOM).  The original goal of RTEMS was
to provide a portable, standards-based real-time executive for which
source code was available and royalties were paid.

In other words, RTEMS was open source before open source was cool.

@section Are there no similar commercial products?

Yes, but not all are based on standards and the open source philosophy.

@section How can I get RTEMS?

RTEMS is distributed by OAR Corporation via the Internet.  Point your
favorite browser at the following URL and following the link:

@c this version should work
@c @set OAR-URL http://www.OARcorp.com
@c @uref{@value{OAR-URL},@value{OAR-URL}}
@uref{http://www.OARcorp.com,http://www.OARcorp.com}

@section What about support?

OAR Corporation provides numerous services for RTEMS including support,
training, and custom development.

@section Are there any mailing lists?

The primary RTEMS mailing list is @code{rtems-users@@oarcorp.com}.  This
list is for general RTEMS discussions, questions, design help, advice,
etc..  Subscribe by sending an empty mail
message to @code{rtems-users-subscribe@@OARcorp.com}.  This
mailing list is archived at:

@example
http://www.oarcorp.com/rtems/maillistArchives/rtems-users
@end example

The @code{rtems-snapshots@@oarcorp.com} mailing list is for those
interested in taking a more active role in the design, development,
and maintenance of RTEMS.  Discussions on this list tend to focus
on problems in the development source, design of new features, problem
reports, etc..  Subscribe by sending an empty mail
message to @code{rtems-snapshots-subscribe@@OARcorp.com}.
mailing list is archived at:

@example
http://www.oarcorp.com/rtems/maillistArchives/rtems-snapshots
@end example

The archives for both mailing lists include discussions back 
to 1997.

@section Are there any license restrictions?

RTEMS is licensed under a modified version of the GNU General Public License
(GPL).  The modification places no restrictions on the applications which
use RTEMS but protects the interests of those who work on RTEMS.

The TCP/IP network stack included with RTEMS is a port of the FreeBSD
network stack and is licensed under different terms that also do not
place restrictions on the application.

@section Are there any export restrictions?

No.


