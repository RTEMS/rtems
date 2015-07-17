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
@item @code{rtems_task_set_scheduler} - Set scheduler of a task
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

@subsection Task Migration

@cindex task migration
@cindex thread migration

With more than one processor in the system tasks can migrate from one processor
to another.  There are three reasons why tasks migrate in RTEMS.

@itemize @bullet
@item The scheduler changes explicitly via @code{rtems_task_set_scheduler()} or
similar directives.
@item The task resumes execution after a blocking operation.  On a priority
based scheduler it will evict the lowest priority task currently assigned to a
processor in the processor set managed by the scheduler instance.
@item The task moves temporarily to another scheduler instance due to locking
protocols like @cite{Migratory Priority Inheritance} or the
@cite{Multiprocessor Resource Sharing Protocol}.
@end itemize

Task migration should be avoided so that the working set of a task can stay on
the most local cache level.

The current implementation of task migration in RTEMS has some implications
with respect to the interrupt latency.  It is crucial to preserve the system
invariant that a task can execute on at most one processor in the system at a
time.  This is accomplished with a boolean indicator in the task context.  The
processor architecture specific low-level task context switch code will mark
that a task context is no longer executing and waits that the heir context
stopped execution before it restores the heir context and resumes execution of
the heir task.  So there is one point in time in which a processor is without a
task.  This is essential to avoid cyclic dependencies in case multiple tasks
migrate at once.  Otherwise some supervising entity is necessary to prevent
life-locks.  Such a global supervisor would lead to scalability problems so
this approach is not used.  Currently the thread dispatch is performed with
interrupts disabled.  So in case the heir task is currently executing on
another processor then this prolongs the time of disabled interrupts since one
processor has to wait for another processor to make progress.

It is difficult to avoid this issue with the interrupt latency since interrupts
normally store the context of the interrupted task on its stack.  In case a
task is marked as not executing we must not use its task stack to store such an
interrupt context.  We cannot use the heir stack before it stopped execution on
another processor.  So if we enable interrupts during this transition we have
to provide an alternative task independent stack for this time frame.  This
issue needs further investigation.

@subsection Scheduler Helping Protocol

The scheduler provides a helping protocol to support locking protocols like
@cite{Migratory Priority Inheritance} or the @cite{Multiprocessor Resource
Sharing Protocol}.  Each ready task can use at least one scheduler node at a
time to gain access to a processor.  Each scheduler node has an owner, a user
and an optional idle task.  The owner of a scheduler node is determined a task
creation and never changes during the life time of a scheduler node.  The user
of a scheduler node may change due to the scheduler helping protocol.  A
scheduler node is in one of the four scheduler help states:

@table @dfn

@item help yourself

This scheduler node is solely used by the owner task.  This task owns no
resources using a helping protocol and thus does not take part in the scheduler
helping protocol.  No help will be provided for other tasks.

@item help active owner

This scheduler node is owned by a task actively owning a resource and can be
used to help out tasks.

In case this scheduler node changes its state from ready to scheduled and the
task executes using another node, then an idle task will be provided as a user
of this node to temporarily execute on behalf of the owner task.  Thus lower
priority tasks are denied access to the processors of this scheduler instance.

In case a task actively owning a resource performs a blocking operation, then
an idle task will be used also in case this node is in the scheduled state.

@item help active rival

This scheduler node is owned by a task actively obtaining a resource currently
owned by another task and can be used to help out tasks.

The task owning this node is ready and will give away its processor in case the
task owning the resource asks for help.

@item help passive

This scheduler node is owned by a task obtaining a resource currently owned by
another task and can be used to help out tasks.

The task owning this node is blocked.

@end table

The following scheduler operations return a task in need for help

@itemize @bullet
@item unblock,
@item change priority,
@item yield, and
@item ask for help.
@end itemize

A task in need for help is a task that encounters a scheduler state change from
scheduled to ready (this is a pre-emption by a higher priority task) or a task
that cannot be scheduled in an unblock operation.  Such a task can ask tasks
which depend on resources owned by this task for help.

In case it is not possible to schedule a task in need for help, then the
scheduler nodes available for the task will be placed into the set of ready
scheduler nodes of the corresponding scheduler instances.  Once a state change
from ready to scheduled happens for one of scheduler nodes it will be used to
schedule the task in need for help.

The ask for help scheduler operation is used to help tasks in need for help
returned by the operations mentioned above.  This operation is also used in
case the root of a resource sub-tree owned by a task changes.

The run-time of the ask for help procedures depend on the size of the resource
tree of the task needing help and other resource trees in case tasks in need
for help are produced during this operation.  Thus the worst-case latency in
the system depends on the maximum resource tree size of the application.

@subsection Critical Section Techniques and SMP

As discussed earlier, SMP systems have opportunities for true parallelism
which was not possible on uniprocessor systems. Consequently, multiple
techniques that provided adequate critical sections on uniprocessor
systems are unsafe on SMP systems. In this section, some of these
unsafe techniques will be discussed.

