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
PowerPC, SPARC, and PA-RISC are based on an architectural specification
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
across PowerPC implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
c/src/exec/score/cpu/ppc/ppc.h based upon the particular CPU
model defined on the compilation command line.

@subsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the PowerPC 603e
model, this macro is set to the string "PowerPC 603e".

@subsection Floating Point Unit

The macro PPC_HAS_FPU is set to 1 to indicate that this CPU model
has a hardware floating point unit and 0 otherwise.

@subsection Alignment

The macro PPC_ALIGNMENT is set to the PowerPC model's worst case alignment
requirement for data types on a byte boundary.  This value is used
to derive the alignment restrictions for memory allocated from 
regions and partitions.

@subsection Cache Alignment

The macro PPC_CACHE_ALIGNMENT is set to the line size of the cache.  It is
used to align the entry point of critical routines so that as much code
as possible can be retrieved with the initial read into cache.  This
is done for the interrupt handler as well as the context switch routines.

In addition, the "shortcut" data structure used by the PowerPC implementation
to ease access to data elements frequently accessed by RTEMS routines
implemented in assembly language is aligned using this value.

@subsection Maximum Interrupts

The macro PPC_INTERRUPT_MAX is set to the number of exception sources
supported by this PowerPC model.

@subsection Has Double Precision Floating Point

The macro PPC_HAS_DOUBLE is set to 1 to indicate that the PowerPC model
has support for double precision floating point numbers.  This is
important because the floating point registers need only be four bytes
wide (not eight) if double precision is not supported.

@subsection Critical Interrupts

The macro PPC_HAS_RFCI is set to 1 to indicate that the PowerPC model
has the Critical Interrupt capability as defined by the IBM 403 models.

@subsection Use Multiword Load/Store Instructions

The macro PPC_USE_MULTIPLE is set to 1 to indicate that multiword load and
store instructions should be used to perform context switch operations.
The relative efficiency of multiword load and store instructions versus
an equivalent set of single word load and store instructions varies based
upon the PowerPC model.

@subsection Instruction Cache Size

The macro PPC_I_CACHE is set to the size in bytes of the instruction cache.

@subsection Data Cache Size

The macro PPC_D_CACHE is set to the size in bytes of the data cache.

@subsection Debug Model

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

@subsection Low Power Model

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
