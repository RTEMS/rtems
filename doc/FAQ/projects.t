@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter RTEMS Projects

The questions in this category are regarding things that people
are working on or that the RTEMS community would like to see work on. 

There are multiple ways to support the RTEMS Project.  Financial support
is always welcomed.  This can be via sponsorship of a specific project
such as one of the ones listed here or via volunteering in some 
capacity.

@section Other Filesystems

This is a list of the filesystems that would be nice to have support
for in RTEMS.  For each filsystem, status and contact information is
provided for those who have expressed interest in helping out or are actively
working on it.

@itemize @bullet
@item TFTP client - read only, no write capability
@itemize @bullet
@item No Contact
@end itemize

@item DOS Filesystem - ???
@itemize @bullet
@item Peter Shoebridge <peter@@zeecube.com>
@item Victor V. Vengerov <vvv@@tepkom.ru>
@end itemize

@item CD-ROM Filesystem - ???
@itemize @bullet
@item Peter Shoebridge <peter@@zeecube.com>
@end itemize

@item Flash Filesystem(s) - ???
@itemize @bullet
@item Rod Barman <rodb@@ieee.org>
@item Victor V. Vengerov <vvv@@tepkom.ru>
@end itemize

@item Remote Host Filesystem - ???
@itemize @bullet
@item Wayne Bullaughey <wayne@@wmi.com>
@end itemize

@item NFS client - ???
@itemize @bullet
@item No Contact
@end itemize

@end itemize

@section Java

@subsection Kaffe

This porting effort is underway and active.

@itemize @bullet
@item Jiri Gaisler <jgais@@ws.estec.esa.nl>
@item Oscar Martinez de la Torre <omt@@wm.estec.esa.nl>
@end itemize

NOTE: An older version of Kaffe was ported to a pre-4.0 version of RTEMS.
The network support in RTEMS was immature at that port and Kaffe had
portability problems.  Together these resulted in an unclean port which
was never merged.

@subsection GNU Java Compiler (gjc)

This porting effort is underway and active.

@itemize @bullet
@item Charles-Antoine Gauthier <charles.gauthier@@iit.nrc.ca>
@end itemize

@section CORBA

@subsection TAO 

This porting effort is pending testing.  Erik ported the code but then
discovered that his target board did not have enough memory to run 
any TAO tests.

@itemize @bullet
@item Erik Ivanenko <erik.ivanenko@@utoronto.ca>
@end itemize

@section APIs

@subsection POSIX 1003.1b

Support for POSIX 1003.1b is mature but there are a few remaining
items including a handful of services, performance tests, and
documentation.  Please refer to the Status chapter of the 
@i{POSIX API User's Guide} for more details.

@subsection ITRON 3.0

Support for ITRON 3.0 is in its beginning stages.  There are
numerous managers left to implement, both functional and
performance tests to write, and much documentation remaining.
Please refer to the Status chapter of the @i{ITRON 3.0 API User's Guide}
for specific details.