In general, applications must use proper operating system provided mutual
exclusion mechanisms to ensure correct behavior. This primarily means
the use of binary semaphores or mutexes to implement critical sections.

@subsubsection Disable Interrupts and Interrupt Locks

A low overhead means to ensure mutual exclusion in uni-processor configurations
is to disable interrupts around a critical section.  This is commonly used in
device driver code and throughout the operating system core.  On SMP
configurations, however, disabling the interrupts on one processor has no
effect on other processors.  So, this is insufficient to ensure system wide
mutual exclusion.  The macros
@itemize @bullet
@item @code{rtems_interrupt_disable()},
@item @code{rtems_interrupt_enable()}, and
@item @code{rtems_interrupt_flush()}
@end itemize
are disabled on SMP configurations and its use will lead to compiler warnings
and linker errors.  In the unlikely case that interrupts must be disabled on
the current processor, then the
@itemize @bullet
@item @code{rtems_interrupt_local_disable()}, and
@item @code{rtems_interrupt_local_enable()}
@end itemize
macros are now available in all configurations.

Since disabling of interrupts is not enough to ensure system wide mutual
exclusion on SMP, a new low-level synchronization primitive was added - the
interrupt locks.  They are a simple API layer on top of the SMP locks used for
low-level synchronization in the operating system core.  Currently they are
implemented as a ticket lock.  On uni-processor configurations they degenerate
to simple interrupt disable/enable sequences.  It is disallowed to acquire a
single interrupt lock in a nested way.  This will result in an infinite loop
with interrupts disabled.  While converting legacy code to interrupt locks care
must be taken to avoid this situation.

@example
@group
void legacy_code_with_interrupt_disable_enable( void )
@{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level );
  /* Some critical stuff */
  rtems_interrupt_enable( level );
@}

RTEMS_INTERRUPT_LOCK_DEFINE( static, lock, "Name" )

void smp_ready_code_with_interrupt_lock( void )
@{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire( &lock, &lock_context );
  /* Some critical stuff */
  rtems_interrupt_lock_release( &lock, &lock_context );
@}
@end group
@end example

The @code{rtems_interrupt_lock} structure is empty on uni-processor
configurations.  Empty structures have a different size in C
(implementation-defined, zero in case of GCC) and C++ (implementation-defined
non-zero value, one in case of GCC).  Thus the
@code{RTEMS_INTERRUPT_LOCK_DECLARE()}, @code{RTEMS_INTERRUPT_LOCK_DEFINE()},
@code{RTEMS_INTERRUPT_LOCK_MEMBER()}, and
@code{RTEMS_INTERRUPT_LOCK_REFERENCE()} macros are provided to ensure ABI
compatibility.

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

As task variables are unsafe for use on SMP systems, the use of these services
must be eliminated in all software that is to be used in an SMP environment.
The task variables API is disabled on SMP. Its use will lead to compile-time
and link-time errors. It is recommended that the application developer consider
the use of POSIX Keys or Thread Local Storage (TLS). POSIX Keys are available
in all RTEMS configurations.  For the availablity of TLS on a particular
architecture please consult the @cite{RTEMS CPU Architecture Supplement}.

The only remaining user of task variables in the RTEMS code base is the Ada
support.  So basically Ada is not available on RTEMS SMP.

@subsection Thread Dispatch Details

This section gives background information to developers interested in the
interrupt latencies introduced by thread dispatching.  A thread dispatch
consists of all work which must be done to stop the currently executing thread
on a processor and hand over this processor to an heir thread.

On SMP systems, scheduling decisions on one processor must be propagated to
other processors through inter-processor interrupts.  So, a thread dispatch
which must be carried out on another processor happens not instantaneous.  Thus
several thread dispatch requests might be in the air and it is possible that
some of them may be out of date before the corresponding processor has time to
deal with them.  The thread dispatch mechanism uses three per-processor
variables,
@itemize @bullet
@item the executing thread,
@item the heir thread, and
@item an boolean flag indicating if a thread dispatch is necessary or not.
@end itemize
Updates of the heir thread and the thread dispatch necessary indicator are
synchronized via explicit memory barriers without the use of locks.  A thread
can be an heir thread on at most one processor in the system.  The thread context
is protected by a TTAS lock embedded in the context to ensure that it is used
on at most one processor at a time.  The thread post-switch actions use a
per-processor lock.  This implementation turned out to be quite efficient and
no lock contention was observed in the test suite.

