@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 

@chapter Error Reporting Support 

@section Introduction

These error reporting facilities are an RTEMS support
component that provide convenient facilities for handling
error conditions in an RTEMS application.
of each task using a period.  The services provided by the error
reporting support component are:

@itemize @bullet
@item @code{rtems_error} - Report an Error
@item @code{rtems_panic} - Report an Error and Panic
@item @code{rtems_status_text} - ASCII Version of RTEMS Status
@end itemize

@section Background

@subsection Error Handling in an Embedded System

Error handling in an embedded system is a difficult problem.  If the error
is severe, then the only recourse is to shut the system down in a safe
manner.  Other errors can be detected and compensated for.  The 
error reporting routines in this support component -- @code{rtems_error}
and @code{rtems_panic} assume that if the error is severe enough,
then the system should be shutdown.  If a simple shutdown with
some basic diagnostic information is not sufficient, then
these routines should not be used in that particular system.  In this case,
use the @code{rtems_status_text} routine to construct an application
specific error reporting routine.

@section Operations

@subsection Reporting an Error

The @code{rtems_error} and @code{rtems_panic} routines
can be used to print some diagnostic information and
shut the system down.  The @code{rtems_error} routine
is invoked with a user specified error level indicator.
This error indicator is used to determine if the system
should be shutdown after reporting this error.

@section Routines

This section details the error reporting support compenent's routine.
A subsection is dedicated to each of this manager's routines
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection rtems_status_text - ASCII Version of RTEMS Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
const char *rtems_status_text(
  rtems_status_code status
);
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES:

Returns a pointer to a constant string that describes the given
RTEMS status code.

@subheading DESCRIPTION:

This routine returns a pointer to a string that describes
the RTEMS status code specified by @code{status}.

@subheading NOTES:

NONE

@page
@subsection rtems_error - Report an Error

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rtems_error(
  int         error_code,
  const char *printf_format,
  ...
);
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES:

Returns the number of characters written.

@subheading DESCRIPTION:

This routine prints the requested information as specified by the
@code{printf_format} parameter and the zero or more optional arguments
following that parameter.  The @code{error_code} parameter is an error
number with either @code{RTEMS_ERROR_PANIC} or @code{RTEMS_ERROR_ABORT}
bitwise or'ed with it.  If the @code{RTEMS_ERROR_PANIC} bit is set, then
then the system is system is shutdown via a call to @code{_exit}.  
If the @code{RTEMS_ERROR_ABORT} bit is set, then
then the system is system is shutdown via a call to @code{abort}.

@subheading NOTES:

NONE

@page
@subsection rtems_panic - Report an Error and Panic

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rtems_panic(
   const char *printf_format,
   ...
);
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES:

Returns the number of characters written.

@subheading DESCRIPTION:

This routine is a wrapper for the @code{rtems_error} routine with
an implied error level of @code{RTEMS_ERROR_PANIC}.  See 
@code{rtems_error} for more information.

@subheading NOTES:

NONE

