@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GNU C/C++ Cross Compiler Toolset

This chapter describes the steps required to acquire the
source code for a GNU cross compiler toolset, apply 
any required RTEMS specific patches, compile that 
toolset and install it.

@section Get all the pieces 

The tree structure in the figure below is assumed to be present in the
following discussions: 

@center @b{XXX directory tree figure goes here}

Gather the components that will be required for the installation and place
them in an archive directory. Call this directory arc. Be sure that there
is sufficient space to hold all necessary information. This will amount to
approximately 20 megabytes. 

@subheading @value{GCC-VERSION}
@example
    FTP Site:    @value{GCC-FTPSITE}
    Directory:   @value{GCC-FTPDIR}
    File:        @value{GCC-TAR}
@end example

@subheading @value{BINUTILS-VERSION}
@example
    FTP Site:    @value{BINUTILS-FTPSITE}
    Directory:   @value{BINUTILS-FTPDIR}
    File:        @value{BINUTILS-TAR}
@end example

@subheading @value{NEWLIB-VERSION}
@example
    FTP Site:    @value{NEWLIB-FTPSITE}
    Directory:   @value{NEWLIB-FTPDIR}
    File:        @value{NEWLIB-TAR}
@end example

@subheading @value{RTEMS-VERSION}
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}
    File:        @value{RTEMS-TAR}
    File:        bit
    File:        @value{BINUTILS-RTEMSPATCH}
    File:        @value{NEWLIB-RTEMSPATCH}
    File:        @value{GCC-RTEMSPATCH}
    File:        hello_world_c.tgz
@end example


@section Create the tools directory

Create a directory called tools that will serve as a working directory to
perform the build of the cross compiler tools.

Unpack the compressed tar files using the following command sequence: 

@example
cd tools
tar xzf ../arc/@value{GCC-TAR}
tar xzf ../arc/@value{BINUTILS-TAR}
tar xzf ../arc/@value{NEWLIB-TAR}
@end example

After the compressed tar files have been unpacked, the following
directories will have been created under tools. 

@itemize @bullet
@item @value{BINUTILS-UNTAR}
@item @value{GCC-UNTAR}
@item @value{NEWLIB-UNTAR}
@end itemize

@c
@c  EGCS patches
@c

@section Apply RTEMS Patch to EGCS

@ifclear GCC-RTEMSPATCH
No RTEMS specific patches are required for @value{GCC-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GCC-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GCC-UNTAR}
zcat arc/@value{GCC-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GCC-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file.

To see the files that have been modified use the sequence:

@example
cd tools/@value{GCC-UNTAR}
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file.

@end ifset

@c
@c  BINUTILS patches
@c

@section Apply RTEMS Patch to binutils

@ifclear BINUTILS-RTEMSPATCH
No RTEMS specific patches are required for @value{BINUTILS-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset BINUTILS-RTEMSPATCH
Apply the patch using the following command sequence:

@example
cd tools/@value{BINUTILS-UNTAR}
zcat arc/@value{BINUTILS-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{BINUTILS-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file. 

To see the files that have been modified use the sequence:

@example
cd tools/@value{BINUTILS-UNTAR}
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file. 

@end ifset

@c
@c  Newlib patches
@c

@section Apply RTEMS Patch to newlib

@ifclear NEWLIB-RTEMSPATCH
No RTEMS specific patches are required for @value{NEWLIB-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset NEWLIB-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{NEWLIB-UNTAR}
zcat arc/@value{NEWLIB-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{NEWLIB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file. 

To see the files that have been modified use the sequence:

@example
cd tools/@value{NEWLIB-UNTAR}
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file. 

@end ifset

@c
@c  Modify the bit script
@c

@section Modify the bit script

Copy the @code{bit} script from arc to the tools directory.

Edit the @code{bit} file to alter the following environmental variables:

@itemize @bullet
@item INSTALL_POINT
@item BINUTILS
@item NEWLIB
@item GCC
@item BUILD_DOCS
@item BUILD_OTHER_LANGUAGES
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
#   BUILD_OTHER_LANGUAGES:
#                 Set to "yes" if you want to build Fortran and Objective-C
#
BINUTILS=@value{BINUTILS-UNTAR}
GCC=@value{GCC-UNTAR}
NEWLIB=@value{NEWLIB-UNTAR}
BUILD_DOCS=yes
BUILD_OTHER_LANGUAGES=yes
INSTALL_POINT=/home/joel/$@{GCC@}/$@{target@}

# USERCHANGE - uncomment this if you want to watch the commands.
@end example

Where each of the variables which may be modified is described below:

@table @code
@item INSTALL_POINT
is the location where you wish the GNU C/C++ cross compilation tools for
RTEMS to be built. It is recommended that the directory chosen to receive
these tools be named so that it is clear from which egcs distribution it
was generated and for which target system the tools are to produce code for.

@item BINUTILS
is the directory under tools that contains @value{BINUTILS-UNTAR}. 
For example,

@example
BINUTILS=@value{BINUTILS-UNTAR}.
@end example

@item GCC
is the directory under tools that contains @value{GCC-UNTAR}.
For example,

@example
GCC=@value{GCC-UNTAR}.
@end example

@item NEWLIB
is the directory under tools that contains @value{NEWLIB-UNTAR}.
For example,

@example
NEWLIB=@value{NEWLIB-UNTAR}.
@end example

@item BUILD_DOCS
is set to "yes" if you want to install documentation.
For example, 

@example
BUILD_DOCS=yes
@end example

@item BUILD_OTHER_LANGUAGES
is set to "yes" if you want to build languages other than C and C++.  At
the current time, this enables Fortan and Objective-C.
For example,

@example
BUILD_OTHER_LANGUAGES=yes
@end example

@end table

@section Running the bit script

After the @code{bit} script has been modified to reflect the
local installation, the modified @code{bit} script is run
using the following sequence:

@example
cd tools
./bit <target configuration>
@end example

Where <target configuration> is one of the following:

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

If no errors are encountered, the @code{bit} script will conclude by 
printing messages similar to the following:

@example

The src and build-i386-tools subdirectory may now be removed.

Started:  Fri Apr 10 10:14:07 CDT 1998
Finished: Fri Apr 10 12:01:33 CDT 1998
@end example

If the @code{bit} script successfully completes, then the
GNU C/C++ cross compilation tools are installed.

If the @code{bit} script does not successfully complete, then investigation
will be required to determine the source of the error.

