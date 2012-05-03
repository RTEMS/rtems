@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

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
@item @code{putchar_unlocked} - Put Character to stdin without Locking
@item @code{setjmp} - Save Context for Non-Local Goto
@item @code{longjmp} - Non-Local Jump to a Saved Context
@item @code{sigsetjmp} - Save Context with Signal Status for Non-Local Goto
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

@c
@c
@c
@page
@subsection setlocale - Set the Current Locale

@findex setlocale
@cindex  set the current locale

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

@c
@c
@c
@page
@subsection fileno - Obtain File Descriptor Number for this File

@findex fileno
@cindex  obtain file descriptor number for this file

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

@c
@c
@c
@page
@subsection fdopen - Associate Stream with File Descriptor

@findex fdopen
@cindex  associate stream with file descriptor

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

@c
@c
@c
@page
@subsection flockfile - Acquire Ownership of File Stream

@findex flockfile
@cindex  acquire ownership of file stream

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int flockfile(
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
@subsection ftrylockfile - Poll to Acquire Ownership of File Stream

@findex ftrylockfile
@cindex  poll to acquire ownership of file stream

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

@c
@c
@c
@page
@subsection funlockfile - Release Ownership of File Stream

@findex funlockfile
@cindex  release ownership of file stream

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

@c
@c
@c
@page
@subsection getc_unlocked - Get Character without Locking

@findex getc_unlocked
@cindex  get character without locking

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

@c
@c
@c
@page
@subsection getchar_unlocked - Get Character from stdin without Locking

@findex getchar_unlocked
@cindex  get character from stdin without locking

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

@c
@c
@c
@page
@subsection putc_unlocked - Put Character without Locking

@findex putc_unlocked
@cindex  put character without locking

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

@c
@c
@c
@page
@subsection putchar_unlocked - Put Character to stdin without Locking

@findex putchar_unlocked
@cindex  put character to stdin without locking

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

@c
@c
@c
@page
@subsection setjmp - Save Context for Non-Local Goto

@findex setjmp
@cindex  save context for non

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

@c
@c
@c
@page
@subsection longjmp - Non-Local Jump to a Saved Context

@findex longjmp
@cindex  non

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

@c
@c
@c
@page
@subsection sigsetjmp - Save Context with Signal Status for Non-Local Goto

@findex sigsetjmp
@cindex  save context with signal status for non

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

@c
@c
@c
@page
@subsection siglongjmp - Non-Local Jump with Signal Status to a Saved Context

@findex siglongjmp
@cindex  non

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

@c
@c
@c
@page
@subsection tzset - Initialize Time Conversion Information

@findex tzset
@cindex  initialize time conversion information

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

@c
@c
@c
@page
@subsection strtok_r - Reentrant Extract Token from String

@findex strtok_r
@cindex  reentrant extract token from string

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

@c
@c
@c
@page
@subsection asctime_r - Reentrant struct tm to ASCII Time Conversion

@findex asctime_r
@cindex  reentrant struct tm to ascii time conversion

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

@c
@c
@c
@page
@subsection ctime_r - Reentrant time_t to ASCII Time Conversion

@findex ctime_r
@cindex  reentrant time_t to ascii time conversion

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

@c
@c
@c
@page
@subsection gmtime_r - Reentrant UTC Time Conversion

@findex gmtime_r
@cindex  reentrant utc time conversion

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

@c
@c
@c
@page
@subsection localtime_r - Reentrant Local Time Conversion

@findex localtime_r
@cindex  reentrant local time conversion

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

@c
@c
@c
@page
@subsection rand_r - Reentrant Random Number Generation

@findex rand_r
@cindex  reentrant random number generation

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

