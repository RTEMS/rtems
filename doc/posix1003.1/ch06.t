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
F_DUPFD, Constant, Implemented
F_GETFD, Constant, Implemented
F_GETLK, Constant, Implemented
F_SETFD, Constant, Implemented
F_GETFL, Constant, Implemented
F_SETFL, Constant, Implemented
F_SETLK, Constant, Implemented
F_SETLKW, Constant, Implemented
FD_CLOEXEC, Constant, Implemented
F_RDLCK, Constant, Implemented
F_UNLCK, Constant, Implemented
F_WRLCK, Constant, Implemented
O_ACCMODE, Constant, Implemented
@end example

NOTE: A number of constants are used by both @code{open} and @code{fcntl}.
@code{O_CREAT}, @code{O_EXCL}, @code{O_NOCTTY}, @code{O_TRUNC},
@code{O_APPEND}, @code{O_DSYNC}, @code{O_NONBLOCK}, @code{O_RSYNC},
@code{O_SYNC}, @code{O_RDONLY}, @code{O_RDWR}, and @code{O_WRONLY}
are also included in another section.  @xref{Open a File}.

@subsection Reposition Read/Write File Offset

@example
lseek(), Function, Partial Implementation
SEEK_SET, Constant, Implemented
SEEK_CUR, Constant, Implemented
SEEK_END, Constant, Implemented
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

@subsubsection Asynchronous I/O Control Block 

@example
struct aiocb, Type, Untested Implementation
@end example

@subsubsection Asynchronous I/O Manifest Constants

@example
AIO_CANCELED, Constant, Implemented
AIO_NOTCANCELED, Constant, Implemented
AIO_ALLDONE, Constant, Implemented
LIO_WAIT, Constant, Implemented
LIO_NOWAIT, Constant, Implemented
LIO_READ, Constant, Implemented
LIO_WRITE, Constant, Implemented
LIO_NOP, Constant, Implemented
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

