@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter CPU Initialization

This section describes the general CPU and system initialization sequence
as it pertains to the CPU dependent code.

@section Introduction

XXX general startup sequence description rewritten to make it more
applicable to CPU depdent code in executive

@section CPU Dependent Configuration Table

The CPU Dependent Configuration Table contains information which tailors
the behavior of RTEMS base Some of the fields in this table are required
to be present in all ports of RTEMS.  These fields appear at the beginning
of the data structure.  Fields past this point may be CPU family and CPU
model dependent.  For example, a port may add a field to specify the
default value for an interrupt mask register on the CPU.  This table is
initialized by the Board Support Package and passed to the
rtems_initialize_executive or rtems_initialize_executive_early directive.

@example
typedef struct @{
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  unsigned32   idle_task_stack_size;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( unsigned32 );
  void       (*stack_free_hook)( void* );
  /* end of fields required on all CPUs */

  unsigned32   some_other_cpu_dependent_info;
@}   rtems_cpu_table;
@end example

@table @code
@item pretasking_hook
is the address of the user provided routine which is invoked
once RTEMS APIs are initialized.  This routine will be invoked
before any system tasks are created.  Interrupts are disabled.
This field may be NULL to indicate that the hook is not utilized.

@item predriver_hook
is the address of the user provided
routine that is invoked immediately before the
the device drivers and MPCI are initialized. RTEMS
initialization is complete but interrupts and tasking are disabled.
This field may be NULL to indicate that the hook is not utilized.

@item postdriver_hook
is the address of the user provided
routine that is invoked immediately after the
the device drivers and MPCI are initialized. RTEMS
initialization is complete but interrupts and tasking are disabled.
This field may be NULL to indicate that the hook is not utilized.

@item idle_task
is the address of the optional user
provided routine which is used as the system's IDLE task.  If
this field is not NULL, then the RTEMS default IDLE task is not
used.  This field may be NULL to indicate that the default IDLE
is to be used.

@item do_zero_of_workspace
indicates whether RTEMS should
zero the Workspace as part of its initialization.  If set to
TRUE, the Workspace is zeroed.  Otherwise, it is not.

@item idle_task_stack_size
is the size of the RTEMS idle task stack in bytes.
If this number is less than MINIMUM_STACK_SIZE, then the
idle task's stack will be MINIMUM_STACK_SIZE in byte.

@item interrupt_stack_size
is the size of the RTEMS allocated interrupt stack in bytes.
This value must be at least as large as MINIMUM_STACK_SIZE.

@item extra_mpci_receive_server_stack
is the extra stack space allocated for the RTEMS MPCI receive server task
in bytes.  The MPCI receive server may invoke nearly all directives and
may require extra stack space on some targets.

@item stack_allocate_hook
is the address of the optional user provided routine which allocates
memory for task stacks.  If this hook is not NULL, then a stack_free_hook
must be provided as well.

@item stack_free_hook
is the address of the optional user provided routine which frees
memory for task stacks.  If this hook is not NULL, then a stack_allocate_hook
must be provided as well.

@end table

@section Initializing the CPU

The _CPU_Initialize routine performs processor dependent initialization.

@example
void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void            (*thread_dispatch)  /* may be ignored */
)
@end example

The thread_dispatch argument is the address of the entry point for the
routine called at the end of an ISR once it has been decided a context
switch is necessary.  On some compilation systems it is difficult to call
a high-level language routine from assembly.  Providing the address of the
_Thread_ISR_Dispatch routine allows the porter an easy way to obtain this
critical address and thus provides an easy way to work around this
limitation on these systems.

If you encounter this problem save the entry point in a CPU dependent
variable as shown below:

@example
_CPU_Thread_dispatch_pointer = thread_dispatch;
@end example


During the initialization of the context for tasks with floating point,
the CPU dependent code is responsible for initializing the floating point
context.  If there is not an easy way to initialize the FP context during
Context_Initialize, then it is usually easier to save an "uninitialized"
FP context here and copy it to the task's during Context_Initialize.  If
this technique is used to initialize the FP contexts, then it is important
to ensure that the state of the floating point unit is in a coherent,
initialized state.

Finally, this routine is responsible for copying the application's CPU
Table into a locally accessible and modifiable area.  This is shown below:

@example
_CPU_Table = *cpu_table;
@end example


