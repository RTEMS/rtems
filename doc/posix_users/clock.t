@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Clock Manager

@section Introduction

The clock manager provides services two primary classes
of services.  The first focuses on obtaining and setting
the current date and time.  The other category of services
focus on allowing a thread to delay for a specific length
of time.

The directives provided by the clock manager are:

@itemize @bullet
@item @code{clock_gettime} - Obtain Time of Day
@item @code{clock_settime} - Set Time of Day
@item @code{clock_getres} - Get Clock Resolution
@item @code{sleep} - Delay Process Execution
@item @code{usleep} - Delay Process Execution in Microseconds
@item @code{nanosleep} - Delay with High Resolution
@item @code{gettimeofday} - Get the Time of Day
@item @code{time} - Get time in seconds
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the clock manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@subsection clock_gettime - Obtain Time of Day

@findex clock_gettime
@cindex obtain time of day

@subheading CALLING SEQUENCE:

@example
#include <time.h>

int clock_gettime(
  clockid_t        clock_id,
  struct timespec *tp
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item EINVAL
The tp pointer parameter is invalid.

@item EINVAL
The clock_id specified is invalid.
@end table

@subheading DESCRIPTION:

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection clock_settime - Set Time of Day

@findex clock_settime
@cindex  set time of day

@subheading CALLING SEQUENCE:

@example
#include <time.h>

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item EINVAL
The tp pointer parameter is invalid.

@item EINVAL
The clock_id specified is invalid.

@item EINVAL
The contents of the tp structure are invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection clock_getres - Get Clock Resolution

@findex clock_getres
@cindex  get clock resolution

@subheading CALLING SEQUENCE:

@example
#include <time.h>

int clock_getres(
  clockid_t        clock_id,
  struct timespec *res
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item EINVAL
The res pointer parameter is invalid.

@item EINVAL
The clock_id specified is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

If res is NULL, then the resolution is not returned.

@c
@c
@c
@page
@subsection sleep - Delay Process Execution

@findex sleep
@cindex  delay process execution

@subheading CALLING SEQUENCE:

@example
#include <unistd.h>

unsigned int sleep(
  unsigned int seconds
);
@end example

@subheading STATUS CODES:

This routine returns the number of unslept seconds.

@subheading DESCRIPTION:

The @code{sleep()} function delays the calling thread by the specified
number of @code{seconds}.

@subheading NOTES:

This call is interruptible by a signal.

@c
@c
@c
@page
@subsection usleep - Delay Process Execution in Microseconds

@findex usleep
@cindex  delay process execution
@cindex  delay process execution
@cindex  usecs delay process execution
@cindex  microsecond delay process execution

@subheading CALLING SEQUENCE:

@example
#include <time.h>

useconds_t usleep(
  useconds_t useconds
);
@end example

@subheading STATUS CODES:

This routine returns the number of unslept seconds.

@subheading DESCRIPTION:

The @code{sleep()} function delays the calling thread by the specified
number of @code{seconds}.

The @code{usleep()} function suspends the calling thread from execution
until either the number of microseconds specified by the
@code{useconds} argument has elapsed or a signal is delivered to the
calling thread and its action is to invoke a signal-catching function
or to terminate the process.

Because of other activity, or because of the time spent in
processing the call, the actual length of time the thread is
blocked may be longer than
the amount of time specified.

@subheading NOTES:

This call is interruptible by a signal.

The Single UNIX Specification allows this service to be implemented using
the same timer as that used by the @code{alarm()} service.  This is 
@b{NOT} the case for @b{RTEMS} and this call has no interaction with
the @code{SIGALRM} signal.

@c
@c
@c
@page
@subsection nanosleep - Delay with High Resolution

@findex nanosleep
@cindex  delay with high resolution

@subheading CALLING SEQUENCE:

@example
#include <time.h>

int nanosleep(
  const struct timespec *rqtp,
  struct timespec       *rmtp
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item EINTR
The routine was interrupted by a signal.

@item EAGAIN
The requested sleep period specified negative seconds or nanoseconds.

@item EINVAL
The requested sleep period specified an invalid number for the nanoseconds
field.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This call is interruptible by a signal.

@c
@c
@c
@page
@subsection gettimeofday - Get the Time of Day

@findex gettimeofday
@cindex  get the time of day

@subheading CALLING SEQUENCE:

@example
#include <sys/time.h>
#include <unistd.h>

int gettimeofday(
  struct timeval  *tp,
  struct timezone *tzp
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets @code{errno} as appropriate.

@table @b
@item EPERM
@code{settimeofdat} is called by someone other than the superuser.

@item EINVAL
Timezone (or something else) is invalid.

@item EFAULT
One of @code{tv} or @code{tz} pointed outside your accessible address
space

@end table

@subheading DESCRIPTION:

This routine returns the current time of day in the @code{tp} structure.

@subheading NOTES:

Currently, the timezone information is not supported. The @code{tzp}
argument is ignored.

@c
@c
@c
@page
@subsection time - Get time in seconds

@findex time
@cindex  get time in seconds

@subheading CALLING SEQUENCE:

@example
#include <time.h>

int time(
  time_t *tloc
);
@end example

@subheading STATUS CODES:

This routine returns the number of seconds since the Epoch.

@subheading DESCRIPTION:

@code{time} returns the time since 00:00:00 GMT, January 1, 1970,
measured in seconds

If @code{tloc} in non null, the return value is also stored in the
memory pointed to by @code{t}.

@subheading NOTES:

NONE

