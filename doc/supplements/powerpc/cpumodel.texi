@c
@c  COPYRIGHT (c) 1988-1996.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node CPU Model Dependent Features, CPU Model Dependent Features Introduction, Preface, Top
@end ifinfo
@chapter CPU Model Dependent Features
@ifinfo
@menu
* CPU Model Dependent Features Introduction::
* CPU Model Dependent Features CPU Model Feature Flags::
* CPU Model Dependent Features CPU Model Implementation Notes::
@end menu
@end ifinfo

@ifinfo
@node CPU Model Dependent Features Introduction, CPU Model Dependent Features CPU Model Feature Flags, CPU Model Dependent Features, CPU Model Dependent Features
@end ifinfo
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

@ifinfo
@node CPU Model Dependent Features CPU Model Feature Flags, CPU Model Dependent Features CPU Model Name, CPU Model Dependent Features Introduction, CPU Model Dependent Features
@end ifinfo
@section CPU Model Feature Flags
@ifinfo
@menu
* CPU Model Dependent Features CPU Model Name::
* CPU Model Dependent Features Floating Point Unit::
* CPU Model Dependent Features Alignment::
* CPU Model Dependent Features Cache Alignment::
* CPU Model Dependent Features Maximum Interrupts::
* CPU Model Dependent Features Has Double Precision Floating Point::
* CPU Model Dependent Features Critical Interrupts::
* CPU Model Dependent Features MSR Values::
* CPU Model Dependent Features FPU Status Control Register Values::
* CPU Model Dependent Features Use Multiword Load/Store Instructions::
* CPU Model Dependent Features Instruction Cache Size::
* CPU Model Dependent Features Data Cache Size::
@end menu
@end ifinfo

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

@ifinfo
@node CPU Model Dependent Features CPU Model Name, CPU Model Dependent Features Floating Point Unit, CPU Model Dependent Features CPU Model Feature Flags, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the PowerPC 603e
model, this macro is set to the string "PowerPC 603e".

@ifinfo
@node CPU Model Dependent Features Floating Point Unit, CPU Model Dependent Features Alignment, CPU Model Dependent Features CPU Model Name, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Floating Point Unit

The macro PPC_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.

@ifinfo
@node CPU Model Dependent Features Alignment, CPU Model Dependent Features Cache Alignment, CPU Model Dependent Features Floating Point Unit, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Alignment

The macro PPC_ALIGNMENT is set to 

@ifinfo
@node CPU Model Dependent Features Cache Alignment, CPU Model Dependent Features Maximum Interrupts, CPU Model Dependent Features Alignment, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Cache Alignment

The macro PPC_CACHE_ALIGNMENT is set to 

Similarly, the macro PPC_CACHE_ALIGN_POWER is set to the 

@ifinfo
@node CPU Model Dependent Features Maximum Interrupts, CPU Model Dependent Features Has Double Precision Floating Point, CPU Model Dependent Features Cache Alignment, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Maximum Interrupts

The macro PPC_INTERRUPT_MAX is set to

@ifinfo
@node CPU Model Dependent Features Has Double Precision Floating Point, CPU Model Dependent Features Critical Interrupts, CPU Model Dependent Features Maximum Interrupts, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Has Double Precision Floating Point

The macro PPC_HAS_DOUBLE is set to

@ifinfo
@node CPU Model Dependent Features Critical Interrupts, CPU Model Dependent Features MSR Values, CPU Model Dependent Features Has Double Precision Floating Point, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Critical Interrupts

The macro PPC_HAS_RFCI is set to

@ifinfo
@node CPU Model Dependent Features MSR Values, CPU Model Dependent Features FPU Status Control Register Values, CPU Model Dependent Features Critical Interrupts, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection MSR Values

The macro PPC_MSR_DISABLE_MASK is set to

The macro PPC_MSR_INITIAL is set to

The macro PPC_MSR_0 is set to

The macro PPC_MSR_1 is set to

The macro PPC_MSR_2 is set to

The macro PPC_MSR_3 is set to

@ifinfo
@node CPU Model Dependent Features FPU Status Control Register Values, CPU Model Dependent Features Use Multiword Load/Store Instructions, CPU Model Dependent Features MSR Values, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection FPU Status Control Register Values

The macro PPC_INIT_FPSCR is set to

@ifinfo
@node CPU Model Dependent Features Use Multiword Load/Store Instructions, CPU Model Dependent Features Instruction Cache Size, CPU Model Dependent Features FPU Status Control Register Values, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Use Multiword Load/Store Instructions

The macro PPC_USE_MULTIPLE is set to

@ifinfo
@node CPU Model Dependent Features Instruction Cache Size, CPU Model Dependent Features Data Cache Size, CPU Model Dependent Features Use Multiword Load/Store Instructions, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Instruction Cache Size

The macro PPC_I_CACHE is set to 

@ifinfo
@node CPU Model Dependent Features Data Cache Size, CPU Model Dependent Features CPU Model Implementation Notes, CPU Model Dependent Features Instruction Cache Size, CPU Model Dependent Features CPU Model Feature Flags
@end ifinfo
@subsection Data Cache Size

The macro PPC_D_CACHE is set to

@ifinfo
@node CPU Model Dependent Features CPU Model Implementation Notes, Calling Conventions, CPU Model Dependent Features Data Cache Size, CPU Model Dependent Features
@end ifinfo
@section CPU Model Implementation Notes 

TBD
