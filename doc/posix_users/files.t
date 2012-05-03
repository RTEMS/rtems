@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Files and Directories Manager

@section Introduction

The files and directories manager is ...

The directives provided by the files and directories manager are:

@itemize @bullet
@item @code{opendir} - Open a Directory
@item @code{readdir} - Reads a directory
@item @code{rewinddir} - Resets the @code{readdir()} pointer
@item @code{scandir} - Scan a directory for matching entries
@item @code{telldir} - Return current location in directory stream
@item @code{closedir} - Ends directory read operation
@item @code{getdents} - Get directory entries
@item @code{chdir} - Changes the current working directory
@item @code{fchdir} - Changes the current working directory
@item @code{getcwd} - Gets current working directory
@item @code{open} - Opens a file
@item @code{creat} - Create a new file or rewrite an existing one
@item @code{umask} - Sets a file creation mask
@item @code{link} - Creates a link to a file
@item @code{symlink} - Creates a symbolic link to a file
@item @code{readlink} - Obtain the name of the link destination
@item @code{mkdir} - Makes a directory
@item @code{mkfifo} - Makes a FIFO special file
@item @code{unlink} - Removes a directory entry
@item @code{rmdir} - Delete a directory
@item @code{rename} - Renames a file
@item @code{stat} - Gets information about a file.
@item @code{fstat} - Gets file status
@item @code{lstat} - Gets file status
@item @code{access} - Check permissions for a file.
@item @code{chmod} - Changes file mode
@item @code{fchmod} - Changes permissions of a file
@item @code{chown} - Changes the owner and/ or group of a file
@item @code{utime} - Change access and/or modification times of an inode
@item @code{ftruncate} - Truncate a file to a specified length
@item @code{truncate} - Truncate a file to a specified length
@item @code{pathconf} - Gets configuration values for files
@item @code{fpathconf} - Get configuration values for files
@item @code{mknod} - Create a directory
@end itemize

@section Background

@subsection Path Name Evaluation

A pathname is a string that consists of no more than @code{PATH_MAX}
bytes, including the terminating null character. A pathname has an optional
beginning slash, followed by zero or more filenames separated by slashes.
If the pathname refers to a directory, it may also have one or more trailing
slashes. Multiple successive slahes are considered to be the same as
one slash.

POSIX allows a pathname that begins with precisely two successive slashes to be
interpreted in an implementation-defined manner. RTEMS does not currently
recognize this as a special condition. Any number of successive
slashes is treated the same as a single slash. POSIX requires that
an implementation treat more than two leading slashes as a single slash.

@section Operations

There is currently no text in this section.

@section Directives

This section details the files and directories manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection opendir - Open a Directory

@findex opendir
@cindex  open a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <dirent.h>

