@c
@c COPYRIGHT (c) 1988-1999.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.
@c
@c $Id$
@c

@chapter Services Provided by C Library (libc)

@section Introduction

This section lists the routines that provided by the Newlib C Library.

@section Standard Utility Functions (stdlib.h)

@itemize @bullet
@item abort - Abnormal termination of a program
@item abs - Integer absolute value (magnitude)
@item assert - Macro for Debugging Diagnostics
@item atexit - Request execution of functions at program exit
@item atof - String to double or float
@item atoi - String to integer
@item bsearch - Binary search
@item calloc - Allocate space for arrays
@item div - Divide two integers
@item ecvtbuf - Double or float to string of digits
@item ecvt - Double or float to string of digits (malloc result)
@item __env_lock - Lock environment list for getenv and setenv
@item gvcvt - Format double or float as string
@item exit - End program execution
@item getenv - Look up environment variable
@item labs - Long integer absolute value (magnitude)
@item ldiv - Divide two long integers
@item malloc - Allocate and manage memory (malloc, realloc, free)
@item mallinfo - Get information about allocated memory
@item __malloc_lock - Lock memory pool for malloc and free
@item mbstowcs - Minimal multibyte string to wide string converter
@item mblen - Minimal multibyte length
@item mbtowc - Minimal multibyte to wide character converter
@item qsort - Sort an array
@item rand - Pseudo-random numbers
@item strtod - String to double or float
@item strtol - String to long
@item strtoul - String to unsigned long
@item system - Execute command string
@item wcstombs - Minimal wide string to multibyte string converter
@item wctomb - Minimal wide character to multibyte converter
@end itemize

@section Character Type Macros and Functions (ctype.h)

@itemize @bullet
@item isalnum - Alphanumeric character predicate
@item isalpha - Alphabetic character predicate
@item isascii - ASCII character predicate
@item iscntrl - Control character predicate
@item isdigit - Decimal digit predicate
@item islower - Lower-case character predicate
@item isprint - Printable character predicates (isprint, isgraph)
@item ispunct - Punctuation character predicate
@item isspace - Whitespace character predicate
@item isupper - Uppercase character predicate
@item isxdigit - Hexadecimal digit predicate
@item toascii - Force integers to ASCII range
@item tolower - Translate characters to lower case
@item toupper - Translate characters to upper case
@end itemize

@section Input and Output (stdio.h)

@itemize @bullet
@item clearerr - Clear file or stream error indicator
@item fclose - Close a file
@item feof - Test for end of file
@item ferror - Test whether read/write error has occurred
@item fflush - Flush buffered file output
@item fgetc - Get a character from a file or stream
@item fgetpos - Record position in a stream or file
@item fgets - Get character string from a file or stream
@item fiprintf - Write formatted output to file (integer only)
@item fopen - Open a file
@item fdopen - Turn an open file into a stream
@item fputc - Write a character on a stream or file
@item fputs - Write a character string in a file or stream
@item fread - Read array elements from a file
@item freopen - Open a file using an existing file descriptor
@item fseek - Set file position
@item fsetpos - Restore position of a stream or file
@item ftell - Return position in a stream or file
@item fwrite - Write array elements from memory to a file or stream
@item getc - Get a character from a file or stream (macro)
@item getchar - Get a character from standard input (macro)
@item gets - Get character string from standard input (obsolete)
@item iprintf - Write formatted output (integer only)
@item mktemp - Generate unused file name
@item perror - Print an error message on standard error
@item putc - Write a character on a stream or file (macro)
@item putchar - Write a character on standard output (macro)
@item puts - Write a character string on standard output
@item remove - Delete a file's name
@item rename - Rename a file
@item rewind - Reinitialize a file or stream
@item setbuf - Specify full buffering for a file or stream
@item setvbuf - Specify buffering for a file or stream
@item siprintf - Write formatted output (integer only)
@item printf - Write formatted output
@item scanf - Scan and format input
@item tmpfile - Create a temporary file
@item tmpnam - Generate name for a temporary file
@item vprintf - Format variable argument list
@end itemize

@section Strings and Memory (string.h)

@itemize @bullet
@item bcmp - Compare two memory areas
@item bcopy - Copy memory regions
@item bzero - Initialize memory to zero
@item index - Search for character in string
@item memchr - Find character in memory
@item memcmp - Compare two memory areas
@item memcpy - Copy memory regions
@item memmove - Move possibly overlapping memory
@item memset - Set an area of memory
@item rindex - Reverse search for character in string
@item strcasecmp - Compare strings ignoring case
@item strcat - Concatenate strings
@item strchr - Search for character in string
@item strcmp - Character string compare
@item strcoll - Locale specific character string compare
@item strcpy - Copy string
@item strcspn - Count chars not in string
@item strerror - Convert error number to string
@item strlen - Character string length
@item strlwr - Convert string to lower case
@item strncasecmp - Compare strings ignoring case
@item strncat - Concatenate strings
@item strncmp - Character string compare
@item strncpy - Counted copy string
@item strpbrk - Find chars in string
@item strrchr - Reverse search for character in string
@item strspn - Find initial match
@item strstr - Find string segment
@item strtok - Get next token from a string
@item strupr - Convert string to upper case
@item strxfrm - Transform string
@end itemize

