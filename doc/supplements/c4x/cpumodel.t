@c
@c  COPYRIGHT (c) 1988-1999.
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
across the entire family.  Each processor family supported by
RTEMS has a list of features which vary between CPU models
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

This chapter presents the set of features which vary
across the various implementations of the C3x/C4x architecture
that are of importance to rtems.
the set of cpu model feature macros are defined in the file
cpukit/score/cpu/c4x/rtems/score/c4x.h and are based upon
the particular cpu model defined in the bsp's custom configuration
file as well as the compilation command line.

@section CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the name of this cpu model.  for example, for the c32
processor, this macro is set to the string "c32".

@section Floating Point Unit

The Texas Instruments C3x/C4x family makes little distinction
between the various cpu registers.  Although floating point
operations may only be performed on a subset of the cpu registers,
these same registers may be used for normal integer operations.
as a result of this, this port of rtems makes no distinction 
between integer and floating point contexts.  The routine
@code{_CPU_Context_switch} saves all of the registers that
comprise a task's context.  the routines that initialize,
save, and restore floating point contexts are not present
in this port.

Moreover, there is no floating point context pointer and
the code in @code{_Thread_Dispatch} that manages the
floating point context switching process is disabled
on this port.

This not only simplifies the port, it also speeds up context
switches by reducing the code involved and reduces the code 
space footprint of the executive on the Texas Instruments
C3x/C4x.

