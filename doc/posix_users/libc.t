@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Services Provided by C Library (libc)

@section Introduction

This section lists the routines that provided by the Newlib C Library.

@section Standard Utility Functions (stdlib.h)

@itemize @bullet
@item @code{abort} - Abnormal termination of a program
@item @code{abs} - Integer absolute value (magnitude)
@item @code{assert} - Macro for Debugging Diagnostics
@item @code{atexit} - Request execution of functions at program exit
@item @code{atof} - String to double or float
@item @code{atoi} - String to integer
@item @code{bsearch} - Binary search
@item @code{calloc} - Allocate space for arrays
@item @code{div} - Divide two integers
@item @code{ecvtbuf} - Double or float to string of digits
@item @code{ecvt} - Double or float to string of digits (malloc result)
@item @code{__env_lock} - Lock environment list for getenv and setenv
@item @code{gvcvt} - Format double or float as string
@item @code{exit} - End program execution
@item @code{getenv} - Look up environment variable
@item @code{labs} - Long integer absolute value (magnitude)
@item @code{ldiv} - Divide two long integers
@item @code{malloc} - Allocate memory 
@item @code{realloc} - Reallocate memory 
@item @code{free} - Free previously allocated memory
@item @code{mallinfo} - Get information about allocated memory
@item @code{__malloc_lock} - Lock memory pool for malloc and free
@item @code{mbstowcs} - Minimal multibyte string to wide string converter
@item @code{mblen} - Minimal multibyte length
@item @code{mbtowc} - Minimal multibyte to wide character converter
@item @code{qsort} - Sort an array
@item @code{rand} - Pseudo-random numbers
@item @code{strtod} - String to double or float
@item @code{strtol} - String to long
@item @code{strtoul} - String to unsigned long
@item @code{system} - Execute command string
@item @code{wcstombs} - Minimal wide string to multibyte string converter
@item @code{wctomb} - Minimal wide character to multibyte converter
@end itemize

@section Character Type Macros and Functions (ctype.h)

@itemize @bullet
@item @code{isalnum} - Alphanumeric character predicate
@item @code{isalpha} - Alphabetic character predicate
@item @code{isascii} - ASCII character predicate
@item @code{iscntrl} - Control character predicate
@item @code{isdigit} - Decimal digit predicate
@item @code{islower} - Lower-case character predicate
@item @code{isprint} - Printable character predicates (isprint, isgraph)
@item @code{ispunct} - Punctuation character predicate
@item @code{isspace} - Whitespace character predicate
@item @code{isupper} - Uppercase character predicate
@item @code{isxdigit} - Hexadecimal digit predicate
@item @code{toascii} - Force integers to ASCII range
@item @code{tolower} - Translate characters to lower case
@item @code{toupper} - Translate characters to upper case
@end itemize

@section Input and Output (stdio.h)

@itemize @bullet
@item @code{clearerr} - Clear file or stream error indicator
@item @code{fclose} - Close a file
@item @code{feof} - Test for end of file
@item @code{ferror} - Test whether read/write error has occurred
@item @code{fflush} - Flush buffered file output
@item @code{fgetc} - Get a character from a file or stream
@item @code{fgetpos} - Record position in a stream or file
@item @code{fgets} - Get character string from a file or stream
@item @code{fiprintf} - Write formatted output to file (integer only)
@item @code{fopen} - Open a file
@item @code{fdopen} - Turn an open file into a stream
@item @code{fputc} - Write a character on a stream or file
@item @code{fputs} - Write a character string in a file or stream
@item @code{fread} - Read array elements from a file
@item @code{freopen} - Open a file using an existing file descriptor
@item @code{fseek} - Set file position
@item @code{fsetpos} - Restore position of a stream or file
@item @code{ftell} - Return position in a stream or file
@item @code{fwrite} - Write array elements from memory to a file or stream
@item @code{getc} - Get a character from a file or stream (macro)
@item @code{getchar} - Get a character from standard input (macro)
@item @code{gets} - Get character string from standard input (obsolete)
@item @code{iprintf} - Write formatted output (integer only)
@item @code{mktemp} - Generate unused file name
@item @code{perror} - Print an error message on standard error
@item @code{putc} - Write a character on a stream or file (macro)
@item @code{putchar} - Write a character on standard output (macro)
@item @code{puts} - Write a character string on standard output
@item @code{remove} - Delete a file's name
@item @code{rename} - Rename a file
@item @code{rewind} - Reinitialize a file or stream
@item @code{setbuf} - Specify full buffering for a file or stream
@item @code{setvbuf} - Specify buffering for a file or stream
@item @code{siprintf} - Write formatted output (integer only)
@item @code{printf} - Write formatted output
@item @code{scanf} - Scan and format input
@item @code{tmpfile} - Create a temporary file
@item @code{tmpnam} - Generate name for a temporary file
@item @code{vprintf} - Format variable argument list
@end itemize

