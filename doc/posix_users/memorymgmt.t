@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Memory Management Manager

@section Introduction

The
memory management manager is ...

The directives provided by the memory management manager are:

@itemize @bullet
@item @code{mlockall} - Lock the Address Space of a Process
@item @code{munlockall} - Unlock the Address Space of a Process
@item @code{mlock} - Lock a Range of the Process Address Space
@item @code{munlock} - Unlock a Range of the Process Address Space
@item @code{mmap} - Map Process Addresses to a Memory Object
@item @code{munmap} - Unmap Previously Mapped Addresses
@item @code{mprotect} - Change Memory Protection
@item @code{msync} - Memory Object Synchronization
@item @code{shm_open} - Open a Shared Memory Object
@item @code{shm_unlink} - Remove a Shared Memory Object
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the memory management manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection mlockall - Lock the Address Space of a Process

@findex mlockall
@cindex  lock the address space of a process

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mlockall(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection munlockall - Unlock the Address Space of a Process

@findex munlockall
@cindex  unlock the address space of a process

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int munlockall(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection mlock - Lock a Range of the Process Address Space

@findex mlock
@cindex  lock a range of the process address space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mlock(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection munlock - Unlock a Range of the Process Address Space

@findex munlock
@cindex  unlock a range of the process address space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int munlock(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection mmap - Map Process Addresses to a Memory Object

@findex mmap
@cindex  map process addresses to a memory object

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mmap(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection munmap - Unmap Previously Mapped Addresses

@findex munmap
@cindex  unmap previously mapped addresses

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int munmap(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection mprotect - Change Memory Protection

@findex mprotect
@cindex  change memory protection

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mprotect(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection msync - Memory Object Synchronization

@findex msync
@cindex  memory object synchronization

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int msync(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection shm_open - Open a Shared Memory Object

@findex shm_open
@cindex  open a shared memory object

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int shm_open(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection shm_unlink - Remove a Shared Memory Object

@findex shm_unlink
@cindex  remove a shared memory object

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int shm_unlink(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

