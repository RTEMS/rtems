@c
@c COPYRIGHT (c) 1989-2008.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Mutex Manager

@section Introduction

The mutex manager implements the functionality required of the mutex
manager as defined by POSIX 1003.1b-1996. This standard requires that
a compliant operating system provide the facilties to ensure that
threads can operate with mutual exclusion from one another and
defines the API that must be provided.

The services provided by the mutex manager are:

@itemize @bullet
@item @code{pthread_mutexattr_init} - Initialize a Mutex Attribute Set
@item @code{pthread_mutexattr_destroy} - Destroy a Mutex Attribute Set
@item @code{pthread_mutexattr_setprotocol} - Set the Blocking Protocol
@item @code{pthread_mutexattr_getprotocol} - Get the Blocking Protocol
@item @code{pthread_mutexattr_setprioceiling} - Set the Priority Ceiling
@item @code{pthread_mutexattr_getprioceiling} - Get the Priority Ceiling
@item @code{pthread_mutexattr_setpshared} - Set the Visibility
@item @code{pthread_mutexattr_getpshared} - Get the Visibility
@item @code{pthread_mutex_init} - Initialize a Mutex
@item @code{pthread_mutex_destroy} - Destroy a Mutex
@item @code{pthread_mutex_lock} - Lock a Mutex
@item @code{pthread_mutex_trylock} - Poll to Lock a Mutex
@item @code{pthread_mutex_timedlock} - Lock a Mutex with Timeout
@item @code{pthread_mutex_unlock} - Unlock a Mutex
@item @code{pthread_mutex_setprioceiling} - Dynamically Set the Priority Ceiling
@item @code{pthread_mutex_getprioceiling} - Dynamically Get the Priority Ceiling
@end itemize

@section Background

@subsection Mutex Attributes

Mutex attributes are utilized only at mutex creation time. A mutex
attribute structure may be initialized and passed as an argument to
the @code{mutex_init} routine. Note that the priority ceiling of
a mutex may be set at run-time.

@table @b
@item blocking protcol
is the XXX

@item priority ceiling
is the XXX

@item pshared
is the XXX

@end table

@subsection PTHREAD_MUTEX_INITIALIZER

This is a special value that a variable of type @code{pthread_mutex_t}
may be statically initialized to as shown below:

@example
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
@end example

This indicates that @code{my_mutex} will be automatically initialized
by an implicit call to @code{pthread_mutex_init} the first time
the mutex is used.

Note that the mutex will be initialized with default attributes.

@section Operations

There is currently no text in this section.

@section Services

This section details the mutex manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection pthread_mutexattr_init - Initialize a Mutex Attribute Set

@findex pthread_mutexattr_init
@cindex  initialize a mutex attribute set

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_init(
  pthread_mutexattr_t *attr
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_mutexattr_init} routine initializes the mutex attributes
object specified by @code{attr} with the default value for all of the
individual attributes.

@subheading NOTES:

XXX insert list of default attributes here.

@c
@c
@c
@page
@subsection pthread_mutexattr_destroy - Destroy a Mutex Attribute Set

