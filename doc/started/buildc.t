@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GNU C/C++ Cross Compiler Toolset

NOTE:  This chapter does @b{NOT} apply if you installed
prebuilt toolset executables for BINUTILS, GCC, NEWLIB,
and GDB.  If you installed prebuilt executables for all
of those, proceed to @ref{Building RTEMS}.  If you require
a GDB with a special configuration to connect to your
target board, then proceed to @ref{Building the GNU Debugger GDB}
for some advice.

This chapter describes the steps required to acquire the
source code for a GNU cross compiler toolset, apply 
any required RTEMS specific patches, compile that 
toolset and install it.

It is recommended that when toolset binaries are available for
your particular host, that they be used.  Prebuilt binaries
are much easier to install.

@c
@c  Building BINUTILS GCC and NEWLIB
@c
@section Building BINUTILS GCC and NEWLIB

NOTE: This step is NOT required if prebuilt executables for
BINUTILS, GCC, and NEWLIB were installed.

This section describes the process of building BINUTILS, GCC, and
NEWLIB using a variety of methods.  Included is information on
obtaining the source code and patches, applying patches, and
building and installing the tools using multiple methods.

@c
@c  Obtain Source and Patches for BINUTILS GCC and NEWLIB
@c

@subsection Obtain Source and Patches for BINUTILS GCC and NEWLIB

NOTE: This step is required for all methods of building BINUTILS,
GCC, and NEWLIB.

This section lists the components required to build BINUTILS, GCC,
and NEWLIB from source to target RTEMS.  These files should be
placed in your @code{archive} directory.  Included are the locations
of each component as well as any required RTEMS specific patches.

@need 1000
@subheading @value{GCC-VERSION}
@example
    FTP Site:    @value{GCC-FTPSITE}
    Directory:   @value{GCC-FTPDIR}
    File:        @value{GCC-TAR}
