@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Requirements

A fairly large amount of disk space is required to perform the build of the
GNU C/C++ Cross Compiler Tools for RTEMS. The following table may help in
assessing the amount of disk space required for your installation: 

@example
+----------------------------------------+------------------------------+
|                Component               |     Disk Space Required      |
+----------------------------------------+------------------------------+
|          archive directory             |          30 Mbytes           |
|          tools src unzipped            |         100 Mbytes           |
|    each individual build directory     |    300 Mbytes worst case     |
|       each installation directory      |        20-400 Mbytes         |
+----------------------------------------+------------------------------+
@end example

The disk space required for each installation directory depends 
primarily on the number of RTEMS BSPs which are to be installed.
If a single BSP is installed, then the size of each install directory 
will tend to be in the 40-60 Mbyte range.

The instructions in this manual should work on any computer running
a UNIX variant.  Some native GNU tools are used by this procedure
including:

@itemize @bullet
@item GCC
@item GNU make
@item GNU makeinfo
@end itemize

In addition, some native utilities may be deficient for building 
the GNU tools.

@section GNU makeinfo Version Requirements

In order to build egcs 1.1b or newer, the GNU @code{makeinfo} program
installed on your system must be at least version 1.68.  The appropriate
version of @code{makeinfo} is distributed with egcs 1.1b.

The following demonstrates how to determine the version of @code{makeinfo}
on your machine:

@example
makeinfo --version
@end example

