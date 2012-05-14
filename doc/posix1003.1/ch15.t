@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Message Passing

@section Data Definitions for Message Queues

@subsection Data Structures

NOTE: Semaphores are implemented but only unnamed semaphores
are currently tested.

@example
mqd_t, Type, Implemented
struct mq_attr, Type, Implemented
@end example

@section Message Passing Functions

@subsection Open a Message Queue

@example
mq_open(), Function, Implemented
@end example

@subsection Close a Message Queue

@example
mq_close(), Function, Implemented
@end example

@subsection Remove a Message Queue

@example
mq_unlink(), Function, Implemented
@end example

@subsection Send a Message to a Message Queue

@example
mq_send(), Function, Implemented
@end example

@subsection Receive a Message From a Message Queue

@example
mq_receive(), Function, Implemented
@end example

@subsection Notify Process That a Message is Available on a Queue

@example
mq_notify(), Function, Implemented
@end example

@subsection Set Message Queue Attributes

@example
mq_setattr(), Function, Implemented
@end example

@subsection Get Message Queue Attributes

@example
mq_getattr(), Function, Implemented
@end example

