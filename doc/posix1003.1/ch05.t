@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Files and Directories

@section Directories

@subsection Format of Directory Entries

@subsection Directory Operations

@example
struct dirent, Type, Unimplemented
opendir(), Function, Untested Implementation, assumes directory services
readdir(), Function, Untested Implementation, assumes directory services
readdir_r(), Function, Untested Implementation, assumes directory services
rewinddir(), Function, Untested Implementation, assumes directory services
closedir(), Function, Untested Implementation, assumes directory services
@end example

@section Working Directory

@subsection Change Current Working Directory

@example
chdir(), Function, Unimplemented
@end example

@subsection Get Working Directory Pathname

@example
getcwd(), Function, Untested Implementation, assumes directory services
@end example

@section General File Creation

@subsection Open a File

@example
open(), Function, Implemented, requires rework for directory services
O_RDONLY, Constant, Implemented
O_WRONLY, Constant, Implemented
O_RDWR, Constant, Implemented
O_APPEND, Constant, Implemented
O_CREAT, Constant, Implemented
O_DSYNC, Constant, Unimplemented
O_EXCL, Constant, Implemented
O_NOCTTY, Constant, Implemented
O_NONBLOCK, Constant, Implemented
O_RSYNC, Constant, Unimplemented
O_SYNC, Constant, Implemented
O_TRUNC, Constant, Implemented
@end example

NOTE: In the newlib fcntl.h, O_SYNC is defined only if _POSIX_SOURCE is
not defined.  This seems wrong.

@subsection Create a New File or Rewrite an Existing One

@example
creat(), Function, Untested Implementation
@end example

@subsection Set File Creation Mask

@example
umask(), Function, Unimplemented
@end example

@subsection Link to a File

@example
link(), Function, Dummy Implementation
@end example

@section Special File Creation

@subsection Make a Directory

@example
mkdir(), Function, Unimplemented, assumes directory services
@end example

@subsection Make a FIFO Special File

@example
mkfifo(), Function, Unimplemented
@end example

@section File Removal

@subsection Remove Directory Entries

@example
unlink(), Function, Dummy Implementation
@end example

@subsection Remove a Directory

@example
rmdir(), Function, Unimplemented
@end example

@subsection Rename a File

@example
rename(), Function, Untested Implementation, assumes link/unlink
@end example

@section File Characteristics

@subsection File Characteristics Header and Data Structure

@example
struct stat, Type, Untested Implementation
@end example

@subsubsection <sys/stat.h> File Types

@example
S_ISBLK(), Function, Implemented
S_ISCHR(), Function, Implemented
S_ISDIR(), Function, Implemented
S_ISFIFO(), Function, Implemented
S_ISREG(), Function, Implemented
S_TYPEISMQ(), Function, Unimplemented
S_TYPEISSEM(), Function, Unimplemented
S_TYPEISSHM(), Function, Unimplemented
@end example

@subsubsection <sys/stat.h> File Modes

@example
S_IRWXU, Constant, Implemented
S_IRUSR, Constant, Implemented
S_IWUSR, Constant, Implemented
S_IXUSR, Constant, Implemented
S_IRWXG, Constant, Implemented
S_IRGRP, Constant, Implemented
S_IWGRP, Constant, Implemented
S_IXGRP, Constant, Implemented
S_IRWXO, Constant, Implemented
S_IROTH, Constant, Implemented
S_IWOTH, Constant, Implemented
S_IXOTH, Constant, Implemented
S_ISUID, Constant, Implemented
S_ISGID, Constant, Implemented
@end example

@subsubsection <sys/stat.h> Time Entries

@subsection Get File Status

@example
stat(), Function, Partial Implementation
fstat(), Function, Partial Implementation
@end example

@subsection Check File Accessibility

@example
access(), Function, Unimplemented
@end example

@subsection Change File Modes

@example
chmod(), Function, Unimplemented
fchmod(), Function, Unimplemented
@end example

@subsection Change Owner and Group of a File

@example
chown(), Function, Unimplemented
@end example

@subsection Set File Access and Modification Times

@example
struct utimbuf, Type, Unimplemented
utime(), Function, Unimplemented
@end example

@subsection Truncate a File to a Specified Length

@example
ftruncate(), Function, Unimplemented
@end example

@section Configurable Pathname Variable

@subsection Get Configurable Pathname Variables

@example
pathconf(), Function, Unimplemented
fpathconf(), Function, Unimplemented
_PC_LINK_MAX, Constant, Unimplemented
_PC_MAX_CANON, Constant, Unimplemented
_PC_MAX_INPUT, Constant, Unimplemented
_PC_MAX_INPUT, Constant, Unimplemented
_PC_NAME_MAX, Constant, Unimplemented
_PC_PATH_MAX, Constant, Unimplemented
_PC_PIPE_BUF, Constant, Unimplemented
_PC_ASYNC_IO, Constant, Unimplemented
_PC_CHOWN_RESTRICTED, Constant, Unimplemented
_PC_NO_TRUNC, Constant, Unimplemented
_PC_PRIO_IO, Constant, Unimplemented
_PC_SYNC_IO, Constant, Unimplemented
_PC_VDISABLE, Constant, Unimplemented
@end example

NOTE: The newlib unistd.h and sys/unistd.h are installed and the
include search patch is used to get the right one.  There are 
conflicts between the newlib unistd.h and RTEMS' version.' version.
