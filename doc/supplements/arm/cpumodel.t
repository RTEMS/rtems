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
ARM, SPARC, and PA-RISC are based on an architectural specification
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
across ARM implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
cpukit/score/cpu/arm/rtems/score/arm.h based upon the particular CPU
model defined on the compilation command line.

@section CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the architectural level of this CPU model.  The following is
a list of the settings for this string based upon @code{gcc}
CPU model predefines:

@example
__ARM_ARCH4__   "ARMv4"
__ARM_ARCH4T__  "ARMv4T"
__ARM_ARCH5__   "ARMv5"
__ARM_ARCH5T__  "ARMv5T"
__ARM_ARCH5E__  "ARMv5E"
__ARM_ARCH5TE__ "ARMv5TE"
@end example

@section Count Leading Zeroes Instruction

The macro @code{ARM_HAS_CLZ} is set to 1 to indicate that
the architectural version has the @code{clz} instruction.
On ARM architectural version 5 and above, the count
leading zeroes instruction (@code{clz}) is available and
can be used to speed up the find first bit operation.  
The use of this instruction significantly speeds up the
scheduling associated with a thread blocking.

@section Floating Point Unit

The macro ARM_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.  It does not matter whether the hardware floating
point support is incorporated on-chip or is an external
coprocessor.

