@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter ARM Specific Information

This chapter discusses the
@uref{http://en.wikipedia.org/wiki/ARM_architecture,ARM architecture}
dependencies in this port of RTEMS.  The ARMv4T (and compatible), ARMv7-A,
ARMv7-R and ARMv7-M architecture versions are supported by RTEMS.  Processors
with a MMU use a static configuration which is set up during system start.  SMP
is supported.

@subheading Architecture Documents

For information on the ARM architecture refer to the
@uref{http://infocenter.arm.com,ARM Infocenter}.

@section CPU Model Dependent Features

This section presents the set of features which vary
across ARM implementations and are of importance to RTEMS.  The set of CPU
model feature macros are defined in the file
@file{cpukit/score/cpu/arm/rtems/score/arm.h} based upon the particular CPU
model flags specified on the compilation command line.

@subsection CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the architectural level of this CPU model.  See in
@file{cpukit/score/cpu/arm/rtems/score/arm.h} for the values.

@subsection Count Leading Zeroes Instruction

The ARMv5 and later has the count leading zeroes @code{clz} instruction which
could be used to speed up the find first bit operation.  The use of this
instruction should significantly speed up the scheduling associated with a
thread blocking.  This is currently not used.

@subsection Floating Point Unit

The following floating point units are supported.

@itemize @bullet

@item VFPv3-D32/NEON (for example available on Cortex-A processors)
@item VFPv3-D16 (for example available on Cortex-R processors)
@item FPv4-SP-D16 (for example available on Cortex-M processors)

@end itemize

@c
@c
@c
@section Multilibs

The following multilibs are available:

@enumerate
@item @code{.}: ARMv4T, ARM instruction set
@item @code{thumb}: ARMv4T, Thumb-1 instruction set
@item @code{thumb/armv6-m}: ARMv6M, subset of Thumb-2 instruction set
@item @code{thumb/armv7-a}: ARMv7-A, Thumb-2 instruction set
@item @code{thumb/armv7-a/neon/hard}: ARMv7-A, Thumb-2 instruction set with
hard-float ABI Neon and VFP-D32 support
@item @code{thumb/armv7-r}: ARMv7-R, Thumb-2 instruction set
@item @code{thumb/armv7-r/vfpv3-d16/hard}: ARMv7-R, Thumb-2 instruction set
with hard-float ABI VFP-D16 support
@item @code{thumb/armv7-m}: ARMv7-M, Thumb-2 instruction set with hardware
integer division (SDIV/UDIV)
@item @code{thumb/armv7-m/fpv4-sp-d16}: ARMv7-M, Thumb-2 instruction set with
hardware integer division (SDIV/UDIV) and hard-float ABI FPv4-SP support
@item @code{eb/thumb/armv7-r}: ARMv7-R, Big-endian Thumb-2 instruction set
@item @code{eb/thumb/armv7-r/vfpv3-d16/hard}: ARMv7-R, Big-endian Thumb-2
instruction set with hard-float ABI VFP-D16 support
@end enumerate

Multilib 1. and 2. support the standard ARM7TDMI and ARM926EJ-S targets.

Multilib 3. supports the Cortex-M0 and Cortex-M1 cores.

Multilib 8. supports the Cortex-M3 and Cortex-M4 cores, which have a special
hardware integer division instruction (this is not present in the A and R
profiles).

Multilib 9. supports the Cortex-M4 cores with a floating point unit.

Multilib 4. and 5. support the Cortex-A processors.

Multilib 6., 7., 10. and 11. support the Cortex-R processors.  Here also
big-endian variants are available.

Use for example the following GCC options

@example
-mthumb -march=armv7-a -mfpu=neon -mfloat-abi=hard -mtune=cortex-a9
@end example

to build an application or BSP for the ARMv7-A architecture and tune the code
for a Cortex-A9 processor.  It is important to select the options used for the
multilibs. For example

@example
-mthumb -mcpu=cortex-a9
@end example

alone will not select the ARMv7-A multilib.

@section Calling Conventions

Please refer to the
@uref{http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042c/IHI0042C_aapcs.pdf,Procedure Call Standard for the ARM Architecture}.

@section Memory Model

A flat 32-bit memory model is supported.  The board support package must take
care about the MMU if necessary.

@section Interrupt Processing

The ARMv4T (and compatible) architecture has seven exception types:

@itemize @bullet

@item Reset
@item Undefined
@item Software Interrupt (SWI)
@item Prefetch Abort
@item Data Abort
@item Interrupt (IRQ)
@item Fast Interrupt (FIQ)

@end itemize

Of these types only the IRQ has explicit operating system support.  It is
intentional that the FIQ is not supported by the operating system.  Without
operating system support for the FIQ it is not necessary to disable them during
critical sections of the system.

The ARMv7-M architecture has a completely different exception model.  Here
interrupts are disabled with a write of 0x80 to the @code{basepri_max}
register.  This means that all exceptions and interrupts with a priority value
of greater than or equal to 0x80 are disabled.  Thus exceptions and interrupts
with a priority value of less than 0x80 are non-maskable with respect to the
operating system and therefore must not use operating system services.  Several
support libraries of chip vendors implicitly shift the priority value somehow
before the value is written to the NVIC IPR register.  This can easily lead to
confusion.

@subsection Interrupt Levels

There are exactly two interrupt levels on ARM with respect to RTEMS.  Level
zero corresponds to interrupts enabled.  Level one corresponds to interrupts
disabled.
 
@subsection Interrupt Stack

The board support package must initialize the interrupt stack. The memory for
the stacks is usually reserved in the linker script. 

@section Default Fatal Error Processing

The default fatal error handler for this architecture performs the
following actions:

@itemize @bullet
@item disables operating system supported interrupts (IRQ),
@item places the error code in @code{r0}, and
@item executes an infinite loop to simulate a halt processor instruction.
@end itemize

@section Symmetric Multiprocessing

SMP is supported on ARMv7-A.  Available platforms are the Altera Cyclone V and
the Xilinx Zynq.

@section Thread-Local Storage

Thread-local storage is supported.
