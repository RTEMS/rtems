@c
@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building RTEMS

@section Unpack the RTEMS Source

Use the following command sequence to unpack the RTEMS source into the
tools directory: 

@example
cd tools
tar xzf ../archive/@value{RTEMS-TAR}
@end example

@section Add <INSTALL_POINT>/bin to Executable PATH

In order to compile RTEMS, you must have the cross compilation toolset
in your search patch.  The following command appends the directory
where the tools were installed in the previous chapter:

@example
export PATH=$PATH:<INSTALL_POINT>/bin
@end example

@b{NOTE:}  The above command is in Bourne shell (@code{sh}) syntax and should
work with the Korn (@code{ksh}) and GNU Bourne Again Shell (@code{bash}).  
It will not work with the C Shell (@code{csh})or derivatives of the C Shell.

@section Verifying the Operation of the Cross Toolset

In order to insure that the cross-compiler is invoking the correct
subprograms (like @code{as} and @code{ld}), one can test assemble
a small program.  When in verbose more, @code{gcc} prints out information
showing where it found the subprograms it invokes.  Place the following
function in a file named @code{f.c}:

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

@b{NOTE:} One of the most common installation errors is for the
cross-compiler not to be able to find the cross assembler and default
to using the native @code{as}.  This can result in very confusing
error messages.

@section Generate RTEMS for a Specific Target and BSP

@subsection Using the bit_rtems script

The simplest way to build RTEMS is to use the @code{bit_rtems} script.
This script interprets the settings in the @code{user.cfg} file to
enable or disable the various RTEMS options.  

This script is invoked as follows: 

@example
./bit_rtems CPU [BSP]
@end example

Where CPU is one of the RTEMS supported CPU families from the following
list:

@itemize @bullet
@item hppa1.1
@item i386
@item i386-elf
@item i386-go32
@item i960
@item m68k
@item mips64orion
@item powerpc
@item sh
@item sparc
@end itemize

BSP is a supported BSP for the selected CPU family.  The list of
supported BSPs may be found in the file 
tools/@value{RTEMS-UNTAR}/README.configure
in the RTEMS source tree.  If the BSP parameter is not specified,
then all supported BSPs for the selected CPU family will be built.

@b{NOTE:}  The POSIX API must be enabled to use GNAT/RTEMS.

@subsection Using the RTEMS configure Script Directly

Make a build directory under tools and build the RTEMS product in this
directory. The ../@value{RTEMS-UNTAR}/configure
command has numerous command line
arguments. These arguments are discussed in detail in documentation that
comes with the RTEMS distribution. In the installation described in the
section "Unpack the RTEMS source", these configuration options can be found
in the file tools/@value{RTEMS-UNTAR}/README.configure.

The GNAT/RTEMS run-time implementation is based on the POSIX API.  Thus
the RTEMS configuration for a GNAT/RTEMS environment MUST include the
@code{--enable-posix} flag.

The following shows the command sequence required to configure,
compile, and install RTEMS with the POSIX API enabled.
RTEMS will be built to target the @code{BOARD_SUPPORT_PACKAGE} board.

@example
mkdir build-rtems
cd build-rtems
../@value{RTEMS-UNTAR}/configure --target=<TARGET_CONFIGURATION> \
    --enable-posix \
    --enable-rtemsbsp=<BOARD_SUPPORT_PACKAGE>\
    --prefix=<INSTALL_POINT>
gmake all install
@end example

Where the list of currently supported of <TARGET_CONFIGURATION>'s and
<BOARD_SUPPORT_PACKAGE>'s can be found in
tools/@value{RTEMS-UNTAR}/README.configure.

<INSTALL_POINT> is the installation point from the previous step
"Modify the bit_ada Script" in the build of the tools. 


