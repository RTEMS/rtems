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

@subsection POSIX.1 Symbols

@example
_POSIX_C_SOURCE, Feature Flag, 
@end example

@section Numerical Limits

@section C Language Limits

@example
CHAR_BIT
CHAR_MAX
CHAR_MIN
INT_MAX
INT_MIN
LONG_MAX
LONG_MIN
MB_LEN_MAX
SCHAR_MAX
SCHAR_MIN
SHRT_MAX
SHRT_MIN
UCHAR_MAX
UINT_MAX
ULONG_MAX
USHRT_MAX
@end example

@subsection Minimum Values

@example
_POSIX_AIO_LISTIO_MAX
_POSIX_AIO_MAX
_POSIX_ARG_MAX
_POSIX_CHILD_MAX
_POSIX_DELAYTIMER_MAX
_POSIX_LINK_MAX
_POSIX_LOGIN_NAME_MAX
_POSIX_MAX_CANON
_POSIX_MAX_INPUT
_POSIX_MQ_OPEN_MAX
_POSIX_MQ_PRIO_MAX
_POSIX_NAME_MAX
_POSIX_NGROUPS_MAX
_POSIX_OPEN_MAX
_POSIX_PATH_MAX
_POSIX_PIPE_BUF
_POSIX_RTSIG_MAX
_POSIX_SEM_NSEMS_MAX
_POSIX_SEM_VALUE_MAX
_POSIX_SIGQUEUE_MAX
_POSIX_SSIZE_MAX
_POSIX_STREAM_MAX
_POSIX_THREAD_DESTRUCTOR_ITERATIONS
_POSIX_THREAD_KEYS_MAX
_POSIX_THREAD_THREADS_MAX
_POSIX_TTY_NAME_MAX
_POSIX_TIME_MAX
_POSIX_TZNAME_MAX
@end example

@subsection Run-Time Increasable Values

@example
_POSIX_NGROUPS_MAX
@end example

@subsection Run-Time Invariant Values (Possible Indeterminate)

@example
AIO_LISTIO_MAX
AIO_MAX
AIO_PRIO_DELTA_MAX
ARG_MAX
CHILD_MAX
DELAYTIMER_MAX
LOGIN_NAME_MAX
MQ_OPEN_MAX
OPEN_MAX
PAGESIZE
PTHREAD_DESTRUCTOR_ITERATIONS
PTHREAD_KEYS_MAX
PTHREAD_STACK_MIN
PTHJREAD_THREADS_MAX
RTSIG_MAX
SEM_NSEMS_MAX
SEM_VALUE_MAX
SIGQUEUE_MAX
STREAM_MAX
TIMER_MAX
TTY_NAME_MAX
TZNAME_MAX
@end example

@subsection Pathname Variable Values

@example
LINK_MAX
MAX_CANON
MAX_INPUT
NAME_MAX
PATH_MAX
PIPE_BUF
@end example

@subsection Invariant Values

@example
SSIZE_MAX
@end example

@subsection Maximum Values

@example
_POSIX_CLOCKRES_MIN
@end example

@section Symbolic Constants

@subsection Symbolic Constants for the @code{access} Function

@example
R_OK
W_OK
X_OK
F_OK
@end example

@subsection Symbolic Constants for the @code{lseek} Function

@example
SEEK_SET
SEEK_CUR
SEEK_END
@end example

@subsection Compile-Time Symbolic Constants for Portability Specifications

@example
_POSIX_ASYNCHRONOUS_IO
_POSIX_FSYNC
_POSIX_JOB_CONTROL
_POSIX_MAPPED_FILES
_POSIX_MEMLOCK
_POSIX_MEMLOCK_RANGE
_POSIX_MEMORY_PROTECTION
_POSIX_MESSAGE_PASSING
_POSIX_PRIORITIZED_IO
_POSIX_PRIORITY_SCHEDULING
_POSIX_REALTIME_SIGNALS
_POSIX_SAVED_IDS
_POSIX_SEMAPHORES
_POSIX_SHARED_MEMORY_OBJECTS
_POSIX_SYNCHRONIZED_IO
_POSIX_THREADS
_POSIX_THREAD_ATTR_STACKADDR
_POSIX_THREAD_ATTR_STACKSIZE
_POSIX_THREAD_PRIORITY_SCHEDULING
_POSIX_THREAD_PRIO_INHERIT
_POSIX_THREAD_PRIO_CEILING
_POSIX_THREAD_PROCESS_SHARED
_POSIX_THREAD_SAFE_FUNCTIONS
_POSIX_TIMERS
_POSIX_VERSION
@end example

@subsection Execution-Time Symbolic Constants for Portability Specifications

@example
@end example
