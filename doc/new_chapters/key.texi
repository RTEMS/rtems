@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Key Manager, Key Manager Introduction, pthread_cond_timedwait, Top
@end ifinfo
@chapter Key Manager
@ifinfo
@menu
* Key Manager Introduction::
* Key Manager Background::
* Key Manager Operations::
* Key Manager Directives::
@end menu
@end ifinfo

@ifinfo
@node Key Manager Introduction, Key Manager Background, Key Manager, Key Manager
@end ifinfo
@section Introduction

The key manager ...

The directives provided by the key manager are:

@itemize @bullet
@item @code{pthread_key_create} -
@item @code{pthread_key_delete} -
@item @code{pthread_setspecific} -
@item @code{pthread_getspecific} -
@end itemize

@ifinfo
@node Key Manager Background, Key Manager Operations, Key Manager Introduction, Key Manager
@end ifinfo
@section Background

@ifinfo
@node Key Manager Operations, Key Manager Directives, Key Manager Background, Key Manager
@end ifinfo
@section Operations

@ifinfo
@node Key Manager Directives, pthread_key_create, Key Manager Operations, Key Manager
@end ifinfo
@section Directives
@ifinfo
@menu
* pthread_key_create::
* pthread_key_delete::
* pthread_setspecific::
* pthread_getspecific::
@end menu
@end ifinfo

This section details the key manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@ifinfo
@node pthread_key_create, pthread_key_delete, Key Manager Directives, Key Manager Directives
@end ifinfo
@subsection pthread_key_create

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
);
@end example

@subheading STATUS CODES:

@table @b
@item EAGAIN
There were not enough resources available to create another key.

@item ENOMEM
Insufficient memory exists to create the key.

@end table

@page
@ifinfo
@node pthread_key_delete, pthread_setspecific, pthread_key_create, Key Manager Directives
@end ifinfo
@subsection pthread_key_delete
 
@subheading CALLING SEQUENCE:
 
@example
#include <pthread.h>
 
int pthread_key_delete(
  pthread_key_t  key,
);
@end example
 
@subheading STATUS CODES:
 
@table @b
@item EINVAL
The key was invalid
 
@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@ifinfo
@node pthread_setspecific, pthread_getspecific, pthread_key_delete, Key Manager Directives
@end ifinfo
@subsection pthread_setspecific

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
The specified key is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@ifinfo
@node pthread_getspecific, Clock Manager, pthread_setspecific, Key Manager Directives
@end ifinfo
@subsection pthread_getspecific

@subheading CALLING SEQUENCE:

@example
#include <pthread.h>

void *pthread_getspecific(
  pthread_key_t  key
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

