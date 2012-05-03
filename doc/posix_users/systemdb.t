@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

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
@item @code{getpwnam} - Get Password File Entry for Name
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

@c
@c
@c
@page
@subsection getgrgid - Get Group File Entry for ID

@findex getgrgid
@cindex  get group file entry for id

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

@c
@c
@c
@page
@subsection getgrgid_r - Reentrant Get Group File Entry

@findex getgrgid_r
@cindex  reentrant get group file entry

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

@c
@c
@c
@page
@subsection getgrnam - Get Group File Entry for Name

@findex getgrnam
@cindex  get group file entry for name

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

@c
@c
@c
@page
@subsection getgrnam_r - Reentrant Get Group File Entry for Name

@findex getgrnam_r
@cindex  reentrant get group file entry for name

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

@c
@c
@c
@page
@subsection getpwuid - Get Password File Entry for UID

@findex getpwuid
@cindex  get password file entry for uid

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

@c
@c
@c
@page
@subsection getpwuid_r - Reentrant Get Password File Entry for UID

@findex getpwuid_r
@cindex  reentrant get password file entry for uid

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

@c
@c
@c
@page
@subsection getpwnam - Password File Entry for Name

@findex getpwnam
@cindex  password file entry for name

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

@c
@c
@c
@page
@subsection getpwnam_r - Reentrant Get Password File Entry for Name

@findex getpwnam_r
@cindex  reentrant get password file entry for name

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

