@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Files and Directories Manager

@section Introduction

The files and directories manager is ...

The directives provided by the files and directories manager are:

@itemize @bullet
@item @code{opendir} - Open a Directory
@item @code{readdir} - Reads a directory
@item @code{readdir_r} - 
@item @code{rewinddir} - 
@item @code{scandir} - 
@item @code{telldir} - 
@item @code{closedir} - 
@item @code{chdir} - Changes the current working directory
@item @code{getcwd} - Gets current working directory
@item @code{open} - Opens a file
@item @code{creat} - 
@item @code{umask} - Sets a file creation mask
@item @code{link} - Creates a link to a file
@item @code{mkdir} - Makes a directory
@item @code{mkfifo} - 
@item @code{unlink} - Removes a directory entry 
@item @code{rmdir} - 
@item @code{rename} - Renames a file 
@item @code{stat} - Gets information about a file.
@item @code{fstat} - 
@item @code{access} - 
@item @code{chmod} - Changes file mode
@item @code{fchmod} - 
@item @code{chown} - Changes the owner and/ or group of a file
@item @code{utime} - 
@item @code{ftrunctate} - 
@item @code{pathconf} - 
@item @code{fpathconf} - 
@end itemize

@section Background

@section Operations

@section Directives

This section details the files and directories manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection opendir - Open a Directory

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
for the directory itself.

@item E
The

@end table

@subheading DESCRIPTION:

This routine opens a directory stream corresponding to the
directory specified by the @code{dirname} argument.  The 
directory stream is positioned at the first entry.

@subheading NOTES:

The routine is implemented in Cygnus newlib.

@page
@subsection readdir - Reads a directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <dirent.h>

