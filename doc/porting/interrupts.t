@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Interrupts

@section Introduction

@section Interrupt Levels

RTEMS is designed assuming that a CPU family has a level associated with
interrupts.  Interrupts below the current interrupt level are masked and
do not interrupt the CPU until the interrupt level is lowered.  This
design provides for 256 distinct interrupt levels even though most CPU
implementations support far fewer levels.  Interrupt level 0 is assumed to
map to the hardware settings for all interrupts enabled.

Over the years that RTEMS has been available, there has been much
discussion on how to handle CPU families which support very few interrupt
levels such as the i386, PowerPC, and HP-PA RISC. XXX

@subsection Interrupt Level Mask

The CPU_MODES_INTERRUPT_MASK macro defines the number of bits actually used in the interrupt field of the task mode.  How those bits map to the CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().

The following illustrates how the CPU_MODES_INTERRUPT_MASK is set on a CPU
family like the Intel i386 where the CPU itself only recognizes two
interrupt levels - enabled and disabled.

@example
#define CPU_MODES_INTERRUPT_MASK   0x00000001
@end example


@subsection Obtaining the Current Interrupt Level

The _CPU_ISR_Get_level function returns the current interrupt level.

@example
uint32_t _CPU_ISR_Get_level( void )
@end example

@subsection Set the Interrupt Level

The _CPU_ISR_Set_level routine maps the interrupt level in the Classic API
task mode onto the hardware that the CPU actually provides.  Currently,
interrupt levels that do not map onto the CPU in a generic fashion are
undefined.  Someday, it would be nice if these were "mapped" by the
application via a callout.  For example, the Motorola m68k has 8 levels 0
- 7, and levels 8 - 255 are currently undefined.  Levels 8 - 255 would be
available for bsp/application specific meaning. This could be used to
manage a programmable interrupt controller via the rtems_task_mode
directive.

The following is a dummy implementation of the _CPU_ISR_Set_level routine:

@example
#define _CPU_ISR_Set_level( new_level ) \
  @{ \
  @}
@end example

The following is the implementation from the Motorola M68K:

@example
XXX insert m68k implementation here
@end example


@subsection Disable Interrupts

The _CPU_ISR_Disable routine disable all external interrupts.  It returns
the previous interrupt level in the single parameter _isr_cookie.  This
routine is used to disable interrupts during a critical section in the
RTEMS executive.  Great care is taken inside the executive to ensure that
interrupts are disabled for a minimum length of time.  It is important to
note that the way the previous level is returned forces the implementation
to be a macro that translates to either inline assembly language or a
function call whose return value is placed into _isr_cookie.

It is important for the porter to realize that the value of _isr_cookie
has no defined meaning except that it is the most convenient format for
the _CPU_ISR_Disable, _CPU_ISR_Enable, and _CPU_ISR_Disable routines to
manipulate.  It is typically the contents of the processor status
register.  It is NOT the same format as manipulated by the
_CPU_ISR_Get_level and _CPU_ISR_Set_level routines. The following is a
dummy implementation that simply sets the previous level to 0.

@example
#define _CPU_ISR_Disable( _isr_cookie ) \
  @{ \
    (_isr_cookie) = 0;   /* do something to prevent warnings */ \
  @}
@end example

The following is the implementation from the Motorola M68K port:

@example
XXX insert m68k port here
@end example

@subsection Enable Interrupts

The _CPU_ISR_Enable routines enables interrupts to the previous level
(returned by _CPU_ISR_Disable).  This routine is invoked at the end of an
RTEMS critical section to reenable interrupts.  The parameter _level is
not modified but indicates that level that interrupts should be enabled
to.  The following illustrates a dummy implementation of the
_CPU_ISR_Enable routine:

@example
#define _CPU_ISR_Enable( _isr_cookie )  \
  @{ \
  @}
@end example

The following is the implementation from the Motorola M68K port:

@example
XXX insert m68k version here
@end example


@subsection Flash Interrupts

The _CPU_ISR_Flash routine temporarily restores the interrupt to _level
before immediately disabling them again.  This is used to divide long
RTEMS critical sections into two or more parts.  This routine is always
preceded by a call to _CPU_ISR_Disable and followed by a call to
_CPU_ISR_Enable.  The parameter _level is not modified.

The following is a dummy implementation of the _CPU_ISR_Flash routine:

@example
#define _CPU_ISR_Flash( _isr_cookie ) \
  @{ \
  @}
@end example

The following is the implementation from the Motorola M68K port:

@example
XXX insert m68k version here
@end example


@section Interrupt Stack Management

@subsection Hardware or Software Managed Interrupt Stack

