@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Execution Scheduling

@section Scheduling Parameters

@section Scheduling Policies

@subsection SCHED_FIFO

@subsection SCHED_RR

@subsection SCHED_OTHER

@section Process Scheduling Functions

@subsection Set Scheduling Parameters

@example
sched_setparam()
@end example

@subsection Get Scheduling Parameters

@example
sched_getparam()
@end example

@subsection Set Scheduling Policy and Scheduling Parameters

@example
sched_setscheduler()
@end example

@subsection Get Scheduling Policy

@example
sched_getscheduler()
@end example

@subsection Yield Processor

@example
sched_yield()
@end example

@subsection Get Scheduling Parameter Limits

@example
sched_get_priority_max()
sched_get_priority_min()
sched_get_priority_rr_get_interval()
@end example

@section Thread Scheduling

@subsection Thread Scheduling Attributes

@subsection Scheduling Contention Scope

@subsection Scheduling Allocation Domain

@subsection Scheduling Documentation

@section Thread Scheduling Functions

@subsection Thread Creation Scheduling Attributes

@example
pthread_attr_setscope()
pthread_attr_getscope()
pthread_attr_setinheritsched()
pthread_attr_getinheritsched()
pthread_attr_setschedpolicy()
pthread_attr_getschedpolicy()
pthread_attr_setschedparam()
pthread_attr_getschedparam()
@end example

@subsection Dynamic Thread Scheduling Parameters Access

@example
pthread_setschedparam()
pthread_getschedparam()
@end example

@section Synchronization Scheduling

@subsection Mutex Initialization Scheduling Attributes

@example
pthread_mutexattr_setprotocol()
pthread_mutexattr_getprotocol()
pthread_mutexattr_setpriceiling()
pthread_mutexattr_getpriceiling()
@end example

@subsection Change the Priority Ceiling of a Mutex

@example
pthread_mutex_setprioceiling()
pthread_mutex_getprioceiling()
@end example

