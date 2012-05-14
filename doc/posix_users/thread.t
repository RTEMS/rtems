@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Thread Manager

@section Introduction

The thread manager implements the functionality required of the thread
manager as defined by POSIX 1003.1b-1996. This standard requires that
a compliant operating system provide the facilties to manage multiple
threads of control and defines the API that must be provided.

The services provided by the thread manager are:

@itemize @bullet
@item @code{pthread_attr_init} - Initialize a Thread Attribute Set
@item @code{pthread_attr_destroy} - Destroy a Thread Attribute Set
@item @code{pthread_attr_setdetachstate} - Set Detach State
@item @code{pthread_attr_getdetachstate} - Get Detach State
@item @code{pthread_attr_setstacksize} - Set Thread Stack Size
@item @code{pthread_attr_getstacksize} - Get Thread Stack Size
@item @code{pthread_attr_setstackaddr} - Set Thread Stack Address
@item @code{pthread_attr_getstackaddr} - Get Thread Stack Address
@item @code{pthread_attr_setscope} - Set Thread Scheduling Scope
@item @code{pthread_attr_getscope} - Get Thread Scheduling Scope
@item @code{pthread_attr_setinheritsched} - Set Inherit Scheduler Flag
@item @code{pthread_attr_getinheritsched} - Get Inherit Scheduler Flag
@item @code{pthread_attr_setschedpolicy} - Set Scheduling Policy
@item @code{pthread_attr_getschedpolicy} - Get Scheduling Policy
@item @code{pthread_attr_setschedparam} - Set Scheduling Parameters
@item @code{pthread_attr_getschedparam} - Get Scheduling Parameters
@item @code{pthread_create} - Create a Thread
@item @code{pthread_exit} - Terminate the Current Thread
@item @code{pthread_detach} - Detach a Thread
@item @code{pthread_join} - Wait for Thread Termination
@item @code{pthread_self} - Get Thread ID
@item @code{pthread_equal} - Compare Thread IDs
@item @code{pthread_once} - Dynamic Package Initialization
@item @code{pthread_setschedparam} - Set Thread Scheduling Parameters
@item @code{pthread_getschedparam} - Get Thread Scheduling Parameters
@end itemize

@section Background

@subsection Thread Attributes

Thread attributes are utilized only at thread creation time. A thread
attribute structure may be initialized and passed as an argument to
the @code{pthread_create} routine.

@table @b
@item stack address
is the address of the optionally user specified stack area for this thread.
If this value is NULL, then RTEMS allocates the memory for the thread stack
from the RTEMS Workspace Area. Otherwise, this is the user specified
address for the memory to be used for the thread's stack. Each thread must
have a distinct stack area. Each processor family has different alignment
rules which should be followed.

@item stack size
is the minimum desired size for this thread's stack area.
If the size of this area as specified by the stack size attribute
is smaller than the minimum for this processor family and the stack
is not user specified, then RTEMS will automatically allocate a
stack of the minimum size for this processor family.

@item contention scope
specifies the scheduling contention scope. RTEMS only supports the
PTHREAD_SCOPE_PROCESS scheduling contention scope.

@item scheduling inheritance
specifies whether a user specified or the scheduling policy and
parameters of the currently executing thread are to be used. When
this is PTHREAD_INHERIT_SCHED, then the scheduling policy and
parameters of the currently executing thread are inherited by
the newly created thread.

@item scheduling policy and parameters
specify the manner in which the thread will contend for the processor.
The scheduling parameters are interpreted based on the specified policy.
All policies utilize the thread priority parameter.

@end table

@section Operations

There is currently no text in this section.

@section Services

This section details the thread manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection pthread_attr_init - Initialize a Thread Attribute Set

@findex pthread_attr_init
@cindex  initialize a thread attribute set

@subheading CALLING SEQUENCE:


@example
#include <pthread.h>

int pthread_attr_init(
  pthread_attr_t *attr
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_init} routine initializes the thread attributes
object specified by @code{attr} with the default value for all of the
individual attributes.

@subheading NOTES:

The settings in the default attributes are implementation defined. For
RTEMS, the default attributes are as follows:

@itemize @bullet

@item stackadr
is not set to indicate that RTEMS is to allocate the stack memory.

