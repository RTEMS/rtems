@c
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building RTEMS

@section Obtain the RTEMS Source Code

This section provides pointers to the RTEMS source code and
Hello World example program.  These files should be
placed in your @code{archive} directory.  

@subheading @value{RTEMS-VERSION}
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}
    File:        @value{RTEMS-TAR}
@ifset use-html
@c    URL:         @uref{ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}, Download RTEMS components}
    URL:         ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}
@end ifset
@end example

@subheading RTEMS Hello World
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}
    File:        hello_world_c.tgz
@ifset use-html
@c    URL:         @uref{ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/hello_world_c.tgz, Download RTEMS Hello World}
    URL:         ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/hello_world_c.tgz
@end ifset
@end example

@c
@c  Unarchive the RTEMS Source
@c

@section Unarchive the RTEMS Source

Use the following command sequence to unpack the RTEMS source into the
tools directory: 

@example
cd tools
tar xzf ../archive/@value{RTEMS-TAR}
@end example

This creates the directory @value{RTEMS-UNTAR}.


@section Add <INSTALL_POINT>/bin to Executable PATH

In order to compile RTEMS, you must have the cross compilation toolset
in your search path.  The following command appends the directory
where the tools were installed prior to this point:

@example
export PATH=$PATH:<INSTALL_POINT>/bin
@end example

NOTE:  The above command is in Bourne shell (@code{sh}) syntax and
should work with the Korn (@code{ksh}) and GNU Bourne Again Shell
(@code{bash}).  It will not work with the C Shell (@code{csh}) or
derivatives of the C Shell.

@section Verifying the Operation of the Cross Toolset

In order to insure that the cross-compiler is invoking the correct
subprograms (like @code{as} and @code{ld}), one can test assemble
a small program.  When in verbose mode, @code{gcc} prints out information 
showing where it found the subprograms it invokes.  In a temporary
working directory, place the following function in a file named @code{f.c}:

@example
int f( int x )
@{
  return x + 1;
@}
@end example

Then assemble the file using a command similar to the following:

@example
m68k-rtems-gcc -v -S f.c
@end example

Where @code{m68k-rtems-gcc} should be changed to match the installed
name of your cross compiler.  The result of this command will be
a sequence of output showing where the cross-compiler searched for
and found its subcomponents.  Verify that these paths correspond
to your <INSTALL_POINT>.

Look at the created file @code{f.s} and verify that it is in fact
for your target processor.

Then try to compile the file @code{f.c} directly to object code
using a command like the following:

@example
m68k-rtems-gcc -v -c f.c
@end example

If this produces messages that indicate the assembly code is
not valid, then it is likely that you have fallen victim to 
one of the problems described in
@ref{Error Message Indicates Invalid Option to Assembler}
Don't feel bad about this, one of the most common installation errors
is for the cross-compiler not to be able to find the cross assembler
and default to using the native @code{as}.  This can result in very confusing
error messages.

@section Building RTEMS for a Specific Target and BSP

This section describes how to configure and build RTEMS 
so that it is specifically tailored for your BSP and the
CPU model it uses.  There are two methods to compile and install RTEMS:

@itemize @bullet
@item direct invocation of @code{configure} and @code{make}
@item using the @code{bit} script
@end itemize

Direct invocation of @code{configure} and @code{make} provides more control
and easier recovery from problems when building.

This section describes how to build RTEMS.

@subsection Using the RTEMS configure Script Directly

Make a build directory under tools and build the RTEMS product in this
directory. The ../@value{RTEMS-UNTAR}/configure
command has numerous command line
arguments. These arguments are discussed in detail in documentation that
comes with the RTEMS distribution. If you followed the procedure
described in the section @ref{Unarchive the RTEMS Source}, these
configuration options can be found in the file
tools/@value{RTEMS-UNTAR}/README.configure.

@b{NOTE}: The GNAT/RTEMS run-time implementation is based on the POSIX
API.  Thus the RTEMS configuration for a GNAT/RTEMS environment MUST
include the @code{--enable-posix} flag.

The following shows the command sequence required to configure,
compile, and install RTEMS with the POSIX API, FreeBSD TCP/IP,
and C++ support disabled.  RTEMS will be built to target 
the @code{BOARD_SUPPORT_PACKAGE} board.

@example
mkdir build-rtems
cd build-rtems
../@value{RTEMS-UNTAR}/configure --target=<TARGET_CONFIGURATION> \
    --disable-posix --disable-tcpip --disable-cxx \
    --enable-rtemsbsp=<BOARD_SUPPORT_PACKAGE>\
    --prefix=<INSTALL_POINT>
make all install
@end example

Where the list of currently supported <TARGET_CONFIGURATION>'s and
<BOARD_SUPPORT_PACKAGE>'s can be found in
tools/@value{RTEMS-UNTAR}/README.configure.

<INSTALL_POINT> is typically the installation point for the 
tools and is @code{/opt/rtems} when using prebuilt toolset executables.

NOTE: The @code{make} utility used should be GNU make.

@c
@c Using the bit_rtems Script
@c

@subsection Using the bit_rtems Script

If you have not previously unarchived the build tools, then you
will need to unarchive the build scripts at this point if you
plan to use @code{bit_rtems} to build RTEMS.  If this is the
case, you will have to execute the following additional command
since you did not do it as part of building the tools.

@example
cd tools
tar xzf ../archive/@value{BUILDTOOLS-TAR}
@end example

This script interprets the settings in the @code{user.cfg} file to
enable or disable the various RTEMS options.  The RTEMS
specific entries described below must be set to
tailor the RTEMS configuration to meet your application requirements:

@table @code

@item RTEMS
is the directory under tools that contains @value{RTEMS-UNTAR}.

@item ENABLE_RTEMS_POSIX
is set to "yes" if you want to enable the RTEMS POSIX API support.
At this time, this feature is not supported by the UNIX ports of RTEMS
and is forced to "no" for those targets.  This corresponds to the
@code{configure} option @code{--enable-posix}.

@item ENABLE_RTEMS_ITRON
is set to "yes" if you want to enable the RTEMS ITRON API support.
At this time, this feature is not supported by the UNIX ports of RTEMS
and is forced to "no" for those targets.  This corresponds to the
@code{configure} option @code{--enable-itron}.

@item ENABLE_RTEMS_MP
is set to "yes" if you want to enable the RTEMS multiprocessing
support.  This feature is not supported by all RTEMS BSPs and
is automatically forced to "no" for those BSPs.  This corresponds to the
@code{configure} option @code{--enable-multiprocessing}.

@item ENABLE_RTEMS_CXX
is set to "yes" if you want to build the RTEMS C++ support including
the C++ Wrapper for the Classic API.  This corresponds to the
@code{configure} option @code{--enable-cxx}.

@item ENABLE_RTEMS_TESTS
is set to "yes" if you want to build the RTEMS Test Suite.  If this
is set to "no", then only the Sample Tests will be built.  Setting
this option to "yes" significantly increases the amount of disk
space required to build RTEMS.
This corresponds to the @code{configure} option @code{--enable-tests}.

@item ENABLE_RTEMS_TCPIP
is set to "yes" if you want to build the RTEMS TCP/IP Stack.  If a
particular BSP does not support TCP/IP, then this feature is automatically
disabled.  This corresponds to the @code{configure} option
@code{--enable-tcpip}.

@item ENABLE_RTEMS_NONDEBUG
is set to "yes" if you want to build RTEMS in a fully optimized
state.  This corresponds to executing @code{make} after configuring
the source tree.

@item ENABLE_RTEMS_DEBUG
is set to "yes" if you want to build RTEMS in a debug version.
When built for debug, RTEMS will include run-time code to 
perform consistency checks such as heap consistency checks.
Although the precise compilation arguments are BSP dependent,
the debug version of RTEMS is usually built at a lower optimization
level.  This is usually done to reduce inlining which can make
tracing code execution difficult.  This corresponds to executing
@code{make VARIANT=debug} after configuring
the source tree.

@item INSTALL_RTEMS
is set to "yes" if you want to install RTEMS after building it.
This corresponds to executing @code{make install} after configuring
and building the source tree.

@item ENABLE_RTEMS_MAINTAINER_MODE
is set to "yes" if you want to enabled maintainer mode functionality
in the RTEMS Makefile.  This is disabled by default and it is not
expected that most users will want to enable this.  When this option
is enabled, the build process may attempt to regenerate files that
require tools not required when this option is disabled.
This corresponds to the @code{configure} option
@code{--enable-maintainer-mode}.

@end table

After tailoring @code{user.cfg} for your application, the @code{bit_rtems}
script may be invoked as follows:

@example
./bit_rtems CPU [BSP]
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

BSP is a supported BSP for the selected CPU family.  The list of
supported BSPs may be found in the file
tools/@value{RTEMS-UNTAR}/README.configure
in the RTEMS source tree.  If the BSP parameter is not specified,
then all supported BSPs for the selected CPU family will be built.

@b{NOTE:}  The POSIX API must be enabled to use GNAT/RTEMS.

