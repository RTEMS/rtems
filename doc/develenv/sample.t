@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Sample Applications

@section Introduction

The RTEMS source distribution includes a set of sample applications
that are located in the @code{$@{RTEMS_ROOT@}/c/src/tests/samples/}
directory.  These applications are intended to illustrate the
basic format of RTEMS single and multiple processor
applications and the use of some features.  In addition, these
relatively simple applications can be used to test locally
developed board support packages and device drivers as they
exercise a critical subset of RTEMS functionality that is often
broken in new BSPs.

Each of the following sample applications will be listed in
more detail in the following sections:

@table @b
@item Hello World
The RTEMS Hello World test is provided in
the subdirectory @code{$@{RTEMS_ROOT@}/c/src/tests/samples/hello/}.
This test is helpful when testing new
versions of RTEMS, BSPs, or modifications to any portion of the
RTEMS development environment.  

@item Clock Tick
The @code{$@{RTEMS_ROOT@}/c/src/tests/samples/ticker/}
subdirectory provides a test for verification of clock chip
device drivers of BSPs.  

@item Base Single Processor
A simple single processor test similar to those in the
single processor test suite is provided in
@code{$@{RTEMS_ROOT@}/c/src/tests/samples/base_sp/}.

@item Base Multiple Processor
A simple two node multiprocessor test capable of testing an newly
developed MPCI layer is provided in 
@code{$@{RTEMS_ROOT@}/c/src/tests/samples/base_mp/}.

@item Constructor/Destructor C++ Test
The @code{$@{RTEMS_ROOT@}/c/src/tests/samples/cdtest/}
subdirectory provides a simple C++ application using
constructors and destructors.  It is only built when
C++ is enabled.  


@item Paranoia Floating Point Test
The directory @code{$@{RTEMS_ROOT@}/c/src/tests/samples/paranoia/}
contains the public domain floating point and math library test.

@item Minimum Size Test
The directory
@code{$@{RTEMS_ROOT@}/c/src/tests/samples/minimum/}
contains a simple RTEMS program that results in a non-functional 
executable.  It is intended to show the size of a minimum footprint
application based upon the current RTEMS configuration.  

@item Unlimited Object Allocation
The @code{$@{RTEMS_ROOT@}/c/src/tests/samples/unlimited/}
directory contains a sample test that demonstrates the use of the
@i{unlimited} object allocation configuration option to RTEMS.

@item Network Loopback Test
The @code{$@{RTEMS_ROOT@}/c/src/tests/samples/loopback/}
directory contains a sample test that demonstrates the use of 
sockets and the loopback network device.  It does not require
the presence of network hardware in order to run.
It is only built if RTEMS was configured with networking enabled.

@end table

The sample tests are written using the Classic API so the reader
should be familiar with the terms used and
material presented in the @b{RTEMS Applications Users Guide}.

@section Hello World

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/hello/
@end example

It provides a rudimentary test of the BSP start up
code and the console output routine.  The C version of this
sample application uses the printf function from the RTEMS
Standard C Library to output messages.   The Ada version of this
sample use the TEXT_IO package to output the hello messages.
The following messages are printed:

@example
@group
*** HELLO WORLD TEST ***
Hello World
*** END OF HELLO WORLD TEST ***
@end group
@end example

These messages are printed from the application's
single initialization task.  If the above messages are not
printed correctly, then either the BSP start up code or the
console output routine is not operating properly.

@section Clock Tick

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/ticker/
@end example

This application is designed as a simple test of the
clock tick device driver.  In addition, this application also
tests the printf function from the RTEMS Standard C Library by
using it to output the following messages:

@example
@group
*** CLOCK TICK TEST ***
TA1 - tm_get - 09:00:00   12/31/1988
TA2 - tm_get - 09:00:00   12/31/1988
TA3 - tm_get - 09:00:00   12/31/1988
TA1 - tm_get - 09:00:05   12/31/1988
TA1 - tm_get - 09:00:10   12/31/1988
TA2 - tm_get - 09:00:10   12/31/1988
TA1 - tm_get - 09:00:15   12/31/1988
TA3 - tm_get - 09:00:15   12/31/1988
TA1 - tm_get - 09:00:20   12/31/1988
TA2 - tm_get - 09:00:20   12/31/1988
TA1 - tm_get - 09:00:25   12/31/1988
TA1 - tm_get - 09:00:30   12/31/1988
TA2 - tm_get - 09:00:30   12/31/1988
TA3 - tm_get - 09:00:30   12/31/1988
*** END OF CLOCK TICK TEST ***
@end group
@end example

The clock tick sample application utilizes a single
initialization task and three copies of the single application
task.  The initialization task prints the test herald, sets the
time and date, and creates and starts the three application
tasks before deleting itself.  The three application tasks
generate the rest of the output.  Every five seconds, one or
more of the tasks will print the current time obtained via the
tm_get directive.  The first task, TA1, executes every five
seconds, the second task, TA2, every ten seconds, and the third
task, TA3, every fifteen seconds. If the time printed does not
match the above output, then the clock device driver is not
operating properly.

