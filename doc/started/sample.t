@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the Sample Application

@section Unarchive the Sample Application 

Use the following command to unarchive the sample application:

@example
cd tools
tar xzf ../archive/hello_world_c.tgz
@end example

@section Set the Environment Variable RTEMS_MAKEFILE_PATH 

RTEMS_MAKEFILE_PATH must point to the appropriate directory containing
RTEMS build for our target and board support package combination. 

@example
export RTEMS_MAKEFILE_PATH=<INSTALLATION_POINT>/<BOARD_SUPPORT_PACKAGE>
@end example

Where <INSTALLATION_POINT> and <BOARD_SUPPORT_PACKAGE> are those used when
configuring and installing RTEMS.

NOTE:  In release 4.0, BSPs were installed at
@code{<INSTALLATION_POINT>/rtems/<BOARD_SUPPORT_PACKAGE>}.  This
was changed to be more in compliance with GNU standards.

@section Build the Sample Application

Use the following command to start the build of the sample hello 
world application: 

@example
cd hello_world_c
make
@end example

NOTE: GNU make is the preferred @code{make} utility.  Other @code{make}
implementations may work but all testing is done with GNU make.

If no errors are detected during the sample application build, it is
reasonable to assume that the build of the GNU C/C++ Cross Compiler Tools
for RTEMS and RTEMS itself for the selected host and target
combination was done properly. 

@section Application Executable 

If the sample application has successfully been built, then the application
executable is placed in the following directory: 

@example
hello_world_c/o-optimize/<filename>.exe
@end example

How this executable is downloaded to the target board is very dependent
on the BOARD_SUPPORT_PACKAGE selected.  The following is a list of
commonly used BSPs classified by their RTEMS CPU family and pointers
to instructions on how to use them.  [NOTE: All file names should be
prepended with @value{RTEMS-UNTAR}/c/src/lib/libbsp.]

@need 1000
@table @b

@item i386/pc386
See @code{i386/pc386/HOWTO}

@item i386/pc486
The i386/pc386 BSP specially compiled for an i486-class CPU.

@item i386/pc586
The i386/pc386 BSP specially compiled for a Pentium-class CPU.

@item i386/pc686
The i386/pc386 BSP specially compiled for a Pentium II.

@item i386/pck6
The i386/pc386 BSP specially compiled for an AMD K6.

@item m68k/gen68360
This BSP is for a MC68360 CPU.  See @code{m68k/gen68360/README} for details.

@item m68k/mvme162
See @code{m68k/mvme162/README}.

@item m68k/mvme167
See @code{m68k/mvme167/README}.

@item powerpc/mcp750
See @code{powerpc/motorola_shared/README}.

@item powerpc/mvme230x
See @code{powerpc/motorola_shared/README.MVME2300}.

@item powerpc/psim
This is a BSP for the PowerPC simulator included with @code{powerpc-rtems-gdb}.
The simulator is complicated to initialize by hand.  The user is referred
to the script @code{powerpc/psim/tools/psim}.

@item sparc/erc32
The ERC32 is a radiation hardened SPARC V7.  This BSP can be used with
either real ERC32 hardware or with the simulator included with
@code{sparc-rtems-gdb}.  An application can be run on the simulator
by executing the following commands upon entering @code{sparc-rtems-gdb}: 

@example
target sim
load
run
@end example

@end table

RTEMS has many more BSPs and new BSPs for commercial boards and CPUs
with on-CPU peripherals are generally welcomed.

@section More Information on RTEMS Application Makefiles

The hello world sample application is a simple example of an
RTEMS application that uses the RTEMS Application Makefile 
system.  This Makefile system simplifies building
RTEMS applications by providing Makefile templates and
capturing the configuration information used to build
RTEMS specific to your BSP.  Building an RTEMS application
for different BSPs is as simple as switching the
setting of @code{RTEMS_MAKEFILE_PATH}.  This Makefile
system is described in the file
@code{make/README}.
