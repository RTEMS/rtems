@c
@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Where To Go From Here

At this point, you should have successfully installed a
GNU C/C++ Cross Compilation Tools for RTEMS on your host system
as well as the RTEMS OS for the target host.  You should
have successfully linked the "hello world" program. You
may even have downloaded the executable to that target
and run it.  What do you do next?

The answer is that it depends.  You may be interested in
writing an application that uses one of the multiple
APIs supported by RTEMS.  You may need to investigate the
network or filesystem support in RTEMS.  The common
thread is that you are largely finished with this
manual and ready to move on to others.

When writing RTEMS applications, you should find the
following manuals useful because they define the
calling interface to many of the services provided
by RTEMS:

@itemize @bullet
@item @b{RTEMS Applications C User's Guide} describes the
Classic RTEMS API based on the RTEID specification.

@item @b{RTEMS POSIX API User's Guide} describes the 
RTEMS POSIX API that is based on the POSIX 1003.1b API.

@item @b{RTEMS ITRON 3.0 API User's Guide} describes
the RTEMS implementation of the ITRON 3.0 API.

@item @b{RTEMS Network Supplement} provides information
on the network services provided by RTEMS.

@end itemize

In addition, the following manuals from the GNU C/C++ Cross
Compilation Toolset include information on run-time services
available.

@itemize @bullet
@item @b{Cygnus C Support Library} describes the Standard
C Library functionality provided by Newlib's libc.

@item @b{Cygnus C Math Library} describes the Standard
C Math Library functionality provided by Newlib's libm.

@end itemize

Finally, the RTEMS FAQ and mailing list archives are available 
at http://www.oarcorp.com.

There is a wealth of documentation available for RTEMS and
the GNU tools supporting it.  If you run into something
that is not clear or missing, bring it to our attention.

Also, some of the RTEMS documentation is still under
construction.  If you would like to contribute to this
effort, please contact the RTEMS Team.




