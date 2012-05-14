@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Requirements

This chapter describes the build-host system requirements and initial steps
in installing the GNU Cross Compiler Tools and RTEMS on a build-host.

@section Disk Space

A fairly large amount of disk space is required to perform the build of the
GNU C/C++ Cross Compiler Tools for RTEMS. The following table may help in
assessing the amount of disk space required for your installation:

@example
+------------------------------------+--------------------------+
|              Component             |   Disk Space Required    |
+------------------------------------+--------------------------+
|        archive directory           |       120 Mbytes         |
|        tools src unarchived        |      1400 Mbytes         |
|  each individual build directory   |     up to 2500 Mbytes    |
|     each installation directory    |       900 Mbytes         |
+------------------------------------+--------------------------+
@end example

It is important to understand that the above requirements only address the
GNU C/C++ Cross Compiler Tools themselves.  Adding additional languages
such as Ada or Go can increase the size of the build and installation
directories.  Also, the unarchived source and build directories can be
removed after the tools are installed.

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
a single step increases the disk space requirements.  On some
target architectures, this can lead to disk usage during the build
of over one gigabyte.

Similarly enabling optional features increases the build and install
space requirements.  In particular, enabling and building
the RTEMS tests results in a significant increase in build
space requirements but since the tests are not installed has,
enabling them has no impact on installation requirements.

@section General Host Software Requirements

The instructions in this manual should work on any computer running
a POSIX environment including GNU/Linux and Cygwin.  Mingw users may
encounter additional issues due to the limited POSIX compatibility.
Some native GNU tools are used by this procedure including:

@itemize @bullet
@item GCC
@item GNU make
@item GNU makeinfo
@end itemize

In addition, some native utilities may be deficient for building the
GNU tools.  On hosts which have m4 but it is not GNU m4, it is not
uncommon to have to install GNU m4. Similarly, some shells are not
capable of fully supporting the RTEMS configure scripts.

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
are on a Solaris 2.x host, then use the @code{/bin/ksh} or
@code{/bin/bash} shell instead.

@item The native @code{patch} program is broken.  Install the GNU version.

@item The native @code{m4} program is deficient.  Install the GNU version.

@end itemize

@subsection Distribution Independent Potential GNU/Linux Issues

The following problems have been reported by users of various GNU/Linux
distributions:

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

@subsection GNU/Linux Distrobutions using Debian Packaging Format

The RTEMS Project does not currently provide prebuilt toolsets in the Debian packaging format used by the Debian and Ubuntu distributions.  If you are using a distribution using this packaging format, then you have two options for installing the RTEMS toolset.

The first option is to build the toolset from source following the instructions in the @ref{Building the GNU Cross Compiler Toolset}.  This is an approach taken by many users.

Alternatively, it is often possible to extract the contents of the RPM files which contain the portions of the toolset you require.  In this case, you will follow the instructions in @ref{Locating the RPMs for your GNU/Linux Distribution} but assume your distribution is the RedHat Enterprise Linux version which is closest to yours from a shared library perspective.  As of December 2010, this is usually RedHat Enterprise Linux version 5.  As time passes, it is expected that version 6 will be appropriate in more cases.  You will extract the contents of these RPM files using either @code{rpm2cpio} and install them or you may be able to use the @code{alien} tool to convert them to Debian packaging.
