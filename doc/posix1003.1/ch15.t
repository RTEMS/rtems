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

@section Message Passing Functions

@subsection Open a Message Queue

@example
mq_open()
@end example

@subsection Close a Message Queue

@example
mq_close()
@end example

@subsection Remove a Message Queue

@example
mq_unlink
@end example

@subsection Send a Message to a Message Queue

@example
mq_send()
@end example

@subsection Receive a Message From a Message Queue

@example
mq_receive()
@end example

@subsection Notify Process That a Message is Available on a Queue

@example
mq_notify()
@end example

@subsection Set Message Queue Attributes

@example
mq_setattr()
@end example

@subsection Get Message Queue Attributes

@example
mq_getattr()
@end example

