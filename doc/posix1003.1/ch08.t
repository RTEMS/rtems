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
assert()
@end example

ANSI C Section 4.3 --- Character Handling

@example
isalnum()
isalpha()
iscntrl()
isdigit()
isgraph()
islower()
isprint()
ispunct()
isspace()
isupper()
isxdigit()
tolower()
toupper()
@end example

ANSI C Section 4.4 --- Localization

@example
setlocale()
@end example

ANSI C Section 4.5 --- Mathematics

@example
acos()
asin()
atan()
atan2()
cos()
sin()
tan()
cosh()
sinh()
tanh()
exp()
frexp()
ldexp()
log()
log10()
modf()
pow()
sqrt()
ceil()
fabs()
floor()
fmod()
@end example

ANSI C Section 4.6 --- Non-Local Jumps

@example
setjmp()
longjmp()
@end example

ANSI C Section 4.9 --- Input/Output

@example
clearerr()
fclose()
feof()
ferror()
fflush()
fgetc()
fgets()
fopen()
fputc()
fputs()
fread()
freopen()
fseek()
ftell()
fwrite()
getc()
getchar()
gets()
perror()
printf()
fprintf()
sprintf()
putc()
putchar()
puts()
remove()
rename()
rewind()
scanf()
fscanf()
sscanf()
setbuf()
tmpfile()
tmpnam()
ungetc()
@end example

ANSI C Section 4.10 --- General Utilities

@example
abs()
atof()
atoi()
atol()
rand()
srand()
calloc()
free()
malloc()
realloc()
abort()
exit()
getenv()
bsearch()
qsort()
@end example

ANSI C Section 4.11 --- String Handling

@example
strcpy()
strncpy()
strcat()
strncat()
strcmp()
strncmp()
strchr()
strcspn()
strpbrk()
strrchr()
strspn()
strstr()
strtok()
strlen()
@end example

ANSI C Section 4.12 --- Date and Time Handling

@example
time()
asctime()
ctime()
gmtime()
localtime()
mktime()
strftime()
@end example

@subsection Extensions to Time Functions

@subsection Extensions to @code{setlocale} Function

@section C Language Input/Output Functions

@subsection Map a Stream Pointer to a File Descriptor

@example
fileno()
@end example

@subsection Open a Stream on a File Descriptor

@example
fdopen()
@end example

@subsection Interactions of Other FILE-Type C Functions

@subsection Operations on Files -- the @code{remove} Function

@subsection Temporary File Name -- the @code{tmpnam} Function

@subsection Stdio Locking Functions

@example
flockfile()
ftrylockfile()
funlockfile()
@end example

@subsection Stdio With Explicit Client Locking

@example
getc_unlocked()
getchar_unlocked()
putc_unlocked()
putchar_unlocked()
@end example

@section Other C Language Functions

@subsection Nonlocal Jumps

@example
sigsetjmp()
siglongjmp()
@end example

@subsection Set Time Zone

@example
tzset()
@end example

@subsection Find String Token

@example
strtok_r()
@end example

@subsection ASCII Time Representation

@example
asctime_r()
@end example

@subsection Current Time Representation

@example
ctime_r()
@end example

@subsection Coordinated Universal Time

@example
gmtime_r()
@end example

@subsection Local Time

@example
localtime_r()
@end example

@subsection Pseudo-Random Sequence Generation Functions

@example
rand_r()
@end example


