@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Condition Variable Manager, Condition Variable Manager Introduction, pthread_mutex_getprioceiling, Top
@end ifinfo
@chapter Condition Variable Manager
@ifinfo
@menu
* Condition Variable Manager Introduction::
* Condition Variable Manager Background::
* Condition Variable Manager Operations::
* Condition Variable Manager Directives::
@end menu
@end ifinfo

@ifinfo
@node Condition Variable Manager Introduction, Condition Variable Manager Background, Condition Variable Manager, Condition Variable Manager
@end ifinfo
@section Introduction

The condition variable manager ...

The directives provided by the condition variable manager are:

@itemize @bullet
@item @code{pthread_condattr_init} - 
@item @code{pthread_condattr_destroy} - 
@item @code{pthread_condattr_setpshared} - 
@item @code{pthread_condattr_getpshared} - 
@item @code{pthread_cond_init} - 
@item @code{pthread_cond_destroy} - 
@item @code{pthread_cond_signal} - 
@item @code{pthread_cond_broadcast} - 
@item @code{pthread_cond_wait} - 
@item @code{pthread_cond_timedwait} - 
@end itemize

@ifinfo
@node Condition Variable Manager Background, Condition Variable Manager Operations, Condition Variable Manager Introduction, Condition Variable Manager
@end ifinfo
@section Background

@ifinfo
@node Condition Variable Manager Operations, Condition Variable Manager Directives, Condition Variable Manager Background, Condition Variable Manager
@end ifinfo
@section Operations

@ifinfo
@node Condition Variable Manager Directives, pthread_condattr_init, Condition Variable Manager Operations, Condition Variable Manager
@end ifinfo
@section Directives
@ifinfo
@menu
* pthread_condattr_init::
* pthread_condattr_destroy::
* pthread_condattr_setpshared::
* pthread_condattr_getpshared::
* pthread_cond_init::
* pthread_cond_destroy::
* pthread_cond_signal::
* pthread_cond_broadcast::
* pthread_cond_wait::
* pthread_cond_timedwait::
@end menu
@end ifinfo

This section details the condition variable manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@ifinfo
@node pthread_condattr_init, pthread_condattr_destroy, Condition Variable Manager Directives, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_condattr_init

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

@page
@ifinfo
@node pthread_condattr_destroy, pthread_condattr_setpshared, pthread_condattr_init, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_condattr_destroy

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

@page
@ifinfo
@node pthread_condattr_setpshared, pthread_condattr_getpshared, pthread_condattr_destroy, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_condattr_setpshared

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_condattr_setpshared(
  pthread_condattr_t   *attr,
  int                   pshared
);
@end example

@subheading STATUS CODES:
 
@table @b
@item EINVAL
Invalid argument passed.
 
@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@ifinfo
@node pthread_condattr_getpshared, pthread_cond_init, pthread_condattr_setpshared, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_condattr_getpshared

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_condattr_getpshared(
  const pthread_condattr_t   *attr,
  int                        *pshared
);
@end example

@subheading STATUS CODES:
 
@table @b
@item EINVAL
Invalid argument passed.
 
@end table

@subheading DESCRIPTION:

@subheading NOTES:


@page
@ifinfo
@node pthread_cond_init, pthread_cond_destroy, pthread_condattr_getpshared, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_cond_init

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

@page
@ifinfo
@node pthread_cond_destroy, pthread_cond_signal, pthread_cond_init, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_cond_destroy

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_destroy(
  pthread_cond_t           *cond
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

@page
@ifinfo
@node pthread_cond_signal, pthread_cond_broadcast, pthread_cond_destroy, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_cond_signal

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_signal(
  pthread_cond_t           *cond
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

@page
@ifinfo
@node pthread_cond_broadcast, pthread_cond_wait, pthread_cond_signal, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_cond_broadcast

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_broadcast(
  pthread_cond_t  *cond
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

@page
@ifinfo
@node pthread_cond_wait, pthread_cond_timedwait, pthread_cond_broadcast, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_cond_wait

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_cond_wait(
  pthread_cond_t           *cond,
  pthread_mutex_t          *mutex
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

@page
@ifinfo
@node pthread_cond_timedwait, Key Manager, pthread_cond_wait, Condition Variable Manager Directives
@end ifinfo
@subsection pthread_cond_timedwait

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

