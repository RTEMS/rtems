@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Administration Interface Manager

@section Introduction

The administration interface manager provides a portable
interface for some system administrative functions.
The capabilities in this manager were defined in the POSIX
1003.1h/D3 proposed standard titled @b{Services for Reliable,
Available, and Serviceable Systems}.

The directives provided by the administration interface manager are:

@itemize @bullet
@item @code{admin_shutdown} - Shutdown the system
@end itemize

@section Background

@subsection admin_args Structure

@example
put structure here
@end example

@table @b
@item admin_type
This field ...

@table @b
@item ADMIN_AUTOBOOT
This field ...

@item ADMIN_HALT
This field ...

@item ADMIN_FAST
This field ...

@item ADMIN_IMMEDIATE
This field ...

@item ADMIN_ALTSYSTEM
This field ...

@item ADMIN_ALTCONFIG
This field ...

@item ADMIN_SYSDUMP
This field ...

@item ADMIN_INIT
This field ...


@end table

@item admin_data
This field ...

@end table

@section Operations

@section Directives

This section details the administration interface manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection admin_shutdown - Shutdown the system

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int admin_shutdown(
  struct admin_args   *args[],
  size_t               nargs
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
An invalid argument was passed to the function call.

@item EPERM
The caller does not have appropriate permission for shutting down the 
system.

@end table

@subheading DESCRIPTION:

The @code{admin_shutdown} function restarts the system.  The
@code{args} argument specifies alternate or optional behavior
for the @code{admin_shutdown} function.  The @code{admin_type}
member of each element of the @code{args} array specifies the
optional behavior to be performed.  There are som @code{admin_types} 
values that may provoke unspecified behavior.  The @code{nargs}
argument specifies the length of the @code{args} array.

@subheading NOTES:

The @code{_POSIX_ADMIN} feature flag is defined to indicate
this service is available.
