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

XXX insert information from Charles Gauthier and Jiri Gaisler

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

