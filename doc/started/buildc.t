@c
@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building RTEMS

@section Unpack the RTEMS source

Use the following command sequence to unpack the RTEMS source into the
tools directory: 

@example
cd tools
tar xzf ../arc/rtems-980219.tgz
@end example

@section Add the bin directory under the install point to the default PATH

Use the following command to append the <INSTALL_POINT>/bin directory to
the PATH variable: 

@example
PATH=$PATH:<INSTALL_POINT>/bin
@end example

@section Generate RTEMS for a specific target and board support package

Make a build directory under tools and build the RTEMS product in this
directory. The ../rtems-980219/configure command has numerous command line
arguments. These arguments are discussed in detail in documentation that
comes with the RTEMS distribution. In the installation described in the
section "Unpack the RTEMS source", these configuration options can be found
in file:

@example
tools/rtems-980219/README.configure
@end example

A simple example of the configuration appears below:

@example
mkdir build-rtems
cd build-rtems
../rtems-980219/configure --target=<TARGET_CONFIGURATION> \
--disable-posix --disable-ka9q --disable-cpp \
--enable-rtemsbsp=<BOARD_SUPPORT_PACKAGE>\
--prefix=< INSTALL_POINT>
gmake all install
@end example

Where:

The current summary of <TARGET_CONFIGURATION>'s and
<BOARD_SUPPORT_PACKAGE>'s can be found in
tools/rtems-980219/README.configure


<INSTALL_POINT> is the installation point from the previous step
"Modify the bit script" in the build of the tools. 


