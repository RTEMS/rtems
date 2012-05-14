@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Input and Output Primitives Manager

@section Introduction

The input and output primitives manager is ...

The directives provided by the input and output primitives manager are:

@itemize @bullet
@item @code{pipe} - Create an Inter-Process Channel
@item @code{dup} - Duplicates an open file descriptor
@item @code{dup2} - Duplicates an open file descriptor
@item @code{close} - Closes a file
@item @code{read} - Reads from a file
@item @code{write} - Writes to a file
@item @code{fcntl} - Manipulates an open file descriptor
@item @code{lseek} - Reposition read/write file offset
@item @code{fsync} - Synchronize file complete in-core state with that on disk
@item @code{fdatasync} - Synchronize file in-core data with that on disk
@item @code{sync} - Schedule file system updates
@item @code{mount} - Mount a file system
@item @code{unmount} - Unmount file systems
@item @code{readv} - Vectored read from a file
@item @code{writev} - Vectored write to a file
@item @code{aio_read} - Asynchronous Read
@item @code{aio_write} - Asynchronous Write
@item @code{lio_listio} - List Directed I/O
@item @code{aio_error} - Retrieve Error Status of Asynchronous I/O Operation
@item @code{aio_return} - Retrieve Return Status Asynchronous I/O Operation
@item @code{aio_cancel} - Cancel Asynchronous I/O Request
@item @code{aio_suspend} - Wait for Asynchronous I/O Request
@item @code{aio_fsync} - Asynchronous File Synchronization
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the input and output primitives manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection pipe - Create an Inter-Process Channel

