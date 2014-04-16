@c
@c  COPYRIGHT (c) 2014.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c

@chapter Symmetric Multiprocessing Services

@section Introduction

This chapter describes the services related to Symmetric Multiprocessing
provided by RTEMS.

The application level services currently provided are:

@itemize @bullet
@item @code{rtems_get_processor_count} - Get processor count
@item @code{rtems_get_current_processor} - Get current processor index
@item @code{rtems_scheduler_ident} - Get ID of a scheduler
@item @code{rtems_scheduler_get_processor_set} - Get processor set of a scheduler
@item @code{rtems_task_get_scheduler} - Get scheduler of a task
@item @code{rtems_task_get_affinity} - Get task processor affinity
@item @code{rtems_task_set_affinity} - Set task processor affinity
@end itemize

@c
@c
@c
@section Background

@subsection Uniprocessor versus SMP Parallelism

Uniprocessor systems have long been used in embedded systems. In this hardware
model, there are some system execution characteristics which have long been
taken for granted:

@itemize @bullet
@item one task executes at a time
@item hardware events result in interrupts
@end itemize

There is no true parallelism. Even when interrupts appear to occur
at the same time, they are processed in largely a serial fashion.
This is true even when the interupt service routines are allowed to
nest.  From a tasking viewpoint,  it is the responsibility of the real-time
operatimg system to simulate parallelism by switching between tasks.
These task switches occur in response to hardware interrupt events and explicit
application events such as blocking for a resource or delaying.

With symmetric multiprocessing, the presence of multiple processors
allows for true concurrency and provides for cost-effective performance
improvements. Uniprocessors tend to increase performance by increasing
clock speed and complexity. This tends to lead to hot, power hungry
microprocessors which are poorly suited for many embedded applications.

The true concurrency is in sharp contrast to the single task and
interrupt model of uniprocessor systems. This results in a fundamental
change to uniprocessor system characteristics listed above. Developers
are faced with a different set of characteristics which, in turn, break
some existing assumptions and result in new challenges. In an SMP system
with N processors, these are the new execution characteristics.

@itemize @bullet
@item N tasks execute in parallel
@item hardware events result in interrupts
@end itemize

There is true parallelism with a task executing on each processor and
the possibility of interrupts occurring on each processor. Thus in contrast
to their being one task and one interrupt to consider on a uniprocessor,
there are N tasks and potentially N simultaneous interrupts to consider
on an SMP system.

This increase in hardware complexity and presence of true parallelism
results in the application developer needing to be even more cautious
about mutual exclusion and shared data access than in a uniprocessor
embedded system. Race conditions that never or rarely happened when an
application executed on a uniprocessor system, become much more likely
due to multiple threads executing in parallel. On a uniprocessor system,
these race conditions would only happen when a task switch occurred at
just the wrong moment. Now there are N-1 tasks executing in parallel
all the time and this results in many more opportunities for small
windows in critical sections to be hit.

@subsection Task Affinity

@cindex task affinity
@cindex thread affinity

RTEMS provides services to manipulate the affinity of a task. Affinity
is used to specify the subset of processors in an SMP system on which
a particular task can execute.

By default, tasks have an affinity which allows them to execute on any
available processor.

Task affinity is a possible feature to be supported by SMP-aware
schedulers. However, only a subset of the available schedulers support
affinity. Although the behavior is scheduler specific, if the scheduler
does not support affinity, it is likely to ignore all attempts to set
affinity.

@subsection Critical Section Techniques and SMP

As discussed earlier, SMP systems have opportunities for true parallelism
which was not possible on uniprocessor systems. Consequently, multiple
techniques that provided adequate critical sections on uniprocessor
systems are unsafe on SMP systems. In this section, some of these
unsafe techniques will be discussed.

In general, applications must use proper operating system provided mutual
exclusion mechanisms to ensure correct behavior. This primarily means
the use of binary semaphores or mutexes to implement critical sections.

@subsubsection Disable Interrupts

