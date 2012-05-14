@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter CPU Model Variations

XXX enhance using portability presentation from CS595 class.  See
general/portability.ppt.

Since the text in the next section was written, RTEMS view of
portability has grown to distinguish totally portable, CPU 
family dependent, CPU model dependent, peripheral chip dependent
and board dependent.  This text was part of a larger paper that
did not even cover portability completely as it existed when this
was written and certainly is out of date now. :)


@section Overview of RTEMS Portability

RTEMS was designed to be a highly portable, reusable software component.  
This reflects the fundamental nature of embedded systems in which hardware
components, ranging from boards to peripherals to even the processor
itself, are selected specifically to meet the requirements of a particular
project.

@subsection Processor Families

Since there are a wide variety of embedded systems, there are a wide
variety of processors targeting embedded systems. RTEMS alleviates some of
the burden on the embedded systems programmer by providing a consistent,
high-performance environment regardless of the target processor.  RTEMS
has been ported to a variety of microprocessor families including:

@itemize @bullet

@item Motorola ColdFire
@item Motorola MC68xxx
@item Motorola MC683xx
@item Intel ix86 (i386, i486, Pentium and above)
@item ARM
@item MIPS
@item PowerPC 4xx, 5xx, 6xx, 7xx, 8xx, and 84xx
@item SPARC
@item Hitachi H8/300
@item Hitachi SH
@item OpenCores OR32
@item Texas Instruments C3x/C4x

@end itemize


In addition, there is a port of RTEMS to UNIX that uses standard UNIX
services to simulate the embedded environment.

Each RTEMS port supplies a well-defined set of services that are the
foundation for the highly portable RTEMS and POSIX API implementations.  
When porting to a new processor family, one must provide the processor
dependent implementation of these services.  This set of processor
dependent core services includes software to perform interrupt
dispatching, context switches, and manipulate task register sets.

The RTEMS approach to handling varying processor models reflects the
approach taken by the designers of the processors themselves.  In each
processor family, there is a core architecture that must be implemented on
all processor models within the family to provide any level of
compatibility.  Many of the modern RISC architectures refer to this as the
Architectural Definition.  The Architectural Definition is intended to be
independent of any particular implementation. Additionally, there is a
feature set which is allowed to vary in a defined way amongst the
processor models.  These feature sets may be defined as Optional in the
Architectural Definition, be left as implementation defined
characteristics, or be processor model specific extensions.  Support for
floating point, virtual memory, and low power mode are common Optional
features included in an Architectural Definition.

The processor family dependent software in RTEMS includes a definition of
which features are present in each supported processor model.  This often
makes adding support for a new processor model within a supported family
as simple as determining which features are present in the new processor
implementation.  If the new processor model varies in a way previously
unaccounted for, then this must be addressed.  This could be the result of
a new Optional feature set being added to the Architectural Definition.  
Alternatively, this particular processor model could have a new and
different implementation of a feature left as undefined in the
Architectural Definition.  This would require software to be written to
utilize that feature.

There is a relatively small set of features that may vary in a processor
family.  As the number of processor models in the family grow, the
addition of each new model only requires adding an entry for the new model
to the single feature table.  It does not require searching for every
conditional based on processor model and adding the new model in the
appropriate place.  This significantly eases the burden of adding a new
processor model as it centralizes and logically simplifies the process.

@subsection Boards

Being portable both between models within a processor family and across
processor families is not enough to address the needs of embedded systems
developers.  Custom board development is the norm for embedded systems.  
Each of these boards is optimized for a particular project.  The processor
and peripheral set have been chosen to meet a particular set of system
requirements.  The tools in the embedded systems developers’ toolbox must
support their project’s unique board.  RTEMS addresses this issue via the
Board Support Package.

RTEMS segregates board specific code to make it possible for the embedded
systems developer to easily replace and customize this software.  A
minimal Board Support Package includes device drivers for a clock tick,
console I/O, and a benchmark timer (optional) as well as startup and
miscellaneous support code.  The Board Support Package for a project may
be extended to include the device drivers for any peripherals on the
custom board.

@subsection Applications

One important design goal of RTEMS was to provide a bridge between the
application software and the target hardware.  Most hardware dependencies
for real-time applications can be localized to the low level device
drivers which provide an abstracted view of the hardware.  The RTEMS I/O
interface manager provides an efficient tool for incorporating these
hardware dependencies into the system while simultaneously providing a
general mechanism to the application code that accesses them.  A well
designed real-time system can benefit from this architecture by building a
rich library of standard application components which can be used
repeatedly in other real-time projects. The following figure illustrates
how RTEMS serves as a buffer between the project dependent application
code and the target hardware.

@section Coding Issues

XXX deal with this as it applies to score/cpu.  Section name may
be bad.
