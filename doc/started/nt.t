@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@appendix Using MS-Windows as a Development Host

This chapter discusses the installation of the GNU tool chain
on a computer running the Microsoft Windows NT operating system.

@section Cygwin 1.0 or Newer

Recent versions of Cygwin are vastly improved over the beta
versions.  Most of the oddities, instabilities, and performance
problems have been resolved.  The installation procedure
is much simpler.  However, there are a handful of issues
that remain to successfully use Cygwin as an RTEMS development
environment.

@itemize @bullet

@item There is no @code{cc} program by default.  The GNU configure
scripts used by RTEMS require this to be present to work properly.
The solution is to link @code{gcc.exe} to @code{cc.exe}.

@item Make sure you unarchive and build in a binary mounted
filesystem (e.g. mounted with the @code{-b} option).  Otherwise,
many confusing errors will result.

@item If you want to use RPM, you will have to obtain that
separately by following the links from the main Cygwin site.

@item When using the RPMs, there may be warnings about
@code{/etc/mtab} while installing the info files.  This can be
ignored.

@item A user has reported that they needed 
to set CYGWIN=ntsec for chmod to work correctly, but had to set 
CYGWIN=nontsec for compile to work properly (otherwise there were
complaints about permissions on a temporary file).

@item If you want to build the tools from source, you have the 
same options as UNIX users.

@end itemize

@section Cygwin B19

This section is based on a draft provided by 
@uref{mailto:g_montel@@yahoo.com, Geoffroy Montel <g_montel@@yahoo.com>}.
Geoffroy's procedure was based on information from 
@uref{mailto:<D.J@@fiddes.surfaid.org>, David Fiddes <D.J@@fiddes.surfaid.org>}.
Their input and feedback is greatly appreciated.

@b{STATUS:}  This chapter should be considered preliminary.
Please be careful when following these instructions.

This installation process works well under Windows NT. 
Using Windows 95 or 98 is not recommended although it 
should be possible with version 3.77 of GNU make and an updated
cygwinb19.dll.

This procedure should also work with newer versions of 
the tool versions listed in this chapter, but this has
not been verified.  

@subsection MS-Windows Host Specific Requirements

This section details the components required to install
and build a Windows hosted GNU cross development toolset.

@subsubsection Unzipping Archives

You will have to uncompress many archives during this
process.  You must @b{NOT} use @code{WinZip} or
@code{PKZip}.  Instead the un-archiving process uses
the GNU @code{zip} and @code{tar} programs as shown below:

@example
tar -xzvf archive.tgz 
@end example

@code{tar} is provided with Cygwin32.

@subsubsection Text Editor

You absolutely have to use a text editor which can 
save files with Unix format (so don't use Notepad 
nor Wordpad).  There are a number of editors
freely available that can be used.  

