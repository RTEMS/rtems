@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Administration Interface Manager

@section Introduction

The 
administration interface manager is ...

The directives provided by the administration interface manager are:

@itemize @bullet
@item @code{admin_shutdown} - Shutdown the system
@end itemize

@section Background

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
  struct admin_args_   *args[],
  size_t                nargs
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
An invalid argument was passed to the function call.
@item ENOSYS
The function admin_shutdown() is not supported by this implementation.
@item EPERM
The caller does not have appropriate permission for shutting down the 
system.

@end table

@subheading DESCRIPTION:

The @code{admin_shutdown} function restarts the system.

@subheading NOTES:

