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
    File:        bit_ada
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
@end example

After the compressed tar files have been unpacked, the following
directories will have been created under tools. 

@itemize @bullet
@item @value{BINUTILS-UNTAR}
@item @value{GCC-UNTAR}
@item @value{GNAT-UNTAR}
@item @value{NEWLIB-UNTAR}
@end itemize

Copy the @code{bit_ada} script from the @code{archive} directory
to the @code{tools} directory as shown below:

@example
cp ../archive/bit_ada .
@end example

When the @code{bit_ada} script is executed later in this process, 
it will automatically create two other subdirectories:

@itemize @bullet
@item src
@item build-$@{CPU@}-tools
@end itemize

The directory tree should look something like the following figure:

@example
@group
/whatever/prefix/you/choose/
        bit
        archive/
            @value{GCC-TAR}
            @value{GNAT-TAR}
            @value{BINUTILS-TAR}
            @value{NEWLIB-TAR}
            @value{RTEMS-TAR}
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
            hello_world_c.tgz
            bit
        tools/
            @value{BINUTILS-UNTAR}/
            @value{GCC-UNTAR}/
            @value{GNAT-UNTAR}/
            @value{NEWLIB-UNTAR}/
            @value{RTEMS-UNTAR}/
            bit

@end group
@end example

@c @ifset use-html
@c @html
@c <IMG SRC="bit_ada.jpg" WIDTH=816 HEIGHT=267 ALT="Directory Organization">
@c @end html
@c @end ifset

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
zcat archive/@value{GCC-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GCC-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file.

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
zcat archive/@value{BINUTILS-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{BINUTILS-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file. 

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
zcat archive/@value{NEWLIB-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence: 

@example
cd tools/@value{NEWLIB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file. 

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
zcat archive/@value{GNAT-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GNAT-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file.

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
cp -r ada ../../../@value{GCC-UNTAR}
@end example

@c
@c  Modify the bit_ada script
@c

@section Modify the bit_ada Script

Copy the @code{bit_ada} script from @code{archive} to the @code{tools}
directory.

Edit the @code{bit_ada} file to alter the following environmental variables:

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
BINUTILS=@value{BINUTILS-UNTAR}
GCC=@value{GCC-UNTAR}
NEWLIB=@value{NEWLIB-UNTAR}
BUILD_DOCS=yes
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
For example:

@example
BINUTILS=@value{BINUTILS-UNTAR}
@end example

@item GCC
is the directory under tools that contains @value{GCC-UNTAR}.
For example:

@example
GCC=@value{GCC-UNTAR}
@end example

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