Again on a uniprocessor system, there is only a single processor which
logically executes a single task and takes interrupts. On an SMP system,
each processor may take an interrupt. When the application disables
interrupts, it generally does so by altering a processor register to
mask interrupts and later to re-enable them. On a uniprocessor system,
changing this in the single processor is sufficient. However, on an SMP
system, this register in @strong{ALL} processors must be changed. There
are no comparable capabilities in an SMP system to disable all interrupts
across all processors.

@subsubsection Highest Priority Task Assumption

On a uniprocessor system, it is safe to assume that when the highest
priority task in an application executes, it will execute without being
preempted until it voluntarily blocks. Interrupts may occur while it is
executing, but there will be no context switch to another task unless
the highest priority task voluntarily initiates it.

Given the assumption that no other tasks will have their execution
interleaved with the highest priority task, it is possible for this
task to be constructed such that it does not need to acquire a binary
semaphore or mutex for protected access to shared data.

In an SMP system, it cannot be assumed there will never be a single task
executing. It should be assumed that every processor is executing another
application task. Further, those tasks will be ones which would not have
been executed in a uniprocessor configuration and should be assumed to
have data synchronization conflicts with what was formerly the highest
priority task which executed without conflict.

@subsubsection Disable Preemption

On a uniprocessor system, disabling preemption in a task is very similar
to making the highest priority task assumption. While preemption is
disabled, no task context switches will occur unless the task initiates
them voluntarily. And, just as with the highest priority task assumption,
there are N-1 processors also running tasks. Thus the assumption that no
other tasks will run while the task has preemption disabled is violated.

@subsection Task Unique Data and SMP

Per task variables are a service commonly provided by real-time operating
systems for application use. They work by allowing the application
to specify a location in memory (typically a @code{void *}) which is
logically added to the context of a task. On each task switch, the
location in memory is stored and each task can have a unique value in
the same memory location. This memory location is directly accessed as a
variable in a program.

This works well in a uniprocessor environment because there is one task
executing and one memory location containing a task-specific value. But
it is fundamentally broken on an SMP system because there are always N
tasks executing. With only one location in memory, N-1 tasks will not
have the correct value.

This paradigm for providing task unique data values is fundamentally
broken on SMP systems.

@subsubsection Classic API Per Task Variables

The Classic API provides three directives to support per task variables. These are:

@itemize @bullet
@item @code{@value{DIRPREFIX}task_variable_add} - Associate per task variable
@item @code{@value{DIRPREFIX}task_variable_get} - Obtain value of a a per task variable
@item @code{@value{DIRPREFIX}task_variable_delete} - Remove per task variable
@end itemize

As task variables are unsafe for use on SMP systems, the use of these
services should be eliminated in all software that is to be used in
an SMP environment. It is recommended that the application developer
consider the use of POSIX Keys or Thread Local Storage (TLS). POSIX Keys
are not enabled in all RTEMS configurations.

@b{STATUS}: As of March 2014, some support services in the
@code{rtems/cpukit} use per task variables. When these uses are
eliminated, the per task variable directives will be disabled when
building RTEMS in SMP configuration.

@c
@c
@c
@section Operations

@subsection Setting Affinity to a Single Processor

In many embedded applications targeting SMP systems, it is common to lock individual tasks to specific cores. In this way, one can designate a core for I/O tasks, another for computation, etc.. The following illustrates the code sequence necessary to assign a task an affinity for processor zero (0).

@example
rtems_status_code sc;
cpu_set_t         set;

CPU_EMPTY( &set );
CPU_SET( 0, &set );

sc = rtems_task_set_affinity(rtems_task_self(), sizeof(set), &set);
assert(sc == RTEMS_SUCCESSFUL);
@end example

It is important to note that the @code{cpu_set_t} is not validated until the
@code{@value{DIRPREFIX}task_set_affinity} call is made. At that point,
it is validated against the current system configuration.

@c
@c
@c
@section Directives

This section details the symmetric multiprocessing services.  A subsection
is dedicated to each of these services and describes the calling sequence,
related constants, usage, and status codes.

@c
@c rtems_get_processor_count
@c
@page
@subsection GET_PROCESSOR_COUNT - Get processor count

