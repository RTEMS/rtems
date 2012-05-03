@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Task Context Management

@section Introduction

XXX

@section Task Stacks

XXX

@subsection Direction of Stack Growth

The CPU_STACK_GROWS_UP macro is set based upon the answer to the following
question: Does the stack grow up (toward higher addresses) or down (toward
lower addresses)?  If the stack grows upward in memory, then this macro
should be set to TRUE.  Otherwise, it should be set to FALSE to indicate
that the stack grows downward toward smaller addresses.

The following illustrates how the CPU_STACK_GROWS_UP macro is set:

@example
#define CPU_STACK_GROWS_UP               TRUE
@end example

@subsection Minimum Task Stack Size

The CPU_STACK_MINIMUM_SIZE macro should be set to the minimum size of each
task stack.  This size is specified as the number of bytes.  This minimum
stack size should be large enough to run all RTEMS tests.  The minimum
stack size is chosen such that a "reasonable" small application should not
have any problems.  Choosing a minimum stack size that is too small will
result in the RTEMS tests "blowing" their stack and not executing
properly.

There are many reasons a task could require a stack size larger than the
minimum.  For example, a task could have a very deep call path or declare
large data structures on the stack.  Tasks which utilize C++ exceptions
tend to require larger stacks as do Ada tasks.

The following illustrates setting the minimum stack size to 4 kilobytes
per task.

@example
#define CPU_STACK_MINIMUM_SIZE          (1024*4)
@end example

@subsection Stack Alignment Requirements

The CPU_STACK_ALIGNMENT macro is set to indicate the byte alignment
requirement for the stack.  This alignment requirement may be stricter
than that for the data types alignment specified by CPU_ALIGNMENT.  If the
CPU_ALIGNMENT is strict enough for the stack, then this should be set to
0.

The following illustrates how the CPU_STACK_ALIGNMENT macro should be set
when there are no special requirements:

@example
#define CPU_STACK_ALIGNMENT        0
@end example

NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT. [XXX is this true?]

@section Task Context

Associated with each task is a context that distinguishes it from other
tasks in the system and logically gives it its own scratch pad area for
computations.  In addition, when an interrupt occurs some processor
context information must be saved and restored.  This is managed in RTEMS
as three items:

@itemize @bullet

@item Basic task level context (e.g. the Context_Control structure)

@item Floating point task context (e.g. Context_Control_fp structure)

@item Interrupt level context (e.g.  the Context_Control_interrupt
structure)

@end itemize

The integer and floating point context structures and the routines that
manipulate them are discussed in detail in this section, while the
interrupt level context structure is discussed in the XXX.

Additionally, if the GNU debugger gdb is to be made aware of RTEMS tasks
for this CPU, then care should be used in designing the context area.

@example
typedef struct @{
    unsigned32 special_interrupt_register;
@} CPU_Interrupt_frame;
@end example


@subsection Basic Context Data Structure

The Context_Control data structure contains the basic integer context of a
task.  In addition, this context area contains stack and frame pointers,
processor status register(s), and any other registers that are normally
altered by compiler generated code.  In addition, this context must
contain the processor interrupt level since the processor interrupt level
is maintained on a per-task basis.  This is necessary to support the
interrupt level portion of the task mode as provided by the Classic RTEMS
API.

On some processors, it is cost-effective to save only the callee preserved
registers during a task context switch.  This means that the ISR code
needs to save those registers which do not persist across function calls.  
It is not mandatory to make this distinctions between the caller/callee
saves registers for the purpose of minimizing context saved during task
switch and on interrupts.  If the cost of saving extra registers is
minimal, simplicity is the choice.  Save the same context on interrupt
entry as for tasks in this case.

The Context_Control data structure should be defined such that the order
of elements results in the simplest, most efficient implementation of XXX.  
A typical implementation starts with a definition such as the following:

@example
typedef struct @{
    unsigned32 some_integer_register;
    unsigned32 another_integer_register;
    unsigned32 some_system_register;
@} Context_Control;
@end example

@subsection Initializing a Context

The _CPU_Context_Initialize routine initializes the context to a state
suitable for starting a task after a context restore operation.  
Generally, this involves:

@itemize @bullet

@item  setting a starting address,

@item  preparing the stack,

@item  preparing the stack and frame pointers,

@item  setting the proper interrupt level in the context, and

@item  initializing the floating point context

@end itemize

This routine generally does not set any unnecessary register in the
context.  The state of the "general data" registers is undefined at task
start time. The _CPU_Context_initialize routine is prototyped as follows:

@example
void _CPU_Context_Initialize( 
    Context_Control *_the_context,
    void            *_stack_base, 
    unsigned32       _size,
    unsigned32       _isr,
    void            *_entry_point,
    unsigned32       _is_fp
);
@end example

