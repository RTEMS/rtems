@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Building the GNU Cross Compiler Toolset

@b{NOTE}:  This chapter describes the steps required to build cross-compilation
toolset on Linux (and possibly Windows using Cygwin) systems.  This chapter
does @b{NOT} apply if you installed prebuilt toolset executables for BINUTILS,
GCC, NEWLIB, and GDB.  If you installed prebuilt executables for all of those,
proceed to @ref{Building RTEMS}.  If you require a GDB with a special
configuration to connect to your target board, then proceed to
@ref{Installing GDB Without RPM} for some advice.

This chapter describes the steps required to acquire the source code for
a GNU cross compiler toolset, apply any required RTEMS specific patches,
compile that toolset and install it.

It is recommended that when toolset binaries are available for your
particular host, that they be used.  Prebuilt binaries are much easier
to install.  They are also much easier for the RTEMS Project to support.

@c
@c  Preparation
@c
@section Preparation

Before you can build an RTEMS toolset from source, there are some
preparatory steps which must be performed.  You will need to determine
the various tool versions and patches required and download them  You
will also have to unarchive the source and apply any patches.

@c
@c  Determining Tool Version and Patch Revision
@c
@subsection Determining Tool Version and Patch Revision

The tool versions and patch revisions change on a fairly frequent basis.
In addition, these may vary based upon the target architecture.  In some
cases, the RTEMS Project may have to stick with a particular version
of a tool to provide a working version for a specific architecture.
Because of this, it is impossible to provide this information in a
complete and accurate manner in this manual.  You will need to refer
to the configuration files used by the RTEMS RPM specification files to
determine the current versions and, if a patch is required, what version.
This section describes how to locate the appropriate tool versions and
patches for a particular target architecture.

