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
mlockall()
munlockall()
@end example

@subsection Lock/Unlock a Rand of Process Address Space

@example
mlock()
munlock()
@end example

@section Memory Mapping Functions

@subsection Map Process Addresses to a Memory Object

@example
mmap()
@end example

@subsection Unmap Previously Mapped Addresses

@example
munmap()
@end example

@subsection Change Memory Protection

@example
mprotect()
@end example

@subsection Memory Object Synchronization

@example
msync()
@end example

@section Shared Memory Functions

@subsection Open a Shared Memory Object

@example
shm_open()
@end example

@subsection Remove a Shared Memory Object

@example
shm_unlink()
@end example
