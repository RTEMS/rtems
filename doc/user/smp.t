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
@item @code{rtems_smp_get_processor_count} - Obtain Number of CPUs
@item @code{rtems_task_get_affinity} - Obtain Task Affinity
@item @code{rtems_task_set_affinity} - Set Task Affinity
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
@c rtems_smp_get_processor_count
@c
@page
@subsection rtems_smp_get_processor_count - Obtain Number of CPUs

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <rtems.h>

int rtems_smp_get_processor_count(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Returns the number of processors being utilized.

@subheading DESCRIPTION:

The @code{rtems_smp_get_processor_count} routine is used to obtain the
number of processor cores being utilized.

@subheading NOTES:

NONE

@c
@c rtems_task_get_affinity
@c
@page
@subsection rtems_task_get_affinity - Obtain Task Affinity

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <rtems.h>

rtems_status_code rtems_task_get_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item RTEMS_UNSATISFIED
The @code{cpuset} pointer argument is invalid.

@item RTEMS_UNSATISFIED
The @code{cpusetsize} does not match the value of @code{affinitysetsize}
field in the thread attribute object.

@item RTEMS_INVALID_ID
The @code{id} is invalid.

@end table

@subheading DESCRIPTION:

The @code{rtems_task_get_affinity} routine is used to obtain the
@code{affinityset} field from the thread object @code{id}.
The value of this field is returned in @code{cpuset}

@subheading NOTES:

NONE

@c
@c rtems_task_set_affinity
@c
@page
@subsection rtems_task_set_affinity - Set Task Affinity

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <rtems.h>

rtems_status_code rtems_task_set_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item RTEMS_UNSATISFIED
The @code{cpuset} pointer argument is invalid.

@item RTEMS_UNSATISFIED
The @code{cpusetsize} does not match the value of @code{affinitysetsize}
field in the thread attribute object.

@item RTEMS_UNSATISFIED
The @code{cpuset} did not select a valid cpu.

@item RTEMS_UNSATISFIED
The @code{cpuset} selected a cpu that was invalid.

@item RTEMS_INVALID_ID
The @code{id} is invalid.

@end table

@subheading DESCRIPTION:

The @code{rtems_task_set_affinity} routine is used to set the
@code{affinity.set} field of the thread control object associated
with @code{id}.  This value controls the @code{cpuset} that the task can
execute on.

@subheading NOTES:

NONE
