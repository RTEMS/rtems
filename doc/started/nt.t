@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@appendix Using MS-Windows as a Development Host

This chapter discusses the installation of the GNU tool chain
on a computer running the Microsoft Windows operating system.

This chapter was originally written by 
@uref{mailto:g_montel@@yahoo.com, Geoffroy Montel <g_montel@@yahoo.com>} 
with input from 
@uref{mailto:<D.J@@fiddes.surfaid.org>, David Fiddes <D.J@@fiddes.surfaid.org>}.
It was based upon his successful but unnecessarily
painful efforts with Cygwin beta versions.
Cygwin and this chapter have been updated multiple times since
those early days although their pioneering efforts
and input is still greatly appreciated.

@section Microsoft Windows Version Requirements

RTEMS users report fewer problems when using Microsoft
Windows NT, 2000, or XP.  Although, the open source tools
that are used in RTEMS development do execute on Windows 95,
98, or ME, they tend to be more stable when used with
the modern Windows variants.

@section Cygwin 

For RTEMS development, the recommended approach is to use
Cygwin 1.0 or later. Cygwin is  available from
@uref{http://sources.redhat.com/cygwin, http://sources.redhat.com/cygwin} 
Recent versions of Cygwin are vastly improved over the beta
versions.  Most of the oddities, instabilities, and performance
problems have been resolved.  The installation procedure
is much simpler.  However, there are a handful of issues
that remain to successfully use Cygwin as an RTEMS development
environment.

@itemize @bullet

@item There is no @code{cc} program by default.  The GNU configure
scripts used by RTEMS require this to be present to work properly.
The solution is to link @code{gcc.exe} to @code{cc.exe} as follows:

@example
ln -s /bin/gcc.exe  /bin/cc.exe
@end example

@item Make sure @code{/bin/sh.exe} is GNU Bash.  Some Cygwin
versions provide a light Bourne shell which is insufficient to build
RTEMS.  To see which shell is installed as @code{/bin/sh.exe}, execute
the command @code{/bin/sh --version}.  If it looks similar to
the following, then it is GNU Bash and you are OK:

@example
GNU bash, version 2.04.5(12)-release (i686-pc-cygwin)
Copyright 1999 Free Software Foundation, Inc.
@end example

If you get an error or it claims to be any other shell, you need 
to copy it to a fake name and copy
@code{/bin/bash.exe} to @code{/bin/sh.exe}:

@example
cd /bin
mv sh.exe old_sh.exe
cp bash.exe sh.exe
@end example

The Bourne shell has to be present in @code{/bin} directory to run
shell scripts properly.

@item Make sure you unarchive and build in a binary mounted
filesystem (e.g. mounted with the @code{-b} option).  Otherwise,
many confusing errors will result.  

@item A user has reported that they needed 
to set CYGWIN=ntsec for chmod to work correctly, but had to set 
CYGWIN=nontsec for compile to work properly (otherwise there were
complaints about permissions on a temporary file).

@item If you want to build the tools from source, you have the 
same options as UNIX users.

@item You may have to uncompress archives during this
process.  You must @b{NOT} use @code{WinZip} or
@code{PKZip}.  Instead the un-archiving process uses
the GNU @code{zip} and @code{tar} programs as shown below:

@example
tar -xzvf archive.tgz 
@end example

@code{tar} is provided with Cygwin.

@end itemize

@section Text Editor

You absolutely have to use a text editor which can 
save files with Unix format. So do @b{NOT} use Notepad 
or Wordpad!  There are a number of editors
freely available that can be used.  

@itemize @bullet
@item @b{VIM} (@b{Vi IMproved}) is available from
@uref{http://www.vim.org/,http://www.vim.org/}.  
This editor has the very handy ability to easily
read and write files in either DOS or UNIX style.

@item @b{GNU Emacs} is available for many platforms
including MS-Windows.  The official homepage 
is @uref{http://www.gnu.org/software/emacs/emacs.html,
http://www.gnu.org/software/emacs/emacs.html}.
The GNU Emacs on Windows NT and Windows 95/98 FAQ is at
@uref{http://www.gnu.org/software/emacs/windows/ntemacs.html,
http://www.gnu.org/software/emacs/windows/ntemacs.html}.

@end itemize

If you do accidentally end up with files 
having MS-DOS style line termination, then you
may have to convert them to Unix format for some
Cygwin programs to operate on them properly. The 
program @code{dos2unix} can be used to put them
back into Unix format as shown below:

@example
$ dos2unix XYZ
Dos2Unix: Cleaning file XYZ ...
@end example

@section System Requirements

Although the finished cross-compiler is fairly easy on resources,
building it can take a significant amount of processing power and 
disk space.  

@itemize @bullet

@item The faster the CPU, the better. The tools and Cygwin can be
@b{very} CPU hungry.

@item The more RAM, the better.  Reports are that when building GCC
and GDB, peak memory usage can exceed 256 megabytes.

@item The more disk space, the better. You need more if you are building
the GNU tools and the amount of disk space for binaries is obviously
directly dependent upon the number of CPUs you have cross toolsets
installed for.

@end itemize

