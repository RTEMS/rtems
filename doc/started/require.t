@c
@c  COPYRIGHT (c) 1988-1999.
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
+------------------------------------+--------------------------+
|              Component             |   Disk Space Required    |
+------------------------------------+--------------------------+
|        archive directory           |        35 Mbytes         |
|        tools src unarchived        |       150 Mbytes         |
|  each individual build directory   |       300 Mbytes         |
|     each installation directory    |      20-200 Mbytes       |
+------------------------------------+--------------------------+
@end example

It is important to understand that the above requirements only address
the GNU C/C++ Cross Compiler Tools themselves.  Adding additional
languages such as Fortran or Objective-C can increase the size
of the build and installation directories.  Also, the unarchived
source and build directories can be removed after the tools are
installed.

After the tools themselves are installed, RTEMS must be built
and installed for each Board Support Package that you wish
to use.  Thus the precise amount of disk space required
for each installation directory depends highly on the number
of RTEMS BSPs which are to be installed.  If a single BSP is
installed, then the additional size of each install directory
will tend to be in the 40-60 Mbyte range.

There are a number of factors which must be taken into
account in oreder to estimate the amount of disk space required
to build RTEMS itself.  Attempting to build multiple BSPs in
a single step increases the disk space requirements.  Similarly
enabling optional features increases the build and install
space requirements.  In particular, enabling and building
the RTEMS tests results in a significant increase in build
space requirements but since the test are not installed has
no impact on installation requirements.

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

In order to build egcs 1.1b, gcc 2.9.x, or newer versions, the GNU
@code{makeinfo} program
installed on your system must be at least version 1.68.  The appropriate
version of @code{makeinfo} is distributed with @code{gcc}.

The following demonstrates how to determine the version of @code{makeinfo}
on your machine:

@example
makeinfo --version
@end example

