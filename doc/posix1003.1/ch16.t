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
pthread_attr_init()
pthread_attr_destroy()
pthread_attr_setstacksize()
pthread_attr_getstacksize()
pthread_attr_setstackaddr()
pthread_attr_getstackaddr()
pthread_attr_setdetachstate()
pthread_attr_getdetachstate()
@end example

@subsection Thread Creation

@example
pthread_create()
@end example

@subsection Wait for Thread Termination

@example
pthread_join()
@end example

@subsection Detaching a Thread

@example
pthread_detach()
@end example

@subsection Thread Termination

@example
pthread_exit()
@end example

@subsection Get Thread ID

@example
pthread_self()
@end example

@subsection Compare Thread IDs

@example
pthread_equal()
@end example

@subsection Dynamic Package Initialization

@example
pthread_once()
@end example

