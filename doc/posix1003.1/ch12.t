@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Memory Management

@section Memory Locking Functions

@subsection Lock/Unlock the Address Space of a Process

@example
mlockall(), Function
munlockall(), Function
@end example

@subsection Lock/Unlock a Rand of Process Address Space

@example
mlock(), Function
munlock(), Function
@end example

@section Memory Mapping Functions

@subsection Map Process Addresses to a Memory Object

@example
mmap(), Function
@end example

@subsection Unmap Previously Mapped Addresses

@example
munmap(), Function
@end example

@subsection Change Memory Protection

@example
mprotect(), Function
@end example

@subsection Memory Object Synchronization

@example
msync(), Function
@end example

@section Shared Memory Functions

@subsection Open a Shared Memory Object

@example
shm_open(), Function
@end example

@subsection Remove a Shared Memory Object

@example
shm_unlink(), Function
@end example