@subheading CALLING SEQUENCE:

@ifset is-C
@example
uint32_t rtems_get_processor_count(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

The count of processors in the system.

@subheading DESCRIPTION:

On uni-processor configurations a value of one will be returned.

On SMP configurations this returns the value of a global variable set during
system initialization to indicate the count of utilized processors.  The
processor count depends on the physically or virtually available processors and
application configuration.  The value will always be less than or equal to the
maximum count of application configured processors.

@subheading NOTES:

None.

@c
@c rtems_get_current_processor
@c
@page
@subsection GET_CURRENT_PROCESSOR - Get current processor index

@subheading CALLING SEQUENCE:

@ifset is-C
@example
uint32_t rtems_get_current_processor(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

The index of the current processor.

@subheading DESCRIPTION:

On uni-processor configurations a value of zero will be returned.

On SMP configurations an architecture specific method is used to obtain the
index of the current processor in the system.  The set of processor indices is
the range of integers starting with zero up to the processor count minus one.

Outside of sections with disabled thread dispatching the current processor
index may change after every instruction since the thread may migrate from one
processor to another.  Sections with disabled interrupts are sections with
thread dispatching disabled.

@subheading NOTES:

None.

@c
@c rtems_scheduler_ident
@c
@page
@subsection SCHEDULER_IDENT - Get ID of a scheduler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_scheduler_ident(
  rtems_name  name,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - successful operation@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{id} is NULL@*
@code{@value{RPREFIX}INVALID_NAME} - invalid scheduler name

@subheading DESCRIPTION:

Identifies a scheduler by its name.  The scheduler name is determined by the
scheduler configuration.  @xref{Configuring a System Configuring
Clustered/Partitioned Schedulers}.

@subheading NOTES:

None.

@c
@c rtems_scheduler_get_processor_set
@c
@page
@subsection SCHEDULER_GET_PROCESSOR_SET - Get processor set of a scheduler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_scheduler_get_processor_set(
  rtems_id   scheduler_id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - successful operation@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{cpuset} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid scheduler id@*
@code{@value{RPREFIX}INVALID_NUMBER} - the affinity set buffer is too small for
set of processors owned by the scheduler

@subheading DESCRIPTION:

Returns the processor set owned by the scheduler in @code{cpuset}.  A set bit
in the processor set means that this processor is owned by the scheduler and a
cleared bit means the opposite.

@subheading NOTES:

None.

@c
@c rtems_task_get_scheduler
@c
@page
@subsection TASK_GET_SCHEDULER - Get scheduler of a task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_task_get_scheduler(
  rtems_id  id,
  rtems_id *scheduler_id
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - successful operation@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{scheduler_id} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid task id

@subheading DESCRIPTION:

Returns the scheduler identifier of a task in @code{scheduler_id}.

@subheading NOTES:

None.

@c
@c rtems_task_get_affinity
@c
@page
@subsection TASK_GET_AFFINITY - Get task processor affinity

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_task_get_affinity(
  rtems_id   id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - successful operation@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{cpuset} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid task id@*
@code{@value{RPREFIX}INVALID_NUMBER} - the affinity set buffer is too small for
the current processor affinity set of the task

@subheading DESCRIPTION:

Returns the current processor affinity set of the task in @code{cpuset}.  A set
bit in the affinity set means that the task can execute on this processor and a
cleared bit means the opposite.

@subheading NOTES:

None.

@c
@c rtems_task_set_affinity
@c
@page
@subsection TASK_SET_AFFINITY - Set task processor affinity

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_task_set_affinity(
  rtems_id         id,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - successful operation@*
@code{@value{RPREFIX}INVALID_ADDRESS} - @code{cpuset} is NULL@*
@code{@value{RPREFIX}INVALID_ID} - invalid task id@*
@code{@value{RPREFIX}INVALID_NUMBER} - invalid processor affinity set

@subheading DESCRIPTION:

Sets the processor affinity set for the task specified by @code{cpuset}.  A set
bit in the affinity set means that the task can execute on this processor and a
cleared bit means the opposite.

@subheading NOTES:

None.
