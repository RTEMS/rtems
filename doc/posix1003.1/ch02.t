@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Terminology and General Requirements

@section Conventions

@section Definitions

@section General Concepts

@section Error Numbers

@example
E2BIG, Constant, 
EACCES, Constant, 
EAGAIN, Constant, 
EBADF, Constant, 
EBADMSG, Constant, 
EBUSY, Constant, 
ECANCELED, Constant, 
ECHILD, Constant, 
EDEADLK, Constant, 
EDOM, Constant, 
EEXIST, Constant, 
EFAULT, Constant, 
EFBIG, Constant, 
EINPROGRESS, Constant, 
EINTR, Constant, 
EINVAL, Constant, 
EIO, Constant, 
EISDIR, Constant, 
EMFILE, Constant, 
EMLINK, Constant, 
EMSGSIZE, Constant, 
ENAMETOOLONG, Constant, 
ENFILE, Constant, 
ENODEV, Constant, 
ENOENT, Constant, 
ENOEXEC, Constant, 
ENOLCK, Constant, 
ENOMEM, Constant, 
ENOSPC, Constant, 
ENOSYS, Constant, 
ENOTDIR, Constant, 
ENOTEMPTY, Constant, 
ENOTSUP, Constant, 
ENOTTY, Constant, 
ENXIO, Constant, 
EPERM, Constant, 
EPIPE, Constant, 
ERANGE, Constant, 
EROFS, Constant, 
ESPIPE, Constant, 
ESRCH, Constant, 
ETIMEDOUT, Constant, 
EXDEV, Constant, 
@end example

@section Primitive System Types

@example
dev_t, Type, Implemented
gid_t, Type, Implemented
ino_t, Type, Implemented
mode_t, Type, Implemented
nlink_t, Type, Implemented
off_t, Type, Implemented
pid_t, Type, Implemented
pthread_t, Type, Implemented
pthread_attr_t, Type, Implemented
pthread_mutex_t, Type, Implemented
pthread_mutex_attr_t, Type, Implemented
pthread_cond_t, Type, Implemented
pthread_cond_attr_t, Type, Implemented
pthread_key_t, Type, Implemented
pthread_once_t, Type, Implemented
size_t, Type, Implemented
ssize_t, Type, Implemented
time_t, Type, Implemented
uid_t, Type, Implemented
@end example

NOTE: time_t is not listed in this section but is used by many functions.

@section Environment Description

@section C Language Definitions

@subsection Symbols From the C Standard

@example
NULL, Constant, 
@end example

@subsection POSIX.1 Symbols

@example
_POSIX_C_SOURCE, Feature Flag, 
@end example

@section Numerical Limits

@section C Language Limits

@example
CHAR_BIT, Constant, 
CHAR_MAX, Constant, 
CHAR_MIN, Constant, 
INT_MAX, Constant, 
INT_MIN, Constant, 
LONG_MAX, Constant, 
LONG_MIN, Constant, 
MB_LEN_MAX, Constant, 
SCHAR_MAX, Constant, 
SCHAR_MIN, Constant, 
SHRT_MAX, Constant, 
SHRT_MIN, Constant, 
UCHAR_MAX, Constant, 
UINT_MAX, Constant, 
ULONG_MAX, Constant, 
USHRT_MAX, Constant, 
@end example

@subsection Minimum Values

@example
_POSIX_AIO_LISTIO_MAX, Constant, 
_POSIX_AIO_MAX, Constant, 
_POSIX_ARG_MAX, Constant, 
_POSIX_CHILD_MAX, Constant, 
_POSIX_DELAYTIMER_MAX, Constant, 
_POSIX_LINK_MAX, Constant, 
_POSIX_LOGIN_NAME_MAX, Constant, 
_POSIX_MAX_CANON, Constant, 
_POSIX_MAX_INPUT, Constant, 
_POSIX_MQ_OPEN_MAX, Constant, 
_POSIX_MQ_PRIO_MAX, Constant, 
_POSIX_NAME_MAX, Constant, 
_POSIX_NGROUPS_MAX, Constant, 
_POSIX_OPEN_MAX, Constant, 
_POSIX_PATH_MAX, Constant, 
_POSIX_PIPE_BUF, Constant, 
_POSIX_RTSIG_MAX, Constant, 
_POSIX_SEM_NSEMS_MAX, Constant, 
_POSIX_SEM_VALUE_MAX, Constant, 
_POSIX_SIGQUEUE_MAX, Constant, 
_POSIX_SSIZE_MAX, Constant, 
_POSIX_STREAM_MAX, Constant, 
_POSIX_THREAD_DESTRUCTOR_ITERATIONS, Constant, 
_POSIX_THREAD_KEYS_MAX, Constant, 
_POSIX_THREAD_THREADS_MAX, Constant, 
_POSIX_TTY_NAME_MAX, Constant, 
_POSIX_TIME_MAX, Constant, 
_POSIX_TZNAME_MAX, Constant, 
@end example

