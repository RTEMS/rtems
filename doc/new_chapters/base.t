@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Mutex Manager

@section Introduction

The mutex manager ...

The directives provided by the mutex manager are:

@itemize @bullet
@item @code{sigaddset} - 
@item @code{sigdelset} - 
@item @code{sigfillset} - 
@item @code{sigismember} - 
@item @code{sigemptyset} - 
@item @code{sigaction} - 
@item @code{pthread_kill} - 
@item @code{pthread_sigmask} - 
@item @code{kill} - 
@item @code{sigwait} - 
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the mutex manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection sigaddset

@subheading CALLING SEQUENCE:

@example
int sigaddset(
  sigset_t   *set,
  int         signo
);
@end example

@subheading STATUS CODES:

@subheading DESCRIPTION:

@subheading NOTES:

