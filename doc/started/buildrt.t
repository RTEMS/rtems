@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Building RTEMS

@section Obtain the RTEMS Source Code

This section provides pointers to the RTEMS source code and example
programs.  These files should be placed in your @code{archive} directory.
The set of tarballs which comprise an RTEMS release is placed in a
directory whose name is the release on the ftp site.  The RTEMS ftp site
is accessible via both the ftp and http protocols at the following URLs:

@itemize @bullet
@item @uref{http://www.rtems.org/ftp/pub/rtems,http://www.rtems.org/ftp/pub/rtems}
@item @uref{ftp://www.rtems.org/pub/rtems,ftp://www.rtems.org/pub/rtems}
@end itemize

Associated with each RTEMS Release is a set of example programs.
Prior to the 4.10 Release Series, these examples were in a "Class
Examples" and an "Examples" collection.  Beginning with the 4.10 Release
Series, these examples collections were merged and other examples added.
This new collection is called "Examples V2".  It is contained in the file
@code{examples-v2-<VERSION>.tar.bz2>} within the RTEMS release directory.

@c
@c  Unarchive the RTEMS Source
@c

@section Unarchive the RTEMS Source

Use the following command sequence to unpack the RTEMS source into the
tools directory:

@example
cd tools
tar xjf ../archive/rtems-@value{RTEMSAPI}.<VERSION>.tar.bz2
@end example

This creates the directory rtems-@value{RTEMSAPI}.<VERSION>

@section Obtaining the RTEMS Source from CVS

Instead of downloading release tarballs you may choose to check out the current
RTEMS source from CVS. For details on accessing RTEMS CVS repository consult
@uref{http://www.rtems.org/wiki/index.php/RTEMS_CVS_Repository, http://www.rtems.org/wiki/index.php/RTEMS_CVS_Repository}.  The steps required
to obtain the source code from CVS are usually like the following:
@example
$ cvs -d :pserver:anoncvs@@www.rtems.com:/usr1/CVS login
Logging in to :pserver:anoncvs@@www.rtems.com:2401/usr1/CVS
CVS password:
$ cvs -d :pserver:anoncvs@@www.rtems.com:/usr1/CVS -z 9 co -P rtems  # for the main RTEMS source
$ cvs -d :pserver:anoncvs@@www.rtems.com:/usr1/CVS -z 9 co -P examples-v2  # for examples
@end example

@section Add <INSTALL_POINT>/bin to Executable PATH

In order to compile RTEMS, you must have the cross compilation toolset
in your search path.  It is important to have the RTEMS toolset first
in your path to ensure that you are using the intended version of all
tools.  The following command prepends the directory where
the tools were installed in a previous step.  If you are using
binaries provided by the RTEMS Project, the <INSTALL_POINT> will be
@code{/opt/rtems-@value{RTEMSAPI}}

@example
export PATH=<INSTALL_POINT>/bin:$@{PATH@}
@end example

@b{NOTE:}  The above command is in Bourne shell (@code{sh}) syntax and should
work with the Korn (@code{ksh}) and GNU Bourne Again Shell (@code{bash}).
It will not work with the C Shell (@code{csh}) or derivatives of the
C Shell.

@section Verifying the Operation of the Cross Toolset

In order to ensure that the cross-compiler is invoking the correct
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
m68k-rtems@value{RTEMSAPI}-gcc -v -S f.c
@end example

Where @code{m68k} should be changed to match the target architecture
of your cross compiler.  The result of this command will be a sequence
of output showing where the cross-compiler searched for and found
its subcomponents.  Verify that these paths correspond to your
<INSTALL_POINT>.

Look at the created file @code{f.s} and verify that it is in fact
for your target processor.

Then try to compile the file @code{f.c} directly to object code
using a command like the following:

@example
m68k-rtems@code{RTEMSAPI}-gcc -v -c f.c
@end example

If this produces messages that indicate the assembly code is not valid,
then it is likely that you have fallen victim to one of the problems
described in @ref{Error Message Indicates Invalid Option to Assembler}
Please do not feel bad about this and do not give up, one of the most
common installation errors is for the cross-compiler not to be able
to find the cross assembler and default to using the native @code{as}.
This can result in very confusing error messages.

@section Building RTEMS for a Specific Target and BSP

This section describes how to configure and build RTEMS
so that it is specifically tailored for your BSP (Board Support Package)
and the CPU model it uses.  There is currently only one supported
method to compile and install RTEMS:

@itemize @bullet
@item direct invocation of @code{configure} and @code{make}
@end itemize

Direct invocation of @code{configure} and @code{make} provides more control
and easier recovery from problems when building.

This section describes how to build RTEMS.

@subsection Using the RTEMS configure Script Directly

Make a build directory under tools and build the RTEMS product in this
directory. The @code{../rtems-@value{RTEMSAPI}.<VERSION>/configure}
command has numerous command line arguments. These arguments are
discussed in detail in documentation that comes with the RTEMS
distribution. A full list of these arguments can be obtained by running
@code{../rtems-@value{RTEMSAPI}.<VERSION>/configure --help} If you
followed the procedure described in the section @ref{Unarchive the
RTEMS Source} or @ref{Obtaining the RTEMS Source from CVS}, these configuration options can be found in the file
rtems-@value{RTEMSAPI}.<VERSION>/README.configure.

@b{NOTE}: The GNAT/RTEMS run-time implementation is based on the POSIX
API and the GNAT/RTEMS run-time cannot be compiled with networking
disabled. Your application does not have to use networking but it must
be enabled.  Thus the RTEMS configuration for a GNAT/RTEMS environment
MUST include the @code{--enable-posix --enable-networking} flag.

@b{NOTE}: Building RTEMS requires that a few support programs be compiled
natively.  This means there must be a native toolchain installed on your
development host. You will need to have a native compiler such as @i{gcc}
or @i{cc} in your execution path.  If you cannot compile, link and execute
a native hello world program, then you will be unable to build RTEMS.

The following shows the command sequence required to configure,
compile, and install RTEMS with the POSIX API, FreeBSD TCP/IP,
and C++ support disabled.  RTEMS will be built to target
the @code{BOARD_SUPPORT_PACKAGE} board.

@example
mkdir build-rtems
cd build-rtems
../rtems-@value{RTEMSAPI}.VERSION/configure --target=<TARGET_CONFIGURATION> \
    --disable-posix --disable-networking --disable-cxx \
    --enable-rtemsbsp=<BSP>\
    --prefix=<INSTALL_POINT>
make all
make install
@end example

<TARGET> is of the form <CPU>-rtems@value{RTEMSAPI} and the list of
currently supported <TARGET> configuration's and <BSP>'s can be found in
@code{tools/RTEMS-@value{RTEMSAPI}.<VERSION>/README.configure}.

<INSTALL_POINT> is typically the installation point for the tools and
defaults to @code{/opt/rtems-@value{RTEMSAPI}}.

BSP is a supported BSP for the selected CPU family.
The list of supported BSPs may be found in the file
@code{tools/rtems-@value{RTEMSAPI}.<VERSION>/README.configure} in the
RTEMS source tree.  If the BSP parameter is not specified, then all
supported BSPs for the selected CPU family will be built.

@b{NOTE:} The POSIX API and networking must be enabled to use GNAT/RTEMS.

@b{NOTE:} The @code{make} utility used should be GNU make.
