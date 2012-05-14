@c
@c COPYRIGHT(c) 1988-2002.
@c On-Line Applications Research Corporation(OAR).
@c All rights reserved.

@chapter Semaphore Manager

@section Introduction

The semaphore manager provides functions to allocate, delete, and control
semaphores. This manager is based on the POSIX 1003.1 standard.

The directives provided by the semaphore manager are:

@itemize @bullet
@item @code{sem_init} - Initialize an unnamed semaphore
@item @code{sem_destroy} - Destroy an unnamed semaphore
@item @code{sem_open} - Open a named semaphore
@item @code{sem_close} - Close a named semaphore
@item @code{sem_unlink} - Remove a named semaphore
@item @code{sem_wait} - Lock a semaphore
@item @code{sem_trywait} - Lock a semaphore
@item @code{sem_timedwait} - Wait on a Semaphore for a Specified Time
@item @code{sem_post} - Unlock a semaphore
@item @code{sem_getvalue} - Get the value of a semeaphore
@end itemize

@section Background

@subsection Theory
Semaphores are used for synchronization and mutual exclusion by indicating the
availability and number of resources. The task (the task which is returning
resources) notifying other tasks of an event increases the number of resources
held by the semaphore by one. The task (the task which will obtain resources)
waiting for the event decreases the number of resources held by the semaphore
by one. If the number of resources held by a semaphore is insufficient (namely
0), the task requiring resources will wait until the next time resources are
returned to the semaphore. If there is more than one task waiting for a
semaphore, the tasks will be placed in the queue.

@subsection "sem_t" Structure

@findex sem_t

The @code{sem_t} structure is used to represent semaphores. It is passed as an
argument to the semaphore directives and is defined as follows:

@example
typedef int sem_t;
@end example

@subsection Building a Semaphore Attribute Set

@section Operations

@subsection Using as a Binary Semaphore
Although POSIX supports mutexes, they are only visible between threads. To work
between processes, a binary semaphore must be used.

Creating a semaphore with a limit on the count of 1 effectively restricts the
semaphore to being a binary semaphore. When the binary semaphore is available,
the count is 1. When the binary semaphore is unavailable, the count is 0.

Since this does not result in a true binary semaphore, advanced binary features like the Priority Inheritance and Priority Ceiling Protocols are not available.

There is currently no text in this section.

@section Directives

This section details the semaphore manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection sem_init - Initialize an unnamed semaphore