The setting of the CPU_HAS_SOFTWARE_INTERRUPT_STACK indicates whether the
interrupt stack is managed by RTEMS in software or the CPU has direct
support for an interrupt stack.  If RTEMS is to manage a dedicated
interrupt stack in software, then this macro should be set to TRUE and the
memory for the software managed interrupt stack is allocated in
@code{_ISR_Handler_initialization}.  If this macro is set to FALSE, then
RTEMS assumes that the hardware managed interrupt stack is supported by
this CPU.  If the CPU has a hardware managed interrupt stack, then the
porter has the option of letting the BSP allcoate and initialize the
interrupt stack or letting RTEMS do this.  If RTEMS is to allocate the
memory for the interrupt stack, then the macro
CPU_ALLOCATE_INTERRUPT_STACK should be set to TRUE.  If this macro is set
to FALSE, then it is the responsibility of the BSP to allocate the memory
for this stack and initialize it.

If the CPU does not support a dedicated interrupt stack, then the porter
has two options: (1) execute interrupts on the stack of the interrupted
task, and (2) have RTEMS manage a dedicated interrupt stack.

NOTE: If CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE, then the macro
CPU_ALLOCATE_INTERRUPT_STACK should also be set to TRUE.

Only one of CPU_HAS_SOFTWARE_INTERRUPT_STACK and
CPU_HAS_HARDWARE_INTERRUPT_STACK should be set to TRUE.  It is possible
that both are FALSE for a particular CPU.  Although it is unclear what
that would imply about the interrupt processing procedure on that CPU.

@subsection Allocation of Interrupt Stack Memory

Whether or not the interrupt stack is hardware or software managed, RTEMS
may allocate memory for the interrupt stack from the Executive Workspace.  
If RTEMS is going to allocate the memory for a dedicated interrupt stack
in the Interrupt Manager, then the macro CPU_ALLOCATE_INTERRUPT_STACK
should be set to TRUE.

NOTE: This should be TRUE is CPU_HAS_SOFTWARE_INTERRUPT_STACK is TRUE.

@example
#define CPU_ALLOCATE_INTERRUPT_STACK TRUE
@end example

If the CPU_HAS_SOFTWARE_INTERRUPT_STACK macro is set to TRUE, then RTEMS automatically allocates the stack memory in the initialization of the Interrupt Manager and the switch to that stack is performed in @code{_ISR_Handler} on the outermost interrupt.  The _CPU_Interrupt_stack_low and _CPU_Interrupt_stack_high variables contain the addresses of the the lowest and highest addresses of the memory allocated for the interrupt stack.  Although technically only one of these addresses is required to switch to the interrupt stack, by always providing both addresses, the port has more options avaialble to it without requiring modifications to the portable parts of the executive.  Whether the stack  grows up or down, this give the CPU dependent code the option of picking the version it wants to use.

@example
SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;
@end example

NOTE: These two variables are required if the macro
CPU_HAS_SOFTWARE_INTERRUPT_STACK is defined as TRUE.

@subsection Install the Interrupt Stack

The _CPU_Install_interrupt_stack routine XXX

This routine installs the hardware interrupt stack pointer.

NOTE:  It need only be provided if CPU_HAS_HARDWARE_INTERRUPT_STAC is TRUE.

@example
void _CPU_Install_interrupt_stack( void )
@end example


@section ISR Installation

@subsection Install a Raw Interrupt Handler

The _CPU_ISR_install_raw_handler XXX
 
@example
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
@end example

This is where we install the interrupt handler into the "raw" interrupt
table used by the CPU to dispatch interrupt handlers.

@subsection Interrupt Context

@subsection Maximum Number of Vectors

There are two related macros used to defines the number of entries in the
_ISR_Vector_table managed by RTEMS.  The macro
CPU_INTERRUPT_NUMBER_OF_VECTORS is the actual number of vectors supported
by this CPU model.  The second macro is the
CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER.  Since the table is zero-based, this
indicates the highest vector number which can be looked up in the table
and mapped into a user provided handler.

@example
#define CPU_INTERRUPT_NUMBER_OF_VECTORS      32
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER \
        (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)
@end example


@subsection Install RTEMS Interrupt Handler

The _CPU_ISR_install_vector routine installs the RTEMS handler for the
specified vector.

XXX Input parameters:
vector      - interrupt vector number
old_handler - former ISR for this vector number
new_handler - replacement ISR for this vector number
 
@example
void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
@end example

@example
*old_handler = _ISR_Vector_table[ vector ];
@end example

If the interrupt vector table is a table of pointer to isr entry points,
then we need to install the appropriate RTEMS interrupt handler for this
vector number.
 
@example
_CPU_ISR_install_raw_handler( vector, new_handler, old_handler );
@end example

