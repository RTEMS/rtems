@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the Sample Applications

The RTEMS distribution includes a number of sample C, C++, 
Ada, and networking applications.  This chapter will provide
an overview of those sample applications.

@c
@c Set the Environment Variable RTEMS_MAKEFILE_PATH 
@c 
@section Set the Environment Variable RTEMS_MAKEFILE_PATH 

The sample application sets use the RTEMS Application Makefiles.
This requires that the environment variable 
@code{RTEMS_MAKEFILE_PATH} point to the appropriate directory containing
the installed RTEMS image built to target your particular CPU and
board support package combination. 

@example
export RTEMS_MAKEFILE_PATH=<INSTALLATION_POINT>/<BOARD_SUPPORT_PACKAGE>
@end example

Where <INSTALLATION_POINT> and <BOARD_SUPPORT_PACKAGE> are those used when
configuring and installing RTEMS.

NOTE:  In release 4.0, BSPs were installed at
@code{<INSTALLATION_POINT>/rtems/<BOARD_SUPPORT_PACKAGE>}.  This
was changed to be more in compliance with GNU standards.

NOTE: GNU make is the preferred @code{make} utility.  Other @code{make}
implementations may work but all testing is done with GNU make.

If no errors are detected during the sample application build, it is
reasonable to assume that the build of the GNU Cross Compiler Tools
for RTEMS and RTEMS itself for the selected host and target
combination was done properly. 

@c
@c Executing the Sample Applications
@c
@section Executing the Sample Applications

How each sample application executable is downloaded
to your target board and executed is very dependent
on the board you are using.  The following is a list of
commonly used BSPs classified by their RTEMS CPU family and pointers
to instructions on how to use them.  [NOTE: All file names should be
prepended with @value{RTEMSUNTAR}/c/src/lib/libbsp.]

@need 1000
@table @b

@item arm/edp7312
The arm/edp7312 BSP is for the ARM7-based Cogent EDP7312 board.

@item c4x/c4xsim
The c4x/c4xsim BSP is designed to execute on any member of
the Texas Instruments C3x/C4x DSP family using only on-CPU
peripherals for the console and timers.

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

@item mips/jmr3904
This is a BSP for the Toshiba TX3904 evaluation board
simulator included with @code{mipstx39-rtems-gdb}.  The
BSP is located in @code{mips/jmr3904}.
The TX3904 is a MIPS R3000 class CPU with serial ports and timers
integrated with the processor.  This BSP can be used with
either real  hardware or with the simulator included with
@code{mipstx39-rtems-gdb}.  An application can be run on the simulator
by executing the following commands upon entering @code{mipstx39-rtems-gdb}: 

@example
target sim --board=jmr3904
load
run
@end example

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

@c
@c  C/C++ Sample Applications
@c
@section C/C++ Sample Applications

The C/C++ sample application set includes a number of simple applications.
Some demonstrate some basic functionality in RTEMS such as writing
a file, closing it, and reading it back while others can serve as
starting points for RTEMS applications or libraries.  Start by 
unarchiving them so you can peruse them.  Use a command similar to
the following to unarchive the sample applications:

@example
cd tools
tar xjf ../archive/examples-VERSION.tgz
@end example

The sample applications most likely to be of interest to you are:

@itemize @bullet
@item hello_world_c - C Hello World application with a simple
RTEMS configuration and an entry point not called @code{main()}.

@item simple_main - Very simple program starting at @code{main()}
and shutting down RTEMS via @code{exit()} without any other operations.
It uses the default configuration inside RTEMS which is only
intended to satisfy @code{autoconf} probes and extremely simple
console-based applications.

@item libcpp - Simple C++ library for RTEMS showing how to build an
application library written in C++.

@item psx_sched_report - POSIX Scheduler Reporter is a program
that prints out some scheduler attributes of the RTEMS POSIX API.

@end itemize

Each tests is found in a separate subdirectory and built using the
same command sequence.  The @code{hello_world_c} sample will be used
as an example.

@c
@c Build the C Hello World Application
@c
@subheading Build the C Hello World Application

Use the following command to start the build of the sample hello 
world application: 

@example
cd hello_world_c
make
@end example


If the sample application has successfully been built, then the application
executable is placed in the following directory: 

@example
hello_world_c/o-optimize/<filename>.exe
@end example

The other C/C++ sample applications are built using a similar procedure.

@c
@c  Ada Sample Applications
@c
@section Ada Sample Applications

The Ada sample application set primarily includes a
a simple Hello World Ada program which can be used
as a starting point for GNAT/RTEMS applications.
Use the following command to unarchive the Ada sample
applications:

@example
cd tools
tar xzf ../archive/hello_world_ada.tgz
@end example

@subheading Create a BSP Specific Makefile

Currently, the procedure for building and linking an Ada application
is a bit more difficult than a C or C++ application.  This is certainly
an opportunity for a volunteer project.

At this time, there is a 

Provided are example Makefiles for multiple BSPs.  Copy one of these to
the file Makefile.<BOARD_SUPPORT_PACKAGE> and edit it as appropriate for
your local configuration.

Use the  <INSTALLATION_POINT> and <BOARD_SUPPORT_PACKAGE> specified when
configuring and installing RTEMS.

@section Build the Sample Application

Use the following command to start the build of the sample application: 

@example
cd tools/hello_world_ada
make -f Makefile.<BOARD_SUPPORT_PACKAGE>
@end example

NOTE: GNU make is the preferred @code{make} utility.  Other @code{make}
implementations may work but all testing is done with GNU make.

If the BSP specific modifications to the Makefile were correct and
no errors are detected during the sample application build, it is
reasonable to assume that the build of the GNAT/RTEMS Cross Compiler Tools
for RTEMS and RTEMS itself for the selected host and target
combination was done properly. 

@section Application Executable 

If the sample application has successfully been build, then the application
executable is placed in the following directory: 

@example
tools/hello_world_ada/o-optimize/<filename>.exe
@end example

How this executable is downloaded to the target board is very dependent
on the BOARD_SUPPORT_PACKAGE selected.
----------------------------------------------------

@c
@c More Information on RTEMS Application Makefiles
@c
@section More Information on RTEMS Application Makefiles

The hello world sample application is a simple example of an
RTEMS application that uses the RTEMS Application Makefile 
system.  This Makefile system simplifies building
RTEMS applications by providing Makefile templates and
capturing the configuration information used to build
RTEMS specific to your BSP.  Building an RTEMS application
for different BSPs is as simple as switching the
setting of @code{RTEMS_MAKEFILE_PATH}.  This Makefile
system is described in the file @code{make/README}.
