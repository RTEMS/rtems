@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GNU Debugger

The GNU Debugger GDB supports many configurations but requires some
means of communicating between the host computer and target board.
This communication can be via a serial port, Ethernet, BDM, or ROM emulator.
The communication protocol can be the GDB remote protocol or GDB
can talk directly to a ROM monitor.  This setup is target board 
specific.  The following configurations have been
successfully used with RTEMS applications:

@itemize @bullet
@item Sparc Instruction Simulator (SIS)
@item PowerPC Instruction Simulator (PSIM)
@item DINK32
@item BDM with 68360 and MPC860 CPUs
@item Motorola Mxxxbug found on M68xxx MVME boards
@item Motorola PPCbug found on PowerPC MVME boards
@end itemize

GDB is currently RTEMS thread/task aware only if you are using the
remote debugging support via Ethernet.  These are configured
using gdb targets of the form CPU-RTEMS.  Note the capital RTEMS.

It is recommended that when toolset binaries are available for
your particular host, that they be used.  Prebuilt binaries
are much easier to install but in the case of gdb may or may
not include support for your particular target board.

@c
@c  Getting Ready to Build GDB
@c
@section Getting Ready to Build GDB

This section describes the process of unarchiving GDB
as well as applying RTEMS specific patches.  This is required when building
the tools via the instructions in the
@ref{Using the GDB configure Script Directly} or
@ref{Using the bit_gdb Script} sections.  It is @b{NOT} required when
using RPM to build tool binaries.


@c
@c  Unarchive the GDB Distribution
@c
@subsection Unarchive the GDB Distribution

Unarchive the compressed tar files for GDB using the appropriate
command based upon the compression program used for the source
distribution you downloaded.

@example
cd tools
tar xzf ../archive/@{GDB-UNTAR}.tar.gz  # for gzip'ed gdb
tar xIf ../archive/@{GDB-UNTAR}.tar.bz2 # for bzip'ed gdb
@endif


@example
cd tools
tar xzf ../archive/@value{GDB-TAR}
@end example

The directory @value{GDB-UNTAR} is created under the tools directory.

@c
@c  Apply RTEMS Patch to GDB
@c

@subsection Apply RTEMS Patch to GDB

@ifclear GDB-RTEMSPATCH
No RTEMS specific patches are required for @value{GDB-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GDB-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GDB-UNTAR}
zcat archive/@value{GDB-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GDB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file.

@end ifset

@c
@c  Building the GNU Debugger GDB
@c

@section Building the GNU Debugger GDB

There are three methods of build the GNU Debugger:

@itemize @bullet
@item RPM
@item direct invocation of configure and make
@item using the @code{bit_gdb} script
@end itemize

Direct invocation of configure and make provides more control
and easier recovery from problems when building.

@c
@c  Using RPM to Build GDB
@c

@subsection Using RPM to Build GDB

This section describes the process of building binutils, gcc, and
newlib using RPM.  RPM is a packaging format which can be used to
distribute binary files as well as to capture the procedure and
source code used to produce those binary files.  Before
attempting to build any RPM from source, it is necessary to
ensure that all required source and patches are in the @code{SOURCES}
directory under the RPM root (probably @code{/usr/src/redhat} or
@code{/usr/local/src/redhat} on your machine.  This procedure
starts by installing the source RPMs as shown in the following
example:

@example
rpm -i i386-rtems-gdb-collection-4.18-4.nosrc.rpm
@end example

Because RTEMS tool RPMS are called "nosrc" to indicate that one or
more source files required to produce the RPMs are not present.
The RTEMS source GDB RPM does not include the large @code{.tar.gz} or
@code{.tgz} files for GDB.  This is shared by all RTEMS RPMs
regardless of target CPU and there was no reason
to duplicate them.  You will have to get the required source
archive files by hand and place them in the @code{SOURCES} directory
before attempting to build.  If you forget to do this, RPM is
smart -- it will tell you what is missing.  To determine what is
included or referenced by a particular RPM, use a command like the
following:

@example
$ rpm -q -l -p i386-rtems-gdb-collection-4.18-4.nosrc.rpm
gdb-4.18-rtems-20000524.diff
gdb-4.18.tar.gz
i386-rtems-gdb-4.18.spec
@end example

Notice that there is a patch file (the @code{.diff} file), a source archive
file (the @code{.tar.gz}), and a file describing the build procedure and
files produced (the @code{.spec} file).  The @code{.spec} file is placed
in the @code{SPECS} directory under the RPM root directory.

c
@c  Configuring and Building GDB using RPM
@c

@subsubsection Configuring and Building GDB using RPM

The following example illustrates the invocation of RPM to build a new,
locally compiled, binutils binary RPM that matches the installed source
RPM.  This example assumes that all of the required source is installed.

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems-gdb-4.18.spec
@end example

If the build completes successfully, RPMS like the following will
be generated in a build-host architecture specific subdirectory
of the RPMS directory under the RPM root directory.

@example
rtems-base-gdb-4.18-4.i386.rpm
i386-rtems-gdb-4.18-4.i386.rpm
@end example

NOTE: It may be necessary to remove the build tree in the
@code{BUILD} directory under the RPM root directory.

@c
@c Using the GDB configure Script Directly
@c 

@subsection Using the GDB configure Script Directly

This section describes how to configure the GNU debugger for
standard RTEMS configurations as well as some alternative
configurations that have been used in the past.

@subsubsection Standard RTEMS GDB Configuration

The following example illustrates the invocation of configure
and make to build and install @value{GDB-UNTAR} for the
m68k-rtems target:

@example
mkdir b-gdb
cd b-gdb
../@value{GDB-UNTAR}/configure --target=m68k-rtems \
  --prefix=/opt/rtems
make all
make info
make install
@end example

For some configurations, it is necessary to specify extra options
to @code{configure} to enable and configure option components
such as a processor simulator.  The following is a list of
configurations for which there are extra options:

@table @b
@item i960-rtems
@code{--enable-sim}

@item powerpc-rtems
@code{--enable-sim --enable-sim-powerpc --enable-sim-timebase --enable-sim-hardware}

@item sparc-rtems
@code{--enable-sim}

@end table

After @value{GDB-UNTAR} is built and installed the
build directory @code{b-gdb} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for @value{GDB-UNTAR} or
invoke the @value{GDB-UNTAR} configure command with the
@code{--help} option.

@subsection Using the bit_gdb Script

The simplest way to build gdb for RTEMS is to use the @code{bit_gdb} script.
This script interprets the settings in the @code{user.cfg} file to
produce the GDB configuration most appropriate for the target CPU.

This script is invoked as follows:

@example
./bit_gdb CPU
@end example

Where CPU is one of the RTEMS supported CPU families from the following
list:

@itemize @bullet
@item hppa1.1
@item i386
@item i386-coff
@item i386-elf
@item i960
@item m68k
@item m68k-coff
@item mips64orion
@item powerpc
@item sh
@item sh-elf
@item sparc
@end itemize

If gdb supports a CPU instruction simulator for this configuration, then
it is included in the build.