@findex pipe
@cindex  create an inter

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pipe(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection dup - Duplicates an open file descriptor

@findex dup
@cindex  duplicates an open file descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int dup(
  int fildes
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor.

@item EINTR
Function was interrupted by a signal.

@item EMFILE
The process already has the maximum number of file descriptors open
and tried to open a new one.
@end table

@subheading DESCRIPTION:

The @code{dup} function returns the lowest numbered available file
descriptor. This new desciptor refers to the same open file as the
original descriptor and shares any locks.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection dup2 - Duplicates an open file descriptor

@findex dup2
@cindex  duplicates an open file descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int dup2(
  int fildes,
  int fildes2
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor.

@item EINTR
Function was interrupted by a signal.

@item EMFILE
The process already has the maximum number of file descriptors open
and tried to open a new one.
@end table

@subheading DESCRIPTION:

@code{dup2} creates a copy of the file descriptor @code{oldfd}.

The old and new descriptors may be used interchangeably. They share locks, file
position pointers and flags; for example, if the file position is modified by using
@code{lseek} on one of the descriptors, the position is also changed for the other.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection close - Closes a file

@findex close
@cindex  closes a file.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int close(
  int fildes
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@item EINTR
Function was interrupted by a signal.
@end table

@subheading DESCRIPTION:

The @code{close()} function deallocates the file descriptor named by
@code{fildes} and makes it available for reuse. All outstanding
record locks owned by this process for the file are unlocked.

@subheading NOTES:

A signal can interrupt the @code{close()} function. In that case,
@code{close()} returns -1 with @code{errno} set to EINTR. The file
may or may not be closed.

@c
@c
@c
@page
@subsection read - Reads from a file

@findex read
@cindex  reads from a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int read(
  int           fildes,
  void         *buf,
  unsigned int  nbyte
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

On error, this routine returns -1 and sets @code{errno} to one of
the following:

@table @b
@item EAGAIN
The O_NONBLOCK flag is set for a file descriptor and the process
would be delayed in the I/O operation.

@item EBADF
Invalid file descriptor

@item EINTR
Function was interrupted by a signal.

@item EIO
Input or output error

@item EINVAL
Bad buffer pointer

@end table

@subheading DESCRIPTION:

The @code{read()} function reads @code{nbyte} bytes from the file
associated with @code{fildes} into the buffer pointed to by @code{buf}.

The @code{read()} function returns the number of bytes actually read
and placed in the buffer. This will be less than @code{nbyte} if:

@itemize @bullet

@item The number of bytes left in the file is less than @code{nbyte}.

@item The @code{read()} request was interrupted by a signal.

@item The file is a pipe or FIFO or special file with less than @code{nbytes}
immediately available for reading.

@end itemize

When attempting to read from any empty pipe or FIFO:


@itemize @bullet

@item If no process has the pipe open for writing, zero is returned to
indicate end-of-file.

@item If some process has the pipe open for writing and O_NONBLOCK is set,
-1 is returned and @code{errno} is set to EAGAIN.

@item If some process has the pipe open for writing and O_NONBLOCK is clear,
@code{read()} waits for some data to be written or the pipe to be closed.

@end itemize


When attempting to read from a file other than a pipe or FIFO and no data
is available.

@itemize @bullet

@item If O_NONBLOCK is set, -1 is returned and @code{errno} is set to EAGAIN.

@item If O_NONBLOCK is clear, @code{read()} waits for some data to become
available.

@item The O_NONBLOCK flag is ignored if data is available.

@end itemize

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection write - Writes to a file

@findex write
@cindex  writes to a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int write(
  int           fildes,
  const void   *buf,
  unsigned int  nbytes
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EAGAIN
The O_NONBLOCK flag is set for a file descriptor and the process
would be delayed in the I/O operation.

@item EBADF
Invalid file descriptor

@item EFBIG
An attempt was made to write to a file that exceeds the maximum file
size

@item EINTR
The function was interrupted by a signal.

@item EIO
Input or output error.

@item ENOSPC
No space left on disk.

@item EPIPE
Attempt to write to a pope or FIFO with no reader.

@item EINVAL
Bad buffer pointer

@end table

@subheading DESCRIPTION:

The @code{write()} function writes @code{nbyte} from the array pointed
to by @code{buf} into the file associated with @code{fildes}.

If @code{nybte} is zero and the file is a regular file, the @code{write()}
function returns zero and has no other effect. If @code{nbyte} is zero
and the file is a special file, te results are not portable.

The @code{write()} function returns the number of bytes written. This
number will be less than @code{nbytes} if there is an error. It will never
be greater than @code{nbytes}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection fcntl - Manipulates an open file descriptor

@findex fcntl
@cindex  manipulates an open file descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int fcntl(
  int fildes,
  int cmd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCESS
Search permission is denied for a direcotry in a file's path
prefix.

@item EAGAIN
The O_NONBLOCK flag is set for a file descriptor and the process
would be delayed in the I/O operation.

@item EBADF
Invalid file descriptor

@item EDEADLK
An @code{fcntl} with function F_SETLKW would cause a deadlock.

@item EINTR
The functioin was interrupted by a signal.

@item EINVAL
Invalid argument

@item EMFILE
Too many file descriptor or in use by the process.

@item ENOLCK
No locks available

@end table

@subheading DESCRIPTION:

@code{fcntl()} performs one of various miscellaneous operations on
@code{fd}. The operation in question is determined by @code{cmd}:

@table @b

@item F_DUPFD
Makes @code{arg} be a copy of @code{fd}, closing @code{fd} first if necessary.

The same functionality can be more easily achieved by using @code{dup2()}.

The old and new descriptors may be used interchangeably. They share locks,
file position pointers and flags; for example, if the file position is
modified by using @code{lseek()} on one of the descriptors, the position is
also changed for the other.

The two descriptors do not share the close-on-exec flag, however. The
close-on-exec flag of the copy is off, meaning that it will be closed on
exec.

On success, the new descriptor is returned.

@item F_GETFD
Read the close-on-exec flag. If the low-order bit is 0, the file will
remain open across exec, otherwise it will be closed.

@item F_SETFD
Set the close-on-exec flag to the value specified by @code{arg} (only the least
significant bit is used).

@item F_GETFL
Read the descriptor's flags (all flags (as set by open()) are returned).

@item F_SETFL
Set the descriptor's flags to the value specified by @code{arg}. Only
@code{O_APPEND} and @code{O_NONBLOCK} may be set.

The flags are shared between copies (made with @code{dup()} etc.) of the same
file descriptor.

The flags and their semantics are described in @code{open()}.

@item F_GETLK, F_SETLK and F_SETLKW
Manage discretionary file locks. The third argument @code{arg} is a pointer to a
struct flock (that may be overwritten by this call).

@item F_GETLK
Return the flock structure that prevents us from obtaining the lock, or set the
@code{l_type} field of the lock to @code{F_UNLCK} if there is no obstruction.

@item F_SETLK
The lock is set (when @code{l_type} is @code{F_RDLCK} or @code{F_WRLCK}) or
cleared (when it is @code{F_UNLCK}. If lock is held by someone else, this
call returns -1 and sets @code{errno} to EACCES or EAGAIN.

@item F_SETLKW
Like @code{F_SETLK}, but instead of returning an error we wait for the lock to
be released.

@item F_GETOWN
Get the process ID (or process group) of the owner of a socket.

Process groups are returned as negative values.

@item F_SETOWN
Set the process or process group that owns a socket.

For these commands, ownership means receiving @code{SIGIO} or @code{SIGURG}
signals.

Process groups are specified using negative values.

@end table

@subheading NOTES:

The errors returned by @code{dup2} are different from those returned by
@code{F_DUPFD}.

@c
@c
@c
@page
@subsection lseek - Reposition read/write file offset

@findex lseek
@cindex  reposition read/write file offset

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <unistd.h>

int lseek(
  int    fildes,
  off_t  offset,
  int    whence
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
@code{fildes} is not an open file descriptor.

@item ESPIPE
@code{fildes} is associated with a pipe, socket or FIFO.

@item EINVAL
@code{whence} is not a proper value.

@end table

@subheading DESCRIPTION:

The @code{lseek} function repositions the offset of the file descriptor
@code{fildes} to the argument offset according to the directive whence.
The argument @code{fildes} must be an open file descriptor. @code{Lseek}
repositions the file pointer fildes as follows:

@itemize @bullet

@item
If @code{whence} is SEEK_SET, the offset is set to @code{offset} bytes.

@item
If @code{whence} is SEEK_CUR, the offset is set to its current location
plus offset bytes.

@item
If @code{whence} is SEEK_END, the offset is set to the size of the
file plus @code{offset} bytes.

@end itemize

The @code{lseek} function allows the file offset to be set beyond the end
of the existing end-of-file of the file. If data is later written at this
point, subsequent reads of the data in the gap return bytes of zeros
(until data is actually written into the gap).

Some devices are incapable of seeking. The value of the pointer associated
with such a device is undefined.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection fsync - Synchronize file complete in-core state with that on disk

@findex fsync
@cindex  synchronize file complete in

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fsync(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

On success, zero is returned. On error, -1 is returned, and @code{errno}
is set appropriately.

@table @b
@item EBADF
@code{fd} is not a valid descriptor open for writing

@item EINVAL
@code{fd} is bound to a special file which does not support support synchronization

@item EROFS
@code{fd} is bound to a special file which does not support support synchronization

@item EIO
An error occurred during synchronization

@end table

@subheading DESCRIPTION:

@code{fsync} copies all in-core parts of a file to disk.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection fdatasync - Synchronize file in-core data with that on disk

@findex fdatasync
@cindex  synchronize file in

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fdatasync(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

On success, zero is returned. On error, -1 is returned, and @code{errno} is
set appropriately.

@table @b
@item EBADF
@code{fd} is not a valid file descriptor open for writing.

@item EINVAL
@code{fd} is bound to a special file which does not support synchronization.

@item EIO
An error occurred during synchronization.

@item EROFS
@code{fd} is bound to a special file which dows not support synchronization.

@end table

@subheading DESCRIPTION:

@code{fdatasync} flushes all data buffers of a file to disk (before the system call
returns). It resembles @code{fsync} but is not required to update the metadata such
as access time.

Applications that access databases or log files often write a tiny data fragment
(e.g., one line in a log file) and then call @code{fsync} immediately in order to
ensure that the written data is physically stored on the harddisk. Unfortunately,
fsync will always initiate two write operations: one for the newly written data and
another one in order to update the modification time stored in the inode. If the
modification time is not a part of the transaction concept @code{fdatasync} can be
used to avoid unnecessary inode disk write operations.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection sync - Schedule file system updates

@findex sync
@cindex  synchronize file systems

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void sync(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

NONE

@subheading DESCRIPTION:

The @code{sync} service causes all information in memory that updates
file systems to be scheduled for writing out to all file systems.


@subheading NOTES:

The writing of data to the file systems is only guaranteed to be 
scheduled upon return.  It is not necessarily complete upon return
from @code{sync}.

@c
@c
@c
@page
@subsection mount - Mount a file system

@findex mount
@cindex  mount a file system

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <libio.h>

int mount(
  rtems_filesystem_mount_table_entry_t **mt_entry,
  rtems_filesystem_operations_table    *fs_ops,
  rtems_filesystem_options_t            fsoptions,
  char                                 *device,
  char                                 *mount_point
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EXXX

@end table

@subheading DESCRIPTION:

The @code{mount} routines mounts the filesystem class
which uses the filesystem operations specified by @code{fs_ops}
and @code{fsoptions}.  The filesystem is mounted at the directory
@code{mount_point} and the mode of the mounted filesystem is
specified by @code{fsoptions}.  If this filesystem class requires
a device, then the name of the device must be specified by @code{device}.

If this operation succeeds, the mount table entry for the mounted
filesystem is returned in @code{mt_entry}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection unmount - Unmount file systems

@findex unmount
@cindex  unmount file systems

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <libio.h>

int unmount(
  const char *mount_path
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EXXX
@end table

@subheading DESCRIPTION:

The @code{unmount} routine removes the attachment of the filesystem specified
by @code{mount_path}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection readv - Vectored read from a file

@findex readv
@cindex  vectored read from a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/uio.h>

ssize_t readv(
  int                 fd,
  const struct iovec *iov,
  int                 iovcnt
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

In addition to the errors detected by
@code{Input and Output Primitives Manager read - Reads from a file, read()},
this routine may return -1 and sets @code{errno} based upon the following 
errors:

@table @b
@item EINVAL
The sum of the @code{iov_len} values in the iov array overflowed an 
@code{ssize_t}.

@item EINVAL
The @code{iovcnt} argument was less than or equal to 0, or greater
than @code{IOV_MAX}.

@end table

@subheading DESCRIPTION:

The @code{readv()} function is equivalent to @code{read()}
except as described here. The @code{readv()} function shall place
the input data into the @code{iovcnt} buffers specified by the
members of the @code{iov} array: @code{iov[0], iov[1], ..., iov[iovcnt-1]}.

Each @code{iovec} entry specifies the base address and length of an area
in memory where data should be placed. The @code{readv()} function
always fills an area completely before proceeding to the next.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection writev - Vectored write to a file

@findex writev
@cindex  vectored write to a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/uio.h>

ssize_t writev(
  int                 fd,
  const struct iovec *iov,
  int                 iovcnt
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

In addition to the errors detected by
@code{Input and Output Primitives Manager write - Write to a file, write()},
this routine may return -1 and sets @code{errno} based upon the following 
errors:

@table @b
@item EINVAL
The sum of the @code{iov_len} values in the iov array overflowed an 
@code{ssize_t}.

@item EINVAL
The @code{iovcnt} argument was less than or equal to 0, or greater
than @code{IOV_MAX}.

@end table

@subheading DESCRIPTION:

The @code{writev()} function is equivalent to @code{write()},
except as noted here. The @code{writev()} function gathers output
data from the @code{iovcnt} buffers specified by the members of
the @code{iov array}: @code{iov[0], iov[1], ..., iov[iovcnt-1]}.
The @code{iovcnt} argument is valid if greater than 0 and less
than or equal to @code{IOV_MAX}.

Each @code{iovec} entry specifies the base address and length of
an area in memory from which data should be written. The @code{writev()}
function always writes a complete area before proceeding to the next.

If @code{fd} refers to a regular file and all of the @code{iov_len}
members in the array pointed to by @code{iov} are 0, @code{writev()}
returns 0 and has no other effect. For other file types, the behavior
is unspecified by POSIX.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection aio_read - Asynchronous Read

@findex aio_read
@cindex  asynchronous read

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_read(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection aio_write - Asynchronous Write

@findex aio_write
@cindex  asynchronous write

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_write(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection lio_listio - List Directed I/O

@findex lio_listio
@cindex  list directed i/o

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int lio_listio(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection aio_error - Retrieve Error Status of Asynchronous I/O Operation

@findex aio_error
@cindex  retrieve error status of asynchronous i/o operation

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_error(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection aio_return - Retrieve Return Status Asynchronous I/O Operation

@findex aio_return
@cindex  retrieve return status asynchronous i/o operation

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_return(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection aio_cancel - Cancel Asynchronous I/O Request

@findex aio_cancel
@cindex  cancel asynchronous i/o request

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_cancel(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection aio_suspend - Wait for Asynchronous I/O Request

@findex aio_suspend
@cindex  wait for asynchronous i/o request

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_suspend(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection aio_fsync - Asynchronous File Synchronization

@findex aio_fsync
@cindex  asynchronous file synchronization

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_fsync(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.