int opendir(
  const char *dirname
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission was denied on a component of the path
prefix of @code{dirname}, or read permission is denied

@item EMFILE
Too many file descriptors in use by process

@item ENFILE
Too many files are currently open in the system.

@item ENOENT
Directory does not exist, or @code{name} is an empty string.

@item ENOMEM
Insufficient memory to complete the operation.

@item ENOTDIR
@code{name} is not a directory.

@end table

@subheading DESCRIPTION:

This routine opens a directory stream corresponding to the
directory specified by the @code{dirname} argument. The
directory stream is positioned at the first entry.

@subheading NOTES:

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection readdir - Reads a directory

@findex readdir
@cindex  reads a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <dirent.h>

int readdir(
  DIR *dirp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@end table

@subheading DESCRIPTION:

The @code{readdir()} function returns a pointer to a structure @code{dirent}
representing the next directory entry from the directory stream pointed to
by @code{dirp}. On end-of-file, NULL is returned.

The @code{readdir()} function may (or may not) return entries for . or .. Your
program should tolerate reading dot and dot-dot but not require them.

The data pointed to be @code{readdir()} may be overwritten by another call to
@code{readdir()} for the same directory stream. It will not be overwritten by
a call for another directory.

@subheading NOTES:

If @code{ptr} is not a pointer returned by @code{malloc()}, @code{calloc()}, or
@code{realloc()} or has been deallocated with @code{free()} or
@code{realloc()}, the results are not portable and are probably disastrous.

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection rewinddir - Resets the readdir() pointer

@findex rewinddir
@cindex  resets the readdir() pointer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <dirent.h>

void rewinddir(
  DIR *dirp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

No value is returned.

@subheading DESCRIPTION:

The @code{rewinddir()} function resets the position associated with
the directory stream pointed to by @code{dirp}. It also causes the
directory stream to refer to the current state of the directory.

@subheading NOTES:

NONE

If @code{dirp} is not a pointer by @code{opendir()}, the results are
undefined.

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection scandir - Scan a directory for matching entries

@findex scandir
@cindex  scan a directory for matching entries

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <dirent.h>

int scandir(
  const char       *dir,
  struct dirent ***namelist,
  int  (*select)(const struct dirent *),
  int  (*compar)(const struct dirent **, const struct dirent **)
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENOMEM
Insufficient memory to complete the operation.

@end table

@subheading DESCRIPTION:

The @code{scandir()} function scans the directory @code{dir}, calling
@code{select()} on each directory entry. Entries for which @code{select()}
returns non-zero are stored in strings allocated via @code{malloc()},
sorted using @code{qsort()} with the comparison function @code{compar()},
and collected in array @code{namelist} which is allocated via @code{malloc()}.
If @code{select} is NULL, all entries are selected.

@subheading NOTES:

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection telldir - Return current location in directory stream

@findex telldir
@cindex  return current location in directory stream

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <dirent.h>

off_t telldir(
  DIR *dir
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid directory stream descriptor @code{dir}.

@end table

@subheading DESCRIPTION:

The @code{telldir()} function returns the current location associated with the
directory stream @code{dir}.

@subheading NOTES:

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection closedir - Ends directory read operation

@findex closedir
@cindex  ends directory read operation

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <dirent.h>

int closedir(
  DIR *dirp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@end table

@subheading DESCRIPTION:

The directory stream associated with @code{dirp} is closed.
The value in @code{dirp} may not be usable after a call to
@code{closedir()}.

@subheading NOTES:

NONE

The argument to @code{closedir()} must be a pointer returned by
@code{opendir()}. If it is not, the results are not portable and
most likely unpleasant.

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection chdir - Changes the current working directory

@findex chdir
@cindex  changes the current working directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int chdir(
  const char *path
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

On error, this routine returns -1 and sets @code{errno} to one of
the following:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is
in effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the specified pathname was not a directory when directory
was expected.

@end table

@subheading DESCRIPTION:

The @code{chdir()} function causes the directory named by @code{path} to
become the current working directory; that is, the starting point for
searches of pathnames not beginning with a slash.

If @code{chdir()} detects an error, the current working directory is not
changed.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection fchdir - Changes the current working directory

@findex fchdir
@cindex  changes the current working directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int fchdir(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

On error, this routine returns -1 and sets @code{errno} to one of
the following:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is
in effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the specified pathname was not a directory when directory
was expected.

@end table

@subheading DESCRIPTION:

The @code{fchdir()} function causes the directory named by @code{fd} to
become the current working directory; that is, the starting point for
searches of pathnames not beginning with a slash.

If @code{fchdir()} detects an error, the current working directory is not
changed.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getcwd - Gets current working directory

@findex getcwd
@cindex  gets current working directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int getcwd( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument

@item ERANGE
Result is too large

@item EACCES
Search permission is denied for a directory in a file's path prefix.

@end table

@subheading DESCRIPTION:

The @code{getcwd()} function copies the absolute pathname of the current
working directory to the character array pointed to by @code{buf}. The
@code{size} argument is the number of bytes available in @code{buf}

@subheading NOTES:

There is no way to determine the maximum string length that @code{fetcwd()}
may need to return. Applications should tolerate getting @code{ERANGE}
and allocate a larger buffer.

It is possible for @code{getcwd()} to return EACCES if, say, @code{login}
puts the process into a directory without read access.

The 1988 standard uses @code{int} instead of @code{size_t} for the second
parameter.

@c
@c
@c
@page
@subsection open - Opens a file

@findex open
@cindex  opens a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(
  const char *path,
  int         oflag,
  mode_t      mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b

@item EACCES
Search permission is denied for a directory in a file's path prefix.

@item EEXIST
The named file already exists.

@item EINTR
Function was interrupted by a signal.

@item EISDIR
Attempt to open a directory for writing or to rename a file to be a
directory.

@item EMFILE
Too many file descriptors are in use by this process.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENFILE
Too many files are currently open in the system.

@item ENOENT
A file or directory does not exist.

@item ENOSPC
No space left on disk.

@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.

@item ENXIO
No such device. This error may also occur when a device is not ready, for
example, a tape drive is off-line.

@item EROFS
Read-only file system.
@end table

@subheading DESCRIPTION:

The @code{open} function establishes a connection between a file and a file
descriptor. The file descriptor is a small integer that is used by I/O
functions to reference the file. The @code{path} argument points to the
pathname for the file.

The @code{oflag} argument is the bitwise inclusive OR of the values of
symbolic constants. The programmer must specify exactly one of the following
three symbols:

@table @b
@item O_RDONLY
Open for reading only.

@item O_WRONLY
Open for writing only.

@item O_RDWR
Open for reading and writing.

@end table

Any combination of the following symbols may also be used.

@table @b
@item O_APPEND
Set the file offset to the end-of-file prior to each write.

@item O_CREAT
If the file does not exist, allow it to be created. This flag indicates
that the @code{mode} argument is present in the call to @code{open}.

@item O_EXCL
This flag may be used only if O_CREAT is also set. It causes the call
to @code{open} to fail if the file already exists.

@item O_NOCTTY
If @code{path} identifies a terminal, this flag prevents that teminal from
becoming the controlling terminal for thi9s process. See Chapter 8 for a
description of terminal I/O.

@item O_NONBLOCK
Do no wait for the device or file to be ready or available. After the file
is open, the @code{read} and @code{write} calls return immediately. If the
process would be delayed in the read or write opermation, -1 is returned and
@code{errno} is set to @code{EAGAIN} instead of blocking the caller.

@item O_TRUNC
This flag should be used only on ordinary files opened for writing. It
causes the file to be tuncated to zero length..

@end table

Upon successful completion, @code{open} returns a non-negative file
descriptor.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection creat - Create a new file or rewrite an existing one

@findex creat
@cindex  create a new file or rewrite an existing one

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int creat(
  const char *path,
  mode_t      mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EEXIST
@code{path} already exists and O_CREAT and O_EXCL were used.

@item EISDIR
@code{path} refers to a directory and the access requested involved
writing

@item ETXTBSY
@code{path} refers to an executable image which is currently being
executed and write access was requested

@item EFAULT
@code{path} points outside your accessible address space

@item EACCES
The requested access to the file is not allowed, or one of the
directories in @code{path} did not allow search (execute) permission.

@item ENAMETOOLONG
@code{path} was too long.

@item ENOENT
A directory component in @code{path} does not exist or is a dangling
symbolic link.

@item ENOTDIR
A component used as a directory in @code{path} is not, in fact, a
directory.

@item EMFILE
The process alreadyh has the maximum number of files open.

@item ENFILE
The limit on the total number of files open on the system has been
reached.

@item ENOMEM
Insufficient kernel memory was available.

@item EROFS
@code{path} refers to a file on a read-only filesystem and write access
was requested

@end table

@subheading DESCRIPTION:

@code{creat} attempts to create a file and return a file descriptor for
use in read, write, etc.

@subheading NOTES:

NONE

The routine is implemented in Cygnus newlib.

@c
@c
@c
@page
@subsection umask - Sets a file creation mask.

@findex umask
@cindex  sets a file creation mask.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

mode_t umask(
  mode_t cmask
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@subheading DESCRIPTION:

The @code{umask()} function sets the process file creation mask to @code{cmask}.
The file creation mask is used during @code{open()}, @code{creat()}, @code{mkdir()},
@code{mkfifo()} calls to turn off permission bits in the @code{mode} argument.
Bit positions that are set in @code{cmask} are cleared in the mode of the
created file.

@subheading NOTES:

NONE

The @code{cmask} argument should have only permission bits set. All other
bits should be zero.

In a system which supports multiple processes, the file creation mask is inherited
across @code{fork()} and @code{exec()} calls. This makes it possible to alter the
default permission bits of created files. RTEMS does not support multiple processes
so this behavior is not possible.

@c
@c
@c
@page
@subsection link - Creates a link to a file

@findex link
@cindex  creates a link to a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int link(
  const char *existing,
  const char *new
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b

@item EACCES
Search permission is denied for a directory in a file's path prefix

@item EEXIST
The named file already exists.

@item EMLINK
The number of links would exceed @code{LINK_MAX}.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.
@item ENOSPC
No space left on disk.

@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.

@item EPERM
Operation is not permitted. Process does not have the appropriate priviledges
or permissions to perform the requested operations.

@item EROFS
Read-only file system.

@item EXDEV
Attempt to link a file to another file system.

@end table

@subheading DESCRIPTION:

The @code{link()} function atomically creates a new link for an existing file
and increments the link count for the file.

If the @code{link()} function fails, no directories are modified.

The @code{existing} argument should not be a directory.

The caller may (or may not) need permission to access the existing file.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection symlink - Creates a symbolic link to a file

@findex symlink
@cindex  creates a symbolic link to a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int symlink(
  const char *topath,
  const char *frompath
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b

@item EACCES
Search permission is denied for a directory in a file's path prefix

@item EEXIST
The named file already exists.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.

@item ENOSPC
No space left on disk.

@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.

@item EPERM
Operation is not permitted. Process does not have the appropriate priviledges
or permissions to perform the requested operations.

@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The @code{symlink()} function creates a symbolic link from the frombath to the
topath. The symbolic link will be interpreted at run-time.

If the @code{symlink()} function fails, no directories are modified.

The caller may (or may not) need permission to access the existing file.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection readlink - Obtain the name of a symbolic link destination

@findex readlink
@cindex  obtain the name of a symbolic link destination

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int readlink(
  const char *path,
  char       *buf,
  size_t      bufsize
);

@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b

@item EACCES
Search permission is denied for a directory in a file's path prefix

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the prefix pathname was not a directory when a directory
was expected.

@item ELOOP
Too many symbolic links were encountered in the pathname.

@item EINVAL
The pathname does not refer to a symbolic link

@item EFAULT
An invalid pointer was passed into the @code{readlink()} routine.

@end table

@subheading DESCRIPTION:

The @code{readlink()} function places the symbolic link destination into
@code{buf} argument and returns the number of characters copied.

If the symbolic link destination is longer than bufsize characters the
name will be truncated.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection mkdir - Makes a directory

@findex mkdir
@cindex  makes a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int mkdir(
  const char *path,
  mode_t      mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix

@item EEXIST
The name file already exist.

@item EMLINK
The number of links would exceed LINK_MAX

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.

@item ENOSPC
No space left on disk.

@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.

@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The @code{mkdir()} function creates a new diectory named @code{path}. The
permission bits (modified by the file creation mask) are set from @code{mode}.
The owner and group IDs for the directory are set from the effective user ID
and group ID.

The new directory may (or may not) contain entries for.. and .. but is otherwise
empty.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection mkfifo - Makes a FIFO special file

@findex mkfifo
@cindex  makes a fifo special file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>


int mkfifo(
  const char *path,
  mode_t      mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix

@item EEXIST
The named file already exists.

@item ENOENT
A file or directory does not exist.

@item ENOSPC
No space left on disk.

@item ENOTDIR
A component of the specified @code{path} was not a directory when a directory
was expected.

@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The @code{mkfifo()} function creates a new FIFO special file named @code{path}.
The permission bits (modified by the file creation mask) are set from
@code{mode}. The owner and group IDs for the FIFO are set from the efective
user ID and group ID.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection unlink - Removes a directory entry

@findex unlink
@cindex  removes a directory entry

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int unlink(
  const char path
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix

@item EBUSY
The directory is in use.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the specified @code{path} was not a directory when a directory
was expected.

@item EPERM
Operation is not permitted. Process does not have the appropriate priviledges
or permissions to perform the requested operations.

@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The @code{unlink} function removes the link named by @code{path} and decrements the
link count of the file referenced by the link. When the link count goes to zero
and no process has the file open, the space occupied by the file is freed and the
file is no longer accessible.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection rmdir - Delete a directory

@findex rmdir
@cindex  delete a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int rmdir(
  const char *pathname
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The filesystem containing @code{pathname} does not support the removal
of directories.

@item EFAULT
@code{pathname} points ouside your accessible address space.

@item EACCES
Write access to the directory containing @code{pathname} was not
allowed for the process's effective uid, or one of the directories in
@code{pathname} did not allow search (execute) permission.

@item EPERM
The directory containing @code{pathname} has the stickybit (S_ISVTX)
set and the process's effective uid is neither the uid of the file to
be delected nor that of the director containing it.

@item ENAMETOOLONG
@code{pathname} was too long.

@item ENOENT
A dirctory component in @code{pathname} does not exist or is a
dangling symbolic link.

@item ENOTDIR
@code{pathname}, or a component used as a directory in @code{pathname},
is not, in fact, a directory.

@item ENOTEMPTY
@code{pathname} contains entries other than . and .. .

@item EBUSY
@code{pathname} is the current working directory or root directory of
some process

@item EBUSY
@code{pathname} is the current directory or root directory of some
process.

@item ENOMEM
Insufficient kernel memory was available

@item EROGS
@code{pathname} refers to a file on a read-only filesystem.

@item ELOOP
@code{pathname} contains a reference to a circular symbolic link

@end table

@subheading DESCRIPTION:

@code{rmdir} deletes a directory, which must be empty


@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection rename - Renames a file

@findex rename
@cindex  renames a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int rename(
  const char *old,
  const char *new
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix.

@item EBUSY
The directory is in use.

@item EEXIST
The named file already exists.

@item EINVAL
Invalid argument.

@item EISDIR
Attempt to open a directory for writing or to rename a file to be a
directory.

@item EMLINK
The number of links would exceed LINK_MAX.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is
in effect.

@item ENOENT
A file or directory does no exist.

@item ENOSPC
No space left on disk.

@item ENOTDIR
A component of the specified pathname was not a directory when a
directory was expected.

@item ENOTEMPTY
Attempt to delete or rename a non-empty directory.

@item EROFS
Read-only file system

@item EXDEV
Attempt to link a file to another file system.
@end table

@subheading DESCRIPTION:

The @code{rename()} function causes the file known bo @code{old} to
now be known as @code{new}.

Ordinary files may be renamed to ordinary files, and directories may be
renamed to directories; however, files cannot be converted using
@code{rename()}. The @code{new} pathname may not contain a path prefix
of @code{old}.

@subheading NOTES:

If a file already exists by the name @code{new}, it is removed. The
@code{rename()} function is atomic. If the @code{rename()} detects an
error, no files are removed. This guarantees that the
@code{rename("x", "x")} does not remove @code{x}.

You may not rename dot or dot-dot.

The routine is implemented in Cygnus newlib using @code{link()} and
@code{unlink()}.

@c
@c
@c
@page
@subsection stat - Gets information about a file

@findex stat
@cindex  gets information about a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int stat(
  const char  *path,
  struct stat *buf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix.

@item EBADF
Invalid file descriptor.

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is
in effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the specified pathname was not a directory when a
directory was expected.

@end table

@subheading DESCRIPTION:

The @code{path} argument points to a pathname for a file. Read, write, or
execute permission for the file is not required, but all directories listed
in @code{path} must be searchable. The @code{stat()} function obtains
information about the named file and writes it to the area pointed to by
@code{buf}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection fstat - Gets file status

@findex fstat
@cindex  gets file status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int fstat(
  int          fildes,
  struct stat *buf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@end table

@subheading DESCRIPTION:

The @code{fstat()} function obtains information about the file
associated with @code{fildes} and writes it to the area pointed
to by the @code{buf} argument.

@subheading NOTES:

If the filesystem object referred to by @code{fildes} is a
link, then the information returned in @code{buf} refers
to the destination of that link.  This is in contrast to
@code{lstat()} which does not follow the link.

@c
@c
@c
@page
@subsection lstat - Gets file status

@findex lstat
@cindex  gets file status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int lstat(
  int          fildes,
  struct stat *buf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@end table

@subheading DESCRIPTION:

The @code{lstat()} function obtains information about the file
associated with @code{fildes} and writes it to the area pointed
to by the @code{buf} argument.

@subheading NOTES:

If the filesystem object referred to by @code{fildes} is a
link, then the information returned in @code{buf} refers
to the link itself.  This is in contrast to @code{fstat()}
which follows the link.

The @code{lstat()} routine is defined by BSD 4.3 and SVR4
and not included in POSIX 1003.1b-1996.

@c
@c
@c
@page
@subsection access - Check permissions for a file

@findex access
@cindex  check permissions for a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int access(
  const char *pathname,
  int         mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
The requested access would be denied, either to the file itself or
one of the directories in @code{pathname}.

@item EFAULT
@code{pathname} points outside your accessible address space.

@item EINVAL
@code{Mode} was incorrectly specified.

@item ENAMETOOLONG
@code{pathname} is too long.

@item ENOENT
A directory component in @code{pathname} would have been accessible but
does not exist or was a dangling symbolic link.

@item ENOTDIR
A component used as a directory in @code{pathname} is not, in fact,
a directory.

@item ENOMEM
Insufficient kernel memory was available.

@end table

@subheading DESCRIPTION:

@code{Access} checks whether the process would be allowed to read, write or
test for existence of the file (or other file system object) whose name is
@code{pathname}. If @code{pathname} is a symbolic link permissions of the
file referred by this symbolic link are tested.

@code{Mode} is a mask consisting of one or more of R_OK, W_OK, X_OK and F_OK.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection chmod - Changes file mode.

@findex chmod
@cindex  changes file mode.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int chmod(
  const char *path,
  mode_t      mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.

@item EPERM
Operation is not permitted. Process does not have the appropriate priviledges
or permissions to perform the requested operations.

@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

Set the file permission bits, the set user ID bit, and the set group ID bit
for the file named by @code{path} to @code{mode}. If the effective user ID
does not match the owner of the file and the calling process does not have
the appropriate privileges, @code{chmod()} returns -1 and sets @code{errno} to
@code{EPERM}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection fchmod - Changes permissions of a file

@findex fchmod
@cindex  changes permissions of a file

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int fchmod(
  int    fildes,
  mode_t mode
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix.

@item EBADF
The descriptor is not valid.

@item EFAULT
@code{path} points outside your accessible address space.

@item EIO
A low-level I/o error occurred while modifying the inode.

@item ELOOP
@code{path} contains a circular reference

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is
in effect.

@item ENOENT
A file or directory does no exist.

@item ENOMEM
Insufficient kernel memory was avaliable.

@item ENOTDIR
A component of the specified pathname was not a directory when a
directory was expected.

@item EPERM
The effective UID does not match the owner of the file, and is not
zero

@item EROFS
Read-only file system
@end table

@subheading DESCRIPTION:

The mode of the file given by @code{path} or referenced by
@code{filedes} is changed.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection getdents - Get directory entries

@findex getdents
@cindex  get directory entries

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>
#include <linux/dirent.h>
#include <linux/unistd.h>

long getdents(
  int   dd_fd,
  char *dd_buf,
  int   dd_len
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

A successful call to @code{getdents} returns th the number of bytes read.
On end of directory, 0 is returned. When an error occurs, -1 is returned,
and @code{errno} is set appropriately.

@table @b
@item EBADF
Invalid file descriptor @code{fd}.

@item EFAULT
Argument points outside the calling process's address space.

@item EINVAL
Result buffer is too small.

@item ENOENT
No such directory.

@item ENOTDIR
File descriptor does not refer to a directory.

@end table

@subheading DESCRIPTION:

@code{getdents} reads several @code{dirent} structures from the directory
pointed by @code{fd} into the memory area pointed to by @code{dirp}. The
parameter @code{count} is the size of the memory area.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection chown - Changes the owner and/or group of a file.

@findex chown
@cindex  changes the owner and/or group of a file.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <unistd.h>

int chown(
  const char *path,
  uid_t       owner,
  gid_t       group
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for a directory in a file's path prefix

@item EINVAL
Invalid argument

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in
effect.

@item ENOENT
A file or directory does not exist.

@item ENOTDIR
A component of the specified pathname was not a directory when a directory
was expected.

@item EPERM
Operation is not permitted. Process does not have the appropriate priviledges
or permissions to perform the requested operations.

@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The user ID and group ID of the file named by @code{path} are set to
@code{owner} and @code{path}, respectively.

For regular files, the set group ID (S_ISGID) and set user ID (S_ISUID)
bits are cleared.

Some systems consider it a security violation to allow the owner of a file to
be changed, If users are billed for disk space usage, loaning a file to
another user could result in incorrect billing. The @code{chown()} function
may be restricted to privileged users for some or all files. The group ID can
still be changed to one of the supplementary group IDs.

@subheading NOTES:

This function may be restricted for some file. The @code{pathconf} function
can be used to test the @code{_PC_CHOWN_RESTRICTED} flag.



@c
@c
@c
@page
@subsection utime - Change access and/or modification times of an inode

@findex utime
@cindex  change access and/or modification times of an inode

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>

int utime(
  const char     *filename,
  struct utimbuf *buf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Permission to write the file is denied

@item ENOENT
@code{Filename} does not exist

@end table

@subheading DESCRIPTION:

@code{Utime} changes the access and modification times of the inode
specified by @code{filename} to the @code{actime} and @code{modtime}
fields of @code{buf} respectively. If @code{buf} is NULL, then the
access and modification times of the file are set to the current time.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection ftruncate - truncate a file to a specified length

@findex ftruncate
@cindex  truncate a file to a specified length

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int ftrunctate(
  int    fd,
  size_t length
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENOTDIR
A component of the path prefix is not a directory.

@item EINVAL
The pathname contains a character with the high-order bit set.

@item ENAMETOOLONG
A component of a pathname exceeded 255 characters, or an entire
path name exceeded 1023 characters.

@item ENOENT
The named file does not exist.

@item EACCES
The named file is not writable by the user.

@item EACCES
Search permission is denied for a component of the path prefix.

@item ELOOP
Too many symbolic links were encountered in translating the
pathname

@item EISDIR
The named file is a directory.

@item EROFS
The named file resides on a read-only file system

@item ETXTBSY
The file is a pure procedure (shared text) file that is being
executed

@item EIO
An I/O error occurred updating the inode.

@item EFAULT
@code{Path} points outside the process's allocated address space.

@item EBADF
The @code{fd} is not a valid descriptor.

@end table

@subheading DESCRIPTION:

@code{truncate()} causes the file named by @code{path} or referenced by
@code{fd} to be truncated to at most @code{length} bytes in size. If the
file previously was larger than this size, the extra data is lost. With
@code{ftruncate()}, the file must be open for writing.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection truncate - truncate a file to a specified length

@findex truncate
@cindex  truncate a file to a specified length

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int trunctate(
  const char *path,
  size_t      length
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENOTDIR
A component of the path prefix is not a directory.

@item EINVAL
The pathname contains a character with the high-order bit set.

@item ENAMETOOLONG
A component of a pathname exceeded 255 characters, or an entire
path name exceeded 1023 characters.

@item ENOENT
The named file does not exist.

@item EACCES
The named file is not writable by the user.

@item EACCES
Search permission is denied for a component of the path prefix.

@item ELOOP
Too many symbolic links were encountered in translating the
pathname

@item EISDIR
The named file is a directory.

@item EROFS
The named file resides on a read-only file system

@item ETXTBSY
The file is a pure procedure (shared text) file that is being
executed

@item EIO
An I/O error occurred updating the inode.

@item EFAULT
@code{Path} points outside the process's allocated address space.

@item EBADF
The @code{fd} is not a valid descriptor.

@end table

@subheading DESCRIPTION:

@code{truncate()} causes the file named by @code{path} or referenced by
@code{fd} to be truncated to at most @code{length} bytes in size. If the
file previously was larger than this size, the extra data is lost. With
@code{ftruncate()}, the file must be open for writing.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection pathconf - Gets configuration values for files

@findex pathconf
@cindex  gets configuration values for files

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int pathconf(
  const char *path,
  int         name
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument

@item EACCES
Permission to write the file is denied

@item ENAMETOOLONG
Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC
is in effect.

@item ENOENT
A file or directory does not exist

@item ENOTDIR
A component of the specified @code{path} was not a directory whan a
directory was expected.

@end table

@subheading DESCRIPTION:

@code{pathconf()} gets a value for the configuration option @code{name}
for the open file descriptor @code{filedes}.

The possible values for @code{name} are:

@table @b
@item _PC_LINK_MAX
returns the maximum number of links to the file. If @code{filedes} or
@code{path} refer to a directory, then the value applies to the whole
directory. The corresponding macro is @code{_POSIX_LINK_MAX}.

@item _PC_MAX_CANON
returns the maximum length of a formatted input line, where @code{filedes}
or @code{path} must refer to a terminal. The corresponding macro is
@code{_POSIX_MAX_CANON}.

@item _PC_MAX_INPUT
returns the maximum length of an input line, where @code{filedes} or
@code{path} must refer to a terminal. The corresponding macro is
@code{_POSIX_MAX_INPUT}.

@item _PC_NAME_MAX
returns the maximum length of a filename in the directory @code{path} or
@code{filedes}. The process is allowed to create. The corresponding macro
is @code{_POSIX_NAME_MAX}.

@item _PC_PATH_MAX
returns the maximum length of a relative pathname when @code{path} or
@code{filedes} is the current working directory. The corresponding macro
is @code{_POSIX_PATH_MAX}.

@item _PC_PIPE_BUF
returns the size of the pipe buffer, where @code{filedes} must refer to a
pipe or FIFO and @code{path} must refer to a FIFO. The corresponding macro
is @code{_POSIX_PIPE_BUF}.

@item _PC_CHOWN_RESTRICTED
returns nonzero if the chown(2) call may not be used on this file. If
@code{filedes} or @code{path} refer to a directory, then this applies to all
files in that directory. The corresponding macro is
@code{_POSIX_CHOWN_RESTRICTED}.

@end table

@subheading NOTES:

Files with name lengths longer than the value returned for @code{name} equal
@code{_PC_NAME_MAX} may exist in the given directory.

@c
@c
@c
@page
@subsection fpathconf - Gets configuration values for files

@findex fpathconf
@cindex  gets configuration values for files

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int fpathconf(
  int filedes,
  int name
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument

@item EACCES
Permission to write the file is denied
@item ENAMETOOLONG

Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC
is in effect.

@item ENOENT
A file or directory does not exist

@item ENOTDIR
A component of the specified @code{path} was not a directory whan a
directory was expected.

@end table


@subheading DESCRIPTION:

@code{pathconf()} gets a value for the configuration option @code{name}
for the open file descriptor @code{filedes}.

The possible values for name are:

@table @b
@item _PC_LINK_MAX
returns the maximum number of links to the file. If @code{filedes} or
@code{path} refer to a directory, then the value applies to the whole
directory. The corresponding macro is _POSIX_LINK_MAX.

@item _PC_MAX_CANON
returns the maximum length of a formatted input line, where @code{filedes}
or @code{path} must refer to a terminal. The corresponding macro is
@code{_POSIX_MAX_CANON}.

@item _PC_MAX_INPUT
returns the maximum length of an input line, where @code{filedes} or
@code{path} must refer to a terminal. The corresponding macro is
@code{_POSIX_MAX_INPUT}.

@item _PC_NAME_MAX
returns the maximum length of a filename in the directory @code{path} or
@code{filedes}. The process is allowed to create. The corresponding macro
is @code{_POSIX_NAME_MAX}.

@item _PC_PATH_MAX
returns the maximum length of a relative pathname when @code{path} or
@code{filedes} is the current working directory. The corresponding macro
is @code{_POSIX_PATH_MAX}.

@item _PC_PIPE_BUF
returns the size of the pipe buffer, where @code{filedes} must refer to a
pipe or FIFO and @code{path} must refer to a FIFO. The corresponding macro
is @code{_POSIX_PIPE_BUF}.

@item _PC_CHOWN_RESTRICTED
returns nonzero if the @code{chown()} call may not be used on this file. If
@code{filedes} or @code{path} refer to a directory, then this applies to all
files in that directory. The corresponding macro is
@code{_POSIX_CHOWN_RESTRICTED}.

@end table

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection mknod - create a directory

@findex mknod
@cindex  create a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

long mknod(
  const char *pathname,
  mode_t      mode,
  dev_t       dev
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{mknod} returns zero on success, or -1 if an error occurred (in which case,
errno is set appropriately).

@table @b
@item ENAMETOOLONG
@code{pathname} was too long.

@item ENOENT
A directory component in @code{pathname} does not exist or is a dangling symbolic
link.

@item ENOTDIR
A component used in the directory @code{pathname} is not, in fact, a directory.

@item ENOMEM
Insufficient kernel memory was available

@item EROFS
@code{pathname} refers to a file on a read-only filesystem.

@item ELOOP
@code{pathname} contains a reference to a circular symbolic link, ie a symbolic
link whose expansion contains a reference to itself.

@item ENOSPC
The device containing @code{pathname} has no room for the new node.

@end table

@subheading DESCRIPTION:

@code{mknod} attempts to create a filesystem node (file, device special file or
named pipe) named @code{pathname}, specified by @code{mode} and @code{dev}.

@code{mode} specifies both the permissions to use and the type of node to be created.

It should be a combination (using bitwise OR) of one of the file types listed
below and the permissions for the new node.

The permissions are modified by the process's @code{umask} in the usual way: the
permissions of the created node are @code{(mode & ~umask)}.

The file type should be one of @code{S_IFREG}, @code{S_IFCHR}, @code{S_IFBLK} and
@code{S_IFIFO} to specify a normal file (which will be created empty), character
special file, block special file or FIFO (named pipe), respectively, or zero, which
will create a normal file.

If the file type is @code{S_IFCHR} or @code{S_IFBLK} then @code{dev} specifies the major
and minor numbers of the newly created device special file; otherwise it is ignored.

The newly created node will be owned by the effective uid of the process. If the
directory containing the node has the set group id bit set, or if the filesystem
is mounted with BSD group semantics, the new node will inherit the group ownership
from its parent directory; otherwise it will be owned by the effective gid of the
process.


@subheading NOTES:

NONE
