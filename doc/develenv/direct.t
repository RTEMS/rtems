@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Directory Structure

The RTEMS directory structure is designed to meet
the following requirements:

@itemize @bullet
@item encourage development of modular components.

@item isolate processor and target dependent code, while
allowing as much common source code as possible to be shared
across multiple processors and target boards.

@item allow multiple RTEMS users to perform simultaneous
compilation of RTEMS and its support facilities for different
processors and targets.
@end itemize

The resulting directory structure has processor and
board dependent source files isolated from generic files.  When
RTEMS is configured and built, object directories and
an install point will be automatically created based upon
the target CPU family and BSP selected.  

The placement of object files based upon the selected BSP name
ensures that object files are not mixed across CPUs or targets.
This in combination with the makefiles allows the specific
compilation options to be tailored for a particular target
board.  For example, the efficiency of the memory subsystem for
a particular target board may be sensitive to the alignment of
data structures, while on another target board with the same
processor memory may be very limited.  For the first target, the
options could specify very strict alignment requirements, while
on the second the data structures could be "packed" to conserve
memory.  It is impossible to achieve this degree of flexibility
without providing source code.

@c
@c  Directory Organization
@c
@section Directory Organization

The RTEMS source tree is organized based on the
following variables:

@itemize @bullet

@item functionality,
@item target processor family,
@item target processor model,
@item peripherals, and
@item target board.

@end itemize

Each of the following sections will describe the
contents of the directories in the RTEMS source
tree.  The top of the tree will be referenced
as @code{$@{RTEMS_ROOT@}} in this discussion.

@c
@c  Top Level Tree
@c

@c @ifset use-ascii
@example
@group
                      rtems-VERSION
                           |
   +--------+----+----+----+--+-----+---+------+-----+
   |        |    |    |       |     |   |      |     |
aclocal automake c contrib  cpukit doc make scripts tools
@end group
@end example
@c @end ifset

@ifset use-tex
@end ifset

@ifset use-html
@html
@end html
@end ifset

@table @code
@item $@{RTEMS_ROOT@}/aclocal/
This directory contains the custom M4 macros which are available to
the various GNU autoconf @code{configure.ac} scripts throughout 
the RTEMS source tree.  GNU autoconf interprets @code{configure.ac}
files to produce the @code{configure} files used to tailor 
RTEMS build for a particular host and target environment.  The
contents of this directory will not be discussed further in this
document.

@item $@{RTEMS_ROOT@}/automake/
This directory contains the custom GNU automake fragments 
which are used to support the various @code{Makefile.am}
files throughout the RTEMS source tree.  The
contents of this directory will not be discussed
further in this document.

@item $@{RTEMS_ROOT@}/c/
This directory is the root of the portions of the RTEMS source
tree which must be built tailored for a particular CPU model
or BSP.  The contents of this directory will be discussed
in the @ref{Directory Structure c/ Directory} section.

@item $@{RTEMS_ROOT@}/contrib/
This directory contains contributed support software.  Currently
this directory contains the RPM specifications for cross-compilers
hosted on GNU/Linux that target Cygwin and Solaris.  The 
cross-compilers produced using these specifications are then
used in a Canadian cross build procedure to produce the Cygwin
and Solaris hosted RTEMS toolsets on a GNU/Linux host.  This
directory will not be discussed further in this document.

@item $@{RTEMS_ROOT@}/cpukit/
This directory is the root for all of the "multilib'able"
portions of RTEMS.  This is a GNU way of saying the the
contents of this directory can be compiled like the 
C Library (@code{libc.a}) and the functionality is 
neither CPU model nor BSP specific.  The source code
for most RTEMS services reside under this directory.
The contents of this directory will be discussed
in the @ref{Directory Structure CPUKit Directory} section.

@item $@{RTEMS_ROOT@}/doc/
This directory is the root for all RTEMS documentation.
The source for RTEMS is written in GNU TeXinfo and 
used to produce HTML, PDF, and "info" files.
The RTEMS documentation is configured, built,
and installed separately from the RTEMS executive and tests.
The contents of this directory will be discussed
in the @ref{Directory Structure Documentation Directory} section.

