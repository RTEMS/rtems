@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Synchronization

@section Semaphore Characteristics

@section Semaphore Functions

@subsection Initialize an Unnamed Semaphore

@example
sem_init()
@end example

@subsection Destroy an Unnamed Semaphore

@example
sem__destroy()
@end example

@subsection Initialize/Open a Named Semaphore

@example
sem_open()
@end example

@subsection Close a Named Semaphore

@example
sem_close()
@end example

@subsection Remove a Named Semaphore

@example
sem_unlink()
@end example

@subsection Lock a Semaphore

@example
sem_wait()
sem_trywait()
@end example

@subsection Unlock a Semaphore

@example
sem_post()
@end example

@subsection Get the Value of a Semaphore

@example
sem_getvalue()
@end example

@section Mutexes

@subsection Mutex Initialization Attributes

@example
pthread_mutexattr_init()
pthread_mutexattr_destroy()
pthread_mutexattr_getpshared()
pthread_mutexattr_setpshared()
@end example

@subsection Initializing and Destroying a Mutex

@example
pthread_mutex_init()
pthread_mutex_destroy()
@end example

@subsection Locking and Unlocking a Mutex

@example
pthread_mutex_lock()
pthread_mutex_trylock()
pthread_mutex_unlock()
@end example

@section Condition Variables

@subsection Condition Variable Initialization Attributes

@example
pthread_condattr_init()
pthread_condattr_destroy()
pthread_condattr_getpshared()
pthread_condattr_setpshared()
@end example

@subsection Initialization and Destroying Condition Variables

@example
pthread_cond_init()
pthread_cond_destroy()
@end example

@subsection Broadcasting and Signaling a Condition

@example
pthread_cond_signal()
pthread_cond_broadcast()
@end example

@subsection Waiting on a Condition

@example
pthread_cond_wait()
pthread_cond_timedwait()
@end example