@section Strings and Memory (string.h)

@itemize @bullet
@item @code{bcmp} - Compare two memory areas
@item @code{bcopy} - Copy memory regions
@item @code{bzero} - Initialize memory to zero
@item @code{index} - Search for character in string
@item @code{memchr} - Find character in memory
@item @code{memcmp} - Compare two memory areas
@item @code{memcpy} - Copy memory regions
@item @code{memmove} - Move possibly overlapping memory
@item @code{memset} - Set an area of memory
@item @code{rindex} - Reverse search for character in string
@item @code{strcasecmp} - Compare strings ignoring case
@item @code{strcat} - Concatenate strings
@item @code{strchr} - Search for character in string
@item @code{strcmp} - Character string compare
@item @code{strcoll} - Locale specific character string compare
@item @code{strcpy} - Copy string
@item @code{strcspn} - Count chars not in string
@item @code{strerror} - Convert error number to string
@item @code{strlen} - Character string length
@item @code{strlwr} - Convert string to lower case
@item @code{strncasecmp} - Compare strings ignoring case
@item @code{strncat} - Concatenate strings
@item @code{strncmp} - Character string compare
@item @code{strncpy} - Counted copy string
@item @code{strpbrk} - Find chars in string
@item @code{strrchr} - Reverse search for character in string
@item @code{strspn} - Find initial match
@item @code{strstr} - Find string segment
@item @code{strtok} - Get next token from a string
@item @code{strupr} - Convert string to upper case
@item @code{strxfrm} - Transform string
@end itemize

@section Signal Handling (signal.h)

@itemize @bullet
@item @code{raise} - Send a signal
@item @code{signal} - Specify handler subroutine for a signal
@end itemize

@section Time Functions (time.h)

@itemize @bullet
@item @code{asctime} - Format time as string
@item @code{clock} - Cumulative processor time
@item @code{ctime} - Convert time to local and format as string
@item @code{difftime} - Subtract two times
@item @code{gmtime} - Convert time to UTC (GMT) traditional representation
@item @code{localtime} - Convert time to local representation
@item @code{mktime} - Convert time to arithmetic representation
@item @code{strftime} - Flexible calendar time formatter
@item @code{time} - Get current calendar time (as single number)
@end itemize

@section Locale (locale.h)

@itemize @bullet
@item @code{setlocale} - Select or query locale
@end itemize

@section Reentrant Versions of Functions


@itemize @bullet
@item Equivalent for errno variable:

@itemize @bullet
@item @code{errno_r} - XXX
@end itemize

@item Locale functions:

@itemize @bullet
@item @code{localeconv_r} - XXX
@item @code{setlocale_r} - XXX
@end itemize

@item Equivalents for stdio variables:

@itemize @bullet
@item @code{stdin_r} - XXX
@item @code{stdout_r} - XXX
@item @code{stderr_r} - XXX
@end itemize

@item Stdio functions:

