@c
@c  COPYRIGHT (c) 1988-2012.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 


@chapter Timespec Helpers

@section Introduction

The Timespec helpers manager provides directives to assist in manipulating
instances of the POSIX @code{struct timespec} structure.

The directives provided by the timespec helpers manager are:

@itemize @bullet
@item @code{rtems_timespec_set} - Set timespec's value
@item @code{rtems_timespec_zero} - Zero timespec's value
@item @code{rtems_timespec_is_valid} - Check if timespec is valid 
@item @code{rtems_timespec_add_to} - Add two timespecs
@item @code{rtems_timespec_subtract} - Subtract two timespecs
@item @code{rtems_timespec_divide} - Divide two timespecs
@item @code{rtems_timespec_divide_by_integer} - Divide timespec by integer
@item @code{rtems_timespec_less_than} - Less than operator
@item @code{rtems_timespec_greater_than} - Greater than operator
@item @code{rtems_timespec_equal_to} - Check if two timespecs are equal
@item @code{rtems_timespec_get_seconds} - Obtain seconds portion of timespec
@item @code{rtems_timespec_get_nanoseconds} - Obtain nanoseconds portion of timespec
@item @code{rtems_timespec_to_ticks} - Convert timespec to number of ticks
@item @code{rtems_timespec_from_ticks} - Convert ticks to timespec
@end itemize

@section Background

@subsection Time Storage Conventions

Time can be stored in many ways. One of them is the @code{struct timespec}
format which is a structure that consists of the fields @code{tv_sec}
to represent seconds and @code{tv_nsec} to represent nanoseconds.  The
@code{struct timeval} structure is simular and consists of seconds (stored
in @code{tv_sec}) and microseconds (stored in @code{tv_usec}). Either
@code{struct timespec} or @code{struct timeval} can be used to represent
elapsed time, time of executing some operations, or time of day.

@section Operations

@subsection Set and Obtain Timespec Value

A user may write a specific time by passing the desired seconds and
nanoseconds values and the destination @code{struct timespec} using the
@code{rtems_timespec_set} directive.

The @code{rtems_timespec_zero} directive is used to zero the seconds
and nanoseconds portions of a @code{struct timespec} instance.

Users may obtain the seconds or nanoseconds portions of a @code{struct
timespec} instance with the @code{rtems_timespec_get_seconds} or
@code{rtems_timespec_get_nanoseconds} methods, respectively.

@subsection Timespec Math

A user can perform multiple operations on @code{struct timespec}
instances. The helpers in this manager assist in adding, subtracting, and
performing divison on @code{struct timespec} instances.

@itemize @bullet
@item Adding two @code{struct timespec} can be done using the
@code{rtems_timespec_add_to} directive. This directive is used mainly
to calculate total amount of time consumed by multiple operations.

@item The @code{rtems_timespec_subtract} is used to subtract two
@code{struct timespecs} instances and determine the elapsed time between
those two points in time.

@item The @code{rtems_timespec_divide} is used to use to divide one
@code{struct timespec} instance by another. This calculates the percentage
with a precision to three decimal points.

@item The @code{rtems_timespec_divide_by_integer} is used to divide a
@code{struct timespec} instance by an integer. It is commonly used in
benchmark calculations to dividing duration by the number of iterations
performed.

@end itemize

@subsection Comparing struct timespec Instances

A user can compare two @code{struct timespec} instances using the
@code{rtems_timespec_less_than}, @code{rtems_timespec_greater_than}
or @code{rtems_timespec_equal_to} routines.

@subsection Conversions and Validity Check

Conversion to and from clock ticks may be performed by using the
@code{rtems_timespec_to_ticks} and @code{rtems_timespec_from_ticks}
directives.

User can also check validity of timespec with
@code{rtems_timespec_is_valid} routine.

@section Directives

This section details the Timespec Helpers manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection TIMESPEC_SET - Set struct timespec Instance

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_timespec_set(
  struct timespec *time,
  time_t           seconds,
  uint32_t         nanoseconds
);
@end example
@findex rtems_timespec_set
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive sets the @code{struct timespec} @code{time} value to the
desired @code{seconds} and @code{nanoseconds} values.


