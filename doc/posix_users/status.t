@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Status of Implementation

This chapter provides an overview of the status of the implementation
of the POSIX API for RTEMS.  The @i{POSIX 1003.1b Compliance Guide}
provides more detailed information regarding the implementation of
each of the numerous functions, constants, and macros specified by
the POSIX 1003.1b standard.

RTEMS supports many of the process and user/group oriented services
in a "single user/single process" manner.  This means that although
these services may be of limited usefulness or functionality, they
are provided and do work in a coherent manner.  This is significant
when porting existing code from UNIX to RTEMS.

@itemize @bullet
@item Implementation
@itemize @bullet
@item The current implementation of @code{dup()} is insufficient.
@item FIFOs @code{mkfifo()} are not currently implemented.
@item Asynchronous IO is not implemented.
@item The @code{flockfile()} family is not implemented
@item getc/putc unlocked family is not implemented
@item Shared Memory is not implemented
@item Mapped Memory is not implemented
@item NOTES:
@itemize @bullet
@item For Shared Memory and Mapped Memory services, it is unclear what
level of support is appropriate and possible for RTEMS.
@end itemize
@end itemize


@item Functional Testing
@itemize @bullet
@item Tests for unimplemented services
@end itemize

@item Performance Testing
@itemize @bullet
@item There are no POSIX Performance Tests.
@end itemize

@item Documentation
@itemize @bullet
@item Many of the service description pages are not complete in this
manual.  These need to be completed and information added to the
background and operations sections.
@item Example programs (not just tests) would be very nice.
@end itemize

@end itemize


