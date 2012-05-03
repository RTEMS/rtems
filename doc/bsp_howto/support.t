@c
@c  COPYRIGHT (c) 1988-2011.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Miscellaneous Support Files

@section GCC Compiler Specifications File

The file @code{bsp_specs} defines the start files and libraries
that are always used with this BSP.  The format of this file
is admittedly cryptic and this document will make no attempt
to explain it completely.  Below is the @code{bsp_specs}
file from the PowerPC psim BSP:

@example
@group
%rename endfile old_endfile
%rename startfile old_startfile
%rename link old_link

*startfile:
%@{!qrtems: %(old_startfile)@} \
%@{!nostdlib: %@{qrtems: ecrti%O%s rtems_crti%O%s crtbegin.o%s start.o%s@}@}

*link:
%@{!qrtems: %(old_link)@} %@{qrtems: -Qy -dp -Bstatic -e _start -u __vectors@}

*endfile:
%@{!qrtems: %(old_endfile)@} %@{qrtems: crtend.o%s ecrtn.o%s@}
@end group
@end example

The first section of this file renames the built-in definition of
some specification variables so they can be augmented without
embedded their original definition.  The subsequent sections
specify what behavior is expected when the @code{-qrtems} or
@code{-qrtems_debug} option is specified.

The @code{*startfile} section specifies that the BSP specific file
@code{start.o} will be used instead of @code{crt0.o}.  In addition,
various EABI support files (@code{ecrti.o} etc.) will be linked in with
the executable.

The @code{*link} section adds some arguments to the linker when it is
invoked by GCC to link an application for this BSP.

The format of this file is specific to the GNU Compiler Suite.  The
argument used to override and extend the compiler built-in specifications
is available in all recent GCC versions.  The @code{-specs} option is
present in all @code{egcs} distributions and @code{gcc} distributions
starting with version 2.8.0.

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

@end itemize

@section bsp.h Include File

The file @code{include/bsp.h} contains prototypes and definitions
specific to this board.  Every BSP is required to provide a @code{bsp.h}.
The best approach to writing a @code{bsp.h} is copying an existing one
as a starting point.

Many @code{bsp.h} files provide prototypes of variables defined
in the linker script (@code{linkcmds}).

@section tm27.h Include File

The @code{tm27} test from the RTEMS Timing Test Suite is designed to measure the length of time required to vector to and return from an interrupt handler. This test requires some help from the BSP to know how to cause and manipulate the interrupt source used for this measurement.  The following is a list of these:

@itemize @bullet
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

All members of the Timing Test Suite are designed to run @b{WITHOUT}
the Clock Device Driver installed.  This increases the predictability
of the tests' execution as well as avoids occassionally including the
overhead of a clock tick interrupt in the time reported.  Because of
this it is sometimes possible to use the clock tick interrupt source
as the source of this test interrupt.  On other architectures, it is
possible to directly force an interrupt to occur.

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

There is a shared implementation of @code{coverhd.h} which sets all of
the overhead constants to 0.  On faster processors, this is usually the
best alternative for the BSP as the calling overhead is extremely small.
This file is located at:

@example
c/src/lib/libbsp/shared/include/coverhd.h
@end example

@section sbrk() Implementation

Although nearly all BSPs give all possible memory to the C Program Heap
at initialization, it is possible for a BSP to configure the initial
size of the heap small and let it grow on demand.  If the BSP wants
to dynamically extend the heap used by the C Library memory allocation
routines (i.e. @code{malloc} family), then the@code{sbrk} routine must
be functional.  The following is the prototype for this routine:

@example
void * sbrk(size_t increment)
@end example

The @code{increment} amount is based upon the @code{sbrk_amount}
parameter passed to the @code{bsp_libc_init} during system initialization.
Historically initialization of the C Library was done as part of the
BSP's Pretasking Hook but now the BSP Boot Card Framework can perform
this operation.

