@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Message Passing

@section Data Definitions for Message Queues

@subsection Data Structures

@example
mqd_t, Type, Untested Implementation
mq_attr, Type, Untested Implementation
@end example

@section Message Passing Functions

@subsection Open a Message Queue

@example
mq_open(), Function, Untested Implementation
@end example

@subsection Close a Message Queue

@example
mq_close(), Function, Untested Implementation
@end example

@subsection Remove a Message Queue

@example
mq_unlink, Function, Untested Implementation
@end example

@subsection Send a Message to a Message Queue

@example
mq_send(), Function, Untested Implementation
@end example

@subsection Receive a Message From a Message Queue

@example
mq_receive(), Function, Untested Implementation
@end example

@subsection Notify Process That a Message is Available on a Queue

@example
mq_notify(), Function, Untested Implementation
@end example

@subsection Set Message Queue Attributes

@example
mq_setattr(), Function, Untested Implementation
@end example

@subsection Get Message Queue Attributes

@example
mq_getattr(), Function, Untested Implementation
@end example