The @code{is_fp} parameter is TRUE if the thread is to be a floating point
thread.  This is typically only used on CPUs where the FPU may be easily
disabled by software such as on the SPARC where the PSR contains an enable
FPU bit.  The use of an FPU enable bit allows RTEMS to ensure that a
non-floating point task is unable to access the FPU.  This guarantees that
a deferred floating point context switch is safe.

The @code{_stack_base} parameter is the base address of the memory area
allocated for use as the task stack.  It is critical to understand that
@code{_stack_base} may not be the starting stack pointer for this task.
On CPU families where the stack grows from high addresses to lower ones,
(i.e. @code{CPU_STACK_GROWS_UP} is FALSE) the starting stack point
will be near the end of the stack memory area or close to 
@code{_stack_base} + @code{_size}.  Even on CPU families where the stack
grows from low to higher addresses, there may be some required
outermost stack frame that must be put at the address @code{_stack_base}.

The @code{_size} parameter is the requested size in bytes of the stack for 
this task.  It is assumed that the memory area @code{_stack_base}
is of this size.

XXX explain other parameters and check prototype

@subsection Performing a Context Switch

The _CPU_Context_switch performs a normal non-FP context switch from the
context of the current executing thread to the context of the heir thread.

@example
void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);
@end example

This routine begins by saving the current state of the
CPU (i.e. the context) in the context area at @code{run}.
Then the routine should load the CPU context pointed to
by @code{heir}.  Loading the new context will cause a
branch to its task code, so the task that invoked
@code{_CPU_Context_switch} will not run for a while.  
When, eventually, a context switch is made to load
context from @code{*run} again, this task will resume
and @code{_CPU_Context_switch} will return to its caller.

