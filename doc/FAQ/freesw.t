@c
@c  COPYRIGHT (c) 1988-1998.
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

@item gcc/egcs
@item binutils
@item gdb

@end itemize

Although not from the Free Software Foundation, the Cygnus newlib C
library integrates well with the GNU tools and is a standard part of the
RTEMS development environment.

@subsection GNU Ada

For those interested in using the Ada95 programming language, the GNU Ada
compiler (GNAT) is available and has excellent support for RTEMS.

@subsection DDD

By far the easiest way to use DDD if you are on a Redhat Linux system is
to retrieve the latest RPM package. I got mine from
ftp://ftp.crc.ca/systems/linux/redhat/contrib.redhat.com/libc6/i386/ddd-dynamic-
3.1.5-1.i386.rpm.
This particular one is for Redhat 6.0 on i386. For 5.2, I guess you
should use libc5 instead of libc6.

You also need Lesstif 0.88. I got mine from
ftp://ftp.hungry.com/pub/hungry/lesstif/bindist/lesstif-0.88.1-1.i386.rpm

Installed as RPMs, DDD and Lesstif work out-of-the-box.

Both Lesstif and DDD can be built from scratch without any problems.

The isntruction on how to install Lesstif are at
http://www.cs.tu-bs.de/softech/ddd/.  They indicate that

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

The student configure DDD --with-motif-libraries=/usr/local/lib
--with-motif-includes=/usr/local/include

You will need gnuplot 3.7. I got mine from
ftp://ftp.dartmouth.edu/pub/gnuplot/gnuplot-3.7.tar.gz. Build and
install from scratch.

I start DDD from a script in $HOME/bin:

@example
#!/bin/bash
ddd --debugger m68k-elf-gdb $1
@end example


Now for the hard part:

under RH Linux, to get gdb to use the serial ports while running as a
mere mortal, edit /etc/security/console.perms, and create a <serial>
class (call it whatever you want).

@example
<serial>=/dev/ttyS* /dev/cua*
@end example

Now enable the change of ownership of these devices when users log in
from the console:

@example
<console> 0600 <serial> 0600 root
@end example

And voila!

I use minicom to communicate with the target to initiate a TFTP
download. I then suspend minicom,  lauch DDD, and begin debugging.

The procedure should be the same on other platforms, modulo the choice
of terminal emulator program and the scheme used to access the serial
ports. From the cygwin mailing list, I have the impression that GDB has
some problems communicating over serial lines on that platform.

P.S. GDB doesn't like getting lots of input from the program under test
over the serial line. Actually, it doesn't care, but it looses
characters. It would appear that flow control is not re-enabled when it
resumes program execution. At times, it looked like the test were
failing, but everything was OK. We modified the MVME167 serial driver to
send test output to another serial port.

Information provided by Charles-Antoine Gauthier (charles.gauthier@@iit.nrc.ca)
and Jiri Gaisler (jgais@@ws.estec.esa.nl).


@section omniORB

omniORB is a GPL'ed CORBA which has been ported to RTEMS.  It is
available from 
@ifset use-html
(@href{http://www.uk.research.att.com/omniORB/omniORB.html,,,http://www.uk.research.att.com/omniORB/omniORB.html})
@end ifset
@ifclear use-html
http://www.uk.research.att.com/omniORB/omniORB.html
@end ifclear
.

For information on the RTEMS port of omniORB to RTEMS, see the following
URL 
@ifset use-html
(@href{http://www.connecttel.com/corba/rtems_omni.html,,,http://www.connecttel.com/corba/rtems_omni.html})
@end ifset
@ifclear use-html
http://www.connecttel.com/corba/rtems_omni.html
@end ifclear
.

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

