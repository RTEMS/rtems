@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Input and Output Primitives

@section Pipes

@subsection Create an Inter-Process Channel

@example
pipe(), Function, Unimplemented
@end example

@section File Descriptor Manipulation

@subsection Duplicate an Open File Descriptor

@example
dup(), Function, Unimplemented
dup2(), Function, Unimplemented
@end example

@section File Descriptor Deassignment

@subsection Close a File

@example
close(), Function, Partial Implementation
@end example

@section Input and Output

@subsection Read from a File

@example
read(), Function, Partial Implementation
@end example

@subsection Write to a File

@example
write(), Function, Partial Implementation
@end example

@section Control Operations on Files

@subsection Data Definitions for File Control Operations

@subsection File Control

@example
struct flock, Type, Unimplemented
fcntl(), Function, Unimplemented
@end example

@subsection Reposition Read/Write File Offset

@example
lseek(), Function, Partial Implementation
@end example

@section File Synchronization

@subsection Synchronize the State of a File

@example
fsync(), Function, Unimplemented
@end example

@subsection Synchronize the Data of a File

@example
fdatasync(), Function, Unimplemented
@end example

@section Asynchronous Input and Output

@subsection Data Definitions for Asynchronous Input and Output

@example
struct aiocb, Type, Dummy Implementation
@end example

@subsection Asynchronous Read

@example
aio_read(), Function, Dummy Implementation
@end example

@subsection Asynchronous Write

@example
aio_write(), Function, Dummy Implementation
@end example

@subsection List Directed I/O

@example
lio_listio(), Function, Dummy Implementation
@end example

@subsection Retrieve Error Status of Asynchronous I/O Operation

@example
aio_error(), Function, Dummy Implementation
@end example

@subsection Retrieve Return Status of Asynchronous I/O Operation

@example
aio_return(), Function, Dummy Implementation
@end example

@subsection Cancel Asynchronous I/O Request

@example
aio_cancel(), Function, Dummy Implementation
@end example

@subsection Wait for Asynchronous I/O Request

@example
aio_suspend(), Function, Dummy Implementation
@end example

@subsection Asynchronous File Synchronization

@example
aio_fsync(), Function, Dummy Implementation
@end example

