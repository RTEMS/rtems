@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter System Databases Manager

@section Introduction

The 
system databases manager is ...

The directives provided by the system databases manager are:

@itemize @bullet
@item @code{getgrgid} - Get Group File Entry for ID
@item @code{getgrgid_r} - Reentrant Get Group File Entry
@item @code{getgrnam} - Get Group File Entry for Name
@item @code{getgrnam_r} - Reentrant Get Group File Entry for Name
@item @code{getpwuid} - Get Password File Entry for UID
@item @code{getpwuid_r} - Reentrant Get Password File Entry for UID
@item @code{getpwnam}  Get Password File Entry for Name
@item @code{getpwnam_r} - Reentrant Get Password File Entry for Name
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the system databases manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection getgrgid - Get Group File Entry for ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgrgid(
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

@page
@subsection getgrgid_r - Reentrant Get Group File Entry

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgrgid_r(
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

@page
@subsection getgrnam - Get Group File Entry for Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgrnam(
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

@page
@subsection getgrnam_r - Reentrant Get Group File Entry for Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgrnam_r(
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

@page
@subsection getpwuid - Get Password File Entry for UID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpwuid(
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

@page
@subsection getpwuid_r - Reentrant Get Password File Entry for UID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpwuid_r(
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

@page
@subsection getpwnam - Password File Entry for Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpwnam(
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

@page
@subsection getpwnam_r - Reentrant Get Password File Entry for Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpwnam_r(
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

