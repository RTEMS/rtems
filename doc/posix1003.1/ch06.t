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
pipe()
@end example

@section File Descriptor Manipulation

@subsection Duplicate an Open File Descriptor

@example
dup()
dup2()
@end example

@section File Descriptor Deassignment

@subsection Close a File

@example
close()
@end example

@section Input and Output

@subsection Read from a File

@example
read()
@end example

@subsection Write to a File

@example
write()
@end example

@section Control Operations on Files

@subsection Data Definitions for File Control Operations

@subsection File Control

@example
fcntl()
@end example

@subsection Reposition Read/Write File Offset

@example
lseek()
@end example

@section File Synchronization

@subsection Synchronize the State of a File

@example
fsync()
@end example

@subsection Synchronize the Data of a File

@example
fdatasync()
@end example

@section Asynchronous Input and Output

@subsection Data Definitions for Asynchronous Input and Output

@subsection Asynchronous Read

@example
aio_read()
@end example

@subsection Asynchronous Write

@example
aio_write()
@end example

@subsection List Directed I/O

@example
aio_listio()
@end example

@subsection Retrieve Error Status of Asynchronous I/O Operation

@example
aio_error()
@end example

@subsection Retrieve Return Status of Asynchronous I/O Operation

@example
aio_return()
@end example

@subsection Cancel Asynchronous I/O Request

@example
aio_cancel()
@end example

@subsection Wait for Asynchronous I/O Request

@example
aio_suspend()
@end example

@subsection Asynchronous File Synchronization

@example
aio_fsync()
@end example