@findex sem_init
@cindex  initialize an unnamed semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_init(
  sem_t        *sem,
  int           pshared,
  unsigned int  value
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The value argument exceeds SEM_VALUE_MAX

@item ENOSPC
A resource required to initialize the semaphore has been exhausted
The limit on semaphores (SEM_VALUE_MAX) has been reached

@item ENOSYS
The function sem_init is not supported by this implementation

@item EPERM
The process lacks appropriate privileges to initialize the semaphore

@end table

@subheading DESCRIPTION:
The sem_init function is used to initialize the unnamed semaphore referred to
by "sem". The value of the initialized semaphore is the parameter "value". The
semaphore remains valid until it is destroyed.

ADD MORE HERE XXX

@subheading NOTES:
If the functions completes successfully, it shall return a value of zero.
Otherwise, it shall return a value of -1 and set "errno" to specify the error
that occurred.

Multiprocessing is currently not supported in this implementation.

@c
@c
@c
@page
@subsection sem_destroy - Destroy an unnamed semaphore

@findex sem_destroy
@cindex  destroy an unnamed semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_destroy(
  sem_t *sem
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The value argument exceeds SEM_VALUE_MAX

@item ENOSYS
The function sem_init is not supported by this implementation

@item EBUSY
There are currently processes blocked on the semaphore

@end table

@subheading DESCRIPTION:
The sem_destroy function is used to destroy an unnamed semaphore refered to by
"sem". sem_destroy can only be used on a semaphore that was created using
sem_init.

@subheading NOTES:
If the functions completes successfully, it shall return a value of zero.
Otherwise, it shall return a value of -1 and set "errno" to specify the error
that occurred.

Multiprocessing is currently not supported in this implementation.


@c
@c
@c
@page
@subsection sem_open - Open a named semaphore

@findex sem_open
@cindex  open a named semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_open(
  const char *name,
  int         oflag
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading ARGUMENTS:

The following flag bit may be set in oflag:

@code{O_CREAT} - Creates the semaphore if it does not already exist. If O_CREAT
is set and the semaphore already exists then O_CREAT has no effect. Otherwise,
sem_open() creates a semaphore. The O_CREAT flag requires the third and fourth
argument: mode and value of type mode_t and unsigned int, respectively.

@code{O_EXCL} - If O_EXCL and O_CREAT are set, all call to sem_open() shall fail
if the semaphore name exists

@subheading STATUS CODES:

@table @b
@item EACCES
Valid name specified but oflag permissions are denied, or the semaphore name
specified does not exist and permission to create the named semaphore is denied.

@item EEXIST
O_CREAT and O_EXCL are set and the named semaphore already exists.

@item EINTR
The sem_open() operation was interrupted by a signal.

@item EINVAL
The sem_open() operation is not supported for the given name.

@item EMFILE
Too many semaphore descriptors or file descriptors in use by this process.

@item ENAMETOOLONG
The length of the name exceed PATH_MAX or name component is longer than NAME_MAX
while POSIX_NO_TRUNC is in effect.

@item ENOENT
O_CREAT is not set and the named semaphore does not exist.

@item ENOSPC
There is insufficient space for the creation of a new named semaphore.

@item ENOSYS
The function sem_open() is not supported by this implementation.

@end table

@subheading DESCRIPTION:
The sem_open() function establishes a connection between a specified semaphore and
a process. After a call to sem_open with a specified semaphore name, a process
can reference to semaphore by the associated name using the address returned by
the call. The oflag arguments listed above control the state of the semaphore by
determining if the semaphore is created or accessed by a call to sem_open().

@subheading NOTES:


@c
@c
@c
@page
@subsection sem_close - Close a named semaphore

@findex sem_close
@cindex  close a named semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_close(
  sem_t *sem_close
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
The semaphore argument is not a valid semaphore descriptor.

@item ENOSYS
The function sem_close is not supported by this implementation.

@end table

@subheading DESCRIPTION:
The sem_close() function is used to indicate that the calling process is finished
using the named semaphore indicated by sem. The function sem_close deallocates
any system resources that were previously allocated by a sem_open system call. If
sem_close() completes successfully it returns a 1, otherwise a value of -1 is
return and errno is set.

@subheading NOTES:

@c
@c
@c
@page
@subsection sem_unlink - Unlink a semaphore

@findex sem_unlink
@cindex  unlink a semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_unlink(
  const char *name
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCESS
Permission is denied to unlink a semaphore.

@item ENAMETOOLONG
The length of the strong name exceed NAME_MAX while POSIX_NO_TRUNC is in effect.

@item ENOENT
The name of the semaphore does not exist.

@item ENOSPC
There is insufficient space for the creation of a new named semaphore.

@item ENOSYS
The function sem_unlink is not supported by this implementation.

@end table

@subheading DESCRIPTION:
The sem_unlink() function shall remove the semaphore name by the string name. If
a process is currently accessing the name semaphore, the sem_unlink command has
no effect. If one or more processes have the semaphore open when the sem_unlink
function is called, the destruction of semaphores shall be postponed until all
reference to semaphore are destroyed by calls to sem_close, _exit(), or exec.
After all references have been destroyed, it returns immediately.

If the termination is successful, the function shall return 0. Otherwise, a -1
is returned and the errno is set.

@subheading NOTES:

@c
@c
@c
@page
@subsection sem_wait - Wait on a Semaphore

@findex sem_wait
@cindex  wait on a semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_wait(
  sem_t *sem
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The "sem" argument does not refer to a valid semaphore

@end table

@subheading DESCRIPTION:
This function attempts to lock a semaphore specified by @code{sem}. If the
semaphore is available, then the semaphore is locked (i.e., the semaphore
value is decremented). If the semaphore is unavailable (i.e., the semaphore
value is zero), then the function will block until the semaphore becomes
available. It will then successfully lock the semaphore. The semaphore
remains locked until released by a @code{sem_post()} call.

If the call is unsuccessful, then the function returns -1 and sets errno to the
appropriate error code.

@subheading NOTES:
Multiprocessing is not supported in this implementation.

@c
@c
@c
@page
@subsection sem_trywait - Non-blocking Wait on a Semaphore

@findex sem_trywait
@cindex  non

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_trywait(
  sem_t *sem
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EAGAIN
The semaphore is not available (i.e., the semaphore value is zero), so the
semaphore could not be locked.

@item EINVAL
The @code{sem} argument does not refewr to a valid semaphore

@end table

@subheading DESCRIPTION:
This function attempts to lock a semaphore specified by @code{sem}. If the
semaphore is available, then the semaphore is locked (i.e., the semaphore
value is decremented) and the function returns a value of 0. The semaphore
remains locked until released by a @code{sem_post()} call. If the semaphore
is unavailable (i.e., the semaphore value is zero), then the function will
return a value of -1 immediately and set @code{errno} to EAGAIN.

If the call is unsuccessful, then the function returns -1 and sets
@code{errno} to the appropriate error code.

@subheading NOTES:
Multiprocessing is not supported in this implementation.

@c
@c
@c
@page
@subsection sem_timedwait - Wait on a Semaphore for a Specified Time

@findex sem_timedwait
@cindex  wait on a semaphore for a specified time

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_timedwait(
  sem_t                 *sem,
  const struct timespec *abstime
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EAGAIN
The semaphore is not available (i.e., the semaphore value is zero), so the
semaphore could not be locked.

@item EINVAL
The @code{sem} argument does not refewr to a valid semaphore

@end table

@subheading DESCRIPTION:

This function attemtps to lock a semaphore specified by @code{sem},
and will wait for the semaphore until the absolute time specified by
@code{abstime}. If the semaphore is available, then the semaphore is
locked (i.e., the semaphore value is decremented) and the function
returns a value of 0. The semaphore remains locked until released by
a @code{sem_post()} call. If the semaphore is unavailable, then the
function will wait for the semaphore to become available for the amount
of time specified by @code{timeout}.

If the semaphore does not become available within the interval specified by
@code{timeout}, then the function returns -1 and sets @code{errno} to EAGAIN.
If any other error occurs, the function returns -1 and sets @code{errno} to
the appropriate error code.

@subheading NOTES:
Multiprocessing is not supported in this implementation.

@c
@c
@c
@page
@subsection sem_post - Unlock a Semaphore

@findex sem_post
@cindex  unlock a semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_post(
  sem_t *sem
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The @code{sem} argument does not refer to a valid semaphore

@end table

@subheading DESCRIPTION:
This function attempts to release the semaphore specified by @code{sem}. If
other tasks are waiting on the semaphore, then one of those tasks (which one
depends on the scheduler being used) is allowed to lock the semaphore and
return from its @code{sem_wait()}, @code{sem_trywait()}, or
@code{sem_timedwait()} call. If there are no other tasks waiting on the
semaphore, then the semaphore value is simply incremented. @code{sem_post()}
returns 0 upon successful completion.

If an error occurs, the function returns -1 and sets @code{errno} to the
appropriate error code.

@subheading NOTES:
Multiprocessing is not supported in this implementation.

@c
@c
@c
@page
@subsection sem_getvalue - Get the value of a semaphore

@findex sem_getvalue
@cindex  get the value of a semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_getvalue(
  sem_t *sem,
  int   *sval
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The "sem" argument does not refer to a valid semaphore

@item ENOSYS
The function sem_getvalue is not supported by this implementation

@end table

@subheading DESCRIPTION:
The sem_getvalue functions sets the location referenced by the "sval" argument
to the value of the semaphore without affecting the state of the semaphore. The
updated value represents a semaphore value that occurred at some point during
the call, but is not necessarily the actual value of the semaphore when it
returns to the calling process.

If "sem" is locked, the value returned by sem_getvalue will be zero or a
negative number whose absolute value is the number of processes waiting for the
semaphore at some point during the call.

@subheading NOTES:
If the functions completes successfully, it shall return a value of zero.
Otherwise, it shall return a value of -1 and set "errno" to specify the error
that occurred.