@item $@{RTEMS_ROOT@}/make/
This directory contains files which support the
RTEMS Makefile's.  From a user's perspective, the
most important parts are found in the @code{custom/}
subdirectory.  Each ".cfg" file in this directory
is associated with a specific BSP and describes
the CPU model, compiler flags, and procedure to
a produce an executable for the target board.
These files are described in detail in the
@b{RTEMS BSP and Device Driver Development Guide}
and will not be discussed further in this document.

@item $@{RTEMS_ROOT@}/scripts/
This directory contains the RPM specifications for the
prebuilt cross-compilation toolsets provided by the
RTEMS project.  There are separate subdirectories
for each of the components in the RTEMS Cross Compilation
Environment including @code{binutils/}, @code{gcc3newlib/},
and @code{gdb/}.  This directory is configured, built,
and installed separately from the RTEMS executive
and tests.  This directory will not be discussed further
in this document.

@item $@{RTEMS_ROOT@}/tools/
This directory contains RTEMS specific support utilities which
execute on the development host.  These utilities are divided
into subdirectories based upon whether they are used in the process
of building RTEMS and applications, are CPU specific, or are
used to assist in updating the RTEMS source tree and applications. 
The support utilities used in the process of building RTEMS are
described in @ref{RTEMS Specific Utilities}.  These are the
only components of this subtree that will be discussed in this
document. 

@end table



@c
@c  c/ Directions
@c
@subsection c/ Directory

The @code{$@{RTEMS_ROOT@}/c/} directory was formerly
the root directory of all RTEMS source code.  At this time, it contains
the root directory for only those RTEMS components
which must be compiled or linked in a way that is specific to a
particular CPU model or board.  This directory contains the 
following subdirectories:

@table @code
@item $@{RTEMS_ROOT@}/c/src/
This directory is logically the root for the RTEMS components
which are CPU model or board dependent.  Thus this directory
is the root for the BSPs and the various Test Suites as well
as CPU model and BSP dependent libraries.  The contents of
this directory are discussed in the
@ref{Directory Structure c/src/ Directory} section.

@item $@{RTEMS_ROOT@}/c/make/
This directory is used to generate the file @code{target.cfg}
which is installed as part of the Application Makefiles.  This
file contains settings for various Makefile variables to 
tailor them to the particular CPU model and BSP configured.

@end table

@c
@c c/src/ Directory
@c 
@subsubsection c/src/ Directory

As mentioned previously, this directory is logically
the root for the RTEMS components
which are CPU model or board dependent.  The 
following is a list of the subdirectories in this
directorie and a description of each.

@table @code
@item $@{RTEMS_ROOT@}/c/src/ada-tests/
This directory contains the test suite for the Ada
language bindings to the Classic API.

@item $@{RTEMS_ROOT@}/c/src/lib/

@item $@{RTEMS_ROOT@}/c/src/libchip/
This directory contains device drivers for various
peripheral chips which are designed to be CPU and
board dependent.  This directory contains a variety
of drivers for serial devices, network interface
controllers, and real-time clocks.

@item $@{RTEMS_ROOT@}/c/src/libmisc/
This directory contains support facilities which 
are RTEMS specific but otherwise unclassified.  In
general, they do not adhere to a standard API.  
Among the support facilities in this directory are
a @code{/dev/null} device driver, the Stack
Overflow Checker, a mini-shell, the CPU and 
rate monotonic period usage monitoring libraries,
and a utility to "dump a buffer" in a nicely
formatted way similar to many ROM monitors.

@item $@{RTEMS_ROOT@}/c/src/libnetworking/
This directory contains the networking components which
might be tailored based upon the particular BSP.  This
includes the RTEMS telnetd, httpd, and ftpd servers.

@item $@{RTEMS_ROOT@}/c/src/librdbg/
This directory contains the Ethernet-based remote debugging
stub.  This software must be built to be intimately aware
of a particular CPU model.

@item $@{RTEMS_ROOT@}/c/src/librtems++/

@item $@{RTEMS_ROOT@}/c/src/make/

@item $@{RTEMS_ROOT@}/c/src/optman/

@item $@{RTEMS_ROOT@}/c/src/tests/
This directory contains the test suites for the 
various RTEMS APIs and support libraries.  This
contents of this directory are discussed in the
@ref{Directory Structure Test Suites} section.

@item $@{RTEMS_ROOT@}/c/src/wrapup/

@end table

@c
@c  Test Suites
@c
@lowersections
@subsubsection Test Suites

The following table lists the test suites currently included with the
RTEMS and the directory in which they may be located:

@table @code

@item @{RTEMS_ROOT@}c/src/tests/itrontests
ITRON API Tests

@item @{RTEMS_ROOT@}c/src/tests/libtests
Support Library Tests

@item @{RTEMS_ROOT@}c/src/tests/mptests
Classic API Multiprocessor Tests

@item @{RTEMS_ROOT@}c/src/tests/psxtests
POSIX API Tests

@item @{RTEMS_ROOT@}c/src/tests/samples
Sample Applications

@item @{RTEMS_ROOT@}c/src/tests/sptests
Classic API Single Processor Tests

@item @{RTEMS_ROOT@}c/src/tests/support
Test Support Software

@item @{RTEMS_ROOT@}c/src/tests/tmitrontests
ITRON API Timing Tests

@item @{RTEMS_ROOT@}c/src/tests/tmtests
Classic API Timing Tests

@item @{RTEMS_ROOT@}c/src/tests/tools
XXX

@end table

@raisesections


@c
@c  CPUKit Directory
@c
@subsection CPUKit Directory

The @code{cpukit/} directory structure is as follows:

@c
@c  CPUKit Tree
@c

@ifset use-ascii
@example
@group
                       CPUKit
                         |
  +-----------+----------+-----------+----------+
  |           |          |           |          |
posix       rtems       sapi       score     wrapup
@end group
@end example
@end ifset

This directory contains a set of subdirectories which
contains the source files comprising the executive portion of
the RTEMS development environment as well as portable support
libraries such as support for the C Library and filesystems.
The API specific and "SuperCore" (e.g. @code{score/} directory)
source code files are separated into
distinct directory trees.  The @code{rtems/}, @code{posix/}, and
@code{itron/} subdirectories contain the C language source files for each
module comprising the respective API.  Also included in this
directory are the subdirectories @code{sapi/} and @code{score/} which are
the SuperCore modules.  Within the @code{score/} directory the CPU
dependent modules are found.

The @code{score/cpu/} subdirectory contains a subdirectory for each
target CPU supported by the @value{RELEASE} release of the RTEMS
executive.  Each processor directory contains the CPU dependent
code necessary to host RTEMS.  The "no_cpu" directory provides a
starting point for developing a new port to an unsupported
processor.  The files contained within the "no_cpu" directory
may also be used as a reference for the other ports to specific
processors.


@c
@c  
@c
@subsection Documentation Directory

XXX 

@c
@c  
@c
@subsection Support Library Source Directory

The "lib" directory contains the support libraries and BSPS.  
Board support packages (BSPs), processor environment start up code,
C library support, the FreeBSD TCP/IP stack, common BSP header files,
and miscellaneous support functions are provided in the subdirectories.
These are combined with the RTEMS executive object to form the single 
RTEMS library which installed.  

@c
@c  Tree 6 - Libraries
@c


The "libbsp" directory contains a directory for each CPU family supported
by RTEMS.  Beneath each CPU directory is a directory for each BSP for that
processor family.

@c
@c  Tree 7 - C BSP Library
@c

The "libbsp" directory provides all the BSPs provided with this
release of the RTEMS executive.  The subdirectories are
divided,  as discussed previously, based on specific processor
family, then further breaking down into specific target board
environments.  The "shmdr" subdirectory provides the
implementation of a shared memory driver which supports the
multiprocessing portion of the executive.  In addition, two
starting point subdirectories are provided for reference.  The
"no_cpu" subdirectory provides a template BSP which can be used
to develop a specific BSP for an unsupported target board.  The
"stubdr" subdirectory provides stubbed out BSPs.  These files
may aid in preliminary testing of the RTEMS development
environment that has been built for no particular target in mind.

Below each CPU dependent directory is a directory for each target BSP
supported in this release.

Each BSP provides the modules which comprise an RTEMS BSP.  The 
modules are separated into the subdirectories "clock", "console", 
"include", "shmsupp", "startup", and "timer" as shown in the following 
figure:

@c
@c  Tree 8 - Each BSP
@c

@ifset use-ascii
@example
@group
                           Each BSP
                               |
  +-----------+----------+-----+-----+----------+----------+
  |           |          |           |          |          |
clock      console    include     shmsupp    startup     timer
@end group
@end example
@end ifset

@ifset use-tex
@sp 1

@tex
{\parskip=0pt\offinterlineskip%
\hskip 10.0em
\hskip 10.25em\hbox to 4.50em{\hss{Each BSP}\hss}%
\vrule width0em height1.972ex depth0.812ex\par\penalty10000
\hskip 10.0em
\hskip 12.50em\vrule width.04em%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 10.0em
\hskip 1.25em\vrule width11.25em height-0.407ex depth0.500ex%
\vrule width.04em\vrule width11.25em height-0.407ex depth0.500ex%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 10.0em
\hskip 1.25em\vrule width.04em%
\hskip 4.46em\vrule width.04em%
\hskip 4.46em\vrule width.04em%
\hskip 4.46em\vrule width.04em%
\hskip 4.46em\vrule width.04em%
\hskip 4.46em\vrule width.04em%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 10.0em
\hskip 0.00em\hbox to 2.50em{\hss{clock}\hss}%
\hskip 1.50em\hbox to 3.50em{\hss{console}\hss}%
\hskip 1.00em\hbox to 3.50em{\hss{include}\hss}%
\hskip 1.00em\hbox to 3.50em{\hss{shmsupp}\hss}%
\hskip 1.00em\hbox to 3.50em{\hss{startup}\hss}%
\hskip 1.50em\hbox to 2.50em{\hss{timer}\hss}%
\vrule width0em height1.972ex depth0.812ex\par}
@end tex
@end ifset

@ifset use-html
@example
@group
                           Each BSP
                               |
  +-----------+----------+-----+-----+----------+----------+
  |           |          |           |          |          |
clock      console    include       ...      startup     timer
@end group
@end example
@html
@end html
@end ifset

@c
@c  
@c
@subsection Test Suite Source Directory

The "tests" directory structure for the C
implementation is as follows:

@c
@c  Tree 9 - C Tests
@c

@ifset use-ascii
@example
@group
                                C Tests
                                   |
   +----------+---------+----------+---------+---------+---------+
   |          |         |          |         |         |         |
libtests   sptests   support    tmtests   mptests   psxtest   samples
@end group
@end example
@end ifset

@ifset use-tex
@sp 1

@tex
{\parskip=0pt\offinterlineskip%
\hskip 05.0em
\hskip 14.50em\hbox to 3.50em{\hss{C Tests}\hss}%
\vrule width0em height1.972ex depth0.812ex\par\penalty10000
\hskip 05.0em
\hskip 16.25em\vrule width.04em%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 05.0em
\hskip 2.00em\vrule width14.25em height-0.407ex depth0.500ex%
\vrule width.04em\vrule width14.25em height-0.407ex depth0.500ex%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 05.0em
\hskip 2.00em\vrule width.04em%
\hskip 4.71em\vrule width.04em%
\hskip 4.71em\vrule width.04em%
\hskip 4.71em\vrule width.04em%
\hskip 4.71em\vrule width.04em%
\hskip 4.71em\vrule width.04em%
\hskip 4.71em\vrule width.04em%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 05.0em
\hskip 0.00em\hbox to 4.00em{\hss{libtests}\hss}%
\hskip 1.00em\hbox to 3.50em{\hss{sptests}\hss}%
\hskip 1.25em\hbox to 3.50em{\hss{support}\hss}%
\hskip 1.25em\hbox to 3.50em{\hss{tmtests}\hss}%
\hskip 1.25em\hbox to 3.50em{\hss{mptests}\hss}%
\hskip 1.75em\hbox to 2.50em{\hss{tools}\hss}%
\hskip 1.75em\hbox to 3.50em{\hss{samples}\hss}%
\vrule width0em height1.972ex depth0.812ex\par}
@end tex
@end ifset