All patches and RPM specification files are kept in CVS.  They are
not included in release tarballs.  You will have to access the
CVS branch for RTEMS @value{RTEMSAPI}.  For details on this,
visit @uref{http://www.rtems.org/wiki/index.php/RTEMS_CVS_Repository, http://www.rtems.org/wiki/index.php/RTEMS_CVS_Repository} and look for
instructions on accessing the RTEMS Source Code Repository in read-only mode.
You can either do a complete checkout of the source tree or use a web
interface.  A typical checkout command would look like this:
@c TODO: pointing the user to the wiki entry might be enough.
@example
cvs -d :pserver:anoncvs@@www.rtems.com:/usr1/CVS -z 9 co -P rtems
@end example

In the checked out source code, you will need to look in the subdirectory
@code{contrib/crossrpms/autotools} to determine the versions of AUTOCONF
and AUTOMAKE as well as any patches required.  In this directory are
a few files you will need to look at.  The first is @code{Makefile.am}
which defines the versions of AUTOCONF and AUTOMAKE required for this
RTEMS Release Series.  Make a note of the version numbers required for
AUTOCONF and AUTOMAKE (AUTOCONF_VERS and AUTOMAKE_VERS respectively).  Then
examine the following files to determine the master location for the source
tarballs and to determine if a patch is required for each tool version cited in
the @code{Makefile.am}.

@example
autoconf-sources.add
automake-sources.add
@end example

If any patches are required, they will be in the
@code{contrib/crossrpms/patches} subdirectory of your checked out RTEMS
source tree.

If no patches are required, you can use a package manager provided by your
Linux distribution to install AUTOMAKE and AUTOCONF to avoid building them from
source.

In the checked out source code, you will need to look in the subdirectory
@code{contrib/crossrpms/rtems@value{RTEMSAPI}} to determine the target
specific tool versions and patches required. In this directory, you
will find a number of subdirectories with many named after target
architectures supported by RTEMS.  Descend into the directory for the
architecture you plan to build tools for.  Again, the @code{Makefile.am}
defines the tool versions for this architecture and RTEMS Release Series.
Make a note of the version numbers required for BINUTILS, GCC, NEWLIB,
and GDB.  Then examine the following files to determine the master
location for the source tarballs and to determine if a patch is required
for each tool version cited in the @code{Makefile.am}.

@itemize
binutils-sources.add
gcc-sources.add
gdb-sources.add
@end itemize

If any patches are required, they will be in the
@code{contrib/crossrpms/patches} subdirectory of your checked out RTEMS
source tree.

This is the entire set of source tarballs and patches required for a
toolset targeting the selected architecture.  In many cases, this will be
the same versions required by other targets on this RTEMS Release Series.

@c
@c  Obtain Source and Patches
@c
@subsection Obtain Source and Patches

You will need to download the sources for the various packages from
their master locations as identified in the previous section.

Any patches needed should be in the @code{contrib/crossrpms/patches}
directory of your RTEMS source.

@c
@c  Installing the Tools Without RPM
@c
@section Installing the Tools Without RPM

This section describes the procedure for building and installing an RTEMS
cross toolset from source code without using the RPM build infrastructure.

Direct invocation of @code{configure} and @code{make} provides more control
and easier recovery from problems when building.

@c
@c Archive and Build Directory Format
@c
@subsection Archive and Build Directory Format

When no packaging format requirements are present, the root directory for
the storage of source archives and patches as well as for building the
tools is up to the user.  The only concern is that there be enough
disk space to complete the build.  In this document, the following
organization will be used.

Make an @code{archive} directory to contain the downloaded source code
and pataches.  Additionally, a @code{tools} directory to be used as a
build directory.  The command sequence to do this is shown below:

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

The RTEMS Project tries to submit all of our patches upstream to the
parent projects.  In the event there are patches, the master copy of them
is located in the appropriate branch of the RTEMS source module in CVS.
Patches are in the @code{contrib/crossrpms/patches}.

@c
@c  Unarchiving the Tools
@c
@subsection Unarchiving the Tools

@b{NOTE}:  This step is required if building any of the tools without using RPM.
It is @b{NOT} required if using the procedure described in @ref{Using RPM
to Build Tools}.  This section describes the process of unarchiving the
tools that comprise an RTEMS toolset.

GNU source distributions are archived using @code{tar} and
compressed using either @code{gzip} or @code{bzip}.
If compressed with @code{gzip}, the extension @code{.gz} is used.
If compressed with @code{bzip}, the extension @code{.bz2} is used.

While in the @code{tools} directory, unpack the compressed tar files
using the appropriate command based upon the compression program used.

@example
cd tools
tar xzf ../archive/TOOLNAME.tar.gz  # for gzip'ed tools
tar xjf ../archive/TOOLNAME.tar.bz2 # for bzip'ed tools
@end example

Assuming you are building a complete toolset, after all of the the
compressed tar files have been unpacked using the appropriate commands,
the following directories will have been created under @code{tools}.

@itemize @bullet
@item autoconf-<VERSION>
@item automake-<VERSION>
@item binutils-<VERSION>
@item gcc-<VERSION>
@item newlib-<VERSION>
@item gdb-<VERSION>
@end itemize

The tree should look something like the following figure:

@example
@group
/whatever/prefix/you/choose/
        archive/
          variable tarballs
          variable patches
        tools/
          various tool source trees
@end group
@end example

@c
@c Applying RTEMS Project Tool Patches
@c

@subsection Applying RTEMS Project Tool Patches

@b{NOTE}:  This step is required if building any of the tools IF they have a
patch currently required and you are building the tools without using RPM.
is @b{NOT} required if using the procedure described in @ref{Using RPM
to Build Tools}.  This section describes the process of applying the
RTEMS patches to any of the tools.

If a patch is required for a particular tool source tree, it is placed in the
@code{contrib/crossrpms/patches} directory in the CVS tree.  Make sure the
patch version is the same as of the tool you are building.  You will perform a
command similar to the following to apply the patch.  In this example, <TOOL>
should be replaced by the appropriate tool directory and <TOOL_PATCH> with the
appropriate patch file.

@example
cd tools/<TOOL>
cat ../../archive/<TOOL_PATCH> | patch -p1
@end example

@b{NOTE}: If you add the @code{--dry-run} option to the @code{patch} command
in the above commands, it will attempt to apply the patch and report
any issues without actually modifying any files.

If the patch was compressed with the @code{gzip} program, it will
have a suffix of @code{.gz} and you should use @code{zcat} instead
of @code{cat} as shown above.  If the patch was compressed with
the @code{gzip} program, it will have a suffix of @code{.bz2} and
you should use @code{bzcat} instead of @code{cat} as shown above.

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/<TOOL>
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied.

@c
@c  Installing AUTOCONF From Source
@c

@subsection Installing AUTOCONF From Source

The following example illustrates the invocation of @code{configure}
and @code{make} to build and install autoconf-<version>.  This tool is
installed as a native utility and is independent of any RTEMS target.

@b{NOTE}: If no patch is required for Autoconf and Automake, you can use the
standard package manager provided by your Linux distribution to install them.
Of course, the versions provided by your package manager should be the same
that specified in Makefile.am or better.

@example
mkdir b-autoconf
cd b-autoconf
../autoconf-<VERSION>/configure --prefix=@value{RTEMSPREFIX}
make
make install
@end example

After autoconf-<VERSION> is built and installed the build directory
@code{b-autoconf} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for autoconf-<VERSION> or invoke the
autoconf-VERSION> @code{configure} command with the @code{--help} option.

@c
@c  Installing AUTOMAKE From Source
@c

@subsection Installing AUTOMAKE From Source

The following example illustrates the invocation of @code{configure}
and @code{make} to build and install automake-<version>.  This tool is
installed as a native utility and is independent of any RTEMS target.

@example
mkdir b-automake
cd b-automake
../automake-<VERSION>/configure --prefix=@value{RTEMSPREFIX}
make all
make info
make install
@end example

After automake-<VERSION> is built and installed the build directory
@code{b-automake} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for automake-<VERSION> or invoke the
automake-VERSION> @code{configure} command with the @code{--help} option.

@c
@c  Installing BINUTILS From Source
@c
@subsection Installing BINUTILS From Source

The following example illustrates the invocation of @code{configure}
and @code{make} to build and install binutils-<version>
sparc-rtems@value{RTEMSAPI} target:

@example
mkdir b-binutils
cd b-binutils
../binutils-<VERSION>/configure --target=sparc-rtems@value{RTEMSAPI} \
  --prefix=@value{RTEMSPREFIX}
make all
make info
make install
@end example

After binutils-<VERSION> is built and installed the build directory
@code{b-binutils} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for binutils-<VERSION> or invoke the
binutils-VERSION> @code{configure} command with the @code{--help} option.

NOTE: The shell PATH variable needs to be updated to include the path
the binutils user executables have  been installed in.  The directory
containing the executables is the prefix used above with
@file{bin} post-fixed.

@example
export PATH=@value{RTEMSPREFIX}/bin:$@{PATH@}
@end example

As you will need to frequently run various commands in the
@value{RTEMSPREFIX}/bin, you can update your @code{~/.bashrc} to include this
line. After doing that, don't forget to run
@example
source ~/.bashrc
@end example
for the changes to take place.

Failure to have the binutils in the path will cause the GCC and NEWLIB
build to fail with an error message similar to:

@example
sparc-rtems@value{RTEMSAPI}-ar: command not found
@end example

@c
@c  Installing GCC and NEWLIB Without RPM
@c
@subsection Installing GCC and NEWLIB Without RPM

Before building gcc-<VERSION> and newlib-<VERSION>,
binutils-<VERSION> must be installed and the directory
containing those executables must be in your PATH.

The C Library is built as a subordinate component of
gcc-<VERSION>.  Because of this, the newlib-<VERSION>
directory source must be available inside the gcc-<VERSION>
source tree.  This is normally accomplished using a symbolic
link as shown in this example:

@example
cd gcc-<VERSION>
ln -s ../newlib-<VERSION>/newlib .
@end example

The following example illustrates the invocation of
@code{configure} and @code{make}
to build and install gcc-<VERSION> with only
C and C++ support for the sparc-rtems@value{RTEMSAPI} target:

@example
mkdir b-gcc
cd b-gcc
../gcc-<VERSION>/configure --target=sparc-rtems@value{RTEMSAPI} \
   --with-gnu-as --with-gnu-ld --with-newlib --verbose \
   --enable-threads --enable-languages="c,c++" \
   --prefix=@value{RTEMSPREFIX}
make all
make info
make install
@end example

After gcc-<VERSION> is built and installed the
build directory @code{b-gcc} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for gcc-<VERSION> or
invoke the gcc-<VERSION> @code{configure} command with the
@code{--help} option.

@c
@c Building GCC with Ada Support
@c
@subsection Building GCC with Ada Support

If you want a GCC toolset that includes support for Ada
(e.g. GNAT), there are some additional requirements on
the host environment and additional build steps to perform.
It is critical that you use the same version of GCC/GNAT as
the native compiler.  GNAT must be compiled with an Ada compiler
and when building a GNAT cross-compiler, it should be
the same version of GNAT itself.

It is also important to verify whether there is an RTEMS specific
Ada patch required for GCC.  These can be found in
@uref{http://www.rtems.org/ftp/pub/rtems/people/joel/ada,
http://www.rtems.org/ftp/pub/rtems/people/joel/ada}.  The patch is
often a minor version or two behind GCC but will usually apply cleanly.
This patch must be applied.

After this, it is critical to perform these steps in the correct order.
GNAT requires that the C Library and RTEMS itself be installed before
the language run-time can be built.

@itemize @bullet
@item install native GCC with GNAT
@item place new native GNAT at head of PATH
@item install BINUTILS
@item place RTEMS prefix at head of PATH
@item install C toolset (C++ is optional)
@item install RTEMS built multilib
@item install RTEMS built for your BSP
@end itemize

The build procedure is the same until the Ada configure step.  A GCC
toolset with GNAT enabled requires that @code{ada} be included in the set
of enabled languages.  The following example illustrates the invocation of
@code{configure} and @code{make} to build and install gcc-<VERSION> with
only C, C++, and Ada support for the sparc-rtems@value{RTEMSAPI} target:

@example
mkdir b-gcc
cd b-gcc
../gcc-<VERSION>/configure --target=sparc-rtems@value{RTEMSAPI} \
   --with-gnu-as --with-gnu-ld --with-newlib --verbose \
   --enable-threads --enable-languages="c,c++,ada" \
   --prefix=@value{RTEMSPREFIX}
make all
make info
make install
@end example

After gcc-<VERSION> is built and installed the build directory
@code{b-gcc} may be removed.

@c
@c Installing GDB Without RPM
@c
@subsection Installing GDB Without RPM

@b{NOTE}:  This step is NOT required if prebuilt executables for the
GDB were installed and they meet your target interface
requirements.

GDB supports many configurations but requires some means of communicating
between the host computer and target board.  This communication can be via
a serial port, Ethernet, BDM, or ROM emulator.  The communication protocol
can be the GDB remote protocol or GDB can talk directly to a ROM monitor.
This setup is target board specific.  Some of the configurations that have
been successfully used with RTEMS applications are:

@itemize @bullet
@item BDM with ColdFire, 683xx, MPC860 CPUs
@item Motorola Mxxxbug found on M68xxx VME boards
@item Motorola PPCbug found on PowerPC VME, CompactPCI, and MTX boards
@item ARM based Cogent EDB7312
@item PC's using various Intel and AMD CPUs including i386,
i486, Pentium and above, and Athlon
@item PowerPC Instruction Simulator in GDB (PSIM)
@item MIPS Instruction Simulator in GDB (JMR3904)
@item Sparc Instruction Simulator in GDB (SIS)
@item Sparc Instruction Simulator (TSIM)
@end itemize

GDB is currently RTEMS thread/task aware only if you are using the
remote debugging support via Ethernet.  These are configured
using gdb targets of the form CPU-RTEMS.  Note the capital RTEMS.

It is recommended that when toolset binaries are available for
your particular host, that they be used.  Prebuilt binaries
are much easier to install but in the case of gdb may or may
not include support for your particular target board.

The following example illustrates the invocation of @code{configure}
and @code{make} to build and install gdb-<VERSION> for the
m68k-rtems@value{RTEMSAPI} target:

@example
mkdir b-gdb
cd b-gdb
../gdb-<VERSION>/configure --target=m68k-rtems@value{RTEMSAPI} \
  --prefix=@value{RTEMSPREFIX}
make all
make info
make install
@end example

For some configurations, it is necessary to specify extra options
to @code{configure} to enable and configure option components
such as a processor simulator.  The following is a list of
configurations for which there are extra options:

@table @b
@item powerpc-rtems@value{RTEMSAPI}
@code{--enable-sim --enable-sim-powerpc --enable-sim-timebase --enable-sim-hardware}

@item sparc-rtems@value{RTEMSAPI}
@code{--enable-sim}

@end table

After gdb-<VERSION> is built and installed the
build directory @code{b-gdb} may be removed.

For more information on the invocation of @code{configure}, please
refer to the documentation for gdb-<VERSION> or
invoke the gdb-<VERSION> @code{configure} command with the
@code{--help} option.


@c
@c  Using RPM to Build Tools
@c

@section Using RPM to Build Tools

RPM is a packaging format which can be used to distribute binary files as
well as to capture the procedure and source code used to produce those
binary files.  For RPM, it is assumed that the following subdirectories
are under a root directory such as @code{/usr/src/redhat} or
@code{/usr/local/src/redhat}) on your machine.

@example
BUILD
RPMS
SOURCES
SPECS
SRPMS
@end example

For the purposes of this document, the RPM @code{SOURCES} directory is the
directory into which all tool source and patches are assumed to reside.
The @code{BUILD} directory is where the actual build is performed when
building binaries from a source RPM.

RPM automatically unarchives the source and applies any needed patches
so you do @b{NOT} have to manually perform the procedures described
@ref{Unarchiving the Tools} and @ref{Applying RTEMS Project Tool Patches}.
But you are responsible for placing all source tarballs
and patches in the @code{SOURCES} directory per the instructions in
@ref{Obtain Source and Patches}

This procedure starts by installing the source (e.g. @code{.src.rpm}
extension) RPMs.  The RTEMS tool source RPMS are called "nosrc" to
indicate that one or more source files required to produce the RPMs
are not present.  The RTEMS source RPMs typically include all required
patches, but do not include the large @code{.tar.gz} or @code{.tgz} files
for each component such as BINUTILS, GCC, or NEWLIB.  These are shared
by all RTEMS RPMs regardless of target CPU and there was no reason to
duplicate them.  You will have to get the required source archive files
by hand and place them in the @code{SOURCES} directory before attempting
to build.  If you forget to do this, RPM is smart -- it will tell you
what is missing. You can fetch any missing files and try again.

@c
@c  Building AUTOCONF using RPM
@c
@subsection Building AUTOCONF using RPM

This section illustrates the invocation of RPM to build a new, locally
compiled, AUTOCONF binary RPM that matches the installed source RPM.
This example assumes that all of the required source is installed.

@example
rpm -U @value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-autoconf-<VERSION>-<RPM_RELEASE>.src.rpm
@end example

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems@value{RTEMSAPI}-autoconf-<VERSION>.spec
@end example

If the build completes successfully, RPMS like the following will be
generated in a build-host architecture specific subdirectory of the RPMs
directory under the RPM root directory.

@example
@value{RTEMSRPMPREFIX}rtems@value{RTEMSAPI}-autoconf-<VERSION>-<RPM_RELEASE>.<ARCH>.rpm
@end example

@b{NOTE}:  It may be necessary to remove the build tree in the @code{BUILD}
directory under the RPM root directory.

@c
@c  Building AUTOMAKE using RPM
@c
@subsection Building AUTOMAKE using RPM

This section illustrates the invocation of RPM to build a new, locally
compiled, AUTOMAKE binary RPM that matches the installed source RPM.
This example assumes that all of the required source is installed.

@example
rpm -U @value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-automake-<VERSION>-<RPM_RELEASE>.src.rpm
@end example

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems@value{RTEMSAPI}-automake-<VERSION>.spec
@end example

If the build completes successfully, RPMS like the following will be
generated in a build-host architecture specific subdirectory of the RPMs
directory under the RPM root directory.

@example
@value{RTEMSRPMPREFIX}rtems@value{RTEMSAPI}-automake-<VERSION>-<RPM_RELEASE>.<ARCH>.rpm
@end example

@b{NOTE}:  It may be necessary to remove the build tree in the @code{BUILD}
directory under the RPM root directory.


@c
@c  Building BINUTILS using RPM
@c
@subsection Building BINUTILS using RPM

This section illustrates the invocation of RPM to build a new, locally
compiled, binutils binary RPM that matches the installed source RPM.
This example assumes that all of the required source is installed.

@example
rpm -U @value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-binutils-<VERSION>-<RPM_RELEASE>.src.rpm
@end example

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems@value{RTEMSAPI}-binutils-<VERSION>.spec
@end example

If the build completes successfully, RPMS like the following will be
generated in a build-host architecture specific subdirectory of the RPMS
directory under the RPM root directory.

@example
@value{RTEMSRPMPREFIX}binutils-common-<VERSION>-<RPM_RELEASE>.<ARCH>.rpm
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-binutils-<VERSION>-<RPM_RELEASE>.<ARCH>.rpm
@end example

NOTE: It may be necessary to remove the build tree in the @code{BUILD}
directory under the RPM root directory.

@c
@c  Building GCC and NEWLIB using RPM
@c
@subsection Building GCC and NEWLIB using RPM

This section illustrates the invocation of RPM to build a new,
locally compiled, set of GCC and NEWLIB binary RPMs that match the
installed source RPM.  It is also necessary to install the BINUTILS
RPMs and place them in your PATH.  This example assumes that all of
the required source is installed.

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems@value{RTEMSAPI}-gcc-<VERSION>.spec
@end example

If the build completes successfully, a set of RPMS like the following will
be generated in a build-host architecture specific subdirectory
of the RPMS directory under the RPM root directory.

@example
@value{RTEMSRPMPREFIX}gcc-common-<VERSION>-<RPM>.<DIST>.noarch.rpm \
@value{RTEMSRPMPREFIX}newlib-common-<VERSION>-<RPM>.<DIST>.noarch.rpm \
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-gcc-<VERSION>-<RPM>.<ARCH>.rpm \
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-newlib-<VERSION>-<RPM>.<ARCH>.rpm \
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-libgcc-<VERSION>-<RPM>.<ARCH>.rpm \
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-gcc-c++-<VERSION>-<RPM>.<ARCH>.rpm \
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-libstd++-<VERSION>-<RPM>.<ARCH>.rpm
@end example

@b{NOTE}: Some targets do not support building all languages.

@b{NOTE}: It may be necessary to remove the build tree in the
@code{BUILD} directory under the RPM root directory.

@c
@c Building the GDB using RPM
@c
@subsection Building the GDB using RPM

The following example illustrates the invocation of RPM to build a new,
locally compiled, binutils binary RPM that matches the installed source
RPM.  This example assumes that all of the required source is installed.


@example
rpm -U @value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-gdb-<VERSION>-<RPM_RELEASE>.src.rpm
@end example

@example
cd <RPM_ROOT_DIRECTORY>/SPECS
rpm -bb i386-rtems@value{RTEMSAPI}-gdb-<VERSION>.spec
@end example

If the build completes successfully, RPMS like the following will
be generated in a build-host architecture specific subdirectory
of the RPMS directory under the RPM root directory.

@example
@value{RTEMSRPMPREFIX}gdb-common-<VERSION>-<RPM_RELEASE>.<ARCH>.rpm
@value{RTEMSRPMPREFIX}i386-rtems@value{RTEMSAPI}-gdb-<VERSION>-<RPM_RELEASE>.<ARCH>.rpm
@end example

@b{NOTE}: It may be necessary to remove the build tree in the
@code{BUILD} directory under the RPM root directory.

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

@b{NOTE}:  In some environments, it may be difficult to remove "."
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
find binutils-<VERSION> -name config.status -print
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
-I../../binutils-<VERSION>/gcc -I/binutils-<VERSION>/gcc/include -I.
@end example

Note that the tool source directory is searched before the
build directory.

This situation can be avoided entirely by never using
the source tree as the build directory.