@subheading NOTES:

This method does NOT check if @code{nanoseconds} is less than the
maximum number of nanoseconds in a second.

@page
@subsection TIMESPEC_ZERO - Zero struct timespec Instance

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_timespec_zero(
  struct timespec *time
);
@end example
@findex rtems_timespec_zero
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:
NONE

@subheading DESCRIPTION:

This routine sets the contents of the @code{struct timespec} instance
@code{time} to zero.

@subheading NOTES:
NONE

@page
@subsection TIMESPEC_IS_VALID - Check validity of a struct timespec instance

@subheading CALLING SEQUENCE:

@ifset is-C
@example
bool rtems_timespec_is_valid(
  const struct timespec *time
);
@end example
@findex rtems_timespec_is_valid
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:
This method returns @code{true} if the instance is valid, and @code{false}
otherwise.

@subheading DESCRIPTION:

This routine check validity of a @code{struct timespec} instance. It
checks if the nanoseconds portion of the @code{struct timespec} instanceis
in allowed range (less than the maximum number of nanoseconds per second).

@subheading NOTES:

@page
@subsection TIMESPEC_ADD_TO - Add Two struct timespec Instances

@subheading CALLING SEQUENCE:

@ifset is-C
@example
uint32_t rtems_timespec_add_to(
  struct timespec       *time,
  const struct timespec *add
);
@end example
@findex rtems_timespec_add_to
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:
The method returns the number of seconds @code{time} increased by.

@subheading DESCRIPTION:
This routine adds two @code{struct timespec} instances. The second argument is added to the first. The parameter @code{time} is the base time to which the @code{add} parameter is added.

@subheading NOTES:
NONE
@page
@subsection TIMESPEC_SUBTRACT - Subtract Two struct timespec Instances

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_timespec_subtract(
  const struct timespec *start,
  const struct timespec *end,
  struct timespec       *result
);
@end example
@findex rtems_timespec_subtract
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:
NONE

@subheading DESCRIPTION:
This routine subtracts @code{start} from @code{end} saves the difference
in @code{result}. The primary use of this directive is to calculate
elapsed time.

@subheading NOTES:

It is possible to subtract when @code{end} is less than @code{start}
and it produce negative @code{result}. When doing this you should be
careful and remember that only the seconds portion of a @code{struct
timespec} instance is signed, which means that nanoseconds portion is
always increasing. Due to that when your timespec has seconds = -1 and
nanoseconds=500,000,000 it means that result is -0.5 second, NOT the
expected -1.5!

@page
@subsection TIMESPEC_DIVIDE - Divide Two struct timespec Instances

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_timespec_divide(
  const struct timespec *lhs,
  const struct timespec *rhs,
  uint32_t              *ival_percentage,
  uint32_t              *fval_percentage
);
@end example
@findex rtems_timespec_divide
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

NONE

@subheading DESCRIPTION:

This routine divides the @code{struct timespec} instance @code{lhs} by
the @code{struct timespec} instance @code{rhs}. The result is returned
in the @code{ival_percentage} and @code{fval_percentage}, representing
the integer and fractional results of the division respectively.

The @code{ival_percentage} is integer value of calculated percentage and @code{fval_percentage} is fractional part of calculated percentage.

@subheading NOTES:

The intended use is calculating percentges to three decimal points.

When dividing by zero, this routine return both @code{ival_percentage}
and @code{fval_percentage} equal zero.

The division is performed using exclusively integer operations.

@page
@subsection TIMESPEC_DIVIDE_BY_INTEGER - Divide a struct timespec Instance by an Integer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rtems_timespec_divide_by_integer(
  const struct timespec *time,
  uint32_t               iterations,
  struct timespec       *result
);
@end example
@findex rtems_timespec_divide_by_integer
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

NONE

@subheading DESCRIPTION:
This routine divides the @code{struct timespec} instance @code{time} by the integer value @code{iterations}.
The result is saved in @code{result}.

