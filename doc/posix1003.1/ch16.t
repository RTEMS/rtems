@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Thread Management

@section Threads

@section Thread Functions

@subsection Thread Creation Attributes

@example
pthread_attr_init(), Function, Implemented
pthread_attr_destroy(), Function, Implemented
pthread_attr_setstacksize(), Function, Implemented
pthread_attr_getstacksize(), Function, Implemented
pthread_attr_setstackaddr(), Function, Implemented
pthread_attr_getstackaddr(), Function, Implemented
pthread_attr_setdetachstate(), Function, Implemented
pthread_attr_getdetachstate(), Function, Implemented
PTHREAD_CREATE_JOINABLE, Constant, Implemented
PTHREAD_CREATE_DETACHED, Constant, Implemented
@end example

@subsection Thread Creation

@example
pthread_create(), Function, Implemented
@end example

@subsection Wait for Thread Termination

@example
pthread_join(), Function, Implemented
@end example

@subsection Detaching a Thread

@example
pthread_detach(), Function, Implemented
@end example

@subsection Thread Termination

@example
pthread_exit(), Function, Implemented
@end example

@subsection Get Thread ID

@example
pthread_self(), Function, Implemented
@end example

@subsection Compare Thread IDs

@example
pthread_equal(), Function, Implemented
@end example

@subsection Dynamic Package Initialization

@example
pthread_once(), Function, Implemented
PTHREAD_ONCE_INIT, Constant, Implemented
@end example

