@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@set CYGWIN-VERSION B19
@set CYGWIN-FTP http://www.cygnus.com/misc/gnu-win32/
@set CYGWIN-COOLVIEW http://www.lexa.ru/sos/
@set DOS2UNIX-FTP ftp://ftp.micros.hensa.ac.uk/platforms/ibm-pc/ms-dos/simtelnet/txtutl/dos2unix.zip
@set PFE-FTP http://www.lancs.ac.uk/people/cpaap/pfe/

@chapter Using MS-Windows as a Development Host

This chapter discusses the installation of the GNU tool chain
on a computer running the Microsoft Windows NT operating system.

This chapter is based on a draft provided by 
Geoffroy Montel <g_montel@@yahoo.com>.  Geoffroy's 
procedure was based on information from 
David Fiddes <D.J@@fiddes.surfaid.org>.
Their input and feedback is greatly appreciated.

@b{STATUS:}  This chapter should be considered preliminary.
Please be careful when following these instructions.

@section Version Information

This installation process works well under Windows NT. 
Using Windows 95 or 98 is not recommended although it 
should be possible with version 3.77 of GNU make and an updated
cygwinb19.dll.

This procedure should also work with newer version of 
the tool versions listed in this chapter, but this has
not been verified.  If you have success with a particular
version of the toolset or notice problems in this chapter,
please let the RTEMS maintainers know so they can be
addressed in future revisions of this document.

@section MS-Windows Host Specific Requirements

This section details the components required to install
and build a Windows hosted GNU cross development toolset.

@subsection Unzipping Archives

You will have to uncompress many archives during this
process.  You must @b{NOT} use @code{WinZip} or
@code{PKZip}.  Instead the un-archiving process uses
the GNU @code{zip} and @code{tar} programs as shown below:

@example
tar -xzvf archive.tgz 
@end example

@code{tar} is provided with Cygwin32.

@subsection Text Editor

You absolutely have to use a text editor which can 
save files with Unix format (so don't use Notepad 
nor Wordpad). If you do not have an appropriate
text editor, try @b{Programmers File Editor}, it is
free and very convenient.  This editor may be downloaded
from:

@c @uref{@value{PFE-FTP},@value{PFE-FTP}}
@b{@value{PFE-FTP},@value{PFE-FTP}}

@subsection Bug in Patch Utility

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

@c @uref{@value{DOS2UNIX-FTP},@value{DOS2UNIX-FTP}}
@b{@value{DOS2UNIX-FTP},@value{DOS2UNIX-FTP}}

You @b{must} change the format of every patched file 
for the toolset build to work correctly.

@subsection Files Needed

This section lists the files required to build and install
a Windows hosted GNU cross development toolset and their
home WWW site.  In addition to the sources required 
for the cross environment listed earlier in @ref{Get All the Pieces}, 
you will need to  download the following
files from their respective sites using your favorite
Web browser or ftp client.

@table @b

@item cdk.exe
@c @uref{@value{CYGWIN-FTP},@value{CYGWIN-FTP}}
@b{@value{CYGWIN-FTP},@value{CYGWIN-FTP}}

@item coolview.tar.gz
@c @uref{@value{CYGWIN-COOLVIEW},@value{CYGWIN-COOLVIEW}}
@b{@value{CYGWIN-COOLVIEW},@value{CYGWIN-COOLVIEW}}

@end table

@subsection System Requirements

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


@section Installing Cygwin32 B19

This section describes the process of installing the 
version @value{CYGWIN-VERSION} of the Cygwin32 environment.  It assumes
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
mkdir /build/egcs
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
and change the following line:

@example
-lcygwin %@{mwindows:-luser32 -lgdi32 -lcomdlg32@} -lkernel32
@end example

to:

@example
-lcygwin %@{mwindows:-luser32 -lgdi32 -lcomdlg32@} -lkernel32 -ladvapi32
@end example

@end enumerate

At this point, you have a native installation of Cygwin32 and
are ready to proceed to building a cross-compiler.

@c
@c  BINUTILS
@c

@section Installing binutils

@enumerate

@item Unarchive @value{BINUTILS-TAR} following the 
instructions in @ref{Unarchiving the Tools} into the /source directory.
Apply the appropriate RTEMS specific patch as detailed in
@ref{Apply RTEMS Patch to binutils}.

@item In the @code{/build/binutils} directory, execute the following
command to configure @value{BINUTILS-VERSION}:

@example
/source/@value{BINUTILS-UNTAR}/configure --verbose --target=m68k-rtems \
    --prefix=/gcc-m68k-rtems --with-gnu-as --with-gnu-ld 
@end example

Replace @code{m68k-rtems} with the target configuration
of your choice.  See @ref{Running the bit Script} for a
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
error during installation. Telling make to keep going with -k allows 
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

@section Installing GCC AND NEWLIB

@enumerate
@item Unarchive and patch @value{GCC-TAR} and @value{NEWLIB-TAR}
following the instructions in @ref{Unarchiving the Tools}. 
Apply the appropriate RTEMS specific patches as detailed in
@ref{Apply RTEMS Patch to GCC} and @ref{Apply RTEMS Patch to newlib}.

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

@item Change to the /build/egcs directory to configure the compiler:

@example
/source/@value{GCC-UNTAR}/configure --verbose --target=m68k-rtems \
    --prefix=/gcc-m68k --with-gnu-as --with-gnu-ld \
    --with-newlib
@end example

Replace @code{m68k-rtems} with the target configuration
of your choice.  See @ref{Running the bit Script} for a
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

@example
make -k install
@end example

@end enumerate

With any luck, at this point you having a working cross-compiler.  So
as Geoffroy said:

@center @b{That's it! Celebrate!}

