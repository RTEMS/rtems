@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Prebuilt Toolset Executables

Precompiled toolsets are available for GNU/Linux and MS-Windows. 
Other hosts will need to build from source.  Packaged binaries are
in the following formats:

@itemize @bullet
@item GNU/Linux - RPM
@item Cygwin - tar.bz2
@item Mingw - tar.bz2
@end itemize

RPM is an acronym for the RPM Package Manager.  RPM is the native package
installer for many GNU/Linux distributions including RedHat Enterprise
Linux, Centos, SuSE, and Fedora.

The RTEMS Project maintains a Yum Repository which makes it quite simple
to install and update RTEMS toolsets.

The prebuilt binaries are intended to be easy to install and
the instructions are similar regardless of the host environment.  
There are a few structural issues with the packaging of the RTEMS
Cross Toolset binaries that you need to be aware of.

@enumerate
@item There are dependencies between the various packages.  This requires
that certain packages be installed before others may be.  Some packaging
formats enforce this dependency.

@item Some packages are target CPU family independent and shared
across all target architectures.   These are referred to as 
"base" packages.

@item Pre-built GNU Binary Utilities (binutils) packages are available
for all RTEMS targets.  These include tools such as the assembler and
linker and must be installed.

@item Pre-built C language packages are available which include a C
compiler as well as the Standard C libraries for the embedded RTEMS
targets.  These must be installed.

@item Pre-built C++ language packages are available for most target
architectures which includes a C++ compiler as well as the Standard C++
libraries for the embedded RTEMS targets.  These are not part of the
minimum installation and need only be installed if the application is
using C++.

@end enumerate

NOTE: Installing toolset binaries does not install RTEMS itself, only
the tools required to build RTEMS.  See @ref{Building RTEMS} for the next
step in the process.

@section RPMs

This section provides information on installing and removing RPMs.

Note that RTEMS tools for multiple major versions of RTEMS can be
installed in parallel since they are installed into different host
directories.  The tools also include the RTEMS Release Series in their
name.

@subsection Locating the RPMs for your GNU/Linux Distribution

The RTEMS Project maintains a Yum Repository of RPMs for its
toolsets. Whether you use Yum to install the RPMs or download and install
them via another procedure, you will need to locate the appropriate
set of RPMs on the RTEMS Yum Repository.  The following instructions
are generalized.

If your host operating system uses Yum and RPMs, then you will only have
to download and install two RPMs by hand

