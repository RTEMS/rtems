@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@set CYGWIN-VERSION B19
@set CYGWIN-FTP http://www.cygnus.com/misc/gnu-win32
@set DOS2UNIX-FTP ftp://ftp.micros.hensa.ac.uk/platforms/ibm-pc/ms-dos/simtelnet/txtutl/dos2unix.zip
@set PFE-FTP http://www.lancs.ac.uk/people/cpaap/pfe/

@chapter Using MS-Windows as a Development Host

This chapter discusses the installation of the GNU tool chain
on a computer running either the Microsoft Windows95 
or WindowsNT operating system.

This chapter is based on a draft provided by 
Geoffroy Montel <g_montel@@yahoo.com>.  Geoffroy's 
procedure was based on information from 
David Fiddes <D.J.Fiddes@@hw.ac.uk>.
Their input and feedback is greatly appreciated.

@b{STATUS:}  This chapter should be considered preliminary.
Please be careful when following these instructions.

@section Version Information

This installation process works well under Windows NT. 
It is said to also work under Windows95 (although it
did not work with B18). Anyway, the resulting code works
under both NT and 95.

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
@code{PKZip}.  Instead the un-achiving process uses
the GNU @code{zip} and @code{tar} programs as shown below:

@example
tar -xzvf archive.gzip 
@end example

@code{tar} is provided with Cygwin32.

@subsection Text Editor

You absolutely have to use a text editor which can 
save files with Unix format (so don't use Notepad 
nor Wordpad). If you do not have an appropriate
text editor, try @b{Programmers File Editor}, it is
free and very convenient.  This editor may be downloaded
from @href{@value{PFE-FTP},,@value{PFE-FTP}}.

@subsection Bug in Patch Utility

There is a bug in the @code{patch} utility
provided in Cygwin32 B19. The files modified end up 
having MS-DOS style line termination. They must have
Unix format, so a @code{dos2unix}-like command must
be used to put them back into Unix format as shown below:

@example
dos2unix XXX
@end example

The dos2unix utility may be downloaded from:

@href{@value{DOS2UNIX-FTP},,,@value{DOS2UNIX-FTP}}.

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
@ref{@value{CYGWIN-FTP},,@value{CYGWIN-FTP}}

@item usertools.exe
@ref{@value{CYGWIN-FTP},,@value{CYGWIN-FTP}}

@end table

@section Installing Cygwin32 B19

This section describes the process of installing the 
version @value{CYGWIN-VERSION} of the Cygwin32 environment.  It assumes
that this toolset is installed in a directory 
referred to as @code{<RTOS>}.

@enumerate

@item Execute cdk.exe. These instructions assume that you 
install Cygwin32 under the <RTOS>\GnuWin32\b19 directory.

@item Execute usertools.exe.  These instructions assume that you 
install Cygwin32 under the <RTOS>\GnuWin32\b19 directory.

@item Execute Cygwin.bat (either on the start menu or 
under <RTOS>\GnuWin32\b19).

@item At this point, you are at the command line of @code{bash},
a Unix-like shell. You have to mount the "/" directory. Type:

@example 
umount /
mount -b <RTOS> /
@end example

For example, the following sequence creates an MS-DOS virtual
drive @code{O:} using an MS-DOS command window and then mounts
that virtual drive as the root directory for the Cygwin32 environment:

@example
subst o: <RTOS>
umount /
mount -b o: /
@end example

@item Create the /bin, /tmp, /build and /usr directories.

@example
mkdir /bin
mkdir /tmp
mkdir /build
mkdir /usr
@end example

@item The light Bourne shell provided with Cygwin B19 is buggy.
You should copy it to a fake name and copy @code{bash.exe} to @code{sh.exe}:

@example
cd <RTOS>/GnuWin32/b19/bin
mv sh.exe old_sh.exe
cp bash.exe sh.exe
@end example

The Bourne shell often has to be present in /bin directory when installing
new packages.

@item Open the file 
@code{/gnuwin32/b19/H-i386-cygwin32/lib/gcc-lib/i386-cygwin32/2.7-b19/specs},
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
instructions in @ref{Unarchiving the Tools}.   Apply the
appropriate RTEMS specific patch as detailed in
@ref{Apply RTEMS Patch to binutils}.

@b{NOTE}: See @ref{Bug in Patch Utility}.

