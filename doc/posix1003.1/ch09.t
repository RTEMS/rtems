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
struct group, Type, Implemented
getgrgid(), Function, Implemented
getgrgid_r(), Function, Implemented
getgrname(), Function, Implemented
getgrnam_r(), Function, Implemented
@end example

@subsection User Database Access

@example
struct passwd, Type, Implemented
getpwuid(), Function, Implemented, assumes /etc/passwd exists
getpwuid_r(), Function, Implemented
getpwnam(), Function, Implemented, assumes /etc/passwd exists
getpwnam_r(), Function, Implemented
@end example