@itemize @bullet
@item @b{VIM} (@b{Vi IMproved}) is available from
@b{http://www.vim.org/}.  This editor has the very
handy ability to easily read and write files in 
either DOS or UNIX style.

@item @b{GNU Emacs} is available for many platforms
including MS-Windows.  The official homepage 
is @b{http://www.gnu.org/software/emacs/emacs.html}.
The GNU Emacs on Windows NT and Windows 95/98 FAQ is
at @b{http://www.gnu.org/software/emacs/windows/ntemacs.html}.

@item @b{PFE} (@b{Programmers File Editor}) may be downloaded
from @b{http://www.simtel.net/pub/simtelnet/win95/editor/pfe101i.zip}.  Note this
editor is no longer actively supported.

@c @uref{http://www.simtel.net/pub/simtelnet/win95/editor/pfe101i.zip}

@end itemize

@subsubsection Bug in Patch Utility

There is a bug in the @code{patch} utility
provided in Cygwin32 B19. The files modified end up 
having MS-DOS style line termination. They must have
Unix format, so a @code{dos2unix}-like command must
be used to put them back into Unix format as shown below:

@example
$ dos2unix XYZ
Dos2Unix: Cleaning file XYZ ...
@end example

The dos2unix utility may be downloaded from:

@c @uref{ftp://ftp.micros.hensa.ac.uk/platforms/ibm-pc/ms-dos/simtelnet/txtutl/dos2unix.zip,ftp://ftp.micros.hensa.ac.uk/platforms/ibm-pc/ms-dos/simtelnet/txtutl/dos2unix.zip}
@b{ftp://ftp.micros.hensa.ac.uk/platforms/ibm-pc/ms-dos/simtelnet/txtutl/dos2unix.zip}

You @b{must} change the format of every patched file 
for the toolset build to work correctly.

@subsubsection Files Needed

This section lists the files required to build and install
a Windows hosted GNU cross development toolset and their
home WWW site.  In addition to the sources required 
for the RTEMS cross environment listed earlier in this manual,
you may need to  download the following
files from their respective sites using your favorite
Web browser or ftp client. [NOTE: This information was current when B19
was released and URLs may no longer be correct.]

@table @b

@item cdk.exe
@c @uref{http://www.cygnus.com/misc/gnu-win32/,http://www.cygnus.com/misc/gnu-win32/}
@b{http://www.cygnus.com/misc/gnu-win32/}

@item coolview.tar.gz
@c @uref{http://www.lexa.ru/sos/,http://www.lexa.ru/sos/}
@b{http://www.lexa.ru/sos/}

@end table

@subsubsection System Requirements

Although the finished cross-compiler is fairly easy on resources,
building it can take a significant amount of processing power and 
disk space. The recommended build system spec is:

@enumerate

@item An AMD K6-300, Pentium II-300 or better processor. GNU C and Cygwin32 are
@b{very} CPU hungry.

@item At least 64MB of RAM.

@item At least 400MB of FAT16 disk space or 250MB if you have an NTFS partition. 

@end enumerate

Even with this spec of machine expect the full suite to take over 2 hours to 
build with a further half an hour for RTEMS itself.


@subsection Installing Cygwin32 B19

This section describes the process of installing the 
version B19 of the Cygwin32 environment.  It assumes
that this toolset is installed in a directory 
referred to as @code{<RTOS>}.

@enumerate

@item Execute cdk.exe. These instructions assume that you 
install Cygwin32 under the <RTOS>\cygnus\b19 directory.

@item Execute Cygwin.bat (either on the start menu or 
under <RTOS>\cygnus\b19).

@item At this point, you are at the command line of @code{bash},
a Unix-like shell. You have to mount the "/" directory. Type:

@example 
umount /
mount -b <RTOS> /
@end example

For example, the following sequence mounts the @code{E:\unix} as the 
root directory for the Cygwin32 environment. Note the use of two @code{\}s 
when specifying DOS paths in bash:

@example
umount /
mount -b e:\\unix /
@end example

@item Create the /bin, /tmp, /source and /build directories.

@example
mkdir /bin
mkdir /tmp
mkdir /source
mkdir /build
mkdir /build/binutils
mkdir /build/gcc
@end example

@item The light Bourne shell provided with Cygwin B19 is buggy.
You should copy it to a fake name and copy @code{bash.exe} to @code{sh.exe}:

@example
cd <RTOS>/cygnus/b19/H-i386-cygwin32/bin
mv sh.exe old_sh.exe
cp bash.exe sh.exe
@end example

The Bourne shell has to be present in /bin directory to run shell scripts properly:

@example
cp <RTOS>/cygnus/b19/H-i386-cygwin32/bin/sh.exe /bin
cp <RTOS>/cygnus/b19/H-i386-cygwin32/bin/bash.exe /bin
@end example


@item Open the file 
@code{/cygnus/b19/H-i386-cygwin32/lib/gcc-lib/i386-cygwin32/2.7-b19/specs},
and append

@example
-ladvapi32
@end example

to the following line:

@example
-lcygwin %@{mwindows:-luser32 -lgdi32 -lcomdlg32@} -lkernel32
@end example

@end enumerate

At this point, you have a native installation of Cygwin32 and
are ready to proceed to building a cross-compiler.

@c
@c  BINUTILS
@c

@subsection Installing binutils

@enumerate

@item Unarchive @value{BINUTILS-TAR} following the 
instructions in @ref{Unarchiving the Tools} into the /source directory.
Apply the appropriate RTEMS specific patch as detailed in
@ref{Applying RTEMS Patches}.

@item In the @code{/build/binutils} directory, execute the following
command to configure @value{BINUTILS-VERSION}:

@example
/source/@value{BINUTILS-UNTAR}/configure \
    --verbose --target=m68k-rtems \
    --prefix=/gcc-m68k-rtems --with-gnu-as --with-gnu-ld 
@end example

Replace @code{m68k-rtems} with the target configuration
of your choice.  See @ref{Using the RTEMS configure Script Directly} for a
list of the targets available.

@item Execute the following command to compile the toolset:

@example
make
@end example

@item Install the full package with the following command:

@example
make -k install
@end example

There is a problem with the gnu info package which will cause an 
error during installation. Telling make to keep going with @code{-k} allows 
the install to complete.

@item In the @code{cygnus.bat} file, add the directory
containing the cross-compiler executables to your search path
by inserting the following line:

@example
PATH=E:\unix\gcc-m68k-rtems\bin;%PATH%
@end example

@item You can erase the /build/binutils directory content if 
disk space is tight.

@item Exit bash and run @code{cygnus.bat} to restart
the Cygwin32 environment with the new path.

@end enumerate

@c
@c  GCC
@c

@subsection Installing GCC AND NEWLIB

@enumerate
@item Unarchive and patch @value{GCC-TAR} and @value{NEWLIB-TAR}
following the instructions in @ref{Unarchiving the Tools}. 
Apply the appropriate RTEMS specific patches as detailed in
@ref{Applying RTEMS Patches}.

@b{NOTE}: See @ref{Bug in Patch Utility}.

@item Remove the following directories (we cannot use Fortran
or Objective-C as Cygwin32 cross-compilers):

@example
/source/@value{GCC-UNTAR}/libf2c
/source/@value{GCC-UNTAR}/gcc/objc
/source/@value{GCC-UNTAR}/gcc/f
@end example

@b{NOTE}: See @ref{Bug in Patch Utility}.

@item Link the following directories from Newlib to the main GCC directory, 
/source/@value{GCC-UNTAR}/ :

@itemize @bullet
@item ln -s ../@value{NEWLIB-UNTAR}/newlib newlib
@item ln -s ../@value{NEWLIB-UNTAR}/libgloss libgloss
@end itemize

@item Change to the /build/gcc directory to configure the compiler:

@example
/source/@value{GCC-UNTAR}/configure \
    --verbose --target=m68k-rtems \
    --prefix=/gcc-m68k --with-gnu-as --with-gnu-ld \
    --with-newlib
@end example

Replace @code{m68k-rtems} with the target configuration
of your choice.  See @ref{Using the RTEMS configure Script Directly} for a
list of the targets available.

@item Compile the toolset as follows:

@example
make cross
@end example

You must do a @code{make cross} (not a simple @code{make})
to insure that the different packages are built in the correct
order.   Making the compiler can take several hours even on
fairly fast machines, beware.

@item Install with the following command:

@example
make -k install
@end example

@item Just as with binutils package, a problem with the gnu
info package not building correctly requires that you use -k to 
keep going.

@end enumerate

With any luck, at this point you having a working cross-compiler.  So
as Geoffroy said:

@center @b{That's it! Celebrate!}