We put the actual user ISR address in _ISR_vector_table.  This will be
used by the @code{_ISR_Handler} so the user gets control.

@example
_ISR_Vector_table[ vector ] = new_handler;
@end example

@section Interrupt Processing

@subsection Interrupt Frame Data Structure

When an interrupt occurs, it is the responsibility of the interrupt
dispatching software to save the context of the processor such that an ISR
written in a high-level language (typically C) can be invoked without
damaging the state of the task that was interrupted.  In general, this
results in the saving of registers which are NOT preserved across
subroutine calls as well as any special interrupt state.  A port should
define the @code{CPU_Interrupt_frame} structure so that application code can
examine the saved state.

@example
typedef struct @{
    unsigned32 not_preserved_register_1;
    unsigned32 special_interrupt_register;
@} CPU_Interrupt_frame;
@end example


@subsection Interrupt Dispatching

The @code{_ISR_Handler} routine provides the RTEMS interrupt management.
 
@example
void _ISR_Handler()
@end example

This discussion ignores a lot of the ugly details in a real implementation
such as saving enough registers/state to be able to do something real.  
Keep in mind that the goal is to invoke a user's ISR handler which is
written in C.  That ISR handler uses a known set of registers thus
allowing the ISR to preserve only those that would normally be corrupted
by a subroutine call.

Also note that the exact order is to a large extent flexible.  Hardware
will dictate a sequence for a certain subset of @code{_ISR_Handler} while
requirements for setting the RTEMS state variables that indicate the
interrupt nest level (@code{_ISR_Nest_level}) and dispatching disable
level (@code{_Thread_Dispatch_disable_level}) will also
restrict the allowable order.

Upon entry to @code{_ISR_Handler}, @code{_Thread_Dispatch_disable_level} is
zero if the interrupt occurred while outside an RTEMS service call.
Conversely, it will be non-zero if interrupting an RTEMS service
call.  Thus, @code{_Thread_Dispatch_disable_level} will always be
greater than or equal to @code{_ISR_Nest_level} and not strictly
equal.  

Upon entry to the "common" @code{_ISR_Handler}, the vector number must be
available.  On some CPUs the hardware puts either the vector number or the
offset into the vector table for this ISR in a known place.  If the
hardware does not provide this information, then the assembly portion of
RTEMS for this port will contain a set of distinct interrupt entry points
which somehow place the vector number in a known place (which is safe if
another interrupt nests this one) and branches to @code{_ISR_Handler}.

@example
save some or all context on stack
may need to save some special interrupt information for exit

#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
    if ( _ISR_Nest_level == 0 )
        switch to software interrupt stack
#endif
_ISR_Nest_level++;
_Thread_Dispatch_disable_level++;
(*_ISR_Vector_table[ vector ])( vector );
--_ISR_Nest_level;
if ( _ISR_Nest_level )
    goto the label "exit interrupt (simple case)"
#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
    restore stack
#endif
 
if ( _Thread_Dispatch_disable_level )
    goto the label "exit interrupt (simple case)"
  
if ( _Thread_Dispatch_necessary )
   call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
   prepare to get out of interrupt
   return from interrupt  (maybe to _ISR_Dispatch)
 
LABEL "exit interrupt (simple case):
 prepare to get out of interrupt
 return from interrupt
@end example

Some ports have the special routine @code{_ISR_Dispatch} because
the CPU has a special "interrupt mode" and RTEMS must switch back
to the task stack and/or non-interrupt mode before invoking
@code{_Thread_Dispatch}.  For example, consider the MC68020 where 
upon return from the outermost interrupt, the CPU must switch
from the interrupt stack to the master stack before invoking
@code{_Thread_Dispatch}.  @code{_ISR_Dispatch} is the special port
specific wrapper for @code{_Thread_Dispatch} used in this case.
  
@subsection ISR Invoked with Frame Pointer

Does the RTEMS invoke the user's ISR with the vector number and a pointer
to the saved interrupt frame (1) or just the vector number (0)?

@example
#define CPU_ISR_PASSES_FRAME_POINTER 0
@end example

NOTE: It is desirable to include a pointer to the interrupt stack frame as
an argument to the interrupt service routine.  Eventually, it would be
nice if all ports included this parameter.

@subsection Pointer to _Thread_Dispatch Routine

With some compilation systems, it is difficult if not impossible to call a
high-level language routine from assembly language.  This is especially
true of commercial Ada compilers and name mangling C++ ones.  This
variable can be optionally defined by the CPU porter and contains the
address of the routine _Thread_Dispatch.  This can make it easier to
invoke that routine at the end of the interrupt sequence (if a dispatch is
necessary).

@example
void (*_CPU_Thread_dispatch_pointer)();
@end example

