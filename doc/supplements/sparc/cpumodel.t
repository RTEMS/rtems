@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter CPU Model Dependent Features

@section Introduction

Microprocessors are generally classified into
families with a variety of CPU models or implementations within
that family.  Within a processor family, there is a high level
of binary compatibility.  This family may be based on either an
architectural specification or on maintaining compatibility with
a popular processor.  Recent microprocessor families such as the
SPARC or PA-RISC are based on an architectural specification
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

@section CPU Model Feature Flags

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
c/src/exec/score/cpu/sparc/sparc.h based upon the particular CPU
model defined on the compilation command line.

@subsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the European Space
Agency's ERC32 SPARC model, this macro is set to the string
"erc32".

@subsection Floating Point Unit

The macro SPARC_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.

@subsection Bitscan Instruction

The macro SPARC_HAS_BITSCAN is set to 1 to indicate
that this CPU model has the bitscan instruction.  For example,
this instruction is supported by the Fujitsu SPARClite family.

@subsection Number of Register Windows

The macro SPARC_NUMBER_OF_REGISTER_WINDOWS is set to
indicate the number of register window sets implemented by this
CPU model.  The SPARC architecture allows a for a maximum of
thirty-two register window sets although most implementations
only include eight.

@subsection Low Power Mode

The macro SPARC_HAS_LOW_POWER_MODE is set to one to
indicate that this CPU model has a low power mode.  If low power
is enabled, then there must be CPU model specific implementation
of the IDLE task in c/src/exec/score/cpu/sparc/cpu.c.  The low
power mode IDLE task should be of the form:

@example
while ( TRUE ) @{
  enter low power mode
@}
@end example

The code required to enter low power mode is CPU model specific.

@section CPU Model Implementation Notes 

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
