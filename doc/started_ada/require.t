@c
@c  COPYRIGHT (c) 1988-2002.
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
|        archive directory           |        40 Mbytes         |
|        tools src unarchived        |       200 Mbytes         |
|  each individual build directory   |     up to 500 Mbytes     |
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
@item GNAT
@item GNU make
@end itemize

In addition, some native utilities may be deficient for building 
the GNU tools.

@section Native GNAT

The native GNAT must be installed in the default location or built
from source.  No GCC or GNAT environment variables should be set during
the build or use of the cross GNAT/RTEMS toolset as this could result in
an unpredictable mix of native and cross toolsets.

Binaries for native GNAT installations are available at the primary
GNAT ftp site (@value{GNAT-FTP}.   Installation instructions are
included with the binary GNAT distributions.  The binary installation
should be installed in the default location or installed in a 
non-default location and used ONLY to build a native GNAT from source.
This final native GNAT will be used to build the GNAT/RTEMS cross
development toolset.

@subsection Verifying Correct Operation of Native GNAT

It is imperative that the native GNAT installation work correctly for 
the installation of GNAT/RTEMS to succeed.  It is recommended that the
user verify that the native GNAT is installed correctly by performing
these tests:

@subsubsection Native Hello World Test

Place the following Ada source code in hello.adb:

@example
with Text_IO; use Text_IO;

procedure Hello is
begin
   Put_Line ( "Hello World");
end Hello;
@end example

Use the following command sequence to ompile and execute the above program:

@example
gnatmake hello
./hello
@end example

If the message @code{Hello World} is printed, then the native installation
of GNAT operates well enough to proceed.

@subsubsection Insure GCC and GNAT Environment Variables Are Not Set

If any of the following commands produce output, then you have
environment variables overriding the default behavior of the
native GNAT toolset.  These variables will conflict with the cross
toolset.  Please resolve this problem before proceeding further.

@example
echo $GCC_EXEC_PREFIX
echo $ADA_INCLUDE_PATH
echo $ADA_OBJECTS_PATH
echo $LD_RUN_PATH
echo $C_INCLUDE_PATH
@end example
