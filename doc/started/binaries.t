@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Prebuilt Toolset Executables

Precompiled toolsets are available for Linux, Cygwin, FreeBSD,
and Solaris.  These are packaged in the following formats:

@itemize @bullet
@item Linux - RPM
@item Cygwin - tar.bz2
@item Solaris - tar.bz2
@end itemize

RPM is an acronym for the RPM Package Manager.  RPM is the
native package installer for many Linux distributions including
RedHat, SuSE, and Fedora.

The prebuilt binaries are intended to be easy to install and
the instructions are similar regardless of the host environment.  
There are a few structural issues with the packaging of the RTEMS
Cross Toolset binaries that you need to be aware of.

@enumerate
@item There are dependencies between the various packages.
This requires that certain packages be installed before others may be.
Some packaging formats enforce this dependency.

@item Some packages are target CPU family independent and shared
across all target architectures.   These are referred to as 
"base" packages.

@item Depending upon the version of GCC as well as the development
host and target CPU combination, pre-built supplemental packages may 
be provided for Ada (gnat), Chill, Java (gcj), Fortran (g77), and
Objective-C (objc).  These binaries are strictly optional.

@end enumerate

NOTE: Installing toolset binaries does not install RTEMS itself, only
the tools required to build RTEMS.  See @ref{Building RTEMS} for the next
step in the process.

@section RPMs

This section provides information on installing and removing RPMs.

@subsection Installing RPMs

The following is a sample session illustrating the installation
of a C/C++ toolset targeting the SPARC architecture.  

@example
rpm -i @value{RTEMSRPMPREFIX}rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
rpm -i @value{RTEMSRPMPREFIX}sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
rpm -i @value{RTEMSRPMPREFIX}rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -i @value{RTEMSRPMPREFIX}sparc-rtems-c++-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -i @value{RTEMSRPMPREFIX}sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -i @value{RTEMSRPMPREFIX}rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
rpm -i @value{RTEMSRPMPREFIX}sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
@end example

Upon successful completion of the above command sequence, a 
C/C++ cross development toolset targeting the SPARC is
installed in @code{@value{RTEMSPREFIX}}.  In order to use this toolset,
the directory @code{@value{RTEMSPREFIX}/bin} must be included in your
PATH.

Once you have successfully installed the RPMs for BINUTILS, GCC,
NEWLIB, and GDB, then you may proceed directly to @ref{Building RTEMS}.  

@subsection Determining Which RTEMS RPMs are Installed

The following command will report which RTEMS RPMs are currently
installed:

@example
rpm -q -g @value{RTEMSRPMGROUP}
@end example

@subsection Removing RPMs

The following is a sample session illustrating the removal
of a C/C++ toolset targeting the SPARC architecture.

@example
rpm -e @value{RTEMSRPMPREFIX}sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
rpm -e @value{RTEMSRPMPREFIX}rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
rpm -e @value{RTEMSRPMPREFIX}sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -e @value{RTEMSRPMPREFIX}sparc-rtems-c++-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -e @value{RTEMSRPMPREFIX}rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -e @value{RTEMSRPMPREFIX}sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
rpm -e @value{RTEMSRPMPREFIX}rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
@end example

NOTE:  If you have installed any RTEMS BSPs, then it is likely that
RPM will complain about not being able to remove everything.

@section Zipped Tar Files

This section provides information on installing and removing
Zipped Tar Files (e.g .tar.gz or .tar.bz2).

@subsection Installing Zipped Tar Files

The following is a sample session illustrating the installation
of a C/C++ toolset targeting the SPARC architecture assuming
that GNU tar is installed as @code{tar} for a set of archive
files compressed with GNU Zip (gzip):

@example
cd /
tar xzf @value{RTEMSRPMPREFIX}rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.tar.gz
tar xzf @value{RTEMSRPMPREFIX}rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.tar.gz
tar xzf @value{RTEMSRPMPREFIX}rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.tar.gz
@end example

The following command set is the equivalent command sequence
for the same toolset assuming that is was compressed with
GNU BZip (bzip2):

@example
cd /
tar xjf @value{RTEMSRPMPREFIX}rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.tar.bz2
@end example

Upon successful completion of the above command sequence, a
C/C++ cross development toolset targeting the SPARC is
installed in @code{@value{RTEMSPREFIX}}.  In order to use this toolset,
the directory @code{@value{RTEMSPREFIX}} must be included in your
PATH.

@subsection Removing Zipped Tar Files

There is no automatic way to remove the contents of a @code{tar.gz} 
or @code{tar.bz2} once it is installed.  The contents of the directory
@code{@value{RTEMSPREFIX}} can be removed but this will likely result
in other packages being removed as well.