@subsection Run-Time Increasable Values

@example
_POSIX_NGROUPS_MAX, Constant, 
@end example

@subsection Run-Time Invariant Values (Possible Indeterminate)

@example
AIO_LISTIO_MAX, Constant, 
AIO_MAX, Constant, 
AIO_PRIO_DELTA_MAX, Constant, 
ARG_MAX, Constant, 
CHILD_MAX, Constant, 
DELAYTIMER_MAX, Constant, 
LOGIN_NAME_MAX, Constant, 
MQ_OPEN_MAX, Constant, 
OPEN_MAX, Constant, 
PAGESIZE, Constant, 
PTHREAD_DESTRUCTOR_ITERATIONS, Constant, 
PTHREAD_KEYS_MAX, Constant, 
PTHREAD_STACK_MIN, Constant, 
PTHJREAD_THREADS_MAX, Constant, 
RTSIG_MAX, Constant, 
SEM_NSEMS_MAX, Constant, 
SEM_VALUE_MAX, Constant, 
SIGQUEUE_MAX, Constant, 
STREAM_MAX, Constant, 
TIMER_MAX, Constant, 
TTY_NAME_MAX, Constant, 
TZNAME_MAX, Constant, 
@end example

@subsection Pathname Variable Values

@example
LINK_MAX, Constant, 
MAX_CANON, Constant, 
MAX_INPUT, Constant, 
NAME_MAX, Constant, 
PATH_MAX, Constant, 
PIPE_BUF, Constant, 
@end example

@subsection Invariant Values

@example
SSIZE_MAX, Constant, 
@end example

@subsection Maximum Values

@example
_POSIX_CLOCKRES_MIN, Constant, 
@end example

@section Symbolic Constants

@subsection Symbolic Constants for the @code{access} Function

@example
R_OK, Constant, 
W_OK, Constant, 
X_OK, Constant, 
F_OK, Constant, 
@end example

@subsection Symbolic Constants for the @code{lseek} Function

@example
SEEK_SET, Constant, 
SEEK_CUR, Constant, 
SEEK_END, Constant, 
@end example

@subsection Compile-Time Symbolic Constants for Portability Specifications

@example
_POSIX_ASYNCHRONOUS_IO, Feature Flag, 
_POSIX_FSYNC, Feature Flag, 
_POSIX_JOB_CONTROL, Feature Flag, 
_POSIX_MAPPED_FILES, Feature Flag, 
_POSIX_MEMLOCK, Feature Flag, 
_POSIX_MEMLOCK_RANGE, Feature Flag, 
_POSIX_MEMORY_PROTECTION, Feature Flag, 
_POSIX_MESSAGE_PASSING, Feature Flag, 
_POSIX_PRIORITIZED_IO, Feature Flag, 
_POSIX_PRIORITY_SCHEDULING, Feature Flag, 
_POSIX_REALTIME_SIGNALS, Feature Flag, 
_POSIX_SAVED_IDS, Feature Flag, 
_POSIX_SEMAPHORES, Feature Flag, 
_POSIX_SHARED_MEMORY_OBJECTS, Feature Flag, 
_POSIX_SYNCHRONIZED_IO, Feature Flag, 
_POSIX_THREADS, Feature Flag, 
_POSIX_THREAD_ATTR_STACKADDR, Feature Flag, 
_POSIX_THREAD_ATTR_STACKSIZE, Feature Flag, 
_POSIX_THREAD_PRIORITY_SCHEDULING, Feature Flag, 
_POSIX_THREAD_PRIO_INHERIT, Feature Flag, 
_POSIX_THREAD_PRIO_CEILING, Feature Flag, 
_POSIX_THREAD_PROCESS_SHARED, Feature Flag, 
_POSIX_THREAD_SAFE_FUNCTIONS, Feature Flag, 
_POSIX_TIMERS, Feature Flag, 
_POSIX_VERSION, Feature Flag, 
@end example

@subsection Execution-Time Symbolic Constants for Portability Specifications

@example
_POSIX_ASYNC_IO, Feature Flag, 
_POSIX_CHOWN_RESTRICTED, Feature Flag, 
_POSIX_NO_TRUNC, Feature Flag, 
_POSIX_PRIO_IO, Feature Flag, 
_POSIX_SYNC_IO, Feature Flag, 
_POSIX_VDISABLE, Feature Flag, 
@end example