@subheading NOTES:

The expected use is to assist in benchmark calculations where you
typically divide a duration (@code{time}) by a number of iterations what
gives average time.

@page
@subsection TIMESPEC_LESS_THAN - Less than operator

@subheading CALLING SEQUENCE:

@ifset is-C
@example
bool rtems_timespec_less_than(
  const struct timespec *lhs,
  const struct timespec *rhs
);
@end example
@findex rtems_timespec_less_than
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

This method returns @code{struct true} if @code{lhs} is less than
@code{rhs} and @code{struct false} otherwise.

@subheading DESCRIPTION:

This method is the less than operator for @code{struct timespec} instances. The first parameter is the left hand side and the second is the right hand side of the comparison.

@subheading NOTES:
NONE
@page
@subsection TIMESPEC_GREATER_THAN - Greater than operator

@subheading CALLING SEQUENCE:

@ifset is-C
@example
bool rtems_timespec_greater_than(
  const struct timespec *_lhs,
  const struct timespec *_rhs
);
@end example
@findex rtems_timespec_greater_than
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

This method returns @code{struct true} if @code{lhs} is greater than
@code{rhs} and @code{struct false} otherwise.

@subheading DESCRIPTION:

This method is greater than operator for @code{struct timespec} instances. 

@subheading NOTES:
NONE

@page
@subsection TIMESPEC_EQUAL_TO - Check equality of timespecs

@subheading CALLING SEQUENCE:

@ifset is-C
@example
bool rtems_timespec_equal_to(
  const struct timespec *lhs,
  const struct timespec *rhs
);
@end example
@findex rtems_timespec_equal_to
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

This method returns @code{struct true} if @code{lhs} is equal to
@code{rhs} and @code{struct false} otherwise.


@subheading DESCRIPTION:

This method is equality operator for @code{struct timespec} instances. 

@subheading NOTES:
NONE

@page
@subsection TIMESPEC_GET_SECONDS - Get Seconds Portion of struct timespec Instance

@subheading CALLING SEQUENCE:

@ifset is-C
@example
time_t rtems_timespec_get_seconds(
  struct timespec *time
);
@end example
@findex rtems_timespec_get_seconds
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

This method returns the seconds portion of the specified @code{struct
timespec} instance.

@subheading DESCRIPTION:

This method returns the seconds portion of the specified @code{struct timespec} instance @code{time}.

@subheading NOTES:
NONE
@page
@subsection TIMESPEC_GET_NANOSECONDS - Get Nanoseconds Portion of the struct timespec Instance

@subheading CALLING SEQUENCE:

@ifset is-C
@example
uint32_t rtems_timespec_get_nanoseconds(
  struct timespec *_time
);
@end example
@findex rtems_timespec_get_nanoseconds
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

This method returns the nanoseconds portion of the specified @code{struct
timespec} instance.

@subheading DESCRIPTION:
This method returns the nanoseconds portion of the specified timespec
which is pointed by @code{_time}.

@subheading NOTES:

@page
@subsection TIMESPEC_TO_TICKS - Convert struct timespec Instance to Ticks

@subheading CALLING SEQUENCE:

@ifset is-C
@example
uint32_t rtems_timespec_to_ticks(
  const struct timespec *time
);
@end example
@findex rtems_timespec_to_ticks
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

This directive returns the number of ticks computed.

@subheading DESCRIPTION:

This directive converts the @code{time} timespec to the corresponding number of clock ticks.

@subheading NOTES:
NONE

@page
@subsection TIMESPEC_FROM_TICKS - Convert Ticks to struct timespec Representation

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_timespec_from_ticks(
  uint32_t         ticks,
  struct timespec *time
);
@end example
@findex rtems_timespec_from_ticks
@end ifset

@ifset is-Ada
Not Currently Supported In Ada
@end ifset

@subheading STATUS CODES:

NONE

@subheading DESCRIPTION:

This routine converts the @code{ticks} to the corresponding @code{struct timespec} representation and stores it in @code{time}. 

@subheading NOTES:
NONE

