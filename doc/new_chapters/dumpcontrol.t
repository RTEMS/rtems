@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Process Dump Control Manager

@section Introduction

The process dump control manager provides a portable
interface for changing the path to which a process dump
is written.  The capabilities in this manager were defined in
the POSIX 1003.1h/D3 proposed standard titled @b{Services for Reliable,
Available, and Serviceable Systems}.

The directives provided by the process dump control manager are:

@itemize @bullet
@item @code{dump_setpath} - Dump File Control 
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the process dump control manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection dump_setpath - Dump File Control

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <dump.h>

int dump_setpath(
  const char      *path
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACESS
Search permission is denied for a component of the path prefix,
or write permission is denied on the directory containing the 
file.

@item ENAMETOOLONG
The length of the argument exceeds @code{PATH_MAX} or a pathname
component is longer than @code{NAME_MAX} while @code{_POSIX_NO_TRUNC}
is in effect.

@item ENOENT
The path argument points to an empty string.

@item ENOTDIR
A component of the path prefix is not a directory.

@item EROFS
The directory entry specified resides on a read-only file system.

@end table

@subheading DESCRIPTION:

The @code{dump_setpath()} function defines the pathname where process
dumps are written.  The pathname pointed to by @code{path} defines 
where a process dump file is written if the calling process 
terminates with a dump file.  The @code{path} argument does not 
name a directory.

If the @code{path} argument is NULL, the system does not write a
process dump file if the calling process terminates with a dump 
file.  If the @code{dump_setpath} function fails, the pathname for
writing process dumps does not change.

@subheading NOTES:

The @code{_POSIX_DUMP} feature flag is defined to indicate
this service is available.
