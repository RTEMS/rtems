@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Miscellaneous Support Files

@section GCC Compiler Specifications File

The file @code{bsp_specs} defines the start files and libraries
that are always used with this BSP.  The format of this file
is admittedly cryptic and this document will make no attempt
to explain it completely.  Below is the @code{bsp_specs}
file from the PowerPC psim BSP:

@example
@group
%rename cpp old_cpp
%rename lib old_lib
%rename endfile old_endfile
%rename startfile old_startfile
%rename link old_link

*cpp:
%(old_cpp) %@{qrtems: -D__embedded__@} -Asystem(embedded)

*lib:
%@{!qrtems: %(old_lib)@} %@{qrtems: --start-group \
%@{!qrtems_debug: -lrtemsall@} %@{qrtems_debug: -lrtemsall_g@} \
-lc -lgcc --end-group  ecrtn%O%s \
%@{!qnolinkcmds: -T linkcmds%s@}@}

*startfile:
%@{!qrtems: %(old_startfile)@} %@{qrtems:  ecrti%O%s \
%@{!qrtems_debug: startsim.o%s@} \
%@{qrtems_debug: startsim_g.o%s@}@}

*link:
%@{!qrtems: %(old_link)@} %@{qrtems: -Qy -dp -Bstatic \
-T linkcmds%s -e _start -u __vectors@}
@end group
@end example

The first section of this file renames the built-in definition of 
some specification variables so they can be augmented without
embedded their original definition.  The subsequent sections
specify what behavior is expected when the @code{-qrtems} or
@code{-qrtems_debug} option is specified.  

The @code{*cpp} definition specifies that when @code{-qrtems}
is specified, predefine the preprocessor symbol @code{__embedded__}.

The @code{*lib} section insures that the RTEMS library, BSP specific
linker script, gcc support library, and the EABI specific @code{ecrtn}
file are used.

The @code{*startfile} section specifies that the BSP specific file
@code{startsim.o} will be used instead of @code{crt0.o}.  In addition,
the EABI specific file @code{ecrti.o} will be linked in with the
executable.

The @code{*link} section specifies the arguments that will be passed to
the linker.

The format of this file is specific to the GNU Compiler Suite.  The
argument used to override and extend the compiler built-in specifications
is relatively new to the toolset.  The @code{-specs} option is present
in all @code{egcs} distributions and @code{gcc} distributions starting
with version 2.8.0.

@section README Files

Most BSPs provide one or more @code{README} files.  Generally, there
is a @code{README} file at the top of the BSP source.  This file
describes the board and its hardware configuration, provides vendor
information, local configuration information, information on downloading
code to the board, debugging, etc..  The intent of this
file is to help someone begin to use the BSP faster.  

A @code{README} file in a BSP subdirectory typically explains something
about the contents of that subdirectory in greater detail.  For example,
it may list the documentation available for a particular peripheral
controller and how to obtain that documentation.  It may also explain some
particularly cryptic part of the software in that directory or provide
rationale on the implementation.

@section times

This file contains the results of the RTEMS Timing Test Suite.  It is
in a standard format so that results from one BSP can be easily compared
with those of another target board.

If a BSP supports multiple variants, then there may be multiple @code{times}
files.  Usually these are named @code{times.VARIANTn}.

@section Tools Subdirectory

Some BSPs provide additional tools that aid in using the target board.
These tools run on the development host and are built as part of building
the BSP.  Most common is a script to automate running the RTEMS Test Suites
on the BSP.  Examples of this include:

@itemize @bullet

@item @code{powerpc/psim} includes scripts to ease use of the simulator

@item @code{m68k/mvme162} includes a utility to download across the
VMEbus into target memory if the host is a VMEbus board in the same
chasis.

@item @code{unix/posix} includes scripts to run the tests automatically 
on this BSP.

@end itemize

@section bsp.h Include File

The file @code{include/bsp.h} contains prototypes and definitions 
specific to this board.  Every BSP is required to provide a @code{bsp.h}.
The best approach to writing a @code{bsp.h} is copying an existing one
as a starting point.  

Many @code{bsp.h} files provide prototypes of variables defined
in the linker script (@code{linkcmds}).

There are a number of fields in this file that are used only by the 
RTEMS Test Suites.  The following is a list of these:

@itemize @bullet
@item @code{MAX_LONG_TEST_DURATION} - the longest length of time a 
"long running" test should run.

@item @code{MAX_SHORT_TEST_DURATION} - the longest length of time a
"short running" test should run.

@item @code{MUST_WAIT_FOR_INTERRUPT} - modifies behavior of @code{tm27}.

@item @code{Install_tm27_vector} - installs the interrupt service 
routine for the Interrupt Benchmark Test (@code{tm27}).

@item @code{Cause_tm27_intr} - generates the interrupt source
used in the Interrupt Benchmark Test (@code{tm27}).

@item @code{Clear_tm27_intr} - clears the interrupt source
used in the Interrupt Benchmark Test (@code{tm27}).

@item @code{Lower_tm27_intr} - lowers the interrupt mask so the
interrupt source used in the Interrupt Benchmark Test (@code{tm27})
can generate a nested interrupt.

@end itemize

@section Calling Overhead File

The file @code{include/coverhd.h} contains the overhead associated
with invoking each directive.  This overhead consists of the execution
time required to package the parameters as well as to execute the "jump to
subroutine" and "return from subroutine" sequence.  The intent of this
file is to help separate the calling overhead from the actual execution
time of a directive.  This file is only used by the tests in the
RTEMS Timing Test Suite.

The numbers in this file are obtained by running the "Timer Overhead"
@code{tmoverhd} test.  The numbers in this file may be 0 and no
overhead is subtracted from the directive execution times reported by
the Timing Suite.

@section sbrk() Implementation

If the BSP wants to dynamically extend the heap used by the
C Library memory allocation routines (i.e. @code{malloc} family),
then this routine must be functional.  The following is the 
prototype for this routine:

@example
void * sbrk(size_t increment)
@end example

The @code{increment} amount is based upon the @code{sbrk_amount}
parameter passed to the @code{RTEMS_Malloc_Initialize} during system
initialization.
See @ref{Initialization Code RTEMS Pretasking Callback} for more
information.

There is a default implementation which returns an error to indicate
that the heap can not be extended.  This implementation can be
found in @code{c/src/lib/libbsp/shared/sbrk.c}.  Many of the BSPs
use this shared implementation.  In order to use this implementation,
the file @code{Makefile.in} in the BSP's @code{startup} directory
must be modified so that the @code{$VPATH} variable searches
both the @code{startup} directory and the shared directory.  The following
illustates the @code{VPATH} setting in the PowerPC psim BSP's 
@code{startup/Makefile.in}:

@example
VPATH = @@srcdir@@:@@srcdir@@/../../../shared
@end example

This instructs make to look in all of the directories in the @code{VPATH}
for the source files.  The directories will be examined in the order
they are specified.

@section bsp_cleanup() - Cleanup the Hardware

The @code{bsp_cleanup()} is the last C code invoked.  Most of the BSPs
use the same shared version of @code{bsp_cleanup()} that does nothing.
This implementation is located in the following file:

@example
c/src/lib/libbsp/shared/bspclean.c
@end example

The @code{bsp_cleanup()} routine can be used to return to a ROM monitor,
insure that interrupt sources are disabled, etc..  This routine is the
last place to insure a clean shutdown of the hardware.

@section set_vector() - Install an Interrupt Vector

The @code{set_vector} routine is responsible for installing an interrupt
vector.  It invokes the support routines necessary to install an
interrupt handler as either a "raw" or an RTEMS interrupt handler.  Raw
handlers bypass the RTEMS interrupt structure and are responsible for 
saving and restoring all their own registers.  Raw handlers are useful
for handling traps, debug vectors, etc..

The @code{set_vector} routine is a central place to perform 
interrupt controller manipulation and encapsulate that information.  
It is usually implemented as follows:

@example
@group
rtems_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
) 
@{ 
  if the type is RAW
    install the raw vector
  else
    use rtems_interrupt_catch to install the vector

  perform any interrupt controller necessary to unmask
    the interrupt source

  return the previous handler
@}
@end group
@end example


@b{NOTE:}  @code{set_vector} is provided by the majority of BSPs but
not all.  In particular, the i386 BSPs use a different scheme.  

