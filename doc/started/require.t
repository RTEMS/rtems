@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Requirements

This chapter describes the build-host system requirements and initial steps 
in installing the GNU C/C++ Cross Compiler Tools and RTEMS on a build-host.

@section Disk Space

A fairly large amount of disk space is required to perform the build of the
GNU C/C++ Cross Compiler Tools for RTEMS. The following table may help in
assessing the amount of disk space required for your installation: 

@example
+------------------------------------+--------------------------+
|              Component             |   Disk Space Required    |
+------------------------------------+--------------------------+
|        archive directory           |        35 Mbytes         |
|        tools src unarchived        |       150 Mbytes         |
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
account in order to estimate the amount of disk space required
to build RTEMS itself.  Attempting to build multiple BSPs in
a single step increases the disk space requirements.  Similarly
enabling optional features increases the build and install
space requirements.  In particular, enabling and building
the RTEMS tests results in a significant increase in build
space requirements but since the tests are not installed has,
enabling them has no impact on installation requirements.

@section General Host Software Requirements

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

@subsection GCC

Although RTEMS itself is intended to execute on an embedded target,
there is source code for some native programs included with the RTEMS
distribution.  Some of these programs are used to assist in the building
of RTEMS itself, while others are BSP specific tools.  Regardless,
no attempt has been made to compile these programs with a non-GNU
compiler.

@subsection GNU Make

Both NEWLIB and RTEMS use GNU make specific features and can only be built
using GNU make.  Many systems include a make utility that is not GNU make.
The safest way to meet this requirement is to ensure that when you invoke
the command @code{make}, it is GNU make.  This can be verified by 
attempting to print the GNU make version information:

@example
make --version
@end example

If you have GNU make and another make on your system, it is common to put
the directory containing GNU make before the directory containing other 
implementations of make.

@subsection GNU makeinfo Version Requirements

In order to build gcc 2.9.x or newer versions, the GNU @code{makeinfo} program
installed on your system must be at least version 1.68.  The appropriate
version of @code{makeinfo} is distributed with @code{gcc}.

The following demonstrates how to determine the version of @code{makeinfo}
on your machine:

@example
makeinfo --version
@end example

@c
@c  Host Specific Notes
@c

@section Host Specific Notes

@subsection Solaris 2.x

The following problems have been reported by Solaris 2.x users:

@itemize @bullet

@item The build scripts are written in "shell".  The program @code{/bin/sh}
on Solaris 2.x is not robust enough to execute these scripts.  If you
are on a Solaris 2.x host, then change the first line of the files
@code{bit}, @code{bit_gdb}, and @code{bit_rtems} to use the
@code{/bin/ksh} shell instead.

@item The native @code{patch} program is broken.  Install the GNU version.

@item The native @code{m4} program is deficient.  Install the GNU version.

@end itemize

@subsection Linux

The following problems have been reported by Linux users:

@itemize @bullet

@item Certain versions of GNU fileutils include a version of
@code{install} which does not work properly.  Please perform
the following test to see if you need to upgrade:

@example
install -c -d /tmp/foo/bar
@end example

If this does not create the specified directories your install
program will not install RTEMS properly.   You will need to upgrade
to at least GNU fileutils version 3.16 to resolve this problem.

@end itemize

@section Archive and Build Directories

If you are using RPM or another packaging format that supports
building a package from source, then there is probably a directory
structure assumed by that packaging format.  Otherwise, you
are free to use whatever organization you like.  However, this
document will use the directory organization described
in @ref{Archive and Build Directory Format}.

@subsection RPM Archive and Build Directory Format

For RPM, it is assumed that the following subdirectories
are under a root directory such as @code{/usr/src/redhat}:

@example
BUILD
RPMS
SOURCES
SPECS
SRPMS
@end example

For the purposes of this document, the RPM @code{SOURCES} directory
is the directory into which all tool source and patches are
assumed to reside.  The @code{BUILD} directory is where the actual
build is performed when building binaries from a source RPM.
The @code{SOURCES} and @code{BUILD} are logically equivalent to
the @code{archive} and @code{tools} directory discussed in the 
next section.

@subsection Archive and Build Directory Format

When no packaging format requirements are present, the root directory for
the storage of source archives and patches as well as for building the
tools is up to the user.  The only concern is that there be enough
disk space to complete the build.

Make an @code{archive} directory to contain the downloaded
source code and a @code{tools} directory to be used as a build
directory.  The command sequence to do this is shown
below:

@example
mkdir archive
mkdir tools
@end example

This will result in an initial directory structure similar to the
one shown in the following figure:

@example
@group
/whatever/prefix/you/choose/
        archive/
        tools/

@end group
@end example

@c @ifset use-html
@c @html
@c <IMG SRC="sfile12c.jpg" WIDTH=417 HEIGHT=178
@c      ALT="Starting Directory Organization">
@c @end html
@c @end ifset