@item stacksize
is set to @code{PTHREAD_MINIMUM_STACK_SIZE}.

@item contentionscope
is set to @code{PTHREAD_SCOPE_PROCESS}.

@item inheritsched
is set to @code{PTHREAD_INHERIT_SCHED} to indicate that the created
thread inherits its scheduling attributes from its parent.

@item detachstate
is set to @code{PTHREAD_CREATE_JOINABLE}.

@end itemize


@c
@c
@c
@page
@subsection pthread_attr_destroy - Destroy a Thread Attribute Set

@findex pthread_attr_destroy
@cindex  destroy a thread attribute set

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_destroy(
  pthread_attr_t *attr
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_destroy} routine is used to destroy a thread
attributes object. The behavior of using an attributes object after
it is destroyed is implementation dependent.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pthread_attr_setdetachstate - Set Detach State

@findex pthread_attr_setdetachstate
@cindex  set detach state

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setdetachstate(
  pthread_attr_t *attr,
  int             detachstate
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The detachstate argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setdetachstate} routine is used to value of the
@code{detachstate} attribute. This attribute controls whether the
thread is created in a detached state.

The @code{detachstate} can be either @code{PTHREAD_CREATE_DETACHED} or
@code{PTHREAD_CREATE_JOINABLE}. The default value for all threads is
@code{PTHREAD_CREATE_JOINABLE}.

@subheading NOTES:

If a thread is in a detached state,
then the use of the ID with the @code{pthread_detach} or
@code{pthread_join} routines is an error.

@c
@c
@c
@page
@subsection pthread_attr_getdetachstate - Get Detach State

@findex pthread_attr_getdetachstate
@cindex  get detach state

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getdetachstate(
  const pthread_attr_t *attr,
  int                  *detachstate
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The detatchstate pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getdetachstate} routine is used to obtain the
current value of the @code{detachstate} attribute as specified
by the @code{attr} thread attribute object.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pthread_attr_setstacksize - Set Thread Stack Size

@findex pthread_attr_setstacksize
@cindex  set thread stack size

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setstacksize(
  pthread_attr_t *attr,
  size_t          stacksize
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setstacksize} routine is used to set the
@code{stacksize} attribute in the @code{attr} thread attribute
object.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_ATTR_STACKSIZE} to indicate that this
routine is supported.

If the specified stacksize is below the minimum required for this CPU
(@code{PTHREAD_STACK_MIN}, then the stacksize will be set to the minimum
for this CPU.

@c
@c
@c
@page
@subsection pthread_attr_getstacksize - Get Thread Stack Size

@findex pthread_attr_getstacksize
@cindex  get thread stack size

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getstacksize(
  const pthread_attr_t *attr,
  size_t               *stacksize
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The stacksize pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getstacksize} routine is used to obtain the
@code{stacksize} attribute in the @code{attr} thread attribute
object.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_ATTR_STACKSIZE} to indicate that this
routine is supported.

@c
@c
@c
@page
@subsection pthread_attr_setstackaddr - Set Thread Stack Address

@findex pthread_attr_setstackaddr
@cindex  set thread stack address

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setstackaddr(
  pthread_attr_t *attr,
  void           *stackaddr
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setstackaddr} routine is used to set the
@code{stackaddr} attribute in the @code{attr} thread attribute
object.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_ATTR_STACKADDR} to indicate that this
routine is supported.

It is imperative to the proper operation of the system that
each thread have sufficient stack space.

@c
@c
@c
@page
@subsection pthread_attr_getstackaddr - Get Thread Stack Address

@findex pthread_attr_getstackaddr
@cindex  get thread stack address

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getstackaddr(
  const pthread_attr_t  *attr,
  void                 **stackaddr
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The stackaddr pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getstackaddr} routine is used to obtain the
@code{stackaddr} attribute in the @code{attr} thread attribute
object.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_ATTR_STACKADDR} to indicate that this
routine is supported.

@c
@c
@c
@page
@subsection pthread_attr_setscope - Set Thread Scheduling Scope

@findex pthread_attr_setscope
@cindex  set thread scheduling scope

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setscope(
  pthread_attr_t *attr,
  int             contentionscope
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The contention scope specified is not valid.

@item ENOTSUP
The contention scope specified (PTHREAD_SCOPE_SYSTEM) is not supported.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setscope} routine is used to set the contention
scope field in the thread attribute object @code{attr} to the value
specified by @code{contentionscope}.

