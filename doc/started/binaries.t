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
RedHat and SuSE.
@c RPM supports other operating systems including
@c Cygwin.  @uref{mailto:D.J@@fiddes.surfaid.org,David Fiddes <D.J@@fiddes.surfaid.org>}
@c did the initial groundwork that lead to Cygwin RPMs being available.

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

@item If buildable for a particular CPU, RPMs are provided for 
Ada (gnat), Chill, Java (gcj), Fortran (g77), and Objective-C (objc).
These binaries are strictly optional.

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
rpm -i rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
rpm -i sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
rpm -i rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -i sparc-rtems-c++-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -i sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -i rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
rpm -i sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
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
rpm -q -g rtems
@end example

@subsection Removing RPMs

The following is a sample session illustrating the removal
of a C/C++ toolset targeting the SPARC architecture.

@example
rpm -e sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
rpm -e rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.i386.rpm
rpm -e sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -e sparc-rtems-c++-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -e rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.i386.rpm
rpm -e sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
rpm -e rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.i386.rpm
@end example

NOTE:  If you have installed any RTEMS BSPs, then it is likely that
RPM will complain about not being able to remove everything.

@section Zipped Tar Files

This section provides information on installing and removing
Zipped Tar Files (.tgz).

@subsection Installing Zipped Tar Files

The following is a sample session illustrating the installation
of a C/C++ toolset targeting the SPARC architecture assuming
that GNU tar is installed as @code{tar}:

@example
cd /
tar xzf rtems-base-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.tgz
tar xzf sparc-rtems-binutils-@value{BINUTILSVERSION}-@value{BINUTILSRPMRELEASE}.tgz
tar xzf rtems-base-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.tgz
tar xzf sparc-rtems-gcc-gcc@value{GCCVERSION}newlib@value{NEWLIBVERSION}-@value{GCCRPMRELEASE}.tgz
tar xzf rtems-base-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.tgz
tar xzf sparc-rtems-gdb-@value{GDBVERSION}-@value{GDBRPMRELEASE}.tgz
@end example

Upon successful completion of the above command sequence, a
C/C++ cross development toolset targeting the SPARC is
installed in @code{@value{RTEMSPREFIX}}.  In order to use this toolset,
the directory @code{@value{RTEMSPREFIX}} must be included in your
PATH.

@subsection Removing Zipped Tar Files

There is no automatic way to remove the contents of a @code{tgz} once
it is installed.  The contents of the directory @code{@value{RTEMSPREFIX}}
can be removed but this will likely result in other packages
being removed as well.


