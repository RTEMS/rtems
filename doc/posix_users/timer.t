@c
@c  This is the chapter from the RTEMS POSIX 1003.1b API User's Guide that
@c  documents the services provided by the timer @c  manager.

@chapter Timer Manager

@section Introduction

The timer manager is ...

The services provided by the timer manager are:

@itemize @bullet
@item @code{timer_create} - Create a Per-Process Timer
@item @code{timer_delete} - Delete a Per-Process Timer
@item @code{timer_settime} - Set Next Timer Expiration
@item @code{timer_gettime} - Get Time Remaining on Timer
@item @code{timer_getoverrun} - Get Timer Overrun Count
@end itemize

@section Background

@section Operations

@section System Calls

This section details the timer manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  timer_create
@c

@page
@subsection timer_create - Create a Per-Process Timer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <time.h>
#include <signal.h>

int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  timer_delete
@c

@page
@subsection timer_delete - Delete a Per-Process Timer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <time.h>

int timer_delete(
  timer_t timerid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  timer_settime
@c

@page
@subsection timer_settime - Set Next Timer Expiration

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <time.h>

int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *value,
  struct itimerspec       *ovalue
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  timer_gettime
@c

@page
@subsection timer_gettime - Get Time Remaining on Timer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <time.h>

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  timer_getoverrun
@c

@page
@subsection timer_getoverrun - Get Timer Overrun Count

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <time.h>

int timer_getoverrun(
  timer_t   timerid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

