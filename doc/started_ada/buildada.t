@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GNAT Cross Compiler Toolset

This chapter describes the steps required to acquire the
source code for a GNU cross compiler toolset, apply 
any required RTEMS specific patches, compile that 
toolset and install it.

@section Create the Archive and Build Directories

Start by making the @code{archive} directory to contain the downloaded 
source code and the @code{tools} directory to be used as a build
directory.  The command sequence to do this is shown
below:

@example
mkdir archive
mkdir tools
@end example

This will result in an initial directory structure similar to the
one shown in the following figure:

@example
@group
/whatever/prefix/you/choose/
        archive/
        tools/

@end group
@end example

@c @ifset use-html
@c @html
@c <IMG SRC="sfile12c.jpg" WIDTH=417 HEIGHT=178 
@c     ALT="Starting Directory Organization">
@c @end html
@c @end ifset

@section Get All the Pieces 

This section lists the components of an RTEMS cross development system.
Included are the locations of each component as well as any required RTEMS
specific patches.

@subheading @value{GCC-VERSION}
@example
    FTP Site:    @value{GCC-FTPSITE}
    Directory:   @value{GCC-FTPDIR}
    File:        @value{GCC-TAR}
@ifset use-html
    URL:         @href{Download @value{GCC-VERSION},,,ftp://@value{GCC-FTPSITE}@value{GCC-FTPDIR}/@value{GCC-TAR}}
@end ifset
@end example

@subheading @value{GNAT-VERSION}
@example
    FTP Site:    @value{GNAT-FTPSITE}
    Directory:   @value{GNAT-FTPDIR}
    File:        @value{GNAT-TAR}
@ifset use-html
    URL:         @href{Download @value{GNAT-VERSION},,,ftp://@value{GNAT-FTPSITE}@value{GNAT-FTPDIR}/@value{GNAT-TAR}}
@end ifset
@end example

@subheading @value{BINUTILS-VERSION}
@example
    FTP Site:    @value{BINUTILS-FTPSITE}
    Directory:   @value{BINUTILS-FTPDIR}
    File:        @value{BINUTILS-TAR}
@ifset use-html
    URL:         @href{Download @value{BINUTILS-VERSION},,,ftp://@value{BINUTILS-FTPSITE}@value{BINUTILS-FTPDIR}/@value{BINUTILS-TAR}}
@end ifset
@end example

@subheading @value{NEWLIB-VERSION}
@example
    FTP Site:    @value{NEWLIB-FTPSITE}
    Directory:   @value{NEWLIB-FTPDIR}
    File:        @value{NEWLIB-TAR}
@ifset use-html
    URL:         @href{Download @value{NEWLIB-VERSION},,,ftp://@value{NEWLIB-FTPSITE}@value{NEWLIB-FTPDIR}/@value{NEWLIB-TAR}}
@end ifset
@end example

@subheading @value{RTEMS-VERSION}
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}
    File:        @value{RTEMS-TAR}
@ifset use-html
    URL:         @href{Download RTEMS components,,,ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}}
@end ifset
@end example

@subheading RTEMS Hello World
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}
    File:        hello_world_ada.tgz
@ifset use-html
    URL:         @href{Download RTEMS Hello World,,,ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/ada_tools/hello_world_ada.tgz}
@end ifset       
@end example

@subheading RTEMS Specific Tool Patches and Scripts
@example
    FTP Site:    @value{RTEMS-FTPSITE}
    Directory:   @value{RTEMS-FTPDIR}/ada_tools
    File:        @value{BUILDTOOLS-TAR}
@ifset BINUTILS-RTEMSPATCH
    File:        @value{BINUTILS-RTEMSPATCH}
@end ifset
@ifset NEWLIB-RTEMSPATCH
    File:        @value{NEWLIB-RTEMSPATCH}
@end ifset
@ifset GCC-RTEMSPATCH
    File:        @value{GCC-RTEMSPATCH}
@end ifset
@ifset GNAT-RTEMSPATCH
    File:        @value{GNAT-RTEMSPATCH}
@end ifset
@ifset use-html
    URL:         @href{Download RTEMS patches,,,ftp://@value{RTEMS-FTPSITE}@value{RTEMS-FTPDIR}/ada_tools}
@end ifset
@end example

@section Unarchiving the Tools

While in the @code{tools} directory, unpack the compressed
tar files using the following command sequence: 

@example
cd tools
tar xzf ../archive/@value{GCC-TAR}
tar xzf ../archive/@value{GNAT-TAR}
tar xzf ../archive/@value{BINUTILS-TAR}
tar xzf ../archive/@value{NEWLIB-TAR}
tar xzf ../archive/@value{BUILDTOOLS-TAR}
@end example

After the compressed tar files have been unpacked, the following
directories will have been created under tools. 

@itemize @bullet
@item @value{BINUTILS-UNTAR}
@item @value{GCC-UNTAR}
@item @value{GNAT-UNTAR}
@item @value{NEWLIB-UNTAR}
@end itemize

There will also be a set of scripts in the current directory
which aid in building the tools and RTEMS.  They are:

@itemize @bullet
@item bit_ada
@item bit_gdb
@item bit_rtems
@item common.sh
@item user.cfg
@end itemize

When the @code{bit_ada} script is executed later in this process,
it will automatically create two other subdirectories:

@itemize @bullet
@item src
@item build-$@{CPU@}-tools
@end itemize

Similarly, the @code{bit_gdb} script will create the
subdirectory @code{build-$@{CPU@}-gdb} and
the @code{bit_rtems} script will create the
subdirectory @code{build-$@{CPU@}-rtems}.

The directory tree should look something like the following figure:

@example
@group
/whatever/prefix/you/choose/
        archive/
            @value{GCC-TAR}
            @value{GNAT-TAR}
            @value{BINUTILS-TAR}
            @value{NEWLIB-TAR}
            @value{RTEMS-TAR}
            @value{BUILDTOOLS-TAR}
@ifset GCC-RTEMSPATCH
            @value{GCC-RTEMSPATCH}
@end ifset
@ifset BINUTILS-RTEMSPATCH
            @value{BINUTILS-RTEMSPATCH}
@end ifset
@ifset NEWLIB-RTEMSPATCH
            @value{NEWLIB-RTEMSPATCH}
@end ifset
@ifset GNAT-RTEMSPATCH
            @value{GNAT-RTEMSPATCH}
@end ifset
            hello_world_ada.tgz
            bit_ada
        tools/
            @value{BINUTILS-UNTAR}/
            @value{GCC-UNTAR}/
            @value{GNAT-UNTAR}/
            @value{NEWLIB-UNTAR}/
            @value{RTEMS-UNTAR}/
            bit_ada
            bit_gdb
            bit_rtems
            common.sh
            user.cfg
@end group
@end example

@c @ifset use-html
@c @html
@c <IMG SRC="bit_ada.jpg" WIDTH=816 HEIGHT=267 ALT="Directory Organization">
@c @end html
@c @end ifset

@c
@c  Host Specific Notes
@c

@section Host Specific Notes

@subsection Solaris 2.x

The build scripts are written in "shell".  The program @code{/bin/sh}
on Solaris 2.x is not robust enough to execute these scripts.  If you 
are on a Solaris 2.x host, then change the first line of the files
@code{bit_ada}, @code{bit_gdb}, and @code{bit_rtems} to use the
@code{/bin/ksh} shell instead.

@c
@c  Reading the Documentation
@c

@section Reading the Tools Documentation

Each of the tools in the GNU development suite comes with documentation.
It is in the reader's and tool maintainers' interest that one read the
documentation before posting a problem to a mailing list or news group.


@c
@c  GCC patches
@c

@section Apply RTEMS Patch to GCC

@ifclear GCC-RTEMSPATCH
No RTEMS specific patches are required for @value{GCC-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GCC-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GCC-UNTAR}
zcat ../../archive/@value{GCC-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GCC-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied.

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
zcat ../../archive/@value{BINUTILS-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{BINUTILS-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied. 

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
zcat ../../archive/@value{NEWLIB-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{NEWLIB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied. 

@end ifset

@c  
@c  GNAT patches
@c

@section Apply RTEMS Patch to GNAT

@ifclear GNAT-RTEMSPATCH
No RTEMS specific patches are required for @value{GNAT-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GNAT-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GNAT-UNTAR}
zcat ../../archive/@value{GNAT-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GNAT-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file which is properly applied.

@end ifset

@c
@c  Copy the ada directory
@c 


@section Copy the ada Subdirectory to the GCC Source Tree

Copy the ada subtree in the patched subtree of
tools/@value{GNAT-UNTAR}/src to the
tools/@value{GCC-UNTAR} directory:

@example
cd tools/@value{GNAT-UNTAR}/src
cp -r ada ../../@value{GCC-UNTAR}
@end example

@c
@c  Localizing the Configuration
@c

@section Localizing the Configuration

Edit the @code{user.cfg} file to alter the settings of various
variables which are used to tailor the build process.
Each of the variables set in @code{user.cfg} may be modified
as described below:

@table @code
@item INSTALL_POINT
is the location where you wish the GNU C/C++ cross compilation tools for
RTEMS to be built. It is recommended that the directory chosen to receive
these tools be named so that it is clear from which egcs distribution it
was generated and for which target system the tools are to produce code for.

@b{WARNING}: The @code{INSTALL_POINT} should not be a subdirectory
under the build directory.  The build directory will be removed
automatically upon successful completion of the build procedure.

@item BINUTILS
is the directory under tools that contains @value{BINUTILS-UNTAR}.
For example:

@example
BINUTILS=@value{BINUTILS-UNTAR}
@end example

@item GCC
is the directory under tools that contains @value{GCC-UNTAR}.
For example,

@example
GCC=@value{GCC-UNTAR}
@end example

Note that the gnat version is not needed because the gnat source 
is built as part of building gcc.

@item NEWLIB
is the directory under tools that contains @value{NEWLIB-UNTAR}.
For example:

@example
NEWLIB=@value{NEWLIB-UNTAR}
@end example

@item BUILD_DOCS
is set to "yes" if you want to install documentation.
For example:

@example
BUILD_DOCS=yes
@end example

@item BUILD_OTHER_LANGUAGES
is set to "yes" if you want to build languages other than C and C++.  At
the current time, this enables Fortan and Objective-C.
For example:

@example
BUILD_OTHER_LANGUAGES=yes
@end example

@item RTEMS
is the directory under tools that contails @value{RTEMS-UNTAR}.

@item ENABLE_RTEMS_POSIX
is set to "yes" if you want to enable the RTEMS POSIX API support.
At this time, this feature is not supported by the UNIX ports of RTEMS
and is forced to "no" for those targets.  This corresponds to the
@code{configure} option @code{--enable-posix}.

This must be enabled to support the GNAT/RTEMS run-time.

@item ENABLE_RTEMS_TESTS
is set to "yes" if you want to build the RTEMS Test Suite.  If this
is set to "no", then only the Sample Tests will be built.
This corresponds to the @code{configure} option @code{--enable-tests}.

@item ENABLE_RTEMS_TCPIP
is set to "yes" if you want to build the RTEMS TCP/IP Stack.  If a
particular BSP does not support TCP/IP, then this feature is automatically
disabled.  This corresponds to the @code{configure} option
@code{--enable-tcpip}.

@item ENABLE_RTEMS_CXX
is set to "yes" if you want to build the RTEMS C++ support including
the C++ Wrapper for the Classic API.  This corresponds to the
@code{configure} option @code{--enable-cxx}.
@end table

@section Running the bit_ada Script

After the @code{bit_ada} script has been modified to reflect the
local installation, the modified @code{bit_ada} script is run
using the following sequence:

@example
cd tools
./bit_ada <target configuration>
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

NOTE:  The above list of target configurations is the list of RTEMS supported
targets.  Only a subset of these have been tested with GNAT/RTEMS.  For more
information, contact your GNAT/RTEMS representative.

If no errors are encountered, the @code{bit_ada} script will conclude by 
printing messages similar to the following:

@example

The src and build-i386-tools subdirectory may now be removed.

Started:  Fri Apr 10 10:14:07 CDT 1998
Finished: Fri Apr 10 12:01:33 CDT 1998
@end example

If the @code{bit_ada} script successfully completes, then the
GNU C/C++ cross compilation tools are installed.

If the @code{bit_ada} script does not successfully complete, then investigation
will be required to determine the source of the error.