@ifset use-html
@c    URL:         @uref{Download @value{GCC-VERSION}, ftp://@value{GCC-FTPSITE}@value{GCC-FTPDIR}}
    URL:         ftp://@value{GCC-FTPSITE}@value{GCC-FTPDIR}
@end ifset
@end example

@need 1000
@subheading @value{BINUTILS-VERSION}
@example
    FTP Site:    @value{BINUTILS-FTPSITE}
    Directory:   @value{BINUTILS-FTPDIR}
    File:        @value{BINUTILS-TAR}
@ifset use-html
@c    URL:         @uref{ftp://@value{BINUTILS-FTPSITE}@value{BINUTILS-FTPDIR}/@value{BINUTILS-TAR},Download @value{BINUTILS-VERSION}}
    URL:         ftp://@value{BINUTILS-FTPSITE}@value{BINUTILS-FTPDIR}/@value{BINUTILS-TAR}
@end ifset
@end example

@need 1000
@subheading @value{NEWLIB-VERSION}
@example
    FTP Site:    @value{NEWLIB-FTPSITE}
    Directory:   @value{NEWLIB-FTPDIR}
    File:        @value{NEWLIB-TAR}
@ifset use-html
@c    URL:         @uref{ftp://@value{NEWLIB-FTPSITE}@value{NEWLIB-FTPDIR}/@value{NEWLIB-TAR}, Download @value{NEWLIB-VERSION}}
    URL:         ftp://@value{NEWLIB-FTPSITE}@value{NEWLIB-FTPDIR}/@value{NEWLIB-TAR}
@end ifset
@end example

@need 1000
@subheading RTEMS Specific Tool Patches and Scripts
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}/c_tools/source
    File:        @value{BUILDTOOLS-TAR}
@ifset BINUTILS-RTEMSPATCH
    File:        @value{BINUTILS-RTEMSPATCH}
@end ifset
@ifset NEWLIB-RTEMSPATCH
    File:        @value{NEWLIB-RTEMSPATCH}
@end ifset
@ifset GCC-RTEMSPATCH
    File:        @value{GCC-RTEMSPATCH}
@end ifset
@ifset use-html
@c    URL:         @uref{ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/c_tools/source,Download RTEMS Patches and Scripts}
    URL:         ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/c_tools/source
@end ifset
@end example

@c
@c  Unarchiving the Tools
@c
@subsection Unarchiving the Tools

NOTE: This step is required if building BINUTILS, GCC, and NEWLIB
using the procedure described in @ref{Using configure and make}.
It is @b{NOT} required if using the procedure
described in @ref{Using RPM to Build BINUTILS GCC and NEWLIB}.

GNU source distributions are archived using @code{tar} and
compressed using either @code{gzip} or @code{bzip}.  
If compressed with @code{gzip}, the extension @code{.gz} is used.
If compressed with @code{bzip}, the extension @code{.bz2} is used.

While in the @code{tools} directory, unpack the compressed
tar files for BINUTILS, GCC, and NEWLIB using the appropriate
command based upon the compression program used.

@example
cd tools
tar xzf ../archive/TOOLNAME.tar.gz  # for gzip'ed tools
tar xIf ../archive/TOOLNAME.tar.bz2 # for bzip'ed tools
@end example

After the compressed tar files have been unpacked using
the appropriate commands, the following
directories will have been created under tools. 

@itemize @bullet
@item @value{BINUTILS-UNTAR}
@item @value{GCC-UNTAR}
@item @value{NEWLIB-UNTAR}
@end itemize

The tree should look something like the following figure:

@example
@group
/whatever/prefix/you/choose/
        archive/
            @value{GCC-TAR}
            @value{BINUTILS-TAR}
            @value{NEWLIB-TAR}
@ifset GCC-RTEMSPATCH
            @value{GCC-RTEMSPATCH}
@end ifset
@ifset BINUTILS-RTEMSPATCH
            @value{BINUTILS-RTEMSPATCH}
@end ifset
@ifset NEWLIB-RTEMSPATCH
            @value{NEWLIB-RTEMSPATCH}
@end ifset
        tools/
            @value{BINUTILS-UNTAR}/
            @value{GCC-UNTAR}/
            @value{NEWLIB-UNTAR}/
@end group
@end example

@c
@c  Applying RTEMS Patches
@c

@subsection Applying RTEMS Patches

NOTE: This step is required if building BINUTILS, GCC, and NEWLIB
using the procedures described in @ref{Using configure and make}.
It is @b{NOT} required if using the procedure
described in @ref{Using RPM to Build BINUTILS GCC and NEWLIB}.

This section describes the process of applying the RTEMS patches
to GCC, NEWLIB, and BINUTILS.

@c
@c  GCC patches
@c

@subheading Apply RTEMS Patch to GCC

@ifclear GCC-RTEMSPATCH
No RTEMS specific patches are required for @value{GCC-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GCC-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GCC-UNTAR}
zcat ../../archive/@value{GCC-RTEMSPATCH} | \
    patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GCC-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied.

@end ifset

@c
@c  BINUTILS patches
@c

@subheading Apply RTEMS Patch to binutils

@ifclear BINUTILS-RTEMSPATCH
No RTEMS specific patches are required for @value{BINUTILS-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset BINUTILS-RTEMSPATCH
Apply the patch using the following command sequence:

@example
cd tools/@value{BINUTILS-UNTAR}
zcat ../../archive/@value{BINUTILS-RTEMSPATCH} | \
    patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{BINUTILS-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied. 

@end ifset

@c
@c  Newlib patches
@c

@subheading Apply RTEMS Patch to newlib

@ifclear NEWLIB-RTEMSPATCH
No RTEMS specific patches are required for @value{NEWLIB-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset NEWLIB-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{NEWLIB-UNTAR}
zcat ../../archive/@value{NEWLIB-RTEMSPATCH} | \
    patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{NEWLIB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied. 

@end ifset


@c
@c  Compiling and Installing BINUTILS GCC and NEWLIB
@c

@subsection Compiling and Installing BINUTILS GCC and NEWLIB

There are two supported methods to compile and install BINUTILS, GCC,
and NEWLIB:

@itemize @bullet
@item RPM
@item direct invocation of @code{configure} and @code{make}
@end itemize

Direct invocation of @code{configure} and @code{make} provides more control
and easier recovery from problems when building.

@c
@c  Using RPM to Build BINUTILS GCC and NEWLIB
@c

@subsubsection Using RPM to Build BINUTILS GCC and NEWLIB

NOTE:  The procedures described in the following sections must
be completed before this step:

@itemize @bullet
@item @ref{Obtain Source and Patches for BINUTILS GCC and NEWLIB}
@end itemize

RPM automatically unarchives the source and applies any needed
patches so you do @b{NOT} have to manually perform the procedures
described @ref{Unarchiving the Tools} and 
@ref{Applying RTEMS Patches}.

This section describes the process of building binutils, gcc, and
newlib using RPM.  RPM is a packaging format which can be used to
distribute binary files as well as to capture the procedure and
source code used to produce those binary files.  Before
attempting to build any RPM from source, it is necessary to
ensure that all required source and patches are in the @code{SOURCES}
directory under the RPM root (probably @code{/usr/src/redhat} or
@code{/usr/local/src/redhat}) on your machine.  This procedure
starts by installing the source RPMs as shown in the following 
example:

@example 
rpm -i i386-rtems-binutils-collection-2.9.5.0.24-1.nosrc.rpm
rpm -i i386-rtems-gcc-newlib-gcc2.95.2newlib1.8.2-7.nosrc.rpm
@end example

Because RTEMS tool RPMS are called "nosrc" to indicate that one or
more source files required to produce the RPMs are not present.  
The RTEMS source RPMs do not include the large @code{.tar.gz} or
@code{.tgz} files for
each component such as BINUTILS, GCC, or NEWLIB.  These are shared
by all RTEMS RPMs regardless of target CPU and there was no reason
to duplicate them.  You will have to get the required source
archive files by hand and place them in the @code{SOURCES} directory
before attempting to build.  If you forget to do this, RPM is
smart -- it will tell you what is missing.  To determine what is
included or referenced by a particular RPM, use a command like the
following:

@example
$ rpm -q -l -p i386-rtems-binutils-collection-2.9.5.0.24-1.nosrc.rpm
binutils-2.9.5.0.24-rtems-20000207.diff
binutils-2.9.5.0.24.tar.gz
i386-rtems-binutils-2.9.5.0.24.spec
@end example

Notice that there is a patch file (the @code{.diff} file), a source archive
file (the @code{.tar.gz}), and a file describing the build procedure and
files produced (the @code{.spec} file).  The @code{.spec} file is placed
in the @code{SPECS} directory under the RPM root directory.

@c
@c  Configuring and Building BINUTILS using RPM
@c

@subheading Configuring and Building BINUTILS using RPM

The following example illustrates the invocation of RPM to build a new,
locally compiled, binutils binary RPM that matches the installed source 
RPM.  This example assumes that all of the required source is installed.

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems-binutils-2.9.5.0.24.spec
@end example

If the build completes successfully, RPMS like the following will
be generated in a build-host architecture specific subdirectory
of the RPMS directory under the RPM root directory.

@example
rtems-base-binutils-2.9.5.0.24-1.i386.rpm
i386-rtems-binutils-2.9.5.0.24-1.i386.rpm
@end example

NOTE: It may be necessary to remove the build tree in the
@code{BUILD} directory under the RPM root directory.

@c
@c  Configuring and Building GCC and NEWLIB using RPM
@c

@subheading Configuring and Building GCC and NEWLIB using RPM

The following example illustrates the invocation of RPM to build a new,
locally compiled, set of GCC and NEWLIB binary RPMs that match the
installed source RPM.  It is also necessary to install the BINUTILS
RPMs and place them in your PATH.  This example assumes that all of
the required source is installed.

@example
cd <RPM_ROOT_DIRECTORY>/RPMS/i386
rpm -i rtems-base-binutils-2.9.5.0.24-1.i386.rpm
rpm -i i386-rtems-binutils-2.9.5.0.24-1.i386.rpm
export PATH=/opt/rtems/bin:$PATH
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems-gcc-2.95.2-newlib-1.8.2.spec
@end example

If the build completes successfully, a set of RPMS like the following will
be generated in a build-host architecture specific subdirectory
of the RPMS directory under the RPM root directory.

@example
rtems-base-gcc-gcc2.95.2newlib1.8.2-7.i386.rpm
rtems-base-chill-gcc2.95.2newlib1.8.2-7.i386.rpm
rtems-base-g77-gcc2.95.2newlib1.8.2-7.i386.rpm
rtems-base-gcj-gcc2.95.2newlib1.8.2-7.i386.rpm
i386-rtems-gcc-gcc2.95.2newlib1.8.2-7.i386.rpm
i386-rtems-chill-gcc2.95.2newlib1.8.2-7.i386.rpm
i386-rtems-g77-gcc2.95.2newlib1.8.2-7.i386.rpm
i386-rtems-gcj-gcc2.95.2newlib1.8.2-7.i386.rpm
i386-rtems-objc-gcc2.95.2newlib1.8.2-7.i386.rpm
@end example

NOTE: Some targets do not support building all languages.

NOTE: It may be necessary to remove the build tree in the
@code{BUILD} directory under the RPM root directory.

@c
@c  Using configure and make
@c

@subsubsection Using configure and make

NOTE:  The procedures described in the following sections must
be completed before this step:

@itemize @bullet
@item @ref{Obtain Source and Patches for BINUTILS GCC and NEWLIB}
@item @ref{Unarchiving the Tools}
@item @ref{Applying RTEMS Patches}
@end itemize

This section describes the process of building binutils, gcc, and
newlib manually using @code{configure} and @code{make} directly.

@c
@c  Configuring and Building BINUTILS
@c

@subheading Configuring and Building BINUTILS

The following example illustrates the invocation of
@code{configure} and @code{make}
to build and install @value{BINUTILS-UNTAR} for the
sparc-rtems target:

@example
mkdir b-binutils
cd b-binutils
../@value{BINUTILS-UNTAR}/configure --target=sparc-rtems \
  --prefix=/opt/rtems
make all
make info
make install
@end example

After @value{BINUTILS-UNTAR} is built and installed the 
build directory @code{b-binutils} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for @value{BINUTILS-UNTAR} or 
invoke the @value{BINUTILS-UNTAR} @code{configure} command with the
@code{--help} option.

NOTE: The shell PATH variable needs to be updated to include the path
the binutils has been installed in. This the prefix used above with
@file{bin} post-fixed.

@example
export PATH=$PATH:/opt/rtems/bin
@end example

Failure to have the binutils in the path will cause the GCC and NEWLIB
build to fail with an error message similar to:

@example
sparc-rtems-ar: command not found
@end example

@c
@c  Configuring and Building GCC and NEWLIB
@c

@subheading Configuring and Building GCC and NEWLIB

Before building @value{GCC-UNTAR} and @value{NEWLIB-UNTAR}, 
@value{BINUTILS-UNTAR} must be installed and the directory
containing those executables must be in your PATH.

The C Library is built as a subordinate component of 
@value{GCC-UNTAR}.  Because of this, the @value{NEWLIB-UNTAR}
directory source must be available inside the @value{GCC-UNTAR}
source tree.  This is normally accomplished using a symbolic
link as shown in this example:

@example
cd @value{GCC-UNTAR}
ln -s ../@value{NEWLIB-UNTAR}/newlib .
@end example

The following example illustrates the invocation of
@code{configure} and @code{make}
to build and install @value{BINUTILS-UNTAR} for the
sparc-rtems target:

@example
mkdir b-gcc
cd b-gcc
../@value{GCC-UNTAR}/configure --target=sparc-rtems \
   --with-gnu-as --with-gnu-ld --with-newlib --verbose \
   --enable-threads --prefix=/opt/rtems
make all
make info
make install
@end example

After @value{GCC-UNTAR} is built and installed the 
build directory @code{b-gcc} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for @value{GCC-UNTAR} or
invoke the @value{GCC-UNTAR} @code{configure} command with the
@code{--help} option.

@c
@c Building the GNU Debugger GDB
@c

@section Building the GNU Debugger GDB

NOTE: This step is NOT required if prebuilt executables for
the GNU Debugger GDB were installed.

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
@item Motorola Mxxxbug found on M68xxx VME boards
@item Motorola PPCbug found on PowerPC VME and CompactPCI boards
@end itemize

GDB is currently RTEMS thread/task aware only if you are using the
remote debugging support via Ethernet.  These are configured
using gdb targets of the form CPU-RTEMS.  Note the capital RTEMS.

It is recommended that when toolset binaries are available for
your particular host, that they be used.  Prebuilt binaries
are much easier to install but in the case of gdb may or may
not include support for your particular target board.

@c
@c  Obtain Source and Patches for GDB
@c

@subsection Obtain Source and Patches for GDB

NOTE: This step is required for all methods of building GDB.

This section lists the components required to build GDB
from source to target RTEMS.  These files should be
placed in your @code{archive} directory.  Included are the locations
of each component as well as any required RTEMS specific patches.

@need 1000
@subheading @value{GDB-VERSION}
@example
    FTP Site:    @value{GDB-FTPSITE}
    Directory:   @value{GDB-FTPDIR}
    File:        @value{GDB-TAR}
@ifset use-html
@c    URL:         @uref{Download @value{GDB-VERSION}, ftp://@value{GDB-FTPSITE}@value{GDB-FTPDIR}}
    URL:         ftp://@value{GDB-FTPSITE}@value{GDB-FTPDIR}
@end ifset
@end example

@need 1000
@subheading RTEMS Specific Tool Patches and Scripts
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}/c_tools/source
    File:        @value{BUILDTOOLS-TAR}
@ifset GDB-RTEMSPATCH
    File:        @value{GDB-RTEMSPATCH}
@end ifset
@ifset use-html
@c    URL:         @uref{ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/c_tools/source,Download RTEMS Patches and Scripts}
    URL:         ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/c_tools/source
@end ifset
@end example

@c
@c  Unarchiving the GDB Distribution
@c
@subsection Unarchiving the GDB Distribution

Use the following commands to unarchive the GDB distribution:

@example
cd tools
tar xzf ../archive/@value{GDB-TAR}
@end example

The directory @value{GDB-UNTAR} is created under the tools directory.

@c
@c  Applying RTEMS Patch to GDB
@c

@subsection Applying RTEMS Patch to GDB

@ifclear GDB-RTEMSPATCH
No RTEMS specific patches are required for @value{GDB-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GDB-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GDB-UNTAR}
zcat archive/@value{GDB-RTEMSPATCH} | \
    patch -p1
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
@c  Compiling and Installing the GNU Debugger GDB
@c

@subsection Compiling and Installing the GNU Debugger GDB

There are three methods of building the GNU Debugger:

@itemize @bullet
@item RPM
@item direct invocation of @code{configure} and @code{make}
@end itemize

Direct invocation of @code{configure} and @code{make} provides more control
and easier recovery from problems when building.

@c
@c  Using RPM to Build GDB
@c

@subsubsection Using RPM to Build GDB

This section describes the process of building binutils, gcc, and
newlib using RPM.  RPM is a packaging format which can be used to
distribute binary files as well as to capture the procedure and
source code used to produce those binary files.  Before
attempting to build any RPM from source, it is necessary to
ensure that all required source and patches are in the @code{SOURCES}
directory under the RPM root (probably @code{/usr/src/redhat} or
@code{/usr/local/src/redhat}) on your machine.  This procedure
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

@c
@c  Configuring and Building GDB using RPM
@c

@subheading Configuring and Building GDB using RPM

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

@subsubsection Using the GDB configure Script Directly

This section describes how to configure the GNU debugger for
RTEMS targets using @code{configure} and @code{make} directly.
The following example illustrates the invocation of @code{configure}
and @code{make} to build and install @value{GDB-UNTAR} for the
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
invoke the @value{GDB-UNTAR} @code{configure} command with the
@code{--help} option.

@c
@c Common Problems
@c

@section Common Problems

@subsection Error Message Indicates Invalid Option to Assembler

If a message like this is printed then the new cross compiler
is most likely using the native assembler instead of the cross
assembler or vice-versa (native compiler using new cross assembler).
This can occur for one of the following reasons:

@itemize @bullet

@item Binutils Patch Improperly Applied
@item Binutils Not Built
@item Current Directory is in Your PATH

@end itemize

If you are using binutils 2.9.1 or newer with certain older versions of
gcc, they do not agree on what the name of the newly 
generated cross assembler is.  Older binutils called it @code{as.new}
which became @code{as.new.exe} under Windows.  This is not a valid
file name, so @code{as.new} is now called @code{as-new}.  By using the latest
released tool versions and RTEMS patches, this problem will be avoided.

If binutils did not successfully build the cross assembler, then 
the new cross gcc (@code{xgcc}) used to build the libraries can not
find it.  Make sure the build of the binutils succeeded.

If you include the current directory in your PATH, then there
is a chance that the native compiler will accidentally use
the new cross assembler instead of the native one.  This usually
indicates that "." is before the standard system directories
in your PATH.  As a general rule, including "." in your PATH
is a security risk and should be avoided.  Remove "." from
your PATH.

NOTE:  In some environments, it may be difficult to remove "."
completely from your PATH.  In this case, make sure that "."
is after the system directories containing "as" and "ld".

@subsection Error Messages Indicating Configuration Problems

If you see error messages like the following,

@itemize @bullet

@item cannot configure libiberty
@item coff-emulation not found
@item etc.

@end itemize

Then it is likely that one or more of your gnu tools is 
already configured locally in its source tree.  You can check
for this by searching for the @code{config.status} file
in the various tool source trees.  The following command
does this for the binutils source:

@example
find @value{BINUTILS-UNTAR} -name config.status -print
@end example

The solution for this is to execute the command 
@code{make distclean} in each of the GNU tools
root source directory.  This should remove all
generated files including Makefiles.

This situation usually occurs when you have previously
built the tool source for some non-RTEMS target.  The
generated configuration specific files are still in
the source tree and the include path specified during
the RTEMS build accidentally picks up the previous
configuration.  The include path used is something like
this:

@example
-I../../@value{BINUTILS-UNTAR}/gcc -I/@value{BINUTILS-UNTAR}/gcc/include -I.
@end example

Note that the tool source directory is searched before the 
build directory.

This situation can be avoided entirely by never using 
the source tree as the build directory -- even for

