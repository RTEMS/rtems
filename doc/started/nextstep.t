@c
@c
@c  COPYRIGHT (c) 1988-2002.
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

Whether or not you decide to dive in now and write
application code or read some documentation first,
this chapter is for you.  The first section provides
a quick roadmap of some of the RTEMS documentation.
The next section provides a brief overview of the
RTEMS application structure.

@section Documentation Overview

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
at @uref{http://www.oarcorp.com}.

There is a wealth of documentation available for RTEMS and
the GNU tools supporting it.  If you run into something
that is not clear or missing, bring it to our attention.

Also, some of the RTEMS documentation is still under
construction.  If you would like to contribute to this
effort, please contact the RTEMS Team at
@uref{mailto:@value{RTEMSUSERS}, @value{RTEMSUSERS}}.
If you are interested in sponsoring the development of a new
feature, BSP, device driver, port of an existing library, etc.,
please contact one of the RTEMS Service Providers listed
at @uref{http://www.rtems.com/support.html}.

@section Writing an Application

From an application author's perspective, RTEMS applications do not 
start at @code{main()}.  They begin execution at one or more
application initialization task or thread and @code{main()} is
owned by the Board Support Package.   Each API supported
by RTEMS (Classic, POSIX, and ITRON) allows the user to configure
a set of tasks that are created and started automatically
during RTEMS initialization.  The RTEMS Automatic
Configuration Generation (@code{confdefs.h}) scheme can be
used to easily generate the configuration information for
an application that starts with a single initialization task.  
By convention, unless overridden, the default name of the
initialization task varies based up API.

@itemize @bullet
@item @code{Init} - single Classic API Initialization Task

@item @code{POSIX_Init} - single POSIX API Initialization Thread

@item @code{ITRON_Init} - single ITRON API Initialization Task
@end itemize

See the Configuring a System chapter in the C User's Guide for
more details.

Regardless of the API used, when the initialization task executes,
all non-networking device drivers are normally initialized and
processor interrupts are enabled.  The initialization task then
goes about its business of performing application specific 
initialization.  This often involves creating tasks and other
system resources such as semaphores or message queues and allocating
memory.  In the RTEMS examples and tests, the file @code{init.c} usually
contains the initialization task.  Although not required, in
most of the examples, the initialization task completes by 
deleting itself.

As you begin to write RTEMS application code, you may be confused
by the range of alternatives.  Supporting multiple tasking
APIs can make the choices confusing.  Many application groups
writing new code choose one of the APIs as their primary API
and only use services from the others if nothing comparable
is in their preferred one.  However, the support for multiple
APIs is a powerful feature when integrating code from multiple
sources.  You can write new code using POSIX services and
still use services written in terms of the other APIs.
Moreover, by adding support for yet another API, one could
provide the infrastructure required to migrate from a
legacy RTOS with a non-standard API to an API like POSIX.


