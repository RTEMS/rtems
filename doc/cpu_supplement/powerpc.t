@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@end ifinfo
@chapter PowerPC Specific Information

The Real Time Executive for Multiprocessor Systems
(RTEMS) is designed to be portable across multiple processor
architectures.  However, the nature of real-time systems makes
it essential that the application designer understand certain
processor dependent implementation details.  These processor
dependencies include calling convention, board support package
issues, interrupt processing, exact RTEMS memory requirements,
performance data, header files, and the assembly language
interface to the executive.

This document discusses the PowerPC architecture
dependencies in this port of RTEMS.

It is highly recommended that the PowerPC RTEMS
application developer obtain and become familiar with the
documentation for the processor being used as well as the
specification for the revision of the PowerPC architecture which
corresponds to that processor.

@subheading PowerPC Architecture Documents

For information on the PowerPC architecture, refer to
the following documents available from Motorola and IBM:

@itemize @bullet

@item @cite{PowerPC Microprocessor Family: The Programming Environment}
(Motorola Document MPRPPCFPE-01).

@item @cite{IBM PPC403GB Embedded Controller User's Manual}.

@item @cite{PoweRisControl MPC500 Family RCPU RISC Central Processing
Unit Reference Manual} (Motorola Document RCPUURM/AD).

@item @cite{PowerPC 601 RISC Microprocessor User's Manual}
(Motorola Document MPR601UM/AD).

@item @cite{PowerPC 603 RISC Microprocessor User's Manual}
(Motorola Document MPR603UM/AD).

@item @cite{PowerPC 603e RISC Microprocessor User's Manual}
(Motorola Document MPR603EUM/AD).

@item @cite{PowerPC 604 RISC Microprocessor User's Manual}
(Motorola Document MPR604UM/AD).

@item @cite{PowerPC MPC821 Portable Systems Microprocessor User's Manual}
(Motorola Document MPC821UM/AD).

@item @cite{PowerQUICC MPC860 User's Manual} (Motorola Document MPC860UM/AD).


@end itemize

Motorola maintains an on-line electronic library for the PowerPC
at the following URL:

@itemize @code{ }
@item @cite{http://www.mot.com/powerpc/library/library.html}
@end itemize

This site has a a wealth of information and examples.  Many of the
manuals are available from that site in electronic format.

@subheading PowerPC Processor Simulator Information

PSIM is a program which emulates the Instruction Set Architecture
of the PowerPC microprocessor family.  It is reely available in source
code form under the terms of the GNU General Public License (version
2 or later).  PSIM can be integrated with the GNU Debugger (gdb) to
execute and debug PowerPC executables on non-PowerPC hosts.  PSIM 
supports the addition of user provided device models which can be
used to allow one to develop and debug embedded applications using
the simulator.

The latest version of PSIM is made available to the public via
anonymous ftp at ftp://ftp.ci.com.au/pub/psim or
ftp://cambridge.cygnus.com/pub/psim.  There is also a mailing list 
at powerpc-psim@@ci.com.au.


@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section CPU Model Dependent Features


Microprocessors are generally classified into
families with a variety of CPU models or implementations within
that family.  Within a processor family, there is a high level
of binary compatibility.  This family may be based on either an
architectural specification or on maintaining compatibility with
a popular processor.  Recent microprocessor families such as the
SPARC, and PowerPC are based on an architectural specification
which is independent or any particular CPU model or
implementation.  Older families such as the M68xxx and the iX86
evolved as the manufacturer strived to produce higher
performance processor models which maintained binary
compatibility with older models.

RTEMS takes advantage of the similarity of the
various models within a CPU family.  Although the models do vary
in significant ways, the high level of compatibility makes it
possible to share the bulk of the CPU dependent executive code
across the entire family.

@subsection CPU Model Feature Flags

Each processor family supported by RTEMS has a
list of features which vary between CPU models
within a family.  For example, the most common model dependent
feature regardless of CPU family is the presence or absence of a
floating point unit or coprocessor.  When defining the list of
features present on a particular CPU model, one simply notes
that floating point hardware is or is not present and defines a
single constant appropriately.  Conditional compilation is
utilized to include the appropriate source code for this CPU
model's feature set.  It is important to note that this means
that RTEMS is thus compiled using the appropriate feature set
and compilation flags optimal for this CPU model used.  The
alternative would be to generate a binary which would execute on
all family members using only the features which were always
present.

This section presents the set of features which vary
across PowerPC implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
cpukit/score/cpu/ppc/ppc.h based upon the particular CPU
model defined on the compilation command line.

@subsubsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the PowerPC 603e
model, this macro is set to the string "PowerPC 603e".

@subsubsection Floating Point Unit

The macro PPC_HAS_FPU is set to 1 to indicate that this CPU model
has a hardware floating point unit and 0 otherwise.

@subsubsection Alignment

The macro PPC_ALIGNMENT is set to the PowerPC model's worst case alignment
requirement for data types on a byte boundary.  This value is used
to derive the alignment restrictions for memory allocated from 
regions and partitions.

@subsubsection Cache Alignment

The macro PPC_CACHE_ALIGNMENT is set to the line size of the cache.  It is
used to align the entry point of critical routines so that as much code
as possible can be retrieved with the initial read into cache.  This
is done for the interrupt handler as well as the context switch routines.

In addition, the "shortcut" data structure used by the PowerPC implementation
to ease access to data elements frequently accessed by RTEMS routines
implemented in assembly language is aligned using this value.

@subsubsection Maximum Interrupts

The macro PPC_INTERRUPT_MAX is set to the number of exception sources
supported by this PowerPC model.

@subsubsection Has Double Precision Floating Point

The macro PPC_HAS_DOUBLE is set to 1 to indicate that the PowerPC model
has support for double precision floating point numbers.  This is
important because the floating point registers need only be four bytes
wide (not eight) if double precision is not supported.

@subsubsection Critical Interrupts

The macro PPC_HAS_RFCI is set to 1 to indicate that the PowerPC model
has the Critical Interrupt capability as defined by the IBM 403 models.

@subsubsection Use Multiword Load/Store Instructions

The macro PPC_USE_MULTIPLE is set to 1 to indicate that multiword load and
store instructions should be used to perform context switch operations.
The relative efficiency of multiword load and store instructions versus
an equivalent set of single word load and store instructions varies based
upon the PowerPC model.

@subsubsection Instruction Cache Size

The macro PPC_I_CACHE is set to the size in bytes of the instruction cache.

@subsubsection Data Cache Size

The macro PPC_D_CACHE is set to the size in bytes of the data cache.

@subsubsection Debug Model

The macro PPC_DEBUG_MODEL is set to indicate the debug support features 
present in this CPU model.  The following debug support feature sets
are currently supported:

@table @b

@item @code{PPC_DEBUG_MODEL_STANDARD}
indicates that the single-step trace enable (SE) and branch trace
enable (BE) bits in the MSR are supported by this CPU model.

@item @code{PPC_DEBUG_MODEL_SINGLE_STEP_ONLY}
indicates that only the single-step trace enable (SE) bit in the MSR
is supported by this CPU model. 

@item @code{PPC_DEBUG_MODEL_IBM4xx}
indicates that the debug exception enable (DE) bit in the MSR is supported
by this CPU model.  At this time, this particular debug feature set 
has only been seen in the IBM 4xx series.

@end table

@subsubsection Low Power Model

The macro PPC_LOW_POWER_MODE is set to indicate the low power model
supported by this CPU model.  The following low power modes are currently
supported.

@table @b

@item @code{PPC_LOW_POWER_MODE_NONE}
indicates that this CPU model has no low power mode support.

@item @code{PPC_LOW_POWER_MODE_STANDARD}
indicates that this CPU model follows the low power model defined for
the PPC603e.

@end table
@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Calling Conventions


Each high-level language compiler generates
subroutine entry and exit code based upon a set of rules known
as the compiler's calling convention.   These rules address the
following issues:

@itemize @bullet
@item register preservation and usage

@item parameter passing

@item call and return mechanism
@end itemize

A compiler's calling convention is of importance when
interfacing to subroutines written in another language either
assembly or high-level.  Even when the high-level language and
target processor are the same, different compilers may use
different calling conventions.  As a result, calling conventions
are both processor and compiler dependent.

RTEMS supports the Embedded Application Binary Interface (EABI)
calling convention.  Documentation for EABI is available by sending
a message with a subject line of "EABI" to eabi@@goth.sis.mot.com.

@subsection Programming Model

This section discusses the programming model for the
PowerPC architecture.

@subsubsection Non-Floating Point Registers

The PowerPC architecture defines thirty-two non-floating point registers
directly visible to the programmer.  In thirty-two bit implementations, each
register is thirty-two bits wide.  In sixty-four bit implementations, each
register is sixty-four bits wide.

These registers are referred to as @code{gpr0} to @code{gpr31}.

Some of the registers serve defined roles in the EABI programming model.  
The following table describes the role of each of these registers:

@ifset use-ascii
@example
@group
     +---------------+----------------+------------------------------+
     | Register Name | Alternate Name |         Description          |
     +---------------+----------------+------------------------------+
     |      r1       |      sp        |         stack pointer        |
     +---------------+----------------+------------------------------+
     |               |                |  global pointer to the Small |
     |      r2       |      na        |     Constant Area (SDA2)     |
     +---------------+----------------+------------------------------+
     |    r3 - r12   |      na        | parameter and result passing |
     +---------------+----------------+------------------------------+
     |               |                |  global pointer to the Small |
     |      r13      |      na        |         Data Area (SDA)      |
     +---------------+----------------+------------------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 2.50in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Register Name &&\bf Alternate Names&&\bf Description&\cr\noalign{\hrule}
&r1&&sp&&stack pointer&\cr\noalign{\hrule}
&r2&&NA&&global pointer to the Small&\cr
&&&&&Constant Area (SDA2)&\cr\noalign{\hrule}
&r3 - r12&&NA&&parameter and result passing&\cr\noalign{\hrule}
&r13&&NA&&global pointer to the Small&\cr
&&&&&Data Area (SDA2)&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=3 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Register Name</STRONG></TD>
    <TD ALIGN=center><STRONG>Alternate Name</STRONG></TD>
    <TD ALIGN=center><STRONG>Description</STRONG></TD></TR>
<TR><TD ALIGN=center>r1</TD>
    <TD ALIGN=center>sp</TD>
    <TD ALIGN=center>stack pointer</TD></TR>
<TR><TD ALIGN=center>r2</TD>
    <TD ALIGN=center>na</TD>
    <TD ALIGN=center>global pointer to the Small Constant Area (SDA2)</TD></TR>
<TR><TD ALIGN=center>r3 - r12</TD>
    <TD ALIGN=center>NA</TD>
    <TD ALIGN=center>parameter and result passing</TD></TR>
<TR><TD ALIGN=center>r13</TD>
    <TD ALIGN=center>NA</TD>
    <TD ALIGN=center>global pointer to the Small Data Area (SDA)</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@subsubsection Floating Point Registers

The PowerPC architecture includes thirty-two, sixty-four bit
floating point registers.  All PowerPC floating point instructions 
interpret these registers as 32 double precision floating point registers,
regardless of whether the processor has 64-bit or 32-bit implementation.

The floating point status and control register (fpscr) records exceptions
and the type of result generated by floating-point operations.
Additionally, it controls the rounding mode of operations and allows the
reporting of floating exceptions to be enabled or disabled.

@subsubsection Special Registers

The PowerPC architecture includes a number of special registers
which are critical to the programming model:

@table @b

@item Machine State Register

The MSR contains the processor mode, power management mode, endian mode,
exception information, privilege level, floating point available and
floating point excepiton mode, address translation information and
the exception prefix.

@item Link Register

The LR contains the return address after a function call.  This register
must be saved before a subsequent subroutine call can be made.  The
use of this register is discussed further in the @b{Call and Return
Mechanism} section below.

@item Count Register

The CTR contains the iteration variable for some loops.  It may also be used
for indirect function calls and jumps.

@end table

@subsection Call and Return Mechanism

The PowerPC architecture supports a simple yet effective call
and return mechanism.  A subroutine is invoked
via the "branch and link" (@code{bl}) and
"brank and link absolute" (@code{bla})
instructions.  This instructions place the return address
in the Link Register (LR).  The callee returns to the caller by 
executing a "branch unconditional to the link register" (@code{blr})
instruction.  Thus the callee returns to the caller via a jump
to the return address which is stored in the LR.

The previous contents of the LR are not automatically saved 
by either the @code{bl} or @code{bla}.  It is the responsibility
of the callee to save the contents of the LR before invoking
another subroutine.  If the callee invokes another subroutine,
it must restore the LR before executing the @code{blr} instruction
to return to the caller.

It is important to note that the PowerPC subroutine
call and return mechanism does not automatically save and
restore any registers.

The LR may be accessed as special purpose register 8 (@code{SPR8}) using the
"move from special register" (@code{mfspr}) and
"move to special register" (@code{mtspr}) instructions.

@subsection Calling Mechanism

All RTEMS directives are invoked using the regular
PowerPC EABI calling convention via the @code{bl} or
@code{bla} instructions.

@subsection Register Usage

As discussed above, the call instruction does not
automatically save any registers.  It is the responsibility
of the callee to save and restore any registers which must be preserved
across subroutine calls.  The callee is responsible for saving 
callee-preserved registers to the program stack and restoring them
before returning to the caller.

@subsection Parameter Passing

RTEMS assumes that arguments are placed in the
general purpose registers with the first argument in 
register 3 (@code{r3}), the second argument in general purpose
register 4 (@code{r4}), and so forth until the seventh
argument is in general purpose register 10 (@code{r10}).  
If there are more than seven arguments, then subsequent arguments
are placed on the program stack.  The following pseudo-code
illustrates the typical sequence used to call a RTEMS directive
with three (3) arguments:

@example
load third argument into r5
load second argument into r4
load first argument into r3
invoke directive
@end example

@subsection User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these same calling conventions.


@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Memory Model


A processor may support any combination of memory
models ranging from pure physical addressing to complex demand
paged virtual memory systems.  RTEMS supports a flat memory
model which ranges contiguously over the processor's allowable
address space.  RTEMS does not support segmentation or virtual
memory of any kind.  The appropriate memory model for RTEMS
provided by the targeted processor and related characteristics
of that model are described in this chapter.

@subsection Flat Memory Model

The PowerPC architecture supports a variety of memory models.
RTEMS supports the PowerPC using a flat memory model with 
paging disabled.  In this mode, the PowerPC automatically
converts every address from a logical to a physical address
each time it is used.  The PowerPC uses information provided
in the Block Address Translation (BAT) to convert these addresses.

Implementations of the PowerPC architecture may be thirty-two or sixty-four bit.
The PowerPC architecture supports a flat thirty-two or sixty-four bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes) in thirty-two bit implementations or to 0xFFFFFFFFFFFFFFFF 
in sixty-four bit implementations.  Each address is represented
by either a thirty-two bit or sixty-four bit value and is byte addressable.  
The address may be used to reference a single byte, half-word
(2-bytes), word (4 bytes), or in sixty-four bit implementations a
doubleword (8 bytes).  Memory accesses within the address space are
performed in big or little endian fashion by the PowerPC based
upon the current setting of the Little-endian mode enable bit (LE)
in the Machine State Register (MSR).  While the processor is in
big endian mode, memory accesses which are not properly aligned
generate an "alignment exception" (vector offset 0x00600).  In
little endian mode, the PowerPC architecture does not require
the processor to generate alignment exceptions.

The following table lists the alignment requirements for a variety
of data accesses:

@ifset use-ascii
@example
@group
          +--------------+-----------------------+
          |   Data Type  | Alignment Requirement |
          +--------------+-----------------------+
          |     byte     |          1            |
          |   half-word  |          2            |
          |     word     |          4            |
          |  doubleword  |          8            |
          +--------------+-----------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Data Type &&\bf Alignment Requirement&\cr\noalign{\hrule}
&byte&&1&\cr\noalign{\hrule}
&half-word&&2&\cr\noalign{\hrule}
&word&&4&\cr\noalign{\hrule}
&doubleword&&8&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="60%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Data Type</STRONG></TD>
    <TD ALIGN=center><STRONG>Alignment Requirement</STRONG></TD></TR>
<TR><TD ALIGN=center>byte</TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=center>half-word</TD>
    <TD ALIGN=center>2</TD></TR>
<TR><TD ALIGN=center>word</TD>
    <TD ALIGN=center>4</TD></TR>
<TR><TD ALIGN=center>doubleword</TD>
    <TD ALIGN=center>8</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

Doubleword load and store operations are only available in 
PowerPC CPU models which are sixty-four bit implementations.

RTEMS does not directly support any PowerPC Memory Management
Units, therefore, virtual memory or segmentation systems
involving the PowerPC  are not supported.

@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Interrupt Processing


Different types of processors respond to the
occurrence of an interrupt in its own unique fashion. In
addition, each processor type provides a control mechanism to
allow for the proper handling of an interrupt.  The processor
dependent response to the interrupt modifies the current
execution state and results in a change in the execution stream.
Most processors require that an interrupt handler utilize some
special control mechanisms to return to the normal processing
stream.  Although RTEMS hides many of the processor dependent
details of interrupt processing, it is important to understand
how the RTEMS interrupt manager is mapped onto the processor's
unique architecture. Discussed in this chapter are the PowerPC's
interrupt response and control mechanisms as they pertain to
RTEMS.

RTEMS and associated documentation uses the terms
interrupt and vector.  In the PowerPC architecture, these terms
correspond to exception and exception handler, respectively.  The terms will
be used interchangeably in this manual.

@subsection Synchronous Versus Asynchronous Exceptions

In the PowerPC architecture exceptions can be either precise or 
imprecise and either synchronous or asynchronous.  Asynchronous 
exceptions occur when an external event interrupts the processor.
Synchronous exceptions are caused by the actions of an 
instruction. During an exception SRR0 is used to calculate where 
instruction processing should resume.  All instructions prior to
the resume instruction will have completed execution.  SRR1 is used to 
store the machine status.

There are two asynchronous nonmaskable, highest-priority exceptions
system reset and machine check.  There are two asynchrononous maskable
low-priority exceptions external interrupt and decrementer.  Nonmaskable
execptions are never delayed, therefore if two nonmaskable, asynchronous 
exceptions occur in immediate succession, the state information saved by
the first exception may be overwritten when the subsequent exception occurs. 

The PowerPC arcitecure defines one imprecise exception, the imprecise 
floating point enabled exception.  All other synchronous exceptions are
precise.  The synchronization occuring during asynchronous precise 
exceptions conforms to the requirements for context synchronization.

@subsection Vectoring of Interrupt Handler

Upon determining that an exception can be taken the PowerPC automatically
performs the following actions:

@itemize @bullet
@item an instruction address is loaded into SRR0

@item bits 33-36 and 42-47 of SRR1 are loaded with information 
specific to the exception.

@item bits 0-32, 37-41, and 48-63 of SRR1 are loaded with corresponding
bits from the MSR.

@item the MSR is set based upon the exception type.

@item instruction fetch and execution resumes, using the new MSR value, at a location specific to the execption type. 

@end itemize

If the interrupt handler was installed as an RTEMS
interrupt handler, then upon receipt of the interrupt, the
processor passes control to the RTEMS interrupt handler which
performs the following actions:

@itemize @bullet
@item saves the state of the interrupted task on it's stack,

@item saves all registers which are not normally preserved
by the calling sequence so the user's interrupt service
routine can be written in a high-level language.

@item if this is the outermost (i.e. non-nested) interrupt,
then the RTEMS interrupt handler switches from the current stack
to the interrupt stack,

@item enables exceptions,

@item invokes the vectors to a user interrupt service routine (ISR).
@end itemize

Asynchronous interrupts are ignored while exceptions are
disabled.  Synchronous interrupts which occur while are
disabled result in the CPU being forced into an error mode.

A nested interrupt is processed similarly with the
exception that the current stack need not be switched to the
interrupt stack.

@subsection Interrupt Levels

The PowerPC architecture supports only a single external
asynchronous interrupt source.  This interrupt source
may be enabled and disabled via the External Interrupt Enable (EE)
bit in the Machine State Register (MSR).  Thus only two level (enabled
and disabled) of external device interrupt priorities are 
directly supported by the PowerPC architecture.  

Some PowerPC implementations include a Critical Interrupt capability
which is often used to receive interrupts from high priority external
devices.

The RTEMS interrupt level mapping scheme for the PowerPC is not 
a numeric level as on most RTEMS ports.  It is a bit mapping in
which the least three significiant bits of the interrupt level
are mapped directly to the enabling of specific interrupt 
sources as follows:

@table @b

@item Critical Interrupt
Setting bit 0 (the least significant bit) of the interrupt level
enables the Critical Interrupt source, if it is available on this
CPU model.

@item Machine Check
Setting bit 1 of the interrupt level enables Machine Check execptions.

@item External Interrupt
Setting bit 2 of the interrupt level enables External Interrupt execptions.

@end table

All other bits in the RTEMS task interrupt level are ignored.

@subsection Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables Critical Interrupts, External Interrupts
and Machine Checks before the execution of this section and restores
them to the previous level upon completion of the section.  RTEMS has been
optimized to insure that interrupts are disabled for less than
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds on a 
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz PowerPC 603e with zero
wait states.  These numbers will vary based the number of wait 
states and processor speed present on the target board.
[NOTE:  The maximum period with interrupts disabled is hand calculated.  This
calculation was last performed for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

If a PowerPC implementation provides non-maskable interrupts (NMI)
which cannot be disabled, ISRs which process these interrupts
MUST NEVER issue RTEMS system calls.  If a directive is invoked,
unpredictable results may occur due to the inability of RTEMS
to protect its critical sections.  However, ISRs that make no
system calls may safely execute as non-maskable interrupts.

@subsection Interrupt Stack

The PowerPC architecture does not provide for a
dedicated interrupt stack.  Thus by default, exception handlers would
execute on the stack of the RTEMS task which they interrupted.
This artificially inflates the stack requirements for each task
since EVERY task stack would have to include enough space to
account for the worst case interrupt stack requirements in
addition to it's own worst case usage.  RTEMS addresses this
problem on the PowerPC by providing a dedicated interrupt stack
managed by software.

During system initialization, RTEMS allocates the
interrupt stack from the Workspace Area.  The amount of memory
allocated for the interrupt stack is determined by the
interrupt_stack_size field in the CPU Configuration Table.  As
part of processing a non-nested interrupt, RTEMS will switch to
the interrupt stack before invoking the installed handler.



@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Default Fatal Error Processing


Upon detection of a fatal error by either the
application or RTEMS the fatal error manager is invoked.  The
fatal error manager will invoke the user-supplied fatal error
handlers.  If no user-supplied handlers are configured,  the
RTEMS provided default fatal error handler is invoked.  If the
user-supplied fatal error handlers return to the executive the
default fatal error handler is then invoked.  This chapter
describes the precise operations of the default fatal error
handler.

@subsection Default Fatal Error Handler Operations

The default fatal error handler which is invoked by
the @code{rtems_fatal_error_occurred} directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler performs the following actions:

@itemize @bullet

@item places the error code in r3, and

@item executes a trap instruction which results in a Program Exception.

@end itemize

If the Program Exception returns, then the following actions are performed:

@itemize @bullet

@item disables all processor exceptions by loading a 0 into the MSR, and

@item goes into an infinite loop to simulate a halt processor instruction.

@end itemize

@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Board Support Packages


An RTEMS Board Support Package (BSP) must be designed
to support a particular processor and target board combination.
This chapter presents a discussion of PowerPC specific BSP issues.
For more information on developing a BSP, refer to the chapter
titled Board Support Packages in the RTEMS
Applications User's Guide.

@subsection System Reset

An RTEMS based application is initiated or
re-initiated when the PowerPC processor is reset.  The PowerPC 
architecture defines a Reset Exception, but leaves the
details of the CPU state as implementation specific.  Please
refer to the User's Manual for the CPU model in question.

In general, at power-up the PowerPC begin execution at address
0xFFF00100 in supervisor mode with all exceptions disabled.  For
soft resets, the CPU will vector to either 0xFFF00100 or 0x00000100
depending upon the setting of the Exception Prefix bit in the MSR.
If during a soft reset, a Machine Check Exception occurs, then the
CPU may execute a hard reset.

@subsection Processor Initialization

It is the responsibility of the application's
initialization code to initialize the CPU and board
to a quiescent state before invoking the @code{rtems_initialize_executive}
directive.  It is recommended that the BSP utilize the @code{predriver_hook}
to install default handlers for all exceptions.  These default handlers
may be overwritten as various device drivers and subsystems install
their own exception handlers.  Upon completion of RTEMS executive
initialization, all interrupts are enabled.

If this PowerPC implementation supports on-chip caching
and this is to be utilized, then it should be enabled during the
reset application initialization code.  On-chip caching has been
observed to prevent some emulators from working properly, so it
may be necessary to run with caching disabled to use these emulators.

In addition to the requirements described in the
@b{Board Support Packages} chapter of the RTEMS C
Applications User's Manual for the reset code
which is executed before the call to @code{rtems_initialize_executive},
the PowrePC version has the following specific requirements:

@itemize @bullet
@item Must leave the PR bit of the Machine State Register (MSR) set 
to 0 so the PowerPC remains in the supervisor state.

@item Must set stack pointer (sp or r1) such that a minimum stack
size of MINIMUM_STACK_SIZE bytes is provided for the
@code{rtems_initialize_executive} directive.

@item Must disable all external interrupts (i.e. clear the EI (EE)
bit of the machine state register).

@item Must enable traps so window overflow and underflow
conditions can be properly handled.

@item Must initialize the PowerPC's initial Exception Table with default
handlers.

@end itemize
