@c
@c COPYRIGHT (c) 1988-1998.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.
@c
@c $Id$
@c

@chapter Key Manager

@section Introduction

The key manager ...

The directives provided by the key manager are:

@itemize @bullet
@item @code{pthread_key_create} -
@item @code{pthread_key_delete} -
@item @code{pthread_setspecific} -
@item @code{pthread_getspecific} -
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the key manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection pthread_key_create

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_key_create(
pthread_key_t *key,
void (*destructor)( void CVS Makefile cancel.t clock.t cond.t cspecific.t device.t files.t io.t j key.t memorymgmt.t message.t mutex.t nodesc nodescr posix_users.cps posix_users.fns posix_users.texi preface.texi procenv.t process.t psxmsg.t sched.t semaphores.t signal.t systemdb.t thread.t )
);
@end example

@subheading STATUS CODES:

@table @b
@item EAGAIN
There were not enough resources available to create another key.

@item ENOMEM
Insufficient memory exists to create the key.

@end table

@c
@c
@c
@page
@subsection pthread_key_delete

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_key_delete(
pthread_key_t key,
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
The key was invalid

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_setspecific

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_setspecific(
pthread_key_t key,
const void *value
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified key is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection pthread_getspecific

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

void *pthread_getspecific(
pthread_key_t key
);
@end example

@subheading STATUS CODES:
@table @b
@item NULL
There is no thread-specific data associated with the specified key.

@item non-NULL
The data associated with the specified key.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

