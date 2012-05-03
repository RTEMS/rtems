@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Language-Specific Services for the C Programming Language

@section Referenced C Language Routines

ANSI C Section 4.2 --- Diagnostics

@example
assert(), Function, Implemented
@end example

ANSI C Section 4.3 --- Character Handling

@example
isalnum(), Function, Implemented
isalpha(), Function, Implemented
iscntrl(), Function, Implemented
isdigit(), Function, Implemented
isgraph(), Function, Implemented
islower(), Function, Implemented
isprint(), Function, Implemented
ispunct(), Function, Implemented
isspace(), Function, Implemented
isupper(), Function, Implemented
isxdigit(), Function, Implemented
tolower(), Function, Implemented
toupper(), Function, Implemented
@end example

ANSI C Section 4.4 --- Localization

@example
setlocale(), Function, Implemented
@end example

ANSI C Section 4.5 --- Mathematics

@example
acos(), Function, Implemented
asin(), Function, Implemented
atan(), Function, Implemented
atan2(), Function, Implemented
cos(), Function, Implemented
sin(), Function, Implemented
tan(), Function, Implemented
cosh(), Function, Implemented
sinh(), Function, Implemented
tanh(), Function, Implemented
exp(), Function, Implemented
frexp(), Function, Implemented
ldexp(), Function, Implemented
log(), Function, Implemented
log10(), Function, Implemented
modf(), Function, Implemented
pow(), Function, Implemented
sqrt(), Function, Implemented
ceil(), Function, Implemented
fabs(), Function, Implemented
floor(), Function, Implemented
fmod(), Function, Implemented
@end example

ANSI C Section 4.6 --- Non-Local Jumps

@example
setjmp(), Function, Implemented
longjmp(), Function, Implemented
@end example

ANSI C Section 4.9 --- Input/Output

@example
FILE, Type, Implemented
clearerr(), Function, Implemented
fclose(), Function, Implemented
feof(), Function, Implemented
ferror(), Function, Implemented
fflush(), Function, Implemented
fgetc(), Function, Implemented
fgets(), Function, Implemented
fopen(), Function, Implemented
fputc(), Function, Implemented
fputs(), Function, Implemented
fread(), Function, Implemented
freopen(), Function, Implemented
fseek(), Function, Implemented
ftell(), Function, Implemented
fwrite(), Function, Implemented
getc(), Function, Implemented
getchar(), Function, Implemented
gets(), Function, Implemented
perror(), Function, Implemented
printf(), Function, Implemented
fprintf(), Function, Implemented
sprintf(), Function, Implemented
putc(), Function, Implemented
putchar(), Function, Implemented
puts(), Function, Implemented
remove(), Function, Implemented
rewind(), Function, Implemented
scanf(), Function, Implemented
fscanf(), Function, Implemented
sscanf(), Function, Implemented
setbuf(), Function, Implemented
tmpfile(), Function, Implemented
tmpnam(), Function, Implemented
ungetc(), Function, Implemented
@end example

NOTE: @code{rename} is also included in another section.  @ref{Rename a File}.

ANSI C Section 4.10 --- General Utilities

@example
abs(), Function, Implemented
atof(), Function, Implemented
atoi(), Function, Implemented
atol(), Function, Implemented
rand(), Function, Implemented
srand(), Function, Implemented
calloc(), Function, Implemented
free(), Function, Implemented
malloc(), Function, Implemented
realloc(), Function, Implemented
abort(), Function, Implemented
exit(), Function, Implemented
bsearch(), Function, Implemented
qsort(), Function, Implemented
@end example

NOTE: @code{getenv} is also included in another section.  
@ref{Environment Access}.

ANSI C Section 4.11 --- String Handling

@example
strcpy(), Function, Implemented
strncpy(), Function, Implemented
strcat(), Function, Implemented
strncat(), Function, Implemented
strcmp(), Function, Implemented
strncmp(), Function, Implemented
strchr(), Function, Implemented
strcspn(), Function, Implemented
strpbrk(), Function, Implemented
strrchr(), Function, Implemented
strspn(), Function, Implemented
strstr(), Function, Implemented
strtok(), Function, Implemented
strlen(), Function, Implemented
@end example

ANSI C Section 4.12 --- Date and Time Handling

@example
asctime(), Function, Implemented
ctime(), Function, Implemented
gmtime(), Function, Implemented
localtime(), Function, Implemented
mktime(), Function, Implemented
strftime(), Function, Implemented
@end example

NOTE: RTEMS has no notion of time zones.

NOTE: @code{time} is also included in another section.  
@ref{Get System Time}.

From Surrounding Text

@example
EXIT_SUCCESS, Constant, Implemented
EXIT_FAILURE, Constant, Implemented
@end example


@subsection Extensions to Time Functions

@subsection Extensions to setlocale Function

@example
LC_CTYPE, Constant, Implemented
LC_COLLATE, Constant, Implemented
LC_TIME, Constant, Implemented
LC_NUMERIC, Constant, Implemented
LC_MONETARY, Constant, Implemented
LC_ALL, Constant, Implemented
@end example

@section C Language Input/Output Functions

@subsection Map a Stream Pointer to a File Descriptor

@example
fileno(), Function, Implemented
STDIN_FILENO, Constant, Implemented
STDOUT_FILENO, Constant, Implemented
STDERR_FILENO, Constant, Implemented
@end example

@subsection Open a Stream on a File Descriptor

@example
fdopen(), Function, Implemented
@end example

@subsection Interactions of Other FILE-Type C Functions

@subsection Operations on Files - the remove Function

@subsection Temporary File Name - the tmpnam Function

@subsection Stdio Locking Functions

@example
flockfile(), Function, Unimplemented
ftrylockfile(), Function, Unimplemented
funlockfile(), Function, Unimplemented
@end example

@subsection Stdio With Explicit Client Locking

@example
getc_unlocked(), Function, Unimplemented
getchar_unlocked(), Function, Unimplemented
putc_unlocked(), Function, Unimplemented
putchar_unlocked(), Function, Unimplemented
@end example

@section Other C Language Functions

@subsection Nonlocal Jumps

@example
sigjmp_buf, Type, Implemented
sigsetjmp(), Function, Implemented
siglongjmp(), Function, Implemented
@end example

@subsection Set Time Zone

@example
tzset(), Function, Unimplemented
@end example

@subsection Find String Token

@example
strtok_r(), Function, Implemented
@end example

@subsection ASCII Time Representation

@example
asctime_r(), Function, Implemented
@end example

@subsection Current Time Representation

@example
ctime_r(), Function, Implemented
@end example

@subsection Coordinated Universal Time

@example
gmtime_r(), Function, Implemented
@end example

@subsection Local Time

@example
localtime_r(), Function, Implemented
@end example

@subsection Pseudo-Random Sequence Generation Functions

@example
rand_r(), Function, Implemented
@end example

