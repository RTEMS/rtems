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
sched_setparam(), Function
@end example

@subsection Get Scheduling Parameters

@example
sched_getparam(), Function
@end example

@subsection Set Scheduling Policy and Scheduling Parameters

@example
sched_setscheduler(), Function
@end example

@subsection Get Scheduling Policy

@example
sched_getscheduler(), Function
@end example

@subsection Yield Processor

@example
sched_yield(), Function
@end example

@subsection Get Scheduling Parameter Limits

@example
sched_get_priority_max(), Function
sched_get_priority_min(), Function
sched_get_priority_rr_get_interval(), Function
@end example

@section Thread Scheduling

@subsection Thread Scheduling Attributes

@subsection Scheduling Contention Scope

@subsection Scheduling Allocation Domain

@subsection Scheduling Documentation

@section Thread Scheduling Functions

@subsection Thread Creation Scheduling Attributes

@example
pthread_attr_setscope(), Function
pthread_attr_getscope(), Function
pthread_attr_setinheritsched(), Function
pthread_attr_getinheritsched(), Function
pthread_attr_setschedpolicy(), Function
pthread_attr_getschedpolicy(), Function
pthread_attr_setschedparam(), Function
pthread_attr_getschedparam(), Function
@end example

@subsection Dynamic Thread Scheduling Parameters Access

@example
pthread_setschedparam(), Function
pthread_getschedparam(), Function
@end example

@section Synchronization Scheduling

@subsection Mutex Initialization Scheduling Attributes

@example
pthread_mutexattr_setprotocol(), Function
pthread_mutexattr_getprotocol(), Function
pthread_mutexattr_setpriceiling(), Function
pthread_mutexattr_getpriceiling(), Function
@end example

@subsection Change the Priority Ceiling of a Mutex

@example
pthread_mutex_setprioceiling(), Function
pthread_mutex_getprioceiling(), Function
@end example