@itemize @bullet
@item @code{fdopen_r} - XXX
@item @code{perror_r} - XXX
@item @code{tempnam_r} - XXX
@item @code{fopen_r} - XXX
@item @code{putchar_r} - XXX
@item @code{tmpnam_r} - XXX
@item @code{getchar_r} - XXX
@item @code{puts_r} - XXX
@item @code{tmpfile_r} - XXX
@item @code{gets_r} - XXX
@item @code{remove_r} - XXX
@item @code{vfprintf_r} - XXX
@item @code{iprintf_r} - XXX
@item @code{rename_r} - XXX
@item @code{vsnprintf_r} - XXX
@item @code{mkstemp_r} - XXX
@item @code{snprintf_r} - XXX
@item @code{vsprintf_r} - XXX
@item @code{mktemp_t} - XXX
@item @code{sprintf_r} - XXX
@end itemize

@item Signal functions:

@itemize @bullet
@item @code{init_signal_r} - XXX
@item @code{signal_r} - XXX
@item @code{kill_r} - XXX
@item @code{_sigtramp_r} - XXX
@item @code{raise_r} - XXX
@end itemize

@item Stdlib functions:

@itemize @bullet
@item @code{calloc_r} - XXX
@item @code{mblen_r} - XXX
@item @code{srand_r} - XXX
@item @code{dtoa_r} - XXX
@item @code{mbstowcs_r} - XXX
@item @code{strtod_r} - XXX
@item @code{free_r} - XXX
@item @code{mbtowc_r} - XXX
@item @code{strtol_r} - XXX
@item @code{getenv_r} - XXX
@item @code{memalign_r} - XXX
@item @code{strtoul_r} - XXX
@item @code{mallinfo_r} - XXX
@item @code{mstats_r} - XXX
@item @code{system_r} - XXX
@item @code{malloc_r} - XXX
@item @code{rand_r} - XXX
@item @code{wcstombs_r} - XXX
@item @code{malloc_r} - XXX
@item @code{realloc_r} - XXX
@item @code{wctomb_r} - XXX
@item @code{malloc_stats_r} - XXX
@item @code{setenv_r} - XXX
@end itemize

@item String functions:

@itemize @bullet
@item @code{strtok_r} - XXX
@end itemize

@item System functions:

@itemize @bullet
@item @code{close_r} - XXX
@item @code{link_r} - XXX
@item @code{unlink_r} - XXX
@item @code{execve_r} - XXX
@item @code{lseek_r} - XXX
@item @code{wait_r} - XXX
@item @code{fcntl_r} - XXX
@item @code{open_r} - XXX
@item @code{write_r} - XXX
@item @code{fork_r} - XXX
@item @code{read_r} - XXX
@item @code{fstat_r} - XXX
@item @code{sbrk_r} - XXX
@item @code{gettimeofday_r} - XXX
@item @code{stat_r} - XXX
@item @code{getpid_r} - XXX
@item @code{times_r} - XXX
@end itemize

@item Time function:

@itemize @bullet
@item @code{asctime_r} - XXX
@end itemize

@end itemize

@section Miscellaneous Macros and Functions


@itemize @bullet
@item @code{unctrl} - Return printable representation of a character
@end itemize

@section Variable Argument Lists


@itemize @bullet

@item Stdarg (stdarg.h):
@itemize @bullet
@item @code{va_start} - XXX
@item @code{va_arg} - XXX
@item @code{va_end} - XXX
@end itemize

@item Vararg (varargs.h):
@itemize @bullet
@item @code{va_alist} - XXX
@item @code{va_start-trad} - XXX
@item @code{va_arg-trad} - XXX
@item @code{va_end-trad} - XXX
@end itemize
@end itemize

@section Reentrant System Calls

@itemize @bullet
@item @code{open_r} - XXX
@item @code{close_r} - XXX
@item @code{lseek_r} - XXX
@item @code{read_r} - XXX
@item @code{write_r} - XXX
@item @code{fork_r} - XXX
@item @code{wait_r} - XXX
@item @code{stat_r} - XXX
@item @code{fstat_r} - XXX
@item @code{link_r} - XXX
@item @code{unlink_r} - XXX
@item @code{sbrk_r} - XXX
@end itemize


