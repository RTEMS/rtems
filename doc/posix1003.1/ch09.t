@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

NOTE: Creates /etc/group if none exists. 

@subsection User Database Access

@example
struct passwd, Type, Implemented
getpwuid(), Function, Implemented
getpwuid_r(), Function, Implemented
getpwnam(), Function, Implemented
getpwnam_r(), Function, Implemented
@end example

NOTE: Creates /etc/passwd if none exists. 

