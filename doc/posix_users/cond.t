@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Condition Variable Manager

@section Introduction

The condition variable manager ...

The directives provided by the condition variable manager are:

@itemize @bullet
@item @code{pthread_condattr_init} - Initialize a Condition Variable Attribute Set
@item @code{pthread_condattr_destroy} - Destroy a Condition Variable Attribute Set
@item @code{pthread_condattr_setpshared} - Set Process Shared Attribute
@item @code{pthread_condattr_getpshared} - Get Process Shared Attribute
@item @code{pthread_cond_init} - Initialize a Condition Variable
@item @code{pthread_cond_destroy} - Destroy a Condition Variable
@item @code{pthread_cond_signal} - Signal a Condition Variable
@item @code{pthread_cond_broadcast} - Broadcast a Condition Variable
@item @code{pthread_cond_wait} - Wait on a Condition Variable
@item @code{pthread_cond_timedwait} - With with Timeout a Condition Variable
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the condition variable manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection pthread_condattr_init - Initialize a Condition Variable Attribute Set

@findex pthread_condattr_init
@cindex  initialize a condition variable attribute set

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_condattr_init(
  pthread_condattr_t *attr
);
@end example

@subheading STATUS CODES:
@table @b
@item ENOMEM
Insufficient memory is available to initialize the condition variable
attributes object.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_condattr_destroy - Destroy a Condition Variable Attribute Set

@findex pthread_condattr_destroy
@cindex  destroy a condition variable attribute set

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_condattr_destroy(
  pthread_condattr_t *attr
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The attribute object specified is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_condattr_setpshared - Set Process Shared Attribute

@findex pthread_condattr_setpshared
@cindex  set process shared attribute

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_condattr_setpshared(
  pthread_condattr_t *attr,
  int                 pshared
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_condattr_getpshared - Get Process Shared Attribute

@findex pthread_condattr_getpshared
@cindex  get process shared attribute

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_condattr_getpshared(
  const pthread_condattr_t *attr,
  int                      *pshared
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c
@c
@page
@subsection pthread_cond_init - Initialize a Condition Variable

@findex pthread_cond_init
@cindex  initialize a condition variable

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
);
@end example

@subheading STATUS CODES:
@table @b
@item EAGAIN
The system lacked a resource other than memory necessary to create the
initialize the condition variable object.

@item ENOMEM
Insufficient memory is available to initialize the condition variable object.

@item EBUSY
The specified condition variable has already been initialized.

@item EINVAL
The specified attribute value is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_cond_destroy - Destroy a Condition Variable

@findex pthread_cond_destroy
@cindex  destroy a condition variable

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_destroy(
  pthread_cond_t *cond
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified condition variable is invalid.

@item EBUSY
The specified condition variable is currently in use.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_cond_signal - Signal a Condition Variable

@findex pthread_cond_signal
@cindex  signal a condition variable

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_signal(
  pthread_cond_t *cond
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified condition variable is not valid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine should not be invoked from a handler from an asynchronous signal
handler or an interrupt service routine.

@c
@c
@c
@page
@subsection pthread_cond_broadcast - Broadcast a Condition Variable

@findex pthread_cond_broadcast
@cindex  broadcast a condition variable

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_broadcast(
  pthread_cond_t *cond
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified condition variable is not valid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine should not be invoked from a handler from an asynchronous signal
handler or an interrupt service routine.

@c
@c
@c
@page
@subsection pthread_cond_wait - Wait on a Condition Variable

@findex pthread_cond_wait
@cindex  wait on a condition variable

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_wait(
  pthread_cond_t *cond,
  pthread_mutex_t *mutex
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified condition variable or mutex is not initialized OR different
mutexes were specified for concurrent pthread_cond_wait() and
pthread_cond_timedwait() operations on the same condition variable OR
the mutex was not owned by the current thread at the time of the call.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_cond_timedwait - Wait with Timeout a Condition Variable

@findex pthread_cond_timedwait
@cindex  wait with timeout a condition variable

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_timedwait(
  pthread_cond_t        *cond,
  pthread_mutex_t       *mutex,
  const struct timespec *abstime
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified condition variable or mutex is not initialized OR different
mutexes were specified for concurrent pthread_cond_wait() and
pthread_cond_timedwait() operations on the same condition variable OR
the mutex was not owned by the current thread at the time of the call.

@item ETIMEDOUT
The specified time has elapsed without the condition variable being
satisfied.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

