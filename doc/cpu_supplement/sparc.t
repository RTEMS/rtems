@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter SPARC Specific Information

The Real Time Executive for Multiprocessor Systems
(RTEMS) is designed to be portable across multiple processor
architectures.  However, the nature of real-time systems makes
it essential that the application designer understand certain
processor dependent implementation details.  These processor
dependencies include calling convention, board support package
issues, interrupt processing, exact RTEMS memory requirements,
performance data, header files, and the assembly language
interface to the executive.

This document discusses the SPARC architecture
dependencies in this port of RTEMS.  Currently, only
implementations of SPARC Version 7 are supported by RTEMS.

It is highly recommended that the SPARC RTEMS
application developer obtain and become familiar with the
documentation for the processor being used as well as the
specification for the revision of the SPARC architecture which
corresponds to that processor.

@subheading SPARC Architecture Documents

For information on the SPARC architecture, refer to
the following documents available from SPARC International, Inc.
(http://www.sparc.com):

@itemize @bullet
@item SPARC Standard Version 7.

@item SPARC Standard Version 8.

@item SPARC Standard Version 9.
@end itemize

@subheading ERC32 Specific Information

The European Space Agency's ERC32 is a three chip
computing core implementing a SPARC V7 processor and associated
support circuitry for embedded space applications. The integer
and floating-point units (90C601E & 90C602E) are based on the
Cypress 7C601 and 7C602, with additional error-detection and
recovery functions. The memory controller (MEC) implements
system support functions such as address decoding, memory
interface, DMA interface, UARTs, timers, interrupt control,
write-protection, memory reconfiguration and error-detection.
The core is designed to work at 25MHz, but using space qualified
memories limits the system frequency to around 15 MHz, resulting
in a performance of 10 MIPS and 2 MFLOPS.

Information on the ERC32 and a number of development
support tools, such as the SPARC Instruction Simulator (SIS),
are freely available on the Internet.  The following documents
and SIS are available via anonymous ftp or pointing your web
browser at ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.

@itemize @bullet
@item ERC32 System Design Document

@item MEC Device Specification
@end itemize

Additionally, the SPARC RISC User's Guide from Matra
MHS documents the functionality of the integer and floating
point units including the instruction set information.  To
obtain this document as well as ERC32 components and VHDL models
contact:

@example
Matra MHS SA
3 Avenue du Centre, BP 309,
78054 St-Quentin-en-Yvelines,
Cedex, France
VOICE: +31-1-30607087
FAX: +31-1-30640693
@end example

Amar Guennon (amar.guennon@@matramhs.fr) is familiar with the ERC32.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@section CPU Model Dependent Features


Microprocessors are generally classified into
families with a variety of CPU models or implementations within
that family.  Within a processor family, there is a high level
of binary compatibility.  This family may be based on either an
architectural specification or on maintaining compatibility with
a popular processor.  Recent microprocessor families such as the
SPARC or PowerPC are based on an architectural specification
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
across SPARC implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
cpukit/score/cpu/sparc/sparc.h based upon the particular CPU
model defined on the compilation command line.

@subsubsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the European Space
Agency's ERC32 SPARC model, this macro is set to the string
"erc32".

@subsubsection Floating Point Unit

The macro SPARC_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.

@subsubsection Bitscan Instruction

The macro SPARC_HAS_BITSCAN is set to 1 to indicate
that this CPU model has the bitscan instruction.  For example,
this instruction is supported by the Fujitsu SPARClite family.

@subsubsection Number of Register Windows

The macro SPARC_NUMBER_OF_REGISTER_WINDOWS is set to
indicate the number of register window sets implemented by this
CPU model.  The SPARC architecture allows a for a maximum of
thirty-two register window sets although most implementations
only include eight.

@subsubsection Low Power Mode

The macro SPARC_HAS_LOW_POWER_MODE is set to one to
indicate that this CPU model has a low power mode.  If low power
is enabled, then there must be CPU model specific implementation
of the IDLE task in cpukit/score/cpu/sparc/cpu.c.  The low
power mode IDLE task should be of the form:

@example
while ( TRUE ) @{
  enter low power mode
@}
@end example

The code required to enter low power mode is CPU model specific.

@subsection CPU Model Implementation Notes 

The ERC32 is a custom SPARC V7 implementation based on the Cypress 601/602
chipset.  This CPU has a number of on-board peripherals and was developed by
the European Space Agency to target space applications.  RTEMS currently
provides support for the following peripherals:

@itemize @bullet
@item UART Channels A and B
@item General Purpose Timer
@item Real Time Clock
@item Watchdog Timer (so it can be disabled)
@item Control Register (so powerdown mode can be enabled)
@item Memory Control Register
@item Interrupt Control
@end itemize

The General Purpose Timer and Real Time Clock Timer provided with the ERC32
share the Timer Control Register.  Because the Timer Control Register is write
only, we must mirror it in software and insure that writes to one timer do not
alter the current settings and status of the other timer.  Routines are
provided in erc32.h which promote the view that the two timers are completely
independent.  By exclusively using these routines to access the Timer Control
Register, the application can view the system as having a General Purpose
Timer Control Register and a Real Time Clock Timer Control Register
rather than the single shared value.

The RTEMS Idle thread take advantage of the low power mode provided by the
ERC32.  Low power mode is entered during idle loops and is enabled at
initialization time.
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

@subsection Programming Model

This section discusses the programming model for the
SPARC architecture.

@subsubsection Non-Floating Point Registers

The SPARC architecture defines thirty-two
non-floating point registers directly visible to the programmer.
These are divided into four sets:

@itemize @bullet
@item input registers

@item local registers

@item output registers

@item global registers
@end itemize

Each register is referred to by either two or three
names in the SPARC reference manuals.  First, the registers are
referred to as r0 through r31 or with the alternate notation
r[0] through r[31].  Second, each register is a member of one of
the four sets listed above.  Finally, some registers have an
architecturally defined role in the programming model which
provides an alternate name.  The following table describes the
mapping between the 32 registers and the register sets:

@ifset use-ascii
@example
@group
     +-----------------+----------------+------------------+
     | Register Number | Register Names |   Description    |
     +-----------------+----------------+------------------+
     |     0 - 7       |    g0 - g7     | Global Registers |
     +-----------------+----------------+------------------+
     |     8 - 15      |    o0 - o7     | Output Registers |
     +-----------------+----------------+------------------+
     |    16 - 23      |    l0 - l7     | Local Registers  |
     +-----------------+----------------+------------------+
     |    24 - 31      |    i0 - i7     | Input Registers  |
     +-----------------+----------------+------------------+
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
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Register Number &&\bf Register Names&&\bf Description&\cr\noalign{\hrule}
&0 - 7&&g0 - g7&&Global Registers&\cr\noalign{\hrule}
&8 - 15&&o0 - o7&&Output Registers&\cr\noalign{\hrule}
&16 - 23&&l0 - l7&&Local Registers&\cr\noalign{\hrule}
&24 - 31&&i0 - i7&&Input Registers&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=3 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Register Number</STRONG></TD>
    <TD ALIGN=center><STRONG>Register Names</STRONG></TD>
    <TD ALIGN=center><STRONG>Description</STRONG></TD>
<TR><TD ALIGN=center>0 - 7</TD>
    <TD ALIGN=center>g0 - g7</TD>
    <TD ALIGN=center>Global Registers</TD></TR>
<TR><TD ALIGN=center>8 - 15</TD>
    <TD ALIGN=center>o0 - o7</TD>
    <TD ALIGN=center>Output Registers</TD></TR>
<TR><TD ALIGN=center>16 - 23</TD>
    <TD ALIGN=center>l0 - l7</TD>
    <TD ALIGN=center>Local Registers</TD></TR>
<TR><TD ALIGN=center>24 - 31</TD>
    <TD ALIGN=center>i0 - i7</TD>
    <TD ALIGN=center>Input Registers</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

As mentioned above, some of the registers serve
defined roles in the programming model.  The following table
describes the role of each of these registers:

@ifset use-ascii
@example
@group
     +---------------+----------------+----------------------+
     | Register Name | Alternate Name |      Description     |
     +---------------+----------------+----------------------+
     |     g0        |      na        |    reads return 0    |
     |               |                |  writes are ignored  |
     +---------------+----------------+----------------------+
     |     o6        |      sp        |     stack pointer    |
     +---------------+----------------+----------------------+
     |     i6        |      fp        |     frame pointer    |
     +---------------+----------------+----------------------+
     |     i7        |      na        |    return address    |
     +---------------+----------------+----------------------+
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
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Register Name &&\bf Alternate Names&&\bf Description&\cr\noalign{\hrule}
&g0&&NA&&reads return 0; &\cr
&&&&&writes are ignored&\cr\noalign{\hrule}
&o6&&sp&&stack pointer&\cr\noalign{\hrule}
&i6&&fp&&frame pointer&\cr\noalign{\hrule}
&i7&&NA&&return address&\cr\noalign{\hrule}
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
<TR><TD ALIGN=center>g0</TD>
    <TD ALIGN=center>NA</TD>
    <TD ALIGN=center>reads return 0 ; writes are ignored</TD></TR>
<TR><TD ALIGN=center>o6</TD>
    <TD ALIGN=center>sp</TD>
    <TD ALIGN=center>stack pointer</TD></TR>
<TR><TD ALIGN=center>i6</TD>
    <TD ALIGN=center>fp</TD>
    <TD ALIGN=center>frame pointer</TD></TR>
<TR><TD ALIGN=center>i7</TD>
    <TD ALIGN=center>NA</TD>
    <TD ALIGN=center>return address</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@subsubsection Floating Point Registers

The SPARC V7 architecture includes thirty-two,
thirty-two bit registers.  These registers may be viewed as
follows:

@itemize @bullet
@item 32 single precision floating point or integer registers
(f0, f1,  ... f31)

@item 16 double precision floating point registers (f0, f2,
f4, ... f30)

@item 8 extended precision floating point registers (f0, f4,
f8, ... f28)
@end itemize

The floating point status register (fpsr) specifies
the behavior of the floating point unit for rounding, contains
its condition codes, version specification, and trap information.

A queue of the floating point instructions which have
started execution but not yet completed is maintained.  This
queue is needed to support the multiple cycle nature of floating
point operations and to aid floating point exception trap
handlers.  Once a floating point exception has been encountered,
the queue is frozen until it is emptied by the trap handler.
The floating point queue is loaded by launching instructions.
It is emptied normally when the floating point completes all
outstanding instructions and by floating point exception
handlers with the store double floating point queue (stdfq)
instruction.

@subsubsection Special Registers

The SPARC architecture includes two special registers
which are critical to the programming model: the Processor State
Register (psr) and the Window Invalid Mask (wim).  The psr
contains the condition codes, processor interrupt level, trap
enable bit, supervisor mode and previous supervisor mode bits,
version information, floating point unit and coprocessor enable
bits, and the current window pointer (cwp).  The cwp field of
the psr and wim register are used to manage the register windows
in the SPARC architecture.  The register windows are discussed
in more detail below.

@subsection Register Windows

The SPARC architecture includes the concept of
register windows.  An overly simplistic way to think of these
windows is to imagine them as being an infinite supply of
"fresh" register sets available for each subroutine to use.  In
reality, they are much more complicated.

The save instruction is used to obtain a new register
window.  This instruction decrements the current window pointer,
thus providing a new set of registers for use.  This register
set includes eight fresh local registers for use exclusively by
this subroutine.  When done with a register set, the restore
instruction increments the current window pointer and the
previous register set is once again available.

The two primary issues complicating the use of
register windows are that (1) the set of register windows is
finite, and (2) some registers are shared between adjacent
registers windows.

Because the set of register windows is finite, it is
possible to execute enough save instructions without
corresponding restore's to consume all of the register windows.
This is easily accomplished in a high level language because
each subroutine typically performs a save instruction upon
entry.  Thus having a subroutine call depth greater than the
number of register windows will result in a window overflow
condition.  The window overflow condition generates a trap which
must be handled in software.  The window overflow trap handler
is responsible for saving the contents of the oldest register
window on the program stack.

Similarly, the subroutines will eventually complete
and begin to perform restore's.  If the restore results in the
need for a register window which has previously been written to
memory as part of an overflow, then a window underflow condition
results.  Just like the window overflow, the window underflow
condition must be handled in software by a trap handler.  The
window underflow trap handler is responsible for reloading the
contents of the register window requested by the restore
instruction from the program stack.

The Window Invalid Mask (wim) and the Current Window
Pointer (cwp) field in the psr are used in conjunction to manage
the finite set of register windows and detect the window
overflow and underflow conditions.  The cwp contains the index
of the register window currently in use.  The save instruction
decrements the cwp modulo the number of register windows.
Similarly, the restore instruction increments the cwp modulo the
number of register windows.  Each bit in the  wim represents
represents whether a register window contains valid information.
The value of 0 indicates the register window is valid and 1
indicates it is invalid.  When a save instruction causes the cwp
to point to a register window which is marked as invalid, a
window overflow condition results.  Conversely, the restore
instruction may result in a window underflow condition.

Other than the assumption that a register window is
always available for trap (i.e. interrupt) handlers, the SPARC
architecture places no limits on the number of register windows
simultaneously marked as invalid (i.e. number of bits set in the
wim).  However, RTEMS assumes that only one register window is
marked invalid at a time (i.e. only one bit set in the wim).
This makes the maximum possible number of register windows
available to the user while still meeting the requirement that
window overflow and underflow conditions can be detected.

The window overflow and window underflow trap
handlers are a critical part of the run-time environment for a
SPARC application.  The SPARC architectural specification allows
for the number of register windows to be any power of two less
than or equal to 32.  The most common choice for SPARC
implementations appears to be 8 register windows.  This results
in the cwp ranging in value from 0 to 7 on most implementations.


The second complicating factor is the sharing of
registers between adjacent register windows.  While each
register window has its own set of local registers, the input
and output registers are shared between adjacent windows.  The
output registers for register window N are the same as the input
registers for register window ((N - 1) modulo RW) where RW is
the number of register windows.  An alternative way to think of
this is to remember how parameters are passed to a subroutine on
the SPARC.  The caller loads values into what are its output
registers.  Then after the callee executes a save instruction,
those parameters are available in its input registers.  This is
a very efficient way to pass parameters as no data is actually
moved by the save or restore instructions.

@subsection Call and Return Mechanism

The SPARC architecture supports a simple yet
effective call and return mechanism.  A subroutine is invoked
via the call (call) instruction.  This instruction places the
return address in the caller's output register 7 (o7).  After
the callee executes a save instruction, this value is available
in input register 7 (i7) until the corresponding restore
instruction is executed.

The callee returns to the caller via a jmp to the
return address.  There is a delay slot following this
instruction which is commonly used to execute a restore
instruction -- if a register window was allocated by this
subroutine.

It is important to note that the SPARC subroutine
call and return mechanism does not automatically save and
restore any registers.  This is accomplished via the save and
restore instructions which manage the set of registers windows.

@subsection Calling Mechanism

All RTEMS directives are invoked using the regular
SPARC calling convention via the call instruction.

@subsection Register Usage

As discussed above, the call instruction does not
automatically save any registers.  The save and restore
instructions are used to allocate and deallocate register
windows.  When a register window is allocated, the new set of
local registers are available for the exclusive use of the
subroutine which allocated this register set.

@subsection Parameter Passing

RTEMS assumes that arguments are placed in the
caller's output registers with the first argument in output
register 0 (o0), the second argument in output register 1 (o1),
and so forth.  Until the callee executes a save instruction, the
parameters are still visible in the output registers.  After the
callee executes a save instruction, the parameters are visible
in the corresponding input registers.  The following pseudo-code
illustrates the typical sequence used to call a RTEMS directive
with three (3) arguments:

@example
load third argument into o2
load second argument into o1
load first argument into o0
invoke directive
@end example

@subsection User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

The SPARC architecture supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, half-word (2-bytes), word (4 bytes), or doubleword
(8 bytes).  Memory accesses within this address space are
performed in big endian fashion by the SPARC.  Memory accesses
which are not properly aligned generate a "memory address not
aligned" trap (type number 7).  The following table lists the
alignment requirements for a variety of data accesses:

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

Doubleword load and store operations must use a pair
of registers as their source or destination.  This pair of
registers must be an adjacent pair of registers with the first
of the pair being even numbered.  For example, a valid
destination for a doubleword load might be input registers 0 and
1 (i0 and i1).  The pair i1 and i2 would be invalid.  [NOTE:
Some assemblers for the SPARC do not generate an error if an odd
numbered register is specified as the beginning register of the
pair.  In this case, the assembler assumes that what the
programmer meant was to use the even-odd pair which ends at the
specified register.  This may or may not have been a correct
assumption.]

RTEMS does not support any SPARC Memory Management
Units, therefore, virtual memory or segmentation systems
involving the SPARC are not supported.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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
unique architecture. Discussed in this chapter are the SPARC's
interrupt response and control mechanisms as they pertain to
RTEMS.

RTEMS and associated documentation uses the terms
interrupt and vector.  In the SPARC architecture, these terms
correspond to traps and trap type, respectively.  The terms will
be used interchangeably in this manual.

@subsection Synchronous Versus Asynchronous Traps

The SPARC architecture includes two classes of traps:
synchronous and asynchronous.  Asynchronous traps occur when an
external event interrupts the processor.  These traps are not
associated with any instruction executed by the processor and
logically occur between instructions.  The instruction currently
in the execute stage of the processor is allowed to complete
although subsequent instructions are annulled.  The return
address reported by the processor for asynchronous traps is the
pair of instructions following the current instruction.

Synchronous traps are caused by the actions of an
instruction.  The trap stimulus in this case either occurs
internally to the processor or is from an external signal that
was provoked by the instruction.  These traps are taken
immediately and the instruction that caused the trap is aborted
before any state changes occur in the processor itself.   The
return address reported by the processor for synchronous traps
is the instruction which caused the trap and the following
instruction.

@subsection Vectoring of Interrupt Handler

Upon receipt of an interrupt the SPARC automatically
performs the following actions:

@itemize @bullet
@item disables traps (sets the ET bit of the psr to 0),

@item the S bit of the psr is copied into the Previous
Supervisor Mode (PS) bit of the psr,

@item the cwp is decremented by one (modulo the number of
register windows) to activate a trap window,

@item the PC and nPC are loaded into local register 1 and 2
(l0 and l1),

@item the trap type (tt) field of the Trap Base Register (TBR)
is set to the appropriate value, and

@item if the trap is not a reset, then the PC is written with
the contents of the TBR and the nPC is written with TBR + 4.  If
the trap is a reset, then the PC is set to zero and the nPC is
set to 4.
@end itemize

Trap processing on the SPARC has two features which
are noticeably different than interrupt processing on other
architectures.  First, the value of psr register in effect
immediately before the trap occurred is not explicitly saved.
Instead only reversible alterations are made to it.  Second, the
Processor Interrupt Level (pil) is not set to correspond to that
of the interrupt being processed.  When a trap occurs, ALL
subsequent traps are disabled.  In order to safely invoke a
subroutine during trap handling, traps must be enabled to allow
for the possibility of register window overflow and underflow
traps.

If the interrupt handler was installed as an RTEMS
interrupt handler, then upon receipt of the interrupt, the
processor passes control to the RTEMS interrupt handler which
performs the following actions:

@itemize @bullet
@item saves the state of the interrupted task on it's stack,

@item insures that a register window is available for
subsequent traps,

@item if this is the outermost (i.e. non-nested) interrupt,
then the RTEMS interrupt handler switches from the current stack
to the interrupt stack,

@item enables traps,

@item invokes the vectors to a user interrupt service routine (ISR).
@end itemize

Asynchronous interrupts are ignored while traps are
disabled.  Synchronous traps which occur while traps are
disabled result in the CPU being forced into an error mode.

A nested interrupt is processed similarly with the
exception that the current stack need not be switched to the
interrupt stack.

@subsection Traps and Register Windows

One of the register windows must be reserved at all
times for trap processing.  This is critical to the proper
operation of the trap mechanism in the SPARC architecture.  It
is the responsibility of the trap handler to insure that there
is a register window available for a subsequent trap before
re-enabling traps.  It is likely that any high level language
routines invoked by the trap handler (such as a user-provided
RTEMS interrupt handler) will allocate a new register window.
The save operation could result in a window overflow trap.  This
trap cannot be correctly processed unless (1) traps are enabled
and (2) a register window is reserved for traps.  Thus, the
RTEMS interrupt handler insures that a register window is
available for subsequent traps before enabling traps and
invoking the user's interrupt handler.

@subsection Interrupt Levels

Sixteen levels (0-15) of interrupt priorities are
supported by the SPARC architecture with level fifteen (15)
being the highest priority.  Level zero (0) indicates that
interrupts are fully enabled.  Interrupt requests for interrupts
with priorities less than or equal to the current interrupt mask
level are ignored.

Although RTEMS supports 256 interrupt levels, the
SPARC only supports sixteen.  RTEMS interrupt levels 0 through
15 directly correspond to SPARC processor interrupt levels.  All
other RTEMS interrupt levels are undefined and their behavior is
unpredictable.

@subsection Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables interrupts to level seven (15)
before the execution of this section and restores them to the
previous level upon completion of the section.  RTEMS has been
optimized to insure that interrupts are disabled for less than
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds on a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ 
Mhz ERC32 with zero wait states.
These numbers will vary based the number of wait states and
processor speed present on the target board.
[NOTE:  The maximum period with interrupts disabled is hand calculated.  This
calculation was last performed for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

[NOTE: It is thought that the length of time at which
the processor interrupt level is elevated to fifteen by RTEMS is
not anywhere near as long as the length of time ALL traps are
disabled as part of the "flush all register windows" operation.]

Non-maskable interrupts (NMI) cannot be disabled, and
ISRs which execute at this level MUST NEVER issue RTEMS system
calls.  If a directive is invoked, unpredictable results may
occur due to the inability of RTEMS to protect its critical
sections.  However, ISRs that make no system calls may safely
execute as non-maskable interrupts.

@subsection Interrupt Stack

The SPARC architecture does not provide for a
dedicated interrupt stack.  Thus by default, trap handlers would
execute on the stack of the RTEMS task which they interrupted.
This artificially inflates the stack requirements for each task
since EVERY task stack would have to include enough space to
account for the worst case interrupt stack requirements in
addition to it's own worst case usage.  RTEMS addresses this
problem on the SPARC by providing a dedicated interrupt stack
managed by software.

During system initialization, RTEMS allocates the
interrupt stack from the Workspace Area.  The amount of memory
allocated for the interrupt stack is determined by the
interrupt_stack_size field in the CPU Configuration Table.  As
part of processing a non-nested interrupt, RTEMS will switch to
the interrupt stack before invoking the installed handler.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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
the fatal_error_occurred directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler disables processor interrupts to
level 15, places the error code in g1, and goes into an infinite
loop to simulate a halt processor instruction.


@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@section Board Support Packages


An RTEMS Board Support Package (BSP) must be designed
to support a particular processor and target board combination.
This chapter presents a discussion of SPARC specific BSP issues.
For more information on developing a BSP, refer to the chapter
titled Board Support Packages in the RTEMS
Applications User's Guide.

@subsection System Reset

An RTEMS based application is initiated or
re-initiated when the SPARC processor is reset.  When the SPARC
is reset, the processor performs the following actions:

@itemize @bullet
@item the enable trap (ET) of the psr is set to 0 to disable
traps,

@item the supervisor bit (S) of the psr is set to 1 to enter
supervisor mode, and

@item the PC is set 0 and the nPC is set to 4.
@end itemize

The processor then begins to execute the code at
location 0.  It is important to note that all fields in the psr
are not explicitly set by the above steps and all other
registers retain their value from the previous execution mode.
This is true even of the Trap Base Register (TBR) whose contents
reflect the last trap which occurred before the reset.

@subsection Processor Initialization

It is the responsibility of the application's
initialization code to initialize the TBR and install trap
handlers for at least the register window overflow and register
window underflow conditions.  Traps should be enabled before
invoking any subroutines to allow for register window
management.  However, interrupts should be disabled by setting
the Processor Interrupt Level (pil) field of the psr to 15.
RTEMS installs it's own Trap Table as part of initialization
which is initialized with the contents of the Trap Table in
place when the @code{rtems_initialize_executive} directive was invoked.
Upon completion of executive initialization, interrupts are
enabled.

If this SPARC implementation supports on-chip caching
and this is to be utilized, then it should be enabled during the
reset application initialization code.

In addition to the requirements described in the
Board Support Packages chapter of the C
Applications Users Manual for the reset code
which is executed before the call to
@code{rtems_initialize_executive}, the SPARC version has the following
specific requirements:

@itemize @bullet
@item Must leave the S bit of the status register set so that
the SPARC remains in the supervisor state.

@item Must set stack pointer (sp) such that a minimum stack
size of MINIMUM_STACK_SIZE bytes is provided for the
@code{rtems_initialize_executive} directive.

@item Must disable all external interrupts (i.e. set the pil
to 15).

@item Must enable traps so window overflow and underflow
conditions can be properly handled.

@item Must initialize the SPARC's initial trap table with at
least trap handlers for register window overflow and register
window underflow.
@end itemize
