@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Language-Specific Services for the C Programming Language Manager

@section Introduction

The 
language-specific services for the C programming language manager is ...

The directives provided by the language-specific services for the C programming language manager are:

@itemize @bullet
@item @code{setlocale} - Set the Current Locale
@item @code{fileno} - Obtain File Descriptor Number for this File
@item @code{fdopen} - Associate Stream with File Descriptor
@item @code{flockfile} - Acquire Ownership of File Stream
@item @code{ftrylockfile} - Poll to Acquire Ownership of File Stream
@item @code{funlockfile} - Release Ownership of File Stream
@item @code{getc_unlocked} - Get Character without Locking
@item @code{getchar_unlocked} - Get Character from stdin without Locking
@item @code{putc_unlocked} - Put Character without Locking
@item @code{putchar_unlocked} -  Put Character to stdin without Locking
@item @code{setjmp} - Save Context for Non-Local Goto
@item @code{longjmp} - Non-Local Jump to a Saved Context
@item @code{sigsetjmp} -  Save Context with Signal Status for Non-Local Goto
@item @code{siglongjmp} - Non-Local Jump with Signal Status to a Saved Context
@item @code{tzset} - Initialize Time Conversion Information
@item @code{strtok_r} - Reentrant Extract Token from String
@item @code{asctime_r} - Reentrant struct tm to ASCII Time Conversion
@item @code{ctime_r} - Reentrant time_t to ASCII Time Conversion
@item @code{gmtime_r} - Reentrant UTC Time Conversion
@item @code{localtime_r} - Reentrant Local Time Conversion
@item @code{rand_r} - Reentrant Random Number Generation
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the language-specific services for the C programming language manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection setlocale - Set the Current Locale

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setlocale(
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
@subsection fileno - Obtain File Descriptor Number for this File

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fileno(
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
@subsection fdopen - Associate Stream with File Descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fdopen(
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
@subsection flockfile - Acquire Ownership of File Stream

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int flcokfile(
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
@subsection ftrylockfile - Poll to Acquire Ownership of File Stream

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ftrylockfile(
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
@subsection funlockfile - Release Ownership of File Stream

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int funlockfile(
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
@subsection getc_unlocked - Get Character without Locking

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getc_unlocked(
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
@subsection getchar_unlocked - Get Character from stdin without Locking

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getchar_unlocked(
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
@subsection putc_unlocked - Put Character without Locking

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int putc_unlocked(
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
@subsection putchar_unlocked - Put Character to stdin without Locking

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int putchar_unlocked(
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
@subsection setjmp - Save Context for Non-Local Goto

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setjmp(
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
@subsection longjmp - Non-Local Jump to a Saved Context

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int longjmp(
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
@subsection sigsetjmp - Save Context with Signal Status for Non-Local Goto

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sigsetjmp(
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
@subsection siglongjmp - Non-Local Jump with Signal Status to a Saved Context

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int siglongjmp(
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
@subsection tzset - Initialize Time Conversion Information

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tzset(
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
@subsection strtok_r - Reentrant Extract Token from String

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int strtok_r(
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
@subsection asctime_r - Reentrant struct tm to ASCII Time Conversion

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int asctime_r(
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
@subsection ctime_r - Reentrant time_t to ASCII Time Conversion

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ctime_r(
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
@subsection gmtime_r - Reentrant UTC Time Conversion

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int gmtime_r(
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
@subsection localtime_r - Reentrant Local Time Conversion

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int localtime_r(
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
@subsection rand_r - Reentrant Random Number Generation

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rand_r(
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

