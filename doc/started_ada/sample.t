@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the Sample Application

@section Unpack the Sample Application 

Use the following command to unarchive the sample application:

@example
cd tools
tar xzf ../archive/hello_world_ada.tgz
@end example

@section Create a BSP Specific Makefile

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
