@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Language-Specific Services for the C Programming Language

@section Referenced C Language Routines

ANSI C Section 4.2 --- Diagnostics

@example
assert(), Function
@end example

ANSI C Section 4.3 --- Character Handling

@example
isalnum(), Function
isalpha(), Function
iscntrl(), Function
isdigit(), Function
isgraph(), Function
islower(), Function
isprint(), Function
ispunct(), Function
isspace(), Function
isupper(), Function
isxdigit(), Function
tolower(), Function
toupper(), Function
@end example

ANSI C Section 4.4 --- Localization

@example
setlocale(), Function
@end example

ANSI C Section 4.5 --- Mathematics

@example
acos(), Function
asin(), Function
atan(), Function
atan2(), Function
cos(), Function
sin(), Function
tan(), Function
cosh(), Function
sinh(), Function
tanh(), Function
exp(), Function
frexp(), Function
ldexp(), Function
log(), Function
log10(), Function
modf(), Function
pow(), Function
sqrt(), Function
ceil(), Function
fabs(), Function
floor(), Function
fmod(), Function
@end example

ANSI C Section 4.6 --- Non-Local Jumps

@example
setjmp(), Function
longjmp(), Function
@end example

ANSI C Section 4.9 --- Input/Output

@example
clearerr(), Function
fclose(), Function
feof(), Function
ferror(), Function
fflush(), Function
fgetc(), Function
fgets(), Function
fopen(), Function
fputc(), Function
fputs(), Function
fread(), Function
freopen(), Function
fseek(), Function
ftell(), Function
fwrite(), Function
getc(), Function
getchar(), Function
gets(), Function
perror(), Function
printf(), Function
fprintf(), Function
sprintf(), Function
putc(), Function
putchar(), Function
puts(), Function
remove(), Function
rename(), Function
rewind(), Function
scanf(), Function
fscanf(), Function
sscanf(), Function
setbuf(), Function
tmpfile(), Function
tmpnam(), Function
ungetc(), Function
@end example

ANSI C Section 4.10 --- General Utilities

@example
abs(), Function
atof(), Function
atoi(), Function
atol(), Function
rand(), Function
srand(), Function
calloc(), Function
free(), Function
malloc(), Function
realloc(), Function
abort(), Function
exit(), Function
getenv(), Function
bsearch(), Function
qsort(), Function
@end example

ANSI C Section 4.11 --- String Handling

@example
strcpy(), Function
strncpy(), Function
strcat(), Function
strncat(), Function
strcmp(), Function
strncmp(), Function
strchr(), Function
strcspn(), Function
strpbrk(), Function
strrchr(), Function
strspn(), Function
strstr(), Function
strtok(), Function
strlen(), Function
@end example

ANSI C Section 4.12 --- Date and Time Handling

@example
time(), Function
asctime(), Function
ctime(), Function
gmtime(), Function
localtime(), Function
mktime(), Function
strftime(), Function
@end example

@subsection Extensions to Time Functions

@subsection Extensions to @code{setlocale} Function

@section C Language Input/Output Functions

@subsection Map a Stream Pointer to a File Descriptor

@example
fileno(), Function
@end example

@subsection Open a Stream on a File Descriptor

@example
fdopen(), Function
@end example

@subsection Interactions of Other FILE-Type C Functions

@subsection Operations on Files -- the @code{remove} Function

@subsection Temporary File Name -- the @code{tmpnam} Function

@subsection Stdio Locking Functions

@example
flockfile(), Function
ftrylockfile(), Function
funlockfile(), Function
@end example

@subsection Stdio With Explicit Client Locking

@example
getc_unlocked(), Function
getchar_unlocked(), Function
putc_unlocked(), Function
putchar_unlocked(), Function
@end example

@section Other C Language Functions

@subsection Nonlocal Jumps

@example
sigsetjmp(), Function
siglongjmp(), Function
@end example

@subsection Set Time Zone

@example
tzset(), Function
@end example

@subsection Find String Token

@example
strtok_r(), Function
@end example

@subsection ASCII Time Representation

@example
asctime_r(), Function
@end example

@subsection Current Time Representation

@example
ctime_r(), Function
@end example

@subsection Coordinated Universal Time

@example
gmtime_r(), Function
@end example

@subsection Local Time

@example
localtime_r(), Function
@end example

@subsection Pseudo-Random Sequence Generation Functions

@example
rand_r(), Function
@end example