The @code{contentionscope} must be either @code{PTHREAD_SCOPE_SYSTEM}
to indicate that the thread is to be within system scheduling contention
or @code{PTHREAD_SCOPE_PROCESS} indicating that the thread is to be
within the process scheduling contention scope.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_attr_getscope - Get Thread Scheduling Scope

@findex pthread_attr_getscope
@cindex  get thread scheduling scope

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getscope(
  const pthread_attr_t *attr,
  int                  *contentionscope
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The contentionscope pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getscope} routine is used to obtain the
value of the contention scope field in the thread attributes
object @code{attr}. The current value is returned in
@code{contentionscope}.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.
@c
@c
@c
@page
@subsection pthread_attr_setinheritsched - Set Inherit Scheduler Flag

@findex pthread_attr_setinheritsched
@cindex  set inherit scheduler flag

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setinheritsched(
  pthread_attr_t *attr,
  int             inheritsched
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The specified scheduler inheritance argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setinheritsched} routine is used to set the
inherit scheduler field in the thread attribute object @code{attr} to
the value specified by @code{inheritsched}.

The @code{contentionscope} must be either @code{PTHREAD_INHERIT_SCHED}
to indicate that the thread is to inherit the scheduling policy
and parameters fromthe creating thread, or @code{PTHREAD_EXPLICIT_SCHED}
to indicate that the scheduling policy and parameters for this thread
are to be set from the corresponding values in the attributes object.
If @code{contentionscope} is @code{PTHREAD_INHERIT_SCHED}, then the
scheduling attributes in the @code{attr} structure will be ignored
at thread creation time.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_attr_getinheritsched - Get Inherit Scheduler Flag

@findex pthread_attr_getinheritsched
@cindex  get inherit scheduler flag

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getinheritsched(
  const pthread_attr_t *attr,
  int                  *inheritsched
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The inheritsched pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getinheritsched} routine is used to
object the current value of the inherit scheduler field in
the thread attribute object @code{attr}.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_attr_setschedpolicy - Set Scheduling Policy

@findex pthread_attr_setschedpolicy
@cindex  set scheduling policy

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setschedpolicy(
  pthread_attr_t *attr,
  int             policy
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item ENOTSUP
The specified scheduler policy argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setschedpolicy} routine is used to set the
scheduler policy field in the thread attribute object @code{attr} to
the value specified by @code{policy}.

Scheduling policies may be one of the following:

@itemize @bullet

@item @code{SCHED_DEFAULT}
@item @code{SCHED_FIFO}
@item @code{SCHED_RR}
@item @code{SCHED_SPORADIC}
@item @code{SCHED_OTHER}

@end itemize

The precise meaning of each of these is discussed elsewhere in this
manual.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_attr_getschedpolicy - Get Scheduling Policy

@findex pthread_attr_getschedpolicy
@cindex  get scheduling policy

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getschedpolicy(
  const pthread_attr_t *attr,
  int                  *policy
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The specified scheduler policy argument pointer is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getschedpolicy} routine is used to obtain the
scheduler policy field from the thread attribute object @code{attr}.
The value of this field is returned in @code{policy}.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_attr_setschedparam - Set Scheduling Parameters

@findex pthread_attr_setschedparam
@cindex  set scheduling parameters

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_setschedparam(
  pthread_attr_t           *attr,
  const struct sched_param  param
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The specified scheduler parameter argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_setschedparam} routine is used to set the
scheduler parameters field in the thread attribute object @code{attr} to
the value specified by @code{param}.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_attr_getschedparam - Get Scheduling Parameters

@findex pthread_attr_getschedparam
@cindex  get scheduling parameters

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_attr_getschedparam(
  const pthread_attr_t *attr,
  struct sched_param   *param
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The specified scheduler parameter argument pointer is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_attr_getschedparam} routine is used to obtain the
scheduler parameters field from the thread attribute object @code{attr}.
The value of this field is returned in @code{param}.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_create - Create a Thread

@findex pthread_create
@cindex  create a thread

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_create(
  pthread_t             *thread,
  const pthread_attr_t  *attr,
  void                 (*start_routine)( void *),
  void                  *arg
);
@end example

