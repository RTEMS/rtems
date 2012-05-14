@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Execution Scheduling

@section Scheduling Parameters

@example
struct sched_param, Type, Implemented
@end example

@section Scheduling Policies

@example
SCHED_FIFO, Constant, Implemented
SCHED_RR, Constant, Implemented
SCHED_OTHER, Constant, Implemented
@end example

NOTE: RTEMS adds SCHED_SPORADIC.

@subsection SCHED_FIFO

@subsection SCHED_RR

@subsection SCHED_OTHER

@section Process Scheduling Functions

@subsection Set Scheduling Parameters

@example
sched_setparam(), Function, Dummy Implementation
@end example

@subsection Get Scheduling Parameters

@example
sched_getparam(), Function, Dummy Implementation
@end example

@subsection Set Scheduling Policy and Scheduling Parameters

@example
sched_setscheduler(), Function, Dummy Implementation
@end example

@subsection Get Scheduling Policy

@example
sched_getscheduler(), Function, Dummy Implementation
@end example

@subsection Yield Processor

@example
sched_yield(), Function, Implemented
@end example

@subsection Get Scheduling Parameter Limits

@example
sched_get_priority_max(), Function, Implemented
sched_get_priority_min(), Function, Implemented
sched_get_priority_rr_get_interval(), Function, Implemented
@end example

@section Thread Scheduling

@subsection Thread Scheduling Attributes

@example
PTHREAD_SCOPE_PROCESS, Constant, Implemented
PTHREAD_SCOPE_SYSTEM, Constant, Implemented
@end example

@subsection Scheduling Contention Scope

@subsection Scheduling Allocation Domain

@subsection Scheduling Documentation

@section Thread Scheduling Functions

@subsection Thread Creation Scheduling Attributes

@example
pthread_attr_setscope(), Function, Implemented
pthread_attr_getscope(), Function, Implemented
pthread_attr_setinheritsched(), Function, Implemented
pthread_attr_getinheritsched(), Function, Implemented
pthread_attr_setschedpolicy(), Function, Implemented
pthread_attr_getschedpolicy(), Function, Implemented
pthread_attr_setschedparam(), Function, Implemented
pthread_attr_getschedparam(), Function, Implemented
PTHREAD_INHERIT_SCHED, Constant, Implemented
PTHREAD_EXPLICIT_SCHED, Constant, Implemented
@end example

@subsection Dynamic Thread Scheduling Parameters Access

@example
pthread_setschedparam(), Function, Implemented
pthread_getschedparam(), Function, Implemented
@end example

@section Synchronization Scheduling

@subsection Mutex Initialization Scheduling Attributes

@example
pthread_mutexattr_setprotocol(), Function, Implemented
pthread_mutexattr_getprotocol(), Function, Implemented
pthread_mutexattr_setprioceiling(), Function, Implemented
pthread_mutexattr_getprioceiling(), Function, Implemented
PTHREAD_PRIO_NONE, Constant, Implemented
PTHREAD_PRIO_INHERIT, Constant, Implemented
PTHREAD_PRIO_PROTECT, Constant, Implemented
@end example

@subsection Change the Priority Ceiling of a Mutex

@example
pthread_mutex_setprioceiling(), Function, Implemented
pthread_mutex_getprioceiling(), Function, Implemented
@end example

