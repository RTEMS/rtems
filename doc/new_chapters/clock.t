@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Clock Manager

@section Introduction

The clock manager ...

The directives provided by the clock manager are:

@itemize @bullet
@item @code{clock_gettime} - 
@item @code{clock_settime} -
@item @code{clock_getres} -
@item @code{sleep} - Delay Process Execution
@item @code{nanosleep} -
@item @code{gettimeofday} -
@item @code{time} -
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

@subsection clock_gettime -

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

@page
@subsection clock_settime -
 
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
 
@page
@subsection clock_getres -
 
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

@page
@subsection sleep - Delay Process Execution
 
@subheading CALLING SEQUENCE:
 
@example
#include <time.h>
 
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
 
@page
@subsection nanosleep -
 
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

@page
@subsection gettimeofday -

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

On error, this routine returns -1 and sets errno as appropriate.

@subheading DESCRIPTION:

@subheading NOTES:

Currently, the timezone information is not supported.

@page
@subsection time -
 
@subheading CALLING SEQUENCE:
 
@example
#include <time.h>
 
int time(
  time_t  *tloc
);
@end example
 
@subheading STATUS CODES:

This routine returns the number of seconds since the Epoch.

@subheading DESCRIPTION:
 
@subheading NOTES:

NONE
 
