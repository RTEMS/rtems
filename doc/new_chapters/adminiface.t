@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Administration Interface Manager

@section Introduction

The administration interface manager provides a portable
interface for some system administrative functions.
The capabilities in this manager are defined in the POSIX
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
The default, causing the system to reboot in its usual fashion.  The
@code{admin_data} field points to an implementation defined string
that specifies the system image to reboot.

@item ADMIN_HALT
The system is simply halted; no reboot takes place.

@item ADMIN_FAST
The system does no send SIGTERM to active processes before halting.

@item ADMIN_IMMEDIATE
The system does not perform any of the normal shutdown procedures.

@item ADMIN_ALTSYSTEM
The system reboots using the @code{admin_data} string as a specification
of the system to be booted.

@item ADMIN_ALTCONFIG
The system reboots using the @code{admin_data} string as a specification
of the initial implicit configuration space.

@item ADMIN_SYSDUMP
Dump kernal memory before rebooting.

@item ADMIN_INIT
An option allowing the specification of an alternate initial program
to be run when the system reboots.

@end table

@item admin_data
This field ...

@end table

@section Operations

@subsection Shutting Down the System

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
#include <admin.h>

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
optional behavior to be performed.  There are some @code{admin_types} 
values that may provoke unspecified behavior.  The @code{nargs}
argument specifies the length of the @code{args} array.

@subheading NOTES:

The @code{_POSIX_ADMIN} feature flag is defined to indicate
this service is available.
