@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Getting Started

This section describes the steps which are necessary to set up a
debugging session with RTEMS/GDB.


@section Compiling The System Components

The components that need to be debugged have to be compiled with the -g option
in order the generated file to contain the required information for the debug
session.


@section Starting a debugging session

After having launched GDB, the user must connect to the RTEMS target with the
following commands:

@example
(gdb) setrpcmode sun
(gdb) setdaemontype rdbg
(gdb) target rtems target-name
@end example

Then he can use the file GDB command to specify the binary to be debugged.

This initialization phase can be written in a @code{``.gdbinit''}
file. Each time, the user will launch GDB, it will execute this initialization
sequence.


@section Attaching To The System

The entire target system is viewed as a single multi-threaded process,
the identifier of which is 1. To attach, use:

@example
(gdb) attach 1
@end example

When successfully attached, the user can control the execution of
the target system from the debugger.