@item In the @code{/build} directory, execute the following
command to configure @value{BINUTILS-VERSION}:

@example
@value{BINUTILS-UNTAR}/configure --verbose --target=m68k-rtems \
    --prefix=/gcc-m68k-rtems --with-gnu-as \
    --with-gnu-ld --with-targets=all --with-newlib --with-multilib
@end example

Replace @code{m68k-rtems} with the target configuration
of your choice.  See @ref{Running the bit Script} for a
list of the targets available.

@item Execute the following command to compile the toolset:

@example
make
@end example

@item With B19, an error will occur. Remove the ".exe" suffix 
from the tools by finding them all and renaming them
as shown below:

@example
find . -name *.exe -print
mv XXX/gas.exe XXX/gas
       ....
@end example

@item Install the full package with the following command:

@example
make install
@end example

@item There is a problem with the gnu info package 
which may prevent it from building correctly.  To
get around this try again with:

@example
make -k install
@end example

@item Once the executables are installed, 
you can restore the ".exe" suffix to the files in 
the @code{/gcc-m68k-rtems/bin} and 
@code{/gcc-m68k-rtems/m68k-rtems/bin} directories again.

@item In the @code{cygnus.bat} file, add the directory
to the cross-compiler executables to your search path
by adding the following line:

@example
PATH=O:\gcc-m68k-rtems\bin;%PATH%
@end example

@item You can erase the /build directory content.

@item Exit bash and run @code{cygnus.bat} to restart
the Cygwin32 environment.

@end enumerate

@c
@c  EGCS
@c

@section Installing EGCS AND NEWLIB

@enumerate
@item Unarchive and patch @value{EGCS-TAR} and @value{NEWLIB-TAR}
following the instructions in @ref{Unarchiving the Tools}. 
Apply the appropriate RTEMS specific patches as detailed in
@ref{Apply RTEMS Patch to EGCS} and @ref{Apply RTEMS Patch to newlib}.

@b{NOTE}: See @ref{Bug in Patch Utility}.

@item Remove the following directories (we do not need Fortran
or Objective-C):

@example
· /build/@value{GCC-UNTAR}/libf2c
· /build/@value{GCC-UNTAR}/gcc/objc
· /build/@value{GCC-UNTAR}/gcc/f
@end example

@b{NOTE}: See @ref{Bug in Patch Utility}.

@item Copy to /build/@value{GCC-UNTAR}/ the following directories:

@itemize @bullet
@item /build/@value{NEWLIB-UNTAR}/newlib
@item /build/@value{NEWLIB-UNTAR}/libgloss
@end itemize

@item Go under /build directory and run:

@example
../@value{GCC-UNTAR}/configure --verbose --target=m68k-rtems \
    --prefix=/gcc-m68k --with-gnu-as --with-gnu-ld \
    --with-targets=all --with-newlib --with-multilib
@end example

Replace @code{m68k-rtems} with the target configuration
of your choice.  See @ref{Running the bit Script} for a
list of the targets available.

@item Then run your favourite Unix format compaatible file editor,
and edit /build/@value{GCC-UNTAR}/gcc/Makefile
Search for the section:

@example
# libgcc1-test target (must also be overridable for a target)
LIBGCC1_TEST = libgcc1-test
@end example

and change it to read:

@example
LIBGCC1_TEST = 
@end example

@item Search for the section:

@example
# Set this to `ld' to enable use of collect2.
USE_COLLECT2 = 
MAYBE_USE_COLLECT2 = 
# It is convenient for configure to add the assignment at the beginning,
# so don't override it here.
USE_COLLECT2 = ld
@end example

and change it to read:

@example
USE_COLLECT2 =
@end example

This changes are needed due to a @code{collect2} problem in B19.

@item Compile the toolset as follows:

@example
make cross
@end example

You must do a @code{make cross} (not a simple @code{make})
to insure that the different packages are built in the correct
order.   This command could take a long time to run depending on
the speed of the computer you are using.

@item Install with the following command:

@example
make -k install
@end example

@item Just as with binutils package, a problem with the gnu
info package not building correctly may occur, so try again with:

@example
make -k install
@end example

@end enumerate

With any luck, at this point you having a working cross-compiler.  So
as Geoffroy said:

@center @b{That's it! Celebrate!}

