@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter System Databases

@section System Databases Section

@section Database Access

@subsection Group Database Access

@example
struct group, Type, Untested Implementation
getgrgid(), Function, Unimplemented
getgrgid_r(), Function, Unimplemented
getgrname(), Function, Unimplemented
getgrnam_r(), Function, Unimplemented
@end example

@subsection User Database Access

@example
struct passwd, Type, Untested Implementation
getpwuid(), Function, Untested Implemented, assumes /etc/passwd exists
getpwuid_r(), Function, Unimplemented
getpwnam(), Function, Untested Implemented, assumes /etc/passwd exists
getpwnam_r(), Function, Unimplemented
@end example