@findex CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
If your BSP does not want to support dynamic heap extension, then you do not have to do anything special.  However, if you want to support @code{sbrk}, you must provide an implementation of this method and define @code{CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK} in @code{bsp.h}.  This informs @code{rtems/confdefs.h} to configure the Malloc Family Extensions which support @code{sbrk}.

@section bsp_cleanup(uint32_t status) - Cleanup the Hardware

The @code{bsp_cleanup()} is the last C code invoked.  Most of the BSPs
use the same shared version of @code{bsp_cleanup()} that does nothing.
This implementation is located in the following file:

@example
c/src/lib/libbsp/shared/bspclean.c
@end example

The @code{bsp_cleanup()} routine can be used to return to a ROM monitor,
insure that interrupt sources are disabled, etc..  This routine is the
last place to ensure a clean shutdown of the hardware.  The @code{status}
argument is the value passed to the service which initiated shutting
down RTEMS.  All of the non-fatal shutdown sequences ultimately pass
their exit status to @code{rtems_shutdown_executive} and this is what
is passed to this routine.

On some BSPs, it prints a message indicating that the application
completed execution and waits for the user to press a key before
resetting the board.  The PowerPC/gen83xx and PowerPC/gen5200 BSPs do
this when they are built to support the FreeScale evaluation boards.
This is convenient when using the boards in a development environment
and may be disabled for production use.

@section Configuration Macros

Each BSP can define macros in bsp.h which alter some of the the default configuration parameters in @code{rtems/confdefs.h}.  This section describes those macros:

@itemize @bullet

@findex CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
@item @code{CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK} must be defined if the
BSP has proper support for @code{sbrk}.  This is discussed in more detail
in the previous section.

@findex BSP_IDLE_TASK_BODY
@item @code{BSP_IDLE_TASK_BODY} may be defined to the entry point of a
BSP specific IDLE thread implementation.  This may be overridden if the
application provides its own IDLE task implementation.

@findex BSP_IDLE_TASK_STACK_SIZE
@item @code{BSP_IDLE_TASK_STACK_SIZE} may be defined to the desired
default stack size for the IDLE task as recommended when using this BSP.

@findex BSP_INTERRUPT_STACK_SIZE
@item @code{BSP_INTERRUPT_STACK_SIZE} may be defined to the desired default interrupt stack size as recommended when using this BSP.  This is sometimes required when the BSP developer has knowledge of stack intensive interrupt handlers.

@findex BSP_ZERO_WORKSPACE_AUTOMATICALLY
@item @code{BSP_ZERO_WORKSPACE_AUTOMATICALLY} is defined when the BSP
requires that RTEMS zero out the RTEMS C Program Heap at initialization.
If the memory is already zeroed out by a test sequence or boot ROM,
then the boot time can be reduced by not zeroing memory twice.

@findex BSP_DEFAULT_UNIFIED_WORK_AREAS
@item @code{BSP_DEFAULT_UNIFIED_WORK_AREAS} is defined when the BSP
recommends that the unified work areas configuration should always
be used.  This is desirable when the BSP is known to always have very
little RAM and thus saving memory by any means is desirable.

@end itemize

@section set_vector() - Install an Interrupt Vector

On targets with Simple Vectored Interrupts, the BSP must provide
an implementation of the @code{set_vector} routine.  This routine is
responsible for installing an interrupt vector.  It invokes the support
routines necessary to install an interrupt handler as either a "raw"
or an RTEMS interrupt handler.  Raw handlers bypass the RTEMS interrupt
structure and are responsible for saving and restoring all their own
registers.  Raw handlers are useful for handling traps, debug vectors,
etc..

The @code{set_vector} routine is a central place to perform interrupt
controller manipulation and encapsulate that information.  It is usually
implemented as follows:

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

@b{NOTE:}  The i386, PowerPC and ARM ports use a Programmable
Interrupt Controller model which does not require the BSP to implement
@code{set_vector}.  BSPs for these architectures must provide a different
set of support routines.

