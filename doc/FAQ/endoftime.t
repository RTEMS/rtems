@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Date/Time Issues in Systems Using RTEMS

This section provides technical information regarding
date/time representation issues and RTEMS.  The Y2K problem has
lead numerous people to ask these questions.  The answer to
these questions are actually more complicated than most 
people asking the question expect.  RTEMS supports multiple
standards and each of these standards has its own epoch and
time representation.  These standards include both programming
API and programming language standards.

In addition to the issues inside RTEMS
itself, there is the complicating factor that the Board
Support Package or application itself may interface with hardware
or software that has its own set of date/time representation
issues.  

In conclusion, viewing date/time representation as "the Y2K problem"
is very short-sighted.  Date/time representation should be viewed as
a systems level issue for the system you are building.  Each software
and hardware component in the system as well as the systems being
connected to is a factor in the equation.

@section Hardware Issues

Numerous Real-Time Clock (RTC) controllers provide only a two-digit
Binary Coded Decimal (BCD) representation for the current year.  Without
software correction, these chips are a classic example of the Y2K problem.
When the RTC rolls the year register over from 99 to 00, the device
has no idea whether the year is 1900 or 2000.  It is the responsibility
of the device driver to recognize this condition and correct for it.
The most common technique used is to assume that all years prior
to either the existence of the board or RTEMS are past 2000.  The
starting year (epoch) for RTEMS is 1988.  Thus,

@itemize @bullet
@item Chip year values 88-99 are interpreted as years 1988-2002.
@item Chip year values 00-87 are interpreted as years 2000-2087.
@end itemize

Using this technique, a RTC using a
two-digit BCD representation of the current year will overflow on
January 1, 2088.

@section RTEMS Specific Issues

Internally, RTEMS uses an unsigned thirty-two bit integer to represent the
number of seconds since midnight January 1, 1988.  This counter will
overflow on February 5, 2124. 

The time/date services in the Classic API will overflow when the 
RTEMS internal date/time representation overflows.

The POSIX API uses the type @i{time_t} to represent the number of
seconds since January 1, 1970.  Many traditional UNIX systems as
well as RTEMS define @i{time_t} as a signed thirty-two bit integer.
This representation overflows on January 18, 2038.  The solution
usually proposed is to define @i{time_t} as a sixty-four bit
integer.  This solution is appropriate for for UNIX workstations
as many of them already support sixty-four bit integers natively.
At this time, this imposes a burden on embedded systems which are
still primarily using processors with native integers of thirty-two
bits or less.

@section Language Specific Issues

The Ada95 Language Reference Manual requires that the @i{Ada.Calendar}
package support years through the year 2099.  However, just as the
hardware is layered on top of hardware and may inherit its limits,
the Ada tasking and run-time support is layered on top of an operating
system.  Thus, if the operating system or underlying hardware fail
to correctly report dates after 2099, then it is possible for the
@i{Ada.Calendar} package to fail prior to 2099.

@section Date/Time Conclusion

Each embedded system could be impacted by a variety of date/time
representation issues.  Even whether a particular date/time
representation issue impacts a system is questionable.  A system
using only the RTEMS Classic API is not impacted by the
date/time representation issues in POSIX.  A system not using
date/time at all is not impacted by any of these issues.  Also
the planned end of life for a system may make these issues
moot.

The following is a timeline of the date/time representation
issues presented in this section:

@itemize @bullet

@item 2000 - Two BCD Digit Real-Time Clock Rollover

@item 2038 - POSIX @i{time_t} Rollover

@item 2088 - Correction for Two BCD Digit Real-Time Clock Rollover

@item 2099 - Ada95 @i{Ada.Calendar} Rollover

@item 2124 - RTEMS Internal Seconds Counter Rollover

@end itemize


