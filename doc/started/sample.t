@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the Sample Application

@section Unpack the sample application 

Use the following command to unarchive the sample application:

@example
cd tools
tar xzf ../arc/simple_app.tgz
@end example

@section Set the environment variable RTEMS_MAKEFILE_PATH 

It must point to the appropriate directory containing RTEMS build for our
target and board support package combination. 

@example
export RTEMS_MAKEFILE_PATH = \
<INSTALLATION_POINT>/rtems/<BOARD_SUPPORT_PACKAGE>
@end example

Where:

<INSTALLATION_POINT> and <BOARD_SUPPORT_PACKAGE> are those used in step 4.3

@section Build the Sample Application

Use the following command to start the build of the sample application: 

@example
cd tools/simple_app
gmake
@end example

If no errors are detected during the sample application build, it is
reasonable to assume that the build of the GNU C/C++ Cross Compiler Tools
for RTEMS and the build of the RTEMS OS for the selected host and target
combination was done properly. 

@section Application executable 

If the application build has taken place without error, the application
executable is placed in the following directory: 

@example
tools/simple_app/o-<BOARD_SUPPORT_PACKAGE>/<filename>.exe
@end example