@subheading STATUS CODES:

@table @b

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The user specified a stack address and the size of the area was not
large enough to meet this processor's minimum stack requirements.

@item EINVAL
The specified scheduler inheritance policy was invalid.

@item ENOTSUP
The specified contention scope was PTHREAD_SCOPE_PROCESS.

@item EINVAL
The specified thread priority was invalid.

@item EINVAL
The specified scheduling policy was invalid.

@item EINVAL
The scheduling policy was SCHED_SPORADIC and the specified replenishment
period is less than the initial budget.

@item EINVAL
The scheduling policy was SCHED_SPORADIC and the specified low priority
is invalid.

@item EAGAIN
The system lacked the necessary resources to create another thread, or the
self imposed limit on the total number of threads in a process
PTHREAD_THREAD_MAX would be exceeded.

@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

The @code{pthread_create} routine is used to create a new thread with
the attributes specified by @code{attr}. If the @code{attr} argument
is @code{NULL}, then the default attribute set will be used. Modification
of the contents of @code{attr} after this thread is created does not
have an impact on this thread.

The thread begins execution at the address specified by @code{start_routine}
with @code{arg} as its only argument. If @code{start_routine} returns,
then it is functionally equivalent to the thread executing the
@code{pthread_exit} service.

Upon successful completion, the ID of the created thread is returned in the
@code{thread} argument.

@subheading NOTES:

There is no concept of a single main thread in RTEMS as there is in
a tradition UNIX system. POSIX requires that the implicit return of
the main thread results in the same effects as if there were a call
to @code{exit}. This does not occur in RTEMS.

The signal mask of the newly created thread is inherited from its
creator and the set of pending signals for this thread is empty.

@c
@c
@c
@page
@subsection pthread_exit - Terminate the Current Thread

@findex pthread_exit
@cindex  terminate the current thread

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

void pthread_exit(
  void *status
);
@end example

@subheading STATUS CODES:
@table @b
@item NONE

@end table

@subheading DESCRIPTION:

The @code{pthread_exit} routine is used to terminate the calling thread.
The @code{status} is made available to any successful join with the
terminating thread.

When a thread returns from its start routine, it results in an
implicit call to the @code{pthread_exit} routine with the return
value of the function serving as the argument to @code{pthread_exit}.

@subheading NOTES:

Any cancellation cleanup handlers that hace been pushed and not yet popped
shall be popped in reverse of the order that they were pushed. After
all cancellation cleanup handlers have been executed, if the
thread has any thread-specific data, destructors for that data will
be invoked.

Thread termination does not release or free any application visible
resources including byt not limited to mutexes, file descriptors, allocated
memory, etc.. Similarly, exitting a thread does not result in any
process-oriented cleanup activity.

There is no concept of a single main thread in RTEMS as there is in
a tradition UNIX system. POSIX requires that the implicit return of
the main thread results in the same effects as if there were a call
to @code{exit}. This does not occur in RTEMS.

All access to any automatic variables allocated by the threads is lost
when the thread exits. Thus references (i.e. pointers) to local variables
of a thread should not be used in a global manner without care. As
a specific example, a pointer to a local variable should NOT be used
as the return value.


@c
@c
@c
@page
@subsection pthread_detach - Detach a Thread

@findex pthread_detach
@cindex  detach a thread

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_detach(
  pthread_t thread
);
@end example

@subheading STATUS CODES:
@table @b
@item ESRCH
The thread specified is invalid.

@item EINVAL
The thread specified is not a joinable thread.

@end table

@subheading DESCRIPTION:

The @code{pthread_detach} routine is used to to indicate that storage
for @code{thread} can be reclaimed when the thread terminates without
another thread joinging with it.

@subheading NOTES:

If any threads have previously joined with the specified thread, then they
will remain joined with that thread. Any subsequent calls to
@code{pthread_join} on the specified thread will fail.

@c
@c
@c
@page
@subsection pthread_join - Wait for Thread Termination

@findex pthread_join
@cindex  wait for thread termination

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_join(
  pthread_t    thread,
  void       **value_ptr
);
@end example

@subheading STATUS CODES:
@table @b
@item ESRCH
The thread specified is invalid.

@item EINVAL
The thread specified is not a joinable thread.