@enumerate
@item Point your browser at
@uref{http://www.rtems.org/ftp/pub/rtems/linux,
http://www.rtems.org/ftp/pub/rtems/linux}.  In this directory, you
will see a list of RTEMS major versions such as 4.11, 4.10, 4.9, etc..
Descend into the appropriate directory for the version of RTEMS you
are using.

@item Now that you are in the directory for a specific RTEMS major
version, you will be presented with a list of GNU/Linux distributions.
This will include options like redhat, centos, fedora, and suse.
Select the appropriate distribution.

@item Now that you are in the directory for your selected distribution,
you will be presented with a list of distribution versions for which
RTEMS pre-built RPMs are available.  Select the appropriate distribution
version.

@item Now that you are in the directory for the proper version of
your selected distribution, you will be presented with a choice of
host architecture versions such as i386, i686, and x86_64.  Select the
appropriate version for your development computer.

@item At this point, you will have a long list of RPMs to select from.
@end enumerate

The RTEMS Projects supports a wide variety of host OS and target
combinations.  In addition, these toolsets are specific to a particular
RTEMS Release Series.  Given the large number of possible combinations,
the instructions use variables to indicate where versions go in the real
package names you will use.  These variable are used in the examples of
RPM version names:

@itemize @bullet
@item @code{<VERSION>} is the tool version will be found at this location
in the RPM name. This will be a release number such as @code{2.20}
or @code{4.4.5}.

@item @code{<DIST>} indicates the GNU/Linux distribution version.
This will be a string such as @code{fc14} or @code{el6}.

@item @code{<ARCH>} indicates the architecture used for RPMs on your
GNU/Linux installation.  This will be a string such as @code{i386}
or @code{x86_64}.

@item @code{<RPM>} indicates the RPM revision level.  This will be a
single integer.
@end itemize

The tool VERSION and RPM release may vary within the set of current RPMs for a particular RTEMS Release series based upon the target architecture.  

If you are using Yum, please continue to the next section.  If you are
downloading the RPMs to install by hand, then go to the @ref{Installing
RPMs Without Yum} section.

@subsection Managing RPMs Using Yum

This section describes how to install and remove RTEMS Toolsets using Yum.

@subsubsection Installing RPMs Using Yum

If you are on a host operating system that uses Yum, you are fortunate because this is the one of the simplest ways to install the tools.  After locating the appropriate directory on the RTEMS Yum Repository using the instructions in @ref{Locating the RPMs for your GNU/Linux Distribution}, you will need to install the following RPMs:

@itemize @bullet
@item @value{RTEMSRPMPREFIX}-release-<VERSION>-<RPM>.<DIST>.noarch.rpm
@item @value{RTEMSRPMPREFIX}-yum-conf-<VERSION>-<RPM>.<DIST>.noarch.rpm
@end itemize

You can use the search within page feature of your browser to locate
the RPMs with "release" or "yum" in their names.

You will need to download the RPMs above or RPM can be given the URLs for
them and it will fetch them for you.  Either way, the commands similar
to the following will install the common or base RPMs required.

@example
rpm -U @value{RTEMSRPMPREFIX}-release-<VERSION>-<RPM>.<DIST>.noarch.rpm \
       @value{RTEMSRPMPREFIX}-yum-conf-<VERSION>-<RPM>.<DIST>.noarch.rpm
@end example

Once these are installed, Yum knows about the RTEMS Yum repository
for @value{RTEMSPREFIX}.  This means that you can install and upgrade
RTEMS Toolsets just like the packages provided by your distribution.
To install complete C and C++ toolset targeting the SPARC architecture
for the RTEMS @value{RTEMSAPI} Release series, commands similar to the
following will be used.

@example
yum install @value{RTEMSPREFIX}-auto*
yum install @value{RTEMSPREFIX}-sparc-*
@end example

The first command installs GNU autoconf and automake which are used
by all RTEMS targets.  The second command installs the complete
sparc-@value{RTEMSPREFIX} toolset including all dependencies.

@subsubsection Removing RPMs Using Yum

The following is a sample session illustrating the removal of a C/C++
toolset targeting the SPARC architecture.

@example
yum erase @value{RTEMSRPMPREFIX}-sparc-*
@end example

If this is the last target architecture for which tools are installed, then you can remove the RTEMS GNU autotools and common packages as follows:

@example
yum erase @value{RTEMSRPMPREFIX}-auto*
yum erase @value{RTEMSRPMPREFIX}-*common*
@end example

NOTE:  If you have installed any RTEMS BSPs, then it is likely that RPM
will complain about not being able to remove everything.  These will
have to be removed by hand.

@subsection Managing RPMs Without Using Yum

This section describes how to install and remove RTEMS Toolsets without
using Yum.  This is NOT expected to be the norm for RPM users.

@subsubsection Installing RPMs Without Yum

The following is a sample session illustrating the installation of the
complete C and C++ toolset targeting the SPARC architecture for the
RTEMS @value{RTEMSAPI} Release series.

Since you are not using Yum, you will need to download all of the RPMs
you will install.  Alternatively, RPM can be given a URL for an RPM file
and it will fetch it for you.  Either way, the commands similar to the
following will install the common or base RPMs required.

@example
rpm -U @value{RTEMSRPMPREFIX}binutils-common-<VERSION>-<RPM>.<DIST>.noarch.rpm \
       @value{RTEMSRPMPREFIX}gcc-common-<VERSION>-<RPM>.<DIST>.noarch.rpm \
       @value{RTEMSRPMPREFIX}newlib-common-<VERSION>-<RPM>.<DIST>.noarch.rpm \
       @value{RTEMSRPMPREFIX}gdb-common-<VERSION>-<RPM>.<DIST>.noarch.rpm
@end example

The above RPMs are shared across all RTEMS targets and include common
files such as the documentation.  The following illustrates how to install
the GNU Autoconf and Automake RPMs that match your RTEMS installation.
RTEMS uses the GNU Autotools for its configure and build infrastructure
and you will need these if you modify the build infrastructure or check
out RTEMS from CVS and have to bootstrap the source tree.

@example
rpm -U @value{RTEMSRPMPREFIX}autoconf-<VERSION>-<RPM>.<DIST>.noarch.rpm \
       @value{RTEMSRPMPREFIX}automake-<VERSION>-<RPM>.<DIST>.noarch.rpm
@end example

Now that you have installed all of the RPMs that are independent of the
target architecture you can install the C toolset for a specific target.
The following command will install the target architecture specific set
of the RPMs for a C toolset including GDB.

@example
rpm -U @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-binutils-<VERSION>-<RPM>.<ARCH>.rpm \
       @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gcc-<VERSION>-<RPM>.<ARCH>.rpm \
       @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-newlib-<VERSION>-<RPM>.<ARCH>.rpm \
       @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-libgcc-<VERSION>-<RPM>.<ARCH>.rpm \
       @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gdb-<VERSION>-<RPM>.<ARCH>.rpm
@end example

The following command illustrates how to install the C++ specific portion of the RPMs.

@example
rpm -U @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gcc-c++-<VERSION>-<RPM>.<ARCH>.rpm \
       @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-libstd++-<VERSION>-<RPM>.<ARCH>.rpm
@end example

Upon successful completion of the above command sequence, a C/C++
cross development toolset targeting the SPARC is installed in
@code{@value{RTEMSPREFIX}}.  In order to use this toolset, the directory
@code{@value{RTEMSPREFIX}/bin} should be at the start of your PATH.
At this point, the tools are installed for a specific target architecture
adn you may proceed directly to @ref{Building RTEMS}.

If you want to build RTEMS for multiple target architectures, you will
need to install the target specific portion of the RPMs for each target.

@subsubsection Removing RPMs Without Using Yum

The following is a sample session illustrating the removal of a C/C++
toolset targeting the SPARC architecture.

@example
rpm -e `rpm -qa | grep @value{RTEMSRPMPREFIX}-sparc-`
@end example

If this is the last target architecture for which tools are installed, then you can remove the RTEMS GNU autotools and common packages as follows:

@example
rpm -e `rpm -qa | grep @value{RTEMSRPMPREFIX}-auto`
rpm -e `rpm -qa | grep @value{RTEMSRPMPREFIX} | grep common`
@end example

NOTE:  If you have installed any RTEMS BSPs, then it is likely that RPM
will complain about not being able to remove everything.  These will
have to be removed by hand.

@subsection Determining Which RTEMS RPMs are Installed

The following command will report which RTEMS RPMs are currently
installed:

@example
rpm -qa | grep @value{RTEMSAPI}
@end example

@section Zipped Tar Files

The tool binaries for some hosts are provided as compressed tar files.
This section provides information on installing and removing Zipped Tar
Files (e.g .tar.gz or .tar.bz2).

@subsection Installing Zipped Tar Files

The following is a sample session illustrating the installation
of a C/C++ toolset targeting the SPARC architecture assuming
that GNU tar is installed as @code{tar} for a set of archive
files compressed with GNU Zip (gzip):

@example
cd /
tar xzf @value{RTEMSRPMPREFIX}binutils-common-<VERSION>-<RPM>.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-binutils-<VERSION>-<RPM>.tar.gz
tar xzf @value{RTEMSRPMPREFIX}gcc-common-<VERSION>-<RPM>.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gcc-<VERSION>-<RPM>.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-newlib-<VERSION>-<RPM>.tar.gz
tar xzf @value{RTEMSRPMPREFIX}gdb-common-<VERSION>-<RPM>.tar.gz
tar xzf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gdb-<VERSION>-<RPM>.tar.gz
@end example

The following command set is the equivalent command sequence
for the same toolset assuming that is was compressed with
GNU BZip (bzip2):

@example
cd /
tar xjf @value{RTEMSRPMPREFIX}binutils-common-<VERSION>-<RPM>.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-binutils-<VERSION>-<RPM>.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}gcc-common-<VERSION>-<RPM>.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-newlib-<VERSION>-<RPM>.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gcc-<VERSION>-<RPM>.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}gdb-common-<VERSION>-<RPM>.tar.bz2
tar xjf @value{RTEMSRPMPREFIX}sparc-rtems@value{RTEMSAPI}-gdb-<VERSION>-<RPM>.tar.bz2
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