@ifset use-html
@example
@group
                                C Tests
                                   |
   +----------+---------+----------+---------+---------+---------+
   |          |         |          |         |         |         |
libtests   sptests   support    tmtests   mptests    tools    samples
@end group
@end example
@html
@end html
@end ifset

This directory provides the entire RTEMS Test Suite
which includes the single processor tests, multiprocessor tests,
timing tests, library tests, and sample tests.   Additionally,
subdirectories for support functions and test related header
files are provided.

The "sptests" subdirectory consists of twenty-four
tests designed to cover the entire executive code.  The
"spfatal" test will verify any code associated with the
occurrence of a fatal error.   Also provided is a test which
will determine the size of the RTEMS executive.

The multiprocessor test are provided in "mptests".
Fourteen tests are provided in this subdirectory which address
two node configurations and cover the multiprocessor code found
in RTEMS.

Tests that time each directive and a set of critical
executive functions are provided in the "tmtests" subdirectory.
Within this subdirectory  thirty-one tests are provided along
with a subdirectory to contain each targets timing results.

The "samples" directory structure for the C
implementation is as follows:

@c
@c  Tree 10 - C Samples
@c

@ifset use-ascii
@example
@group
                            C Samples
                                |
   +-----------+----------+-----+-----+----------+----------+
   |           |          |           |          |          |
base_mp     base_sp    cdtest       hello     paranoia    ticker
@end group
@end example
@end ifset

@ifset use-tex
@sp 1

@tex
{\parskip=0pt\offinterlineskip%
\hskip 05.0em
\hskip 12.25em\hbox to 4.50em{\hss{C Samples}\hss}%
\vrule width0em height1.972ex depth0.812ex\par\penalty10000
\hskip 05.0em
\hskip 14.50em\vrule width.04em%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 05.0em
\hskip 2.00em\vrule width12.50em height-0.407ex depth0.500ex%
\vrule width.04em\vrule width12.50em height-0.407ex depth0.500ex%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 05.0em
\hskip 2.00em\vrule width.04em%
\hskip 4.96em\vrule width.04em%
\hskip 4.96em\vrule width.04em%
\hskip 4.96em\vrule width.04em%
\hskip 4.96em\vrule width.04em%
\hskip 4.96em\vrule width.04em%
\vrule width0em height1.500ex depth0.500ex\par\penalty10000
\hskip 05.0em
\hskip 0.00em\hbox to 4.00em{\hss{base\_mp}\hss}%
\hskip 1.00em\hbox to 4.00em{\hss{base\_sp}\hss}%
\hskip 1.50em\hbox to 3.00em{\hss{cdtest}\hss}%
\hskip 2.25em\hbox to 2.50em{\hss{hello}\hss}%
\hskip 1.75em\hbox to 4.00em{\hss{paranoia}\hss}%
\hskip 1.50em\hbox to 3.00em{\hss{ticker}\hss}%
\vrule width0em height1.972ex depth0.812ex\par}
@end tex
@end ifset

@ifset use-html
@example
@group
                            C Samples
                                |
   +-----------+----------+-----+-----+----------+----------+
   |           |          |           |          |          |
base_mp     base_sp    cdtest       hello     paranoia    ticker
@end group
@end example
@html
@end html
@end ifset

This directory provides sample application tests
which aid in the testing a newly built RTEMS environment, a new
BSP, or as starting points for the development of an application
using the RTEMS executive.  A Hello World test is provided in
the subdirectory "hello".  This test is helpful when testing new
versions of RTEMS, BSPs, or modifications to any portion of the
RTEMS development environment.  The "ticker" subdirectory
provides a test for verification of clock chip device drivers of
BSPs.  A simple single processor test similar to those in the
single processor test suite is provided in "base_sp".  A simple
two node multiprocessor test capable of testing an newly
developed MPCI layer is provided in "base_mp".  The "cdtest"
subdirectory provides a simple C++ application using
constructors and destructors.   The final sample test is a
public domain floating point and math library toolset test is
provided in "paranoia".