The current implementation of thread dispatching has some implications with
respect to the interrupt latency.  It is crucial to preserve the system
invariant that a thread can execute on at most one processor in the system at a
time.  This is accomplished with a boolean indicator in the thread context.
The processor architecture specific context switch code will mark that a thread
context is no longer executing and waits that the heir context stopped
execution before it restores the heir context and resumes execution of the heir
thread (the boolean indicator is basically a TTAS lock).  So, there is one
point in time in which a processor is without a thread.  This is essential to
avoid cyclic dependencies in case multiple threads migrate at once.  Otherwise
some supervising entity is necessary to prevent deadlocks.  Such a global
supervisor would lead to scalability problems so this approach is not used.
Currently the context switch is performed with interrupts disabled.  Thus in
case the heir thread is currently executing on another processor, the time of
disabled interrupts is prolonged since one processor has to wait for another
processor to make progress.

It is difficult to avoid this issue with the interrupt latency since interrupts
normally store the context of the interrupted thread on its stack.  In case a
thread is marked as not executing, we must not use its thread stack to store
such an interrupt context.  We cannot use the heir stack before it stopped
execution on another processor.  If we enable interrupts during this
transition, then we have to provide an alternative thread independent stack for
interrupts in this time frame.  This issue needs further investigation.

The problematic situation occurs in case we have a thread which executes with
thread dispatching disabled and should execute on another processor (e.g. it is
an heir thread on another processor).  In this case the interrupts on this
other processor are disabled until the thread enables thread dispatching and
starts the thread dispatch sequence.  The scheduler (an exception is the
scheduler with thread processor affinity support) tries to avoid such a
situation and checks if a new scheduled thread already executes on a processor.
In case the assigned processor differs from the processor on which the thread
already executes and this processor is a member of the processor set managed by
this scheduler instance, it will reassign the processors to keep the already
executing thread in place.  Therefore normal scheduler requests will not lead
to such a situation.  Explicit thread migration requests, however, can lead to
this situation.  Explicit thread migrations may occur due to the scheduler
helping protocol or explicit scheduler instance changes.  The situation can
also be provoked by interrupts which suspend and resume threads multiple times
and produce stale asynchronous thread dispatch requests in the system.

@c
@c
@c
@section Operations

@subsection Setting Affinity to a Single Processor

On some embedded applications targeting SMP systems, it may be beneficial to
lock individual tasks to specific processors.  In this way, one can designate a
processor for I/O tasks, another for computation, etc..  The following
illustrates the code sequence necessary to assign a task an affinity for
processor with index @code{processor_index}.

@example
@group
#include <rtems.h>
#include <assert.h>

void pin_to_processor(rtems_id task_id, int processor_index)
@{
  rtems_status_code sc;
  cpu_set_t         cpuset;

  CPU_ZERO(&cpuset);
  CPU_SET(processor_index, &cpuset);

  sc = rtems_task_set_affinity(task_id, sizeof(cpuset), &cpuset);
  assert(sc == RTEMS_SUCCESSFUL);
@}
@end group
@end example

It is important to note that the @code{cpuset} is not validated until the
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
@code{@value{RPREFIX}INVALID_NAME} - invalid scheduler name@*
@code{@value{RPREFIX}UNSATISFIED} - - a scheduler with this name exists, but
the processor set of this scheduler is empty

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
  rtems_id  task_id,
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

Returns the scheduler identifier of a task identified by @code{task_id} in
@code{scheduler_id}.

@subheading NOTES:

None.

@c
@c rtems_task_set_scheduler
@c
@page
@subsection TASK_SET_SCHEDULER - Set scheduler of a task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_task_set_scheduler(
  rtems_id task_id,
  rtems_id scheduler_id
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading DIRECTIVE STATUS CODES:

@code{@value{RPREFIX}SUCCESSFUL} - successful operation@*
@code{@value{RPREFIX}INVALID_ID} - invalid task or scheduler id@*
@code{@value{RPREFIX}INCORRECT_STATE} - the task is in the wrong state to
perform a scheduler change

@subheading DESCRIPTION:

Sets the scheduler of a task identified by @code{task_id} to the scheduler
identified by @code{scheduler_id}.  The scheduler of a task is initialized to
the scheduler of the task that created it.

@subheading NOTES:

None.

@subheading EXAMPLE:

@example
@group
#include <rtems.h>
#include <assert.h>

void task(rtems_task_argument arg);

void example(void)
@{
  rtems_status_code sc;
  rtems_id          task_id;
  rtems_id          scheduler_id;
  rtems_name        scheduler_name;

  scheduler_name = rtems_build_name('W', 'O', 'R', 'K');

  sc = rtems_scheduler_ident(scheduler_name, &scheduler_id);
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_id);
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, task, 0);
  assert(sc == RTEMS_SUCCESSFUL);
@}
@end group
@end example

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

This function will not change the scheduler of the task.  The intersection of
the processor affinity set and the set of processors owned by the scheduler of
the task must be non-empty.  It is not an error if the processor affinity set
contains processors that are not part of the set of processors owned by the
scheduler instance of the task.  A task will simply not run under normal
circumstances on these processors since the scheduler ignores them.  Some
locking protocols may temporarily use processors that are not included in the
processor affinity set of the task.  It is also not an error if the processor
affinity set contains processors that are not part of the system.
