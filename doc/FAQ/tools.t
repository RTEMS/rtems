@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter General Development Tool Hints

The questions in this category are related to the GNU development tools
in a non-language specific way.

@section What is GNU?

Take a look at @uref{http://www.gnu.org,http://www.gnu.org} for information on the GNU Project.

@section How do I generate a patch?

The RTEMS patches to the development tools are generated using a 
command like this

@example
diff -N -P -r -c TOOL-original-image TOOL-with-changes >PATCHFILE
@end example

where the options are:

@itemize @bullet

@item -N and -P take care of adding and removing files (be careful not to
include junk files like file.mybackup)

@item -r tells diff to recurse through subdirectories

@item -c is a context diff (easy to read for humans)

@end itemize

Please look at the generated PATCHFILE and make sure it does not 
contain anything you did not intend to send to the maintainers.  
It is easy to accidentally leave a backup file in the modified
source tree or have a spurious change that should not be
in the PATCHFILE.

If you end up with the entire contents of a file in the patch 
and can't figure out why, you may have different CR/LF scheme
in the two source files.  The GNU open-source packages usually have
UNIX style CR/LF.  If you edit on a Windows platform, the line 
terminators may have been transformed by the editor into Windows
style.
