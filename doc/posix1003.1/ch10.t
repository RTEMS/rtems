@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Data Interchange Format

@section Archive/Interchange File Format

@subsection Extended tar Format

@example
tar format, Type, Unimplemented
@end example

NOTE: Requires <tar.h> which is not in newlib.

@subsection Extended cpio Format

@example
cpio format, Type, Unimplemented
@end example

NOTE: POSIX does not require a header file or structure.  RedHat Linux
5.0 does not have a <cpio.h> although Solaris 2.6 does.

@subsection Multiple Volumes

