@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Process Dump Control Manager

@section Introduction

The 
process dump control manager is ...

The directives provided by the process dump control manager are:

@itemize @bullet
@item @code{dump_setpath} - Dump File Control 
@end itemize

@section Background

@section Operations

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
The length of the argument exceeds {PATH_MAX} or a pathname
component is longer than {NAME_MAX} while {_POSIX_NO_TRUNC}
is in effect.
@item ENOENT
The path argument points to an empty string.
@item ENOSYS
The function dump_setpath() is not suppo9rted by this implementation.
@item ENOTDIR
A component of the path prefix is not a directory.
@item EROFS
The directory entry specified resides on a read-only file system.

@end table

@subheading DESCRIPTION:

The @code{dump_setpath} function defines the pathname where process
dumps are written.

@subheading NOTES:

