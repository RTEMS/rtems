@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Prebuilt Toolset Executables

Precompiled toolsets are available for Linux and Cygwin.  These are
packaged using the RedHat Package Manager (RPM).  RPM is the
native package installer for many Linux distributions including
RedHat and SUSE.  RPM supports other operating systems including
Cygwin.  David Fiddes <D.J@@fiddes.surfaid.org> has graciously
build Cygwin RPMs for a number of popular target CPU families.

RPMs are very easy to install and the instructions are the same
regardless of the host environment.  There are a few structural
issues with the packaging of the RTEMS Cross Toolset RPMs
that you need to be aware of.

@enumerate
@item There are dependencies between the various packages.
This requires that certain packages be installed before others may be.

@item Some packages are target CPU family indepedent and shared
across all target architectures.   These are referred to as 
"base" packages.

@item If buildable for a particular CPU, RPMs are provided for 
Chill, Java (gjc), Fortran (g77), and Objective-C (objc).  These
RPMs are strictly optional.

@end enumerate

@section Installing RPMs

The following is a sample session illustrating the installation
of a C/C++ toolset targeting the SPARC architecture.  

@example
rpm -i rtems-base-binutils-2.9.5.0.24-1.i386.rpm
rpm -i sparc-rtems-binutils-2.9.5.0.24-1.i386.rpm
rpm -i rtems-base-gcc-gcc2.95.2newlib1.8.2-4.i386.rpm
rpm -i sparc-rtems-gcc-gcc2.95.2newlib1.8.2-4.i386.rpm
rpm -i rtems-base-gdb-4.18-2.i386.rpm
rpm -i sparc-rtems-gdb-4.18-2.i386.rpm
@end example

@section Removing RPMs

The following is a sample session illustrating the removal
of a C/C++ toolset targeting the SPARC architecture.

@example
rpm -e sparc-rtems-gdb-4.18-2.i386.rpm
rpm -e rtems-base-gdb-4.18-2.i386.rpm
rpm -e sparc-rtems-gcc-gcc2.95.2newlib1.8.2-4.i386.rpm
rpm -e rtems-base-gcc-gcc2.95.2newlib1.8.2-4.i386.rpm
rpm -e sparc-rtems-binutils-2.9.5.0.24-1.i386.rpm
rpm -e rtems-base-binutils-2.9.5.0.24-1.i386.rpm
@end example