int readdir(
  DIR    *dirp
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
by @code{dirp}.  On end-of-file, NULL is returned.

The @code{readdir()} function may (or may not) return entries for . or .. Your
program should tolerate reading dot and dot-dot but not require them.

The data pointed to be @code{readdir()} may be overwritten by another call to
@code{readdir()} for the same directory stream.  It will not be overwritten by 
a call for another directory.

@subheading NOTES:

If @code{ptr} is not a pointer returned by @code{malloc()}, @code{calloc()}, or 
@code{realloc()} or has been deallocated with @code{free()} or @code{realloc()}, 
the results are not portable and are probably disastrous.

The routine is implemented in Cygnus newlib.

@page
@subsection readdir_r - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int readdir_r(
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

XXX must be implemented in RTEMS.

@page
@subsection rewinddir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rewinddir(
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

The routine is implemented in Cygnus newlib.

@page
@subsection scandir -

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int scandir(
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

The routine is implemented in Cygnus newlib.

@page
@subsection telldir -

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int telldir(
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

The routine is implemented in Cygnus newlib.


@page
@subsection closedir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int closedir(
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

The routine is implemented in Cygnus newlib.

@page
@subsection chdir - Changes the current working directory

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int chdir( const char  *path
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

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

@subheading NOTES: None

@page
@subsection getcwd - Gets current working directory 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int getcwd(
);
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
working directory to the character array pointed to by @code{buf}.  The
@code{size} argument is the number of bytes available in @code{buf}

@subheading NOTES:

There is no way to determine the maximum string length that @code{fetcwd()}
may need to return.  Applications should tolerate getting @code{ERANGE}
and allocate a larger buffer.

It is possible for @code{getcwd()} to return EACCES if, say, @code{login}
puts the process into a directory without read access.

The 1988 standard uses @code{int} instead of @code{size_t} for the second
parameter.

@page
@subsection open - Opens a file

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
No such device.  This error may also occur when a device is not ready, for 
example, a tape drive is off-line.
@item EROFS
Read-only file system.
@end table

@subheading DESCRIPTION:

The @code{open} function establishes a connection between a file and a file 
descriptor.  The file descriptor is a small integer that is used by I/O 
functions to reference the file.  The @code{path} argument points to the 
pathname for the file.

The @code{oflag} argument is the bitwise inclusive OR of the values of 
symbolic constants.  The programmer must specify exactly one of the following
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
If the file does not exist, allow it to be created.  This flag indicates
that the @code{mode} argument is present in the call to @code{open}.

@item O_EXCL
This flag may be used only if O_CREAT is also set.  It causes the call
to @code{open} to fail if the file already exists.

@item O_NOCTTY
If @code{path} identifies a terminal, this flag prevents that teminal from
becoming the controlling terminal for thi9s process.  See Chapter 8 for a
description of terminal I/O.

@item O_NONBLOCK
Do no wait for the device or file to be ready or available.  After the file
is open, the @code{read} and @code{write} calls return immediately.  If the 
process would be delayed in the read or write opermation, -1 is returned and 
@code{errno} is set to @code{EAGAIN} instead of blocking the caller.

@item O_TRUNC
This flag should be used only on ordinary files opened for writing.  It 
causes the file to be tuncated to zero length..

@end table

Upon successful completion, @code{open} returns a non-negative file 
descriptor.

@subheading NOTES:


@page
@subsection creat - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int creat(
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

The routine is implemented in Cygnus newlib.

@page
@subsection umask - Sets a file creation mask.

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

@subheading NOTES: None

The @code{cmask} argument should have only permission bits set.  All other 
bits should be zero.

In a system which supports multiple processes, the file creation mask is inherited
across @code{fork()} and @code{exec()} calls.  This makes it possible to alter the
default permission bits of created files.  RTEMS does not support multiple processes
so this behavior is not possible.

@page
@subsection link - Creates a link to a file

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
Operation is not permitted.  Process does not have the appropriate priviledges
or permissions to perform the requested operations.
@item EROFS
Read-only file system.
@item EXDEV
Attempt to link a file to another file system.

@end table

@subheading DESCRIPTION:

The @code{link} function atomically creates a new link for an existing file
and increments the link count for the file.

If the @code{link} function fails, no directories are modified.

The @code{existing} argument should not be a directory.

The callder may (or may not) need permission to access the existing file.

@subheading NOTES: None

@page
@subsection mkdir - Makes a directory

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

The @code{mkdir()} function creates a new diectory named @code{path}.  The 
permission bits (modified by the file creation mask) are set from @code{mode}.
The owner and group IDs for the directory are set from the effective user ID
and group ID.

The new directory may (or may not) contain entries for.. and .. but is otherwise
empty.

@subheading NOTES: None

@page
@subsection mkfifo - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mkfifo(
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
@subsection unlink - Removes a directory entry

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
A component of the specified pathname was not a directory when a directory
was expected.
@item EPERM
Operation is not permitted.  Process does not have the appropriate priviledges
or permissions to perform the requested operations.
@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

The @code{unlink} function removes the link named by @code{path} and decrements the
link count of the file referenced by the link.  When the link count goes to zero
and no process has the file open, the space occupied by the file is freed and the
file is no longer accessible. 

@subheading NOTES: None

@page
@subsection rmdir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rmdir(
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
@subsection rename - Renames a file 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <unistd.h>

int rename(const char *old, 
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
@code{rename()}.  The @code{new} pathname may not contain a path prefix
of @code{old}.

@subheading NOTES:

If a file already exists by the name @code{new}, it is removed.  The 
@code{rename()} function is atomic.  If the @code{rename()} detects an
error, no files are removed.  This guarantees that the
@code{rename("x", "x")} does not remove @code{x}.

You may not rename dot or dot-dot.

The routine is implemented in Cygnus newlib using @code{link()} and
@code{unlink()}.

@page
@subsection stat - Gets information about a file 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <sys/types.h>
#include <sys/stat.h>

int stat(const char  *path, 
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

The @code{path} argument points to a pathname for a file.  Read, write, or
execute permission for the file is not required, but all directories listed
in @code{path} must be searchable.  The @code{stat()} function obtains 
information about the named file and writes it to the area pointed to by
@code{but}.

@subheading NOTES: None

@page
@subsection fstat - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fstat(
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
@subsection access - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int access(
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
@subsection chmod - Changes file mode.

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
Operation is not permitted.  Process does not have the appropriate priviledges
or permissions to perform the requested operations.
@item EROFS
Read-only file system.

@end table

@subheading DESCRIPTION:

Set the file permission bits, the set user ID bit, and the set group ID bit 
for the file named by @code{path} to @code{mode}.  If the effective user ID 
does not match the owner of the file and the calling process does not have 
the appropriate privileges, @code{chmod()} returns -1 and sets @code{errno} to
@code{EPERM}.

@subheading NOTES:

@page
@subsection fchmod - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fchmod(
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
@subsection chown - Changes the owner and/or group of a file.

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
Operation is not permitted.  Process does not have the appropriate priviledges
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
be changed,  If users are billed for disk space usage, loaning a file to 
another user could result in incorrect billing.  The @code{chown()} function
may be restricted to privileged users for some or all files.  The group ID can
still be changed to one of the supplementary group IDs.

@subheading NOTES:

This function may be restricted for some file.  The @code{pathconf} function
can be used to test the _PC_CHOWN_RESTRICTED flag.



@page
@subsection utime - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int utime(
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
@subsection ftrunctate - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ftrunctate(
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
@subsection pathconf - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pathconf(
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
@subsection fpathconf - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fpathconf(
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

