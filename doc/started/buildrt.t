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
tar xzf ../arc/@value{RTEMS-TAR}
@end example

@section Add <INSTALL_POINT>/bin to Executable PATH

In order to compile RTEMS, you must have the cross compilation toolset
in your search patch.  The following command appends the directory
where the tools were installed in the previous chapter:

@example
export PATH=$PATH:<INSTALL_POINT>/bin
@end example

NOTE:  The above command is in Bourne shell (@code{sh}) syntax and should work with
the Korn (@code{ksh}) and GNU Bourne Again Shell (@code{bash}).  It will not
work with the C Shell (@code{csh})or derivatives of the C Shell.

@section Generate RTEMS for a Specific Target and BSP

Make a build directory under tools and build the RTEMS product in this
directory. The ../@value{RTEMS-UNTAR}/configure
command has numerous command line
arguments. These arguments are discussed in detail in documentation that
comes with the RTEMS distribution. In the installation described in the
section "Unpack the RTEMS source", these configuration options can be found
in file tools/@value{RTEMS-UNTAR}/README.configure.

The following shows the command sequence required to configure,
compile, and install RTEMS with the POSIX API, KA9Q TCP/IP,
and C++ support disabled.  RTEMS will be built to target 
the @code{BOARD_SUPPORT_PACKAGE} board.

@example
mkdir build-rtems
cd build-rtems
../@value{RTEMS-UNTAR}/configure --target=<TARGET_CONFIGURATION> \
    --disable-posix --disable-ka9q --disable-cxx \
    --enable-rtemsbsp=<BOARD_SUPPORT_PACKAGE>\
    --prefix=<INSTALL_POINT>
gmake all install
@end example

Where the list of currently supported of <TARGET_CONFIGURATION>'s and
<BOARD_SUPPORT_PACKAGE>'s can be found in
tools/@value{RTEMS-UNTAR}/README.configure.

<INSTALL_POINT> is the installation point from the previous step
"Modify the bit script" in the build of the tools. 


