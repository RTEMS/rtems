@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Thread Management

@section Threads

@section Thread Functions

@subsection Thread Creation Attributes

@example
pthread_attr_init(), Function
pthread_attr_destroy(), Function
pthread_attr_setstacksize(), Function
pthread_attr_getstacksize(), Function
pthread_attr_setstackaddr(), Function
pthread_attr_getstackaddr(), Function
pthread_attr_setdetachstate(), Function
pthread_attr_getdetachstate(), Function
@end example

@subsection Thread Creation

@example
pthread_create(), Function
@end example

@subsection Wait for Thread Termination

@example
pthread_join(), Function
@end example

@subsection Detaching a Thread

@example
pthread_detach(), Function
@end example

@subsection Thread Termination

@example
pthread_exit(), Function
@end example

@subsection Get Thread ID

@example
pthread_self(), Function
@end example

@subsection Compare Thread IDs

@example
pthread_equal(), Function
@end example

@subsection Dynamic Package Initialization

@example
pthread_once(), Function
@end example

