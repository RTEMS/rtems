@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Stack Bounds Checker

@section Introduction

The stack bounds checker is an RTEMS support component that determines
if a task has overflowed its run-time stack.  The routines provided
by the stack bounds checker manager are:

@itemize @bullet
@item @code{Stack_check_Initialize} - Initialize the Stack Bounds Checker
@item @code{Stack_check_Dump_usage} - Report Task Stack Usage
@end itemize

@section Background

@subsection Task Stack

Each task in a system has a fixed size stack associated with it.  This
stack is allocated when the task is created.  As the task executes, the
stack is used to contain parameters, return addresses, saved registers,
and local variables.  The amount of stack space required by a task
is dependent on the exact set of routines used.  The peak stack usage 
reflects the worst case of subroutine pushing information on the stack.
For example, if a subroutine allocates a local buffer of 1024 bytes, then
this data must be accounted for in the stack of every task that invokes that
routine.

Recursive routines make calculating peak stack usage difficult, if not
impossible.  Each call to the recursive routine consumes @i{n} bytes
of stack space.  If the routine recursives 1000 times, then @code{1000 * @i{n}}
bytes of stack space are required.

@subsection Execution

The stack bounds checker operates as a set of task extensions.  At
task creation time, the task's stack is filled with a pattern to
indicate the stack is unused.  As the task executes, it will overwrite
this pattern in memory.  At each task switch, the stack bounds checker's
task switch extension is executed.  This extension checks that the last
@code{n} bytes of the task's stack have not been overwritten.  If they
have, then a blown stack error is reported.

The number of bytes checked for an overwrite is processor family dependent.
The minimum stack frame per subroutine call varies widely between processor
families.  On CISC families like the Motorola MC68xxx and Intel ix86, all
that is needed is a return address.  On more complex RISC processors,
the minimum stack frame per subroutine call may include space to save
a significant number of registers.

Another processor dependent feature that must be taken into account by
the stack bounds checker is the direction that the stack grows.  On some
processor families, the stack grows up or to higher addresses as the
task executes.  On other families, it grows down to lower addresses.  The
stack bounds checker implementation uses the stack description definitions
provided by every RTEMS port to get for this information.

@section Operations

@subsection Initializing the Stack Bounds Checker

The stack checker is initialized automatically when its task
create extension runs for the first time.  When this occurs,
the @code{Stack_check_Initialize} is invoked.

The application must include the stack bounds checker extension set
in its set of Initial Extensions.  This set of extensions is
defined as @code{STACK_CHECKER_EXTENSION}.  If using @code{<confdefs.h>}
for Configuration Table generation, then all that is necessary is
to define the macro @code{STACK_CHECKER_ON} before including 
@code{<confdefs.h>} as shown below:

@example
@group
#define STACK_CHECKER_ON
  ...
#include <confdefs.h>
@end group
@end example

@subsection Reporting Task Stack Usage

The application may dynamically report the stack usage for every task
in the system by calling the @code{Stack_check_Dump_usage} routine.
This routine prints a table with the peak usage and stack size of
every task in the system.  The following is an example of the
report generated:

@example
@group
    ID      NAME       LOW        HIGH     AVAILABLE      USED
0x04010001  IDLE  0x003e8a60  0x003e9667       2952        200
0x08010002  TA1   0x003e5750  0x003e7b57       9096       1168
0x08010003  TA2   0x003e31c8  0x003e55cf       9096       1168
0x08010004  TA3   0x003e0c40  0x003e3047       9096       1104
0xffffffff  INTR  0x003ecfc0  0x003effbf      12160        128
@end group
@end example

Notice the last time.  The task id is 0xffffffff and its name is "INTR".
This is not actually a task, it is the interrupt stack.

@subsection When a Task Overflows the Stack

When the stack bounds checker determines that a stack overflow has occurred,
it will attempt to print a message identifying the task and then shut the
system down.  If the stack overflow has caused corruption, then it is 
possible that the message can not be printed.

The following is an example of the output generated:

@example
@group
BLOWN STACK!!! Offending task(0x3eb360): id=0x08010002; name=0x54413120
  stack covers range 0x003e5750 - 0x003e7b57 (9224 bytes)
  Damaged pattern begins at 0x003e5758 and is 128 bytes long
@end group
@end example

The above includes the task id and a pointer to the task control block as
well as enough information so one can look at the task's stack and
see what was happening.

@section Routines

This section details the stack bounds checker's routines.
A subsection is dedicated to each of routines
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection Stack_check_Initialize - Initialize the Stack Bounds Checker

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void Stack_check_Initialize( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

Initialize the stack bounds checker.

@subheading NOTES:

This is performed automatically the first time the stack bounds checker
task create extension executes.

@page
@subsection Stack_check_Dump_usage - Report Task Stack Usage

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void Stack_check_Dump_usage( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine prints a table with the peak stack usage and stack space
allocation of every task in the system.

@subheading NOTES:

NONE
