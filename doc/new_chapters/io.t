@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Input and Output Primitives Manager

@section Introduction

The input and output primitives manager is ...

The directives provided by the input and output primitives manager are:

@itemize @bullet
@item @code{pipe} - YYY
@item @code{dup} - Duplicates an open file descriptor
@item @code{dup2} - Duplicates an open file descriptor
@item @code{close} - Closes a file
@item @code{read} - Reads from a file
@item @code{write} - Writes to a file
@item @code{fcntl} - Manipulates an open file descriptor
@item @code{lseek} - Reposition read/write file offset
@item @code{fsynch} - XXX
@item @code{fdatasynch} - XXX
@item @code{mount} - Mount a file system
@item @code{umount} - Unmount file systems
@item @code{aio_read} - YYY
@item @code{aio_write} - YYY
@item @code{lio_listio} - YYY
@item @code{aio_error} - YYY
@item @code{aio_return} - YYY
@item @code{aio_cancel} - YYY
@item @code{aio_suspend} - YYY
@item @code{aio_fsync} - YYY
@end itemize

@section Background

@section Operations

@section Directives

This section details the input and output primitives manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection pipe - 

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

@page
@subsection dup - Duplicates an open file descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int dup(int fildes
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
descriptor.  This new desciptor refers to the same open file as the
original descriptor and shares any locks.

@subheading NOTES: None

@page
@subsection dup2 - Duplicates an open file descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int dup2(int fildes,
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

@code{Dup2} creates a copy of the file descriptor @code{oldfd}.

The old and new descriptors may be used interchangeably.  They share locks, file
position pointers and flags; for example, if the file position is modified by using
@code{lseek} on one of the descriptors, the position is also changed for the other.

@subheading NOTES: None

@page
@subsection close - Closes a file. 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int close(int fildes
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
@code{fildes} and makes it available for reuse.  All outstanding 
record locks owned by this process for the file are unlocked.

@subheading NOTES:

A signal can interrupt the @code{close()} function.  In that case,
@code{close()} returns -1 with @code{errno} set to EINTR.  The file
may or may not be closed.

@page
@subsection read - Reads from a file.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int read(int f         ildes,
         void         *buf,
         unsigned int  nbyte
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EAGAIN
The
@item EBADF
Invalid file descriptor
@item EINTR
Function was interrupted by a signal.
@item EIO
Input or output error

@end table

@subheading DESCRIPTION:

The @code{read()} function reads @code{nbyte} bytes from the file 
associated with @code{fildes} into the buffer pointed to by @code{buf}.

The @code{read()} function returns the number of bytes actually read
and placed in the buffer. This will be less than @code{nbyte} if:
  - The number of bytes left in the file is less than @code{nbyte} 
  - The @code{read()} request was interrupted by a signal 
  - The file is a pipe or FIFO or special file with less than @code{nbytes}
    immediately available for reading.

When attempting to read from any empty pipe or FIFO:
  - If no process has the pipe open for writing, zero is returned to 
    indicate end-of-file.
  - If some process has the pipe open for writing and O_NONBLOCK is set,
    -1 is returned and @code{errno} is set to EAGAIN.
  - If some process has the pipe open for writing and O_NONBLOCK is clear,
    @code{read()} waits for some data to be written or the pipe to be closed.

When attempting to read from a file other than a pipe or FIFO and no data
is available
  - If O_NONBLOCK is set, -1 is returned and @code{errno} is set to EAGAIN.
  - If O_NONBLOCK is clear, @code{read()} waits for some data to become 
    available.
  - The O_NONBLOCK flag is ignored if data is available.

@subheading NOTES: None

@page
@subsection write - Writes to a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int write(int         fildes,
          const void *buf,
          unsigned int nbytes
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
The
@item EINTR
The function was interrupted by a signal.
@item EIO
Input or output error.
@item ENOSPC
No space left on disk.
@item EPIPE
Attempt to write to a pope or FIFO with no reader.

@end table

@subheading DESCRIPTION:

The @code{write()} function writes @code{nbyte} from the array pointed
to by @code{buf} into the file associated with @code{fildes}.

If @code{nybte} is zero and the file is a regular file, the @code{write()}
function returns zero and has no other effect.  If @code{nbyte} is zero
and the file is a special file, te results are not portable.

The @code{write()} function returns the number of bytes written.  This 
number will be less than @code{nbytes} if there is an error.  It will never
be greater than @code{nbytes}.

@subheading NOTES: None

@page
@subsection fcntl - Manipulates an open file descriptor

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int fcntl(int fildes,
          int    cmd
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

@subheading NOTES:

@page
@subsection lseek - Reposition read/write file offset

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int lseek(
  int   fildes,
  off_t offset,
  int whence
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
@code{Fildes} is not an open file descriptor.

@item ESPIPE
@code{Fildes} is associated with a pipe, socket or FIFO. 

@item EINVAL
@code{Whence} is not a proper value. 

@end table

@subheading DESCRIPTION:

The @code{lseek} function repositions the offset of the file descriptor
@code{fildes} to the argument offset according to the directive whence.   
The argument @code{fildes} must be an open file descriptor. @code{Lseek}
repositions the file pointer fildes as follows:

  If @code{whence} is SEEK_SET, the offset is set to @code{offset} bytes.

  If @code{whence} is SEEK_CUR, the offset is set to its current location
  plus offset bytes.

  If @cdoe{whence} is SEEK_END, the offset is set to the size of the
  file plus @cdoe{offset} bytes.

The @cdoe{lseek} function allows the file offset to be set beyond the end
of the existing end-of-file of the file. If data is later written at this
point, subsequent reads of the data in the gap return bytes of zeros
(until data is actually written into the gap).

Some devices are incapable of seeking.  The value of the pointer asso-
ciated with such a device is undefined.

@subheading NOTES: None

@page
@subsection fsynch - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fsynch(
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

@page
@subsection fdatasynch - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fdatasynch(
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

@page
@subsection mount - Mount a file system

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/mount.h>
#include <linux/fs.h>

int mount(
  const char *specialfile,
  const char * dir,
  const char * filesystemtype,
  unsigned long rwflag,
  const void * data
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The user is not the super-user.

@item ENODEV
@code{Filesystemtype} not configured in the kernel.

@item ENOTBLK
@code{Specialfile} is not a block device (if a device was required).

@item EBUSY
@code{Specialfile} is already mounted.  Or, it cannot be remounted
read-only, because it still holds files open for writing.  Or, it 
cannot be mounted on @code{dir} because @code{dir} is still busy
(it is the working directory of some task, the mount point of another
device, has open files, etc.).

@item EINVAL
@code{Specialfile had an invalid superblock.  Or, a remount was 
attempted, while @code{specialfile} was not already mounted on @code{dir}.
Or, an umount was attempted, while @code{dir} was not a mount point.

@item EFAULT
One of the pointer arguments points outside the user address space.

@item ENOMEM
The kernel could not allocate a free page to copy filenames or data into.

@item ENAMETOOLONG
A pathname was longer than MAXPATHLEN.

@item ENOTDIR
A pathname was empty or had a nonexistent component.

@item EACCES
A component of a path was not searchable.  Or, mounting a read-only 
filesystem was attempted without giving the MS_RDONLY flag.  Or, the 
block device Specialfile is located on a filesystem mounted with
the MS_NODEV option.

@item ENXIO
The major number of the block device @code{specialfile} is out of
range.

@item EMFILE
(In case no block device is required:) Table of dummy devices is full.
              
@end table

@subheading DESCRIPTION:

@code{Mount} attaches the filesystem specified by @code{specialfile}
(which is often a device name) to the directory specified by @code{dir}.

Only the super-user may mount filesystems.

The @code{filesystemtype} argument may take one of the values listed in
/proc/filesystems (link "minix", "ext2", "msdos", "proc", "nfs", 
"iso9660" etc.).

The @code{rwflag} argument has the magic number 0xCOED in the top 16 bits,
and various mount flags in the low order 16 bits.  If the magic number is
absent, then the last two arguments are not used.

The @code{data} argument is interpreted by the different file systems.

@subheading NOTES: None

@page
@subsection umount - Umount file systems

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/mount.h>
#include <linux/fs.h>

int umount(
  const char *specialfile
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The user is not the super-user.

@item ENODEV
@code{Filesystemtype} not configured in the kernel.

@item ENOTBLK
@code{Specialfile} is not a block device (if a device was required).

@item EBUSY
@code{Specialfile} is already mounted.  Or, it cannot be remounted
read-only, because it still holds files open for writing.  Or, it 
cannot be mounted on @code{dir} because @code{dir} is still busy
(it is the working directory of some task, the mount point of another
device, has open files, etc.).

@item EINVAL
@code{Specialfile had an invalid superblock.  Or, a remount was 
attempted, while @code{specialfile} was not already mounted on @code{dir}.
Or, an umount was attempted, while @code{dir} was not a mount point.

@item EFAULT
One of the pointer arguments points outside the user address space.

@item ENOMEM
The kernel could not allocate a free page to copy filenames or data into.

@item ENAMETOOLONG
A pathname was longer than MAXPATHLEN.

@item ENOTDIR
A pathname was empty or had a nonexistent component.

@item EACCES
A component of a path was not searchable.  Or, mounting a read-only 
filesystem was attempted without giving the MS_RDONLY flag.  Or, the 
block device Specialfile is located on a filesystem mounted with
the MS_NODEV option.

@item ENXIO
The major number of the block device @code{specialfile} is out of
range.

@item EMFILE
(In case no block device is required:) Table of dummy devices is full.

@end table

@subheading DESCRIPTION:

@code{Umount} removes the attachment of the filesystem specified
by @code{specialfile} or @code{dir}.

Only the super-user may umount filesystems.

@subheading NOTES: None

@page
@subsection aio_read - 

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

@page
@subsection aio_write - 

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

@page
@subsection lio_listio - 

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

@page
@subsection aio_error - 

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

@page
@subsection aio_return - 

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

@page
@subsection aio_cancel - 

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

@page
@subsection aio_suspend - 

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

@page
@subsection aio_fsync - 

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