Care should be exercise when writing this routine.  All
registers assumed to be preserved across subroutine calls
must be preserved.  These registers may be saved in
the task's context area or on its stack.  However, the
stack pointer and address to resume executing the task
at must be included in the context (normally the subroutine
return address to the caller of @code{_Thread_Dispatch}.
The decision of where to store the task's context is based
on numerous factors including the capabilities of
the CPU architecture itself and simplicity as well
as external considerations such as debuggers wishing
to examine a task's context.  In this case, it is
often simpler to save all data in the context area.

Also there may be special considerations
when loading the stack pointers or interrupt level of the
incoming task.  Independent of CPU specific considerations,
if some context is saved on the task stack, then the porter
must ensure that the stack pointer is adjusted @b{BEFORE}
to make room for this context information before the
information is written.  Otherwise, an interrupt could
occur writing over the context data.  The following is
an example of an @b{INCORRECT} sequence:

@example
save part of context beyond current top of stack
interrupt pushes context -- overwriting written context
interrupt returns
adjust stack pointer
@end example

@subsection Restoring a Context

The _CPU_Context_restore routine is generally used only to restart the
currently executing thread (i.e. self) in an efficient manner.  In many
ports, it can simply be a label in _CPU_Context_switch. It may be
unnecessary to reload some registers.

@example
void _CPU_Context_restore(
  Context_Control *new_context
);
@end example

@subsection Restarting the Currently Executing Task

The _CPU_Context_Restart_self is responsible for somehow restarting the
currently executing task.  If you are lucky when porting RTEMS, then all
that is necessary is restoring the context.  Otherwise, there will need to
be a routine that does something special in this case.  Performing a
_CPU_Context_Restore on the currently executing task after reinitializing
that context should work on most ports.  It will not work if restarting
self conflicts with the stack frame assumptions of restoring a context.

The following is an implementation of _CPU_Context_Restart_self that can
be used when no special handling is required for this case.

@example
#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) )
@end example

XXX find a port which does not do it this way and include it here

@section Floating Point Context

@subsection CPU_HAS_FPU Macro Definition

The CPU_HAS_FPU macro is set based on the answer to the question: Does the
CPU have hardware floating point?  If the CPU has an FPU, then this should
be set to TRUE.  Otherwise, it should be set to FALSE.  The primary
implication of setting this macro to TRUE is that it indicates that tasks
may have floating point contexts.  In the Classic API, this means that the
RTEMS_FLOATING_POINT task attribute specified as part of rtems_task_create
is supported on this CPU.  If CPU_HAS_FPU is set to FALSE, then no tasks
or threads may be floating point and the RTEMS_FLOATING_POINT task
attribute is ignored.  On an API such as POSIX where all threads
implicitly have a floating point context, then the setting of this macro
determines whether every POSIX thread has a floating point context.

The following example illustrates how the CPU_HARDWARE_FP (XXX macro name
is varying) macro is set based on the CPU family dependent macro.

@example
#if ( THIS_CPU_FAMILY_HAS_FPU == 1 ) /* where THIS_CPU_FAMILY */
                                     /* might be M68K */
#define CPU_HARDWARE_FP     TRUE
#else
#define CPU_HARDWARE_FP     FALSE
#endif
@end example

The macro name THIS_CPU_FAMILY_HAS_FPU should be made CPU specific.  It
indicates whether or not this CPU model has FP support.  For example, the
definition of the i386ex and i386sx CPU models would set I386_HAS_FPU to
FALSE to indicate that these CPU models are i386's without an i387 and
wish to leave floating point support out of RTEMS when built for the
i386_nofp processor model.  On a CPU with a built-in FPU like the i486,
this would be defined as TRUE.

On some processor families, the setting of the THIS_CPU_FAMILY_HAS_FPU
macro may be derived from compiler predefinitions.  This can be used when
the compiler distinguishes the individual CPU models for this CPU family
as distinctly as RTEMS requires.  Often RTEMS needs to need more about the
CPU model than the compiler because of differences at the system level
such as caching, interrupt structure.

@subsection CPU_ALL_TASKS_ARE_FP Macro Setting

The CPU_ALL_TASKS_ARE_FP macro is set to TRUE or FALSE based upon the
answer to the following question: Are all tasks RTEMS_FLOATING_POINT tasks
implicitly?  If this macro is set TRUE, then all tasks and threads are
assumed to have a floating point context.  In the Classic API, this is
equivalent to setting the RTEMS_FLOATING_POINT task attribute on all
rtems_task_create calls.  If the CPU_ALL_TASKS_ARE_FP macro is set to
FALSE, then the RTEMS_FLOATING_POINT task attribute in the Classic API is
honored.

The rationale for this macro is that if a function that an application
developer would not think utilize the FP unit DOES, then one can not
easily predict which tasks will use the FP hardware. In this case, this
option should be TRUE.  So far, the only CPU families for which this macro
has been to TRUE are the HP PA-RISC and PowerPC.  For the HP PA-RISC, the
HP C compiler and gcc both implicitly use the floating point registers to
perform integer multiplies.  For the PowerPC, this feature macro is set to
TRUE because the printf routine saves a floating point register whether or
not a floating point number is actually printed.  If the newlib
implementation of printf were restructured to avoid this, then the PowerPC
port would not have to have this option set to TRUE.

The following example illustrates how the CPU_ALL_TASKS_ARE_FP is set on
the PowerPC.  On this CPU family, this macro is set to TRUE if the CPU
model has hardware floating point.

@example
#if (CPU_HARDWARE_FP == TRUE)
#define CPU_ALL_TASKS_ARE_FP     TRUE
#else
#define CPU_ALL_TASKS_ARE_FP     FALSE
#endif
@end example

NOTE: If CPU_HARDWARE_FP is FALSE, then this should be FALSE as well.

@subsection CPU_USE_DEFERRED_FP_SWITCH Macro Setting

The CPU_USE_DEFERRED_FP_SWITCH macro is set based upon the answer to the
following question:  Should the saving of the floating point registers be
deferred until a context switch is made to another different floating
point task?  If the floating point context will not be stored until
necessary, then this macro should be set to TRUE.  When set to TRUE, the
floating point context of a task will remain in the floating point
registers and not disturbed until another floating point task is switched
to.

If the CPU_USE_DEFERRED_FP_SWITCH is set to FALSE, then the floating point
context is saved each time a floating point task is switched out and
restored when the next floating point task is restored.  The state of the
floating point registers between those two operations is not specified.

There are a couple of known cases where the port should not defer saving
the floating point context.  The first case is when the compiler generates
instructions that use the FPU when floating point is not actually used.  
This occurs on the HP PA-RISC for example when an integer multiply is
performed.  On the PowerPC, the printf routine includes a save of a
floating point register to support printing floating point numbers even if
the path that actually prints the floating point number is not invoked.  
In both of these cases, deferred floating point context switches can not
be used.  If the floating point context has to be saved as part of
interrupt dispatching, then it may also be necessary to disable deferred
context switches.

Setting this flag to TRUE results in using a different algorithm for
deciding when to save and restore the floating point context.  The
deferred FP switch algorithm minimizes the number of times the FP context
is saved and restored.  The FP context is not saved until a context switch
is made to another, different FP task.  Thus in a system with only one FP
task, the FP context will never be saved or restored.

The following illustrates setting the CPU_USE_DEFERRED_FP_SWITCH macro on
a processor family such as the M68K or i386 which can use deferred
floating point context switches.

@example
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE
@end example

Note that currently, when in SMP configuration, deferred floating point
context switching is unavailable.

@subsection Floating Point Context Data Structure

The Context_Control_fp contains the per task information for the floating
point unit.  The organization of this structure may be a list of floating
point registers along with any floating point control and status registers
or it simply consist of an array of a fixed number of bytes.  Defining the
floating point context area as an array of bytes is done when the floating
point context is dumped by a "FP save context" type instruction and the
format is either not completely defined by the CPU documentation or the
format is not critical for the implementation of the floating point
context switch routines.  In this case, there is no need to figure out the
exact format -- only the size.  Of course, although this is enough
information for RTEMS, it is probably not enough for a debugger such as
gdb.  But that is another problem.

@example
typedef struct @{
    double      some_float_register;
@} Context_Control_fp;
@end example


On some CPUs with hardware floating point support, the Context_Control_fp
structure will not be used.

@subsection Size of Floating Point Context Macro

The CPU_CONTEXT_FP_SIZE macro is set to the size of the floating point
context area. On some CPUs this will not be a "sizeof" because the format
of the floating point area is not defined -- only the size is.  This is
usually on CPUs with a "floating point save context" instruction.  In
general, though it is easier to define the structure as a "sizeof"
operation and define the Context_Control_fp structure to be an area of
bytes of the required size in this case.

@example
#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )
@end example

@subsection Start of Floating Point Context Area Macro

The _CPU_Context_Fp_start macro is used in the XXX routine and allows the initial pointer into a  floating point context area (used to save the floating point context) to be at an arbitrary place in the floating point context area.  This is necessary because some FP units are designed to have their context saved as a stack which grows into lower addresses.  Other FP units can be saved by simply moving registers into offsets from the base of the context area.  Finally some FP units provide a "dump context" instruction which could fill in from high to low or low to high based on the whim of the CPU designers.  Regardless, the address at which that floating point context area pointer should start within the actual floating point context area varies between ports and this macro provides a clean way of addressing this.

This is a common implementation of the _CPU_Context_Fp_start routine which
is suitable for many processors.  In particular, RISC processors tend to
use this implementation since the floating point context is saved as a
sequence of store operations.

@example
#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )
@end example

In contrast, the m68k treats the floating point context area as a stack
which grows downward in memory.  Thus the following implementation of
_CPU_Context_Fp_start is used in that port:


@example
XXX insert m68k version here
@end example

@subsection Initializing a Floating Point Context

The _CPU_Context_Initialize_fp routine initializes the floating point
context area passed to it to. There are a few standard ways in which to
initialize the floating point context.  The simplest, and least
deterministic behaviorally, is to do nothing.  This leaves the FPU in a
random state and is generally not a suitable way to implement this
routine.  The second common implementation is to place a "null FP status
word" into some status/control register in the FPU.  This mechanism is
simple and works on many FPUs.  Another common way is to initialize the
FPU to a known state during _CPU_Initialize and save the context (using
_CPU_Context_save_fp_context) into the special floating point context
_CPU_Null_fp_context.  Then all that is required to initialize a floating
point context is to copy _CPU_Null_fp_context to the destination floating
point context passed to it.  The following example implementation shows
how to accomplish this:

@example
#define _CPU_Context_Initialize_fp( _destination ) \
  @{ \
   *((Context_Control_fp *) *((void **) _destination)) = \
       _CPU_Null_fp_context; \
  @}
@end example

The _CPU_Null_fp_context is optional.  A port need only include this variable when it uses the above mechanism to initialize a floating point context.  This is typically done on CPUs where it is difficult to generate an "uninitialized" FP context.  If the port requires this variable, then it is declared as follows:

@example
Context_Control_fp  _CPU_Null_fp_context;
@end example


@subsection Saving a Floating Point Context

The _CPU_Context_save_fp_context routine is responsible for saving the FP
context at *fp_context_ptr.  If the point to load the FP context from is
changed then the pointer is modified by this routine.

Sometimes a macro implementation of this is in cpu.h which dereferences
the ** and a similarly named routine in this file is passed something like
a (Context_Control_fp *).  The general rule on making this decision is to
avoid writing assembly language.

@example
void _CPU_Context_save_fp(
  void **fp_context_ptr
)
@end example

@subsection Restoring a Floating Point Context

The _CPU_Context_restore_fp_context is responsible for restoring the FP
context at *fp_context_ptr.  If the point to load the FP context from is
changed then the pointer is modified by this routine.

Sometimes a macro implementation of this is in cpu.h which dereferences
the ** and a similarly named routine in this file is passed something like
a (Context_Control_fp *).  The general rule on making this decision is to
avoid writing assembly language.

@example
void _CPU_Context_restore_fp(
  void **fp_context_ptr
);
@end example

