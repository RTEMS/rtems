@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GNU C/C++ Cross Compiler Toolset

@section Get all the pieces 

The tree structure in the figure below is assumed to be present in the
following discussions: 

Gather the components that will be required for the installation and place
them in an archive directory. Call this directory arc. Be sure that there
is sufficient space to hold all necessary information. This will amount to
approximately 20 megabytes. 

@example
egcs 1.0.2
    FTP Site:    egcs.cygnus.com
    Directory:   /pub/egcs/releases/egcs-1.0.2
    File:        egcs-1.0.2-980309-prerelease.tar.gz 

binutils 2.8.1
    FTP Site:    ftp.gnu.org
    Directory:   /pub/gnu
    File:        binutils-2.8.1.tar.gz

newlib 1.8.0
    FTP Site:    ftp.cygnus.com
    Directory:   /pub/newlib
    File:        newlib-1.8.0.tar.gz

RTEMS @value{version}
FTP Site:        ftp.oarcorp.com
    Directory:   /oarcorp/private/snapshots
    File:        rtems-980219.tgz
    File:        bit
    File:        binutils-2.8.1-rtems-diff-971221.gz
    File:        newlib-1.8.0-diff.980120.gz
    File:        simple_app.tgz
@end example


@section Create the tools directory

Create a directory called tools that will serve as a working directory to
perform the build of the cross compiler tools.

Unpack the compressed tar files using the following command sequence: 

@example
cd tools
tar xzf ../arc/egcs-1.0.2-980309-prerelease.tar.gz
tar xzf ../arc/binutls-2.8.1.tar.gz
tar xzf ../arc/newlib-1.8.0.tar.gz
@end example

After the compressed tar files have been unpacked, the following
directories will have been created under tools. 

@itemize @bullet
@item binutils-2.8.1
@item egcs-1.0.2
@item newlib-1.8.0
@end itemize

@section Apply patches for newlib

Apply the patches using the following command sequence:

@example
cd tools/newlib-1.8.0
zcat arc/newlib-1.8.0-diff.980120.gz|patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/newlib-1.8.0
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file. 

To see the files that have been modified use the sequence:

@example
cd tools/newlib-1.8.0
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file. 


@section Apply patches for binutils

Apply the patches using the following command sequence:

@example
cd tools/binutils-2.8.1
zcat arc/binutils-2.8.1-rtems-diff-971221.gz|patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/binutils-2.8.1
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file. 

To see the files that have been modified use the sequence:

@example
cd tools/binutils-2.8.1
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file. 

@section Modify the bit script

Copy the bit file from arc to the tools directory.

Edit the bit file to alter the following environmental variables:

@itemize @bullet
@item INSTALL_POINT
@item BINUTILS
@item NEWLIB
@item GCC
@item BUILD_DOCS
@end itemize

These variables are located in the script section that resembles the
extract below: 


@example
# USERCHANGE -- localize these.
#
#  INSTALL_POINT: Directory tools are installed into.
#      Recommended installation point for various OS's:
#         Linux:    /usr/local/rtems
#         Solaris:  /opt/gnu/rtems
#   BINUTILS:     Binutils source directory
#   NEWLIB:       Newlib source directory
#   GCC:          Newlib source directory
#   BUILD_DOCS:   Set to "yes" if you want to install documentation.
#
BINUTILS=gas-971208
GCC=egcs-1.0.1
NEWLIB=newlib-1.8.0
BUILD_DOCS=yes
INSTALL_POINT=/home/joel/$@{GCC@}/$@{target@}

# USERCHANGE - uncomment this if you want to watch the commands.
@end example


Where:

@table @code
@item INSTALL_POINT
is the location where you wish the GNU C/C++ cross compilation tools for
RTEMS to be built. It is recommended that the directory chosen to receive
these tools be named so that it is clear from which egcs distribution it
was generated and for which target system the tools are to produce code for.

@item BINUTILS
is the directory under tools that contains binutils-2.8.1. 
BINUTILS=binutils-2.8.1

@item GCC
is the directory under tools that contains egcs-1.0.1. 
GCC=egcs-1.0.2-980309-prerelease

@item NEWLIB
is the directory under tools that contains newlib-1.8.0.
NEWLIB=newlib-1.8.0

@item BUILD_DOCS
is set to "yes" if you want to install documentation.
BUILD_DOCS=yes

@end table

@section Running the bit script

Run the modified bit script using the following sequence:

@example
cd tools
./bit <target configuration>
@end example

Where:

<target configuration> is one of the following:

@example
hppa1.1
i386
i386-elf
i386-go32
i960
m68k
mips64orion
powerpc
sh
sparc
@end example

@section GNU C/C++ cross compiler toolset complete
 
At this point the GNU C/C++ cross compile tools should be built.

