@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Memory Management

@section Memory Locking Functions

@subsection Lock/Unlock the Address Space of a Process

@example
mlockall(), Function, Unimplemented
munlockall(), Function, Unimplemented
MCL_CURRENT, Constant, Unimplemented 
MCL_FUTURE, Constant,  Unimplemented
@end example

@subsection Lock/Unlock a Rand of Process Address Space

@example
mlock(), Function, Unimplemented
munlock(), Function, Unimplemented
@end example

@section Memory Mapping Functions

@subsection Map Process Addresses to a Memory Object

@example
mmap(), Function, Unimplemented
PROT_READ, Constant,  Unimplemented
PROT_WRITE, Constant,  Unimplemented
PROT_EXEC, Constant,  Unimplemented
PROT_NONE, Constant,  Unimplemented
MAP_SHARED, Constant,  Unimplemented
MAP_PRIVATE, Constant,  Unimplemented
MAP_FIXED, Constant,  Unimplemented
@end example

@subsection Unmap Previously Mapped Addresses

@example
munmap(), Function, Unimplemented
@end example

@subsection Change Memory Protection

@example
mprotect(), Function, Unimplemented
@end example

@subsection Memory Object Synchronization

@example
msync(), Function, Unimplemented, Unimplemented
MS_ASYNC, Constant, Unimplemented 
MS_SYNC, Constant,  Unimplemented
MS_INVALIDATE, Constant,  Unimplemented
@end example

@section Shared Memory Functions

@subsection Open a Shared Memory Object

@example
shm_open(), Function, Unimplemented
@end example

@subsection Remove a Shared Memory Object

@example
shm_unlink(), Function, Unimplemented
@end example