@section Base Single Processor Application

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/base_sp/
@end example

It provides a framework from which a single processor
RTEMS application can be developed. The use of the task argument
is illustrated.  This sample application uses the printf
function from the RTEMS Standard C Library or TEXT_IO functions
when using the Ada version to output the following messages:

@example
@group
*** SAMPLE SINGLE PROCESSOR APPLICATION ***
Creating and starting an application task
Application task was invoked with argument (0) and has id of 0x10002
*** END OF SAMPLE SINGLE PROCESSOR APPLICATION ***
@end group
@end example

The first two messages are printed from the
application's single initialization task.  The final messages
are printed from the single application task.

@section Base Multiple Processor Application

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/base_mp/
@end example

It provides a framework from which a multiprocessor
RTEMS application can be developed. This directory has a
subdirectory for each node in the multiprocessor system.  The
task argument is used to distinguish the node on which the
application task is executed.  The first node will print the
following messages:

@example
@group
*** SAMPLE MULTIPROCESSOR APPLICATION ***
Creating and starting an application task
This task was invoked with the node argument (1)
This task has the id of 0x10002
*** END OF SAMPLE MULTIPROCESSOR APPLICATION ***
@end group
@end example

The second node will print the following messages:

@example
@group
*** SAMPLE MULTIPROCESSOR APPLICATION ***
Creating and starting an application task
This task was invoked with the node argument (2)
This task has the id of 0x20002
*** END OF SAMPLE MULTIPROCESSOR APPLICATION ***
@end group
@end example

The herald is printed from the application's single
initialization task on each node.  The final messages are
printed from the single application task on each node.

In this sample application, all source code is shared
between the nodes except for the node dependent configuration
files.  These files contains the definition of the node number
used in the initialization of the  RTEMS Multiprocessor
Configuration Table. This file is not shared because the node
number field in the RTEMS Multiprocessor Configuration Table
must be unique on each node.

@section Constructor/Destructor C++ Application

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/cdtest/
@end example

This sample application demonstrates that RTEMS is
compatible with C++ applications.  It uses constructors,
destructor, and I/O stream output in testing these various
capabilities.  The board support package responsible for this
application must support a C++ environment.

This sample application uses the printf function from
the RTEMS Standard C Library to output the following messages:

@example
@group
Hey I'M in base class constructor number 1 for 0x400010cc.
Hey I'M in base class constructor number 2 for 0x400010d4.
Hey I'M in derived class constructor number 3 for 0x400010d4.
*** CONSTRUCTOR/DESTRUCTOR TEST ***
Hey I'M in base class constructor number 4 for 0x4009ee08.
Hey I'M in base class constructor number 5 for 0x4009ee10.
Hey I'M in base class constructor number 6 for 0x4009ee18.
Hey I'M in base class constructor number 7 for 0x4009ee20.
Hey I'M in derived class constructor number 8 for 0x4009ee20.
Testing a C++ I/O stream
Hey I'M in derived class constructor number 8 for 0x4009ee20.
Derived class - Instantiation order 8
Hey I'M in base class constructor number 7 for 0x4009ee20.
Instantiation order 8
Hey I'M in base class constructor number 6 for 0x4009ee18.
Instantiation order 6
Hey I'M in base class constructor number 5 for 0x4009ee10.
Instantiation order 5
Hey I'M in base class constructor number 4 for 0x4009ee08.
Instantiation order 5
*** END OF CONSTRUCTOR/DESTRUCTOR TEST ***
Hey I'M in base class constructor number 3 for 0x400010d4.
Hey I'M in base class constructor number 2 for 0x400010d4.
Hey I'M in base class constructor number 1 for 0x400010cc.
@end group
@end example

@section Minimum Size Test

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/minimum/
@end example

This sample application is designed to produce the 
minimum code space required for any RTEMS application
based upon the current RTEMS configuration and
BSP.  In many situations, the bulk of this executable
consists of hardware and RTEMS initialization, basic
infrastructure such as malloc(), and RTEMS and
hardware shutdown support.

@section Paranoia Floating Point Application

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/paranoia/
@end example

This sample application uses a public domain floating
point and math library test to verify these capabilities of the
RTEMS executive.  Deviations between actual and expected results
are reported to the screen.  This is a very extensive test which
tests all mathematical and number conversion functions.
Paranoia is also very large and requires a long period of time
to run.   Problems which commonly prevent this test from
executing to completion include stack overflow and FPU exception
handlers not installed.

@section Network Loopback Test

This sample application is in the following directory:

@example
$@{RTEMS_ROOT@}/tests/samples/loopback/
@end example

This sample application uses the network loopback device to
demonstrate the use of the RTEMS TCP/IP stack.  This sample
test illustrates the basic configuration and initialization
of the TCP/IP stack as well as simple socket usage.

