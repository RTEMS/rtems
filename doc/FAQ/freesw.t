@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Free Software that Works with RTEMS

This section describes other free software packages that are known to work
with RTEMS.

@section Development Tools

@subsection Basic Development Environment

The standard RTEMS development environment consists of the following GNU
components:

@itemize @bullet

@item gcc
@item binutils
@item gdb

@end itemize

Although not from the Free Software Foundation, the Cygnus newlib C
library integrates well with the GNU tools and is a standard part of the
RTEMS development environment.

@subsection GNU Ada

For those interested in using the Ada95 programming language, the GNU Ada
compiler (GNAT) is available and has excellent support for RTEMS.

@subsection DDD - Data Display Debugger

By far the easiest way to use DDD if you are on a Redhat or SuSE Linux system 
is to retrieve the RPM package for your OS version.  In general, it is 
easier to install a static binary since doing so avoids all problems
with dynamic library versions.

Some versions of DDD have had trouble with Lesstif.  If you
are using Lesstif, you will need version 0.88 or newer.  It
is also available as an RPM at the popular sites.  Another Motif
clone is Motive and versions 1.2 and newer known to work with DDD
on popular distributions of Linux including RedHat and Slackware.

Installed as RPMs, DDD in conjunction with either Lesstif or Motive
should work out-of-the-box.

User comments indicate that both Lesstif and DDD can be built
from scratch without any problems.  Instructions on installing DDD
are at @uref{http://www.cs.tu-bs.de/softech/ddd/}.  They
indicate that

@itemize @bullet
LessTif should be used in (default) Motif 1.2 compatibility mode.

The Motif 2.0 compatibility mode of LessTif is still incomplete.
@end itemize

So configure lesstif with --enable-default-12.

The configure script is broken (see  www.lesstif.org --> known problems)
for 0.88.1. I didn't fix the script as they show, so I just have links
in /usr/local/lib (also shown).

Watch out: Lesstif installs its libraries in /usr/local/Lesstif. You
will need to update /etc/ld.so.conf and regenerate the cache of shared
library paths to point to the Motif 1.2 library.

The following notes are from an RTEMS user who uses DDD in conjunction
with Lesstif.  Configure DDD "--with-motif-libraries=/usr/local/lib
--with-motif-includes=/usr/local/include" DDD needs gnuplot 3.7.  
@uref{ftp://ftp.dartmouth.edu/pub/gnuplot/gnuplot-3.7.tar.gz}. Build and
install from scratch.  

DDD can be started from a script that specifies the cross debugger.
This simplifies the invocation.  The following example shows what
a script doing this looks like.

@example
#!/bin/bash
ddd --debugger m68k-elf-gdb $1
@end example

Under many flavors of UNIX, you will likely have to relax permissions.

On Linux, to get gdb to use the serial ports while running as a
normal user, edit /etc/security/console.perms, and create a <serial>
class (call it whatever you want).

@example
<serial>=/dev/ttyS* /dev/cua*
@end example

Now enable the change of ownership of these devices when users log in
from the console:

@example
<console> 0600 <serial> 0600 root
@end example

Users report using minicom to communicate with the target to initiate a TFTP
download. They then suspend minicom, launch DDD, and begin debugging.

The procedure should be the same on other platforms, modulo the choice
of terminal emulator program and the scheme used to access the serial
ports. From problem reports on the cygwin mailing list, it appears that
GDB has some problems communicating over serial lines on that platform.

NOTE: GDB does not like getting lots of input from the program under test
over the serial line. Actually, it does not care, but it looses
characters. It would appear that flow control is not re-enabled when it
resumes program execution. At times, it looked like the test were
failing, but everything was OK. We modified the MVME167 serial driver to
send test output to another serial port.  Using two serial ports is
usually the easiest way to get test output while retaining a reliable debug
connection regardless of the debugger/target combination.

NOTE: Enabling gdb's remote cache might prevent this (Observed with SH1
boards, but may also be valid for targets):
@example
gdb > set remotecache
@end example

Information provided by Charles-Antoine Gauthier (charles.gauthier@@iit.nrc.ca)
Jiri Gaisler (jgais@@ws.estec.esa.nl) and Ralf Cors@'epius
(corsepiu@@faw.uni-ulm.de)


@section omniORB

omniORB is a GPL'ed CORBA which has been ported to RTEMS.  It is
available from 
(@uref{http://www.uk.research.att.com/omniORB/omniORB.html,http://www.uk.research.att.com/omniORB/omniORB.html})
.

For information on the RTEMS port of omniORB to RTEMS, see the following
URL 
(@uref{http://www.connecttel.com/corba/rtems_omni.html,http://www.connecttel.com/corba/rtems_omni.html}).

C++ exceptions must work properly on your target for omniORB to work.

The port of omniORB to RTEMS was done by Rosimildo DaSilva
<rdasilva@@connecttel.com>.  

@section TCL

Tool Command Language.

ditto

@section ncurses

Free version of curses.

ditto


@section zlib

Free compression/decompression library.

ditto