@findex pthread_mutexattr_destroy
@cindex  destroy a mutex attribute set

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_destroy(
  pthread_mutexattr_t *attr
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

The @code{pthread_mutex_attr_destroy} routine is used to destroy a mutex
attributes object. The behavior of using an attributes object after
it is destroyed is implementation dependent.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pthread_mutexattr_setprotocol - Set the Blocking Protocol

@findex pthread_mutexattr_setprotocol
@cindex  set the blocking protocol

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_setprotocol(
  pthread_mutexattr_t *attr,
  int                  protocol
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The protocol argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_mutexattr_setprotocol} routine is used to set value of the
@code{protocol} attribute. This attribute controls the order in which
threads waiting on this mutex will receive it.

The @code{protocol} can be one of the following:

@table @b

@item @code{PTHREAD_PRIO_NONE}
in which case blocking order is FIFO.

@item @code{PTHREAD_PRIO_INHERIT}
in which case blocking order is priority with the priority inheritance
protocol in effect.

@item @code{PTHREAD_PRIO_PROTECT}
in which case blocking order is priority with the priority ceiling
protocol in effect.

@end table

@subheading NOTES:

There is currently no way to get simple priority blocking ordering
with POSIX mutexes even though this could easily by supported by RTEMS.

@c
@c
@c
@page
@subsection pthread_mutexattr_getprotocol - Get the Blocking Protocol

@findex pthread_mutexattr_getprotocol
@cindex  get the blocking protocol

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_getprotocol(
  pthread_mutexattr_t *attr,
  int                 *protocol
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The protocol pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_mutexattr_getprotocol} routine is used to obtain
the value of the @code{protocol} attribute. This attribute controls
the order in which threads waiting on this mutex will receive it.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pthread_mutexattr_setprioceiling - Set the Priority Ceiling

@findex pthread_mutexattr_setprioceiling
@cindex  set the priority ceiling

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_setprioceiling(
  pthread_mutexattr_t *attr,
  int                  prioceiling
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The prioceiling argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_mutexattr_setprioceiling} routine is used to set value of the
@code{prioceiling} attribute. This attribute specifies the priority that
is the ceiling for threads obtaining this mutex. Any task obtaining this
mutex may not be of greater priority that the ceiling. If it is of lower
priority, then its priority will be elevated to @code{prioceiling}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pthread_mutexattr_getprioceiling - Get the Priority Ceiling

@findex pthread_mutexattr_getprioceiling
@cindex  get the priority ceiling

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_getprioceiling(
  const pthread_mutexattr_t *attr,
  int                       *prioceiling
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The prioceiling pointer argument is invalid.

@end table

@subheading DESCRIPTION:

The @code{pthread_mutexattr_getprioceiling} routine is used to obtain the
value of the @code{prioceiling} attribute. This attribute specifies the
priority ceiling for this mutex.


@subheading NOTES:


NONE
@c
@c
@c
@page
@subsection pthread_mutexattr_setpshared - Set the Visibility

@findex pthread_mutexattr_setpshared
@cindex  set the visibility

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_setpshared(
  pthread_mutexattr_t *attr,
  int                  pshared
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The pshared argument is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutexattr_getpshared - Get the Visibility

@findex pthread_mutexattr_getpshared
@cindex  get the visibility

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutexattr_getpshared(
  const pthread_mutexattr_t *attr,
  int                       *pshared
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute pointer argument is invalid.

@item EINVAL
The attribute set is not initialized.

@item EINVAL
The pshared pointer argument is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_init - Initialize a Mutex

@findex pthread_mutex_init
@cindex  initialize a mutex

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_init(
  pthread_mutex_t           *mutex,
  const pthread_mutexattr_t *attr
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The attribute set is not initialized.

@item EINVAL
The specified protocol is invalid.

@item EAGAIN
The system lacked the necessary resources to initialize another mutex.

@item ENOMEM
Insufficient memory exists to initialize the mutex.

@item EBUSY
Attempted to reinialize the object reference by mutex, a previously
initialized, but not yet destroyed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_destroy - Destroy a Mutex

@findex pthread_mutex_destroy
@cindex  destroy a mutex

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_destroy(
  pthread_mutex_t *mutex
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The specified mutex is invalid.

@item EBUSY
Attempted to destroy the object reference by mutex, while it is locked or
referenced by another thread.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_lock - Lock a Mutex

@findex pthread_mutex_lock
@cindex  lock a mutex

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_lock(
  pthread_mutex_t *mutex
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The specified mutex is invalid.

@item EINVAL
The mutex has the protocol attribute of PTHREAD_PRIO_PROTECT and the
priority of the calling thread is higher than the current priority
ceiling.

@item EDEADLK
The current thread already owns the mutex.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_trylock - Poll to Lock a Mutex

@findex pthread_mutex_trylock
@cindex  poll to lock a mutex

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_trylock(
  pthread_mutex_t *mutex
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The specified mutex is invalid.

@item EINVAL
The mutex has the protocol attribute of PTHREAD_PRIO_PROTECT and the
priority of the calling thread is higher than the current priority
ceiling.

@item EDEADLK
The current thread already owns the mutex.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_timedlock - Lock a Mutex with Timeout

@findex pthread_mutex_timedlock
@cindex  lock a mutex with timeout

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>
#include <time.h>

int pthread_mutex_timedlock(
  pthread_mutex_t       *mutex,
  const struct timespec *timeout
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The specified mutex is invalid.

@item EINVAL
The nanoseconds field of timeout is invalid.

@item EINVAL
The mutex has the protocol attribute of PTHREAD_PRIO_PROTECT and the
priority of the calling thread is higher than the current priority
ceiling.

@item EDEADLK
The current thread already owns the mutex.

@item ETIMEDOUT
The calling thread was unable to obtain the mutex within the specified
timeout period.

@end table

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c
@c
@page
@subsection pthread_mutex_unlock - Unlock a Mutex

@findex pthread_mutex_unlock
@cindex  unlock a mutex

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_unlock(
  pthread_mutex_t *mutex
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The specified mutex is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_setprioceiling - Dynamically Set the Priority Ceiling

@findex pthread_mutex_setprioceiling
@cindex  dynamically set the priority ceiling

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_setprioceiling(
  pthread_mutex_t *mutex,
  int              prioceiling,
  int             *oldceiling
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The oldceiling pointer parameter is invalid.

@item EINVAL
The prioceiling parameter is an invalid priority.

@item EINVAL
The specified mutex is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_mutex_getprioceiling - Get the Current Priority Ceiling

@findex pthread_mutex_getprioceiling
@cindex  get the current priority ceiling

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_mutex_getprioceiling(
  pthread_mutex_t *mutex,
  int             *prioceiling
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The prioceiling pointer parameter is invalid.

@item EINVAL
The specified mutex is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