@section Signal Handling (signal.h)

@itemize @bullet
@item raise - Send a signal
@item signal - Specify handler subroutine for a signal
@end itemize

@section Time Functions (time.h)

@itemize @bullet
@item asctime - Format time as string
@item clock - Cumulative processor time
@item ctime - Convert time to local and format as string
@item difftime - Subtract two times
@item gmtime - Convert time to UTC (GMT) traditional representation
@item localtime - Convert time to local representation
@item mktime - Convert time to arithmetic representation
@item strftime - Flexible calendar time formatter
@item time - Get current calendar time (as single number)
@end itemize

@section Locale (locale.h)

@itemize @bullet
@item setlocale - Select or query locale
@end itemize

@section Reentrant Versions of Functions


@itemize @bullet
@item Equivalent for errno variable:

@itemize @bullet
@item errno_r - XXX
@end itemize

@item Locale functions:

@itemize @bullet
@item localeconv_r - XXX
@item setlocale_r - XXX
@end itemize

@item Equivalents for stdio variables:

@itemize @bullet
@item stdin_r - XXX
@item stdout_r - XXX
@item stderr_r - XXX
@end itemize

@item Stdio functions:

@itemize @bullet
@item fdopen_r - XXX
@item perror_r - XXX
@item tempnam_r - XXX
@item fopen_r - XXX
@item putchar_r - XXX
@item tmpnam_r - XXX
@item getchar_r - XXX
@item puts_r - XXX
@item tmpfile_r - XXX
@item gets_r - XXX
@item remove_r - XXX
@item vfprintf_r - XXX
@item iprintf_r - XXX
@item rename_r - XXX
@item vsnprintf_r - XXX
@item mkstemp_r - XXX
@item snprintf_r - XXX
@item vsprintf_r - XXX
@item mktemp_t - XXX
@item sprintf_r - XXX
@end itemize

@item Signal functions:

@itemize @bullet
@item init_signal_r - XXX
@item signal_r - XXX
@item kill_r - XXX
@item _sigtramp_r - XXX
@item raise_r - XXX
@end itemize

@item Stdlib functions:

@itemize @bullet
@item calloc_r - XXX
@item mblen_r - XXX
@item srand_r - XXX
@item dtoa_r - XXX
@item mbstowcs_r - XXX
@item strtod_r - XXX
@item free_r - XXX
@item mbtowc_r - XXX
@item strtol_r - XXX
@item getenv_r - XXX
@item memalign_r - XXX
@item strtoul_r - XXX
@item mallinfo_r - XXX
@item mstats_r - XXX
@item system_r - XXX
@item malloc_r - XXX
@item rand_r - XXX
@item wcstombs_r - XXX
@item malloc_r - XXX
@item realloc_r - XXX
@item wctomb_r - XXX
@item malloc_stats_r - XXX
@item setenv_r - XXX
@end itemize

@item String functions:

@itemize @bullet
@item strtok_r - XXX
@end itemize

@item System functions:

@itemize @bullet
@item close_r - XXX
@item link_r - XXX
@item unlink_r - XXX
@item execve_r - XXX
@item lseek_r - XXX
@item wait_r - XXX
@item fcntl_r - XXX
@item open_r - XXX
@item write_r - XXX
@item fork_r - XXX
@item read_r - XXX
@item fstat_r - XXX
@item sbrk_r - XXX
@item gettimeofday_r - XXX
@item stat_r - XXX
@item getpid_r - XXX
@item times_r - XXX
@end itemize

@item Time function:

@itemize @bullet
@item asctime_r - XXX
@end itemize

@end itemize

@section Miscellaneous Macros and Functions


@itemize @bullet
@item unctrl - Return printable representation of a character
@end itemize

@section Variable Argument Lists


@itemize @bullet

@item Stdarg (stdarg.h):
@itemize @bullet
@item va_start - XXX
@item va_arg - XXX
@item va_end - XXX
@end itemize

@item Vararg (varargs.h):
@itemize @bullet
@item va_alist - XXX
@item va_start-trad - XXX
@item va_arg-trad - XXX
@item va_end-trad - XXX
@end itemize
@end itemize

@section Reentrant System Calls

@itemize @bullet
@item open_r - XXX
@item close_r - XXX
@item lseek_r - XXX
@item read_r - XXX
@item write_r - XXX
@item fork_r - XXX
@item wait_r - XXX
@item stat_r - XXX
@item fstat_r - XXX
@item link_r - XXX
@item unlink_r - XXX
@item sbrk_r - XXX
@end itemize