@item EDEADLK
A deadlock was detected or thread is the calling thread.

@end table

@subheading DESCRIPTION:

The @code{pthread_join} routine suspends execution of the calling thread
until @code{thread} terminates. If @code{thread} has already terminated,
then this routine returns immediately. The value returned by @code{thread}
(i.e. passed to @code{pthread_exit} is returned in @code{value_ptr}.

When this routine returns, then @code{thread} has been terminated.

@subheading NOTES:

The results of multiple simultaneous joins on the same thread is undefined.

If any threads have previously joined with the specified thread, then they
will remain joined with that thread. Any subsequent calls to
@code{pthread_join} on the specified thread will fail.

If value_ptr is NULL, then no value is returned.

@c
@c
@c
@page
@subsection pthread_self - Get Thread ID

@findex pthread_self
@cindex  get thread id

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

pthread_t pthread_self( void );
@end example

@subheading STATUS CODES:

The value returned is the ID of the calling thread.

@subheading DESCRIPTION:

This routine returns the ID of the calling thread.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pthread_equal - Compare Thread IDs

@findex pthread_equal
@cindex  compare thread ids

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_equal(
   pthread_t t1,
   pthread_t t2
);
@end example

@subheading STATUS CODES:

@table @b
@item zero
The thread ids are not equal.

@item non-zero
The thread ids are equal.

@end table

@subheading DESCRIPTION:

The @code{pthread_equal} routine is used to compare two thread
IDs and determine if they are equal.

@subheading NOTES:

The behavior is undefined if the thread IDs are not valid.

@c
@c
@c
@page
@subsection pthread_once - Dynamic Package Initialization

@findex pthread_once
@cindex  dynamic package initialization

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

pthread_once_t once_control = PTHREAD_ONCE_INIT;

int pthread_once(
   pthread_once_t   *once_control,
   void            (*init_routine)(void)
);
@end example

@subheading STATUS CODES:

NONE

@subheading DESCRIPTION:

The @code{pthread_once} routine is used to provide controlled initialization
of variables. The first call to @code{pthread_once} by any thread with the
same @code{once_control} will result in the @code{init_routine} being
invoked with no arguments. Subsequent calls to @code{pthread_once} with
the same @code{once_control} will have no effect.

The @code{init_routine} is guaranteed to have run to completion when
this routine returns to the caller.

@subheading NOTES:

The behavior of @code{pthread_once} is undefined if @code{once_control}
is automatic storage (i.e. on a task stack) or is not initialized using
@code{PTHREAD_ONCE_INIT}.

@c
@c
@c
@page
@subsection pthread_setschedparam - Set Thread Scheduling Parameters

@findex pthread_setschedparam
@cindex  set thread scheduling parameters

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_setschedparam(
  pthread_t           thread,
  int                 policy,
  struct sched_param *param
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The scheduling parameters indicated by the parameter param is invalid.

@item EINVAL
The value specified by policy is invalid.

@item EINVAL
The scheduling policy was SCHED_SPORADIC and the specified replenishment
period is less than the initial budget.

@item EINVAL
The scheduling policy was SCHED_SPORADIC and the specified low priority
is invalid.

@item ESRCH
The thread indicated was invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_setschedparam} routine is used to set the
scheduler parameters currently associated with the thread specified
by @code{thread} to the policy specified by @code{policy}. The
contents of @code{param} are interpreted based upon the @code{policy}
argument.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.

@c
@c
@c
@page
@subsection pthread_getschedparam - Get Thread Scheduling Parameters

@findex pthread_getschedparam
@cindex  get thread scheduling parameters

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_getschedparam(
  pthread_t           thread,
  int                *policy,
  struct sched_param *param
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The policy pointer argument is invalid.

@item EINVAL
The scheduling parameters pointer argument is invalid.

@item ESRCH
The thread indicated by the parameter thread is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_getschedparam} routine is used to obtain the
scheduler policy and parameters associated with @code{thread}.
The current policy and associated parameters values returned in
@code{policy} and @code{param}, respectively.

@subheading NOTES:

As required by POSIX, RTEMS defines the feature symbol
@code{_POSIX_THREAD_PRIORITY_SCHEDULING} to indicate that the
family of routines to which this routine belongs is supported.
