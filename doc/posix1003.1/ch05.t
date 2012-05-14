@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Files and Directories

@section Directories

@subsection Format of Directory Entries

@subsection Directory Operations

@example
struct dirent, Type, Implemented
opendir(), Function, Implemented
readdir(), Function, Implemented
readdir_r(), Function, Implemented
rewinddir(), Function, Implemented
closedir(), Function, Implemented
@end example

@section Working Directory

@subsection Change Current Working Directory

@example
chdir(), Function, Implemented
@end example

@subsection Get Working Directory Pathname

@example
getcwd(), Function, Implemented
@end example

@section General File Creation

@subsection Open a File

@example
open(), Function, Implemented
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
creat(), Function, Implemented
@end example

@subsection Set File Creation Mask

@example
umask(), Function, Implemented
@end example

@subsection Link to a File

@example
link(), Function, Implemented
@end example

@section Special File Creation

@subsection Make a Directory

@example
mkdir(), Function, Implemented
@end example

@subsection Make a FIFO Special File

@example
mkfifo(), Function, Untested Implementation
@end example

NOTE: mkfifo() is implemented but no filesystem supports FIFOs.

@section File Removal

@subsection Remove Directory Entries

@example
unlink(), Function, Implemented
@end example

@subsection Remove a Directory

@example
rmdir(), Function, Implemented
@end example

@subsection Rename a File

@example
rename(), Function, Implemented
@end example

@section File Characteristics

@subsection File Characteristics Header and Data Structure

@example
struct stat, Type, Implemented
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
stat(), Function, Implemented
fstat(), Function, Implemented
@end example

@subsection Check File Accessibility

@example
access(), Function, Implemented
@end example

@subsection Change File Modes

@example
chmod(), Function, Implemented
fchmod(), Function, Implemented
@end example

@subsection Change Owner and Group of a File

@example
chown(), Function, Implemented
@end example

@subsection Set File Access and Modification Times

@example
struct utimbuf, Type, Implemented
utime(), Function, Implemented
@end example

@subsection Truncate a File to a Specified Length

@example
ftruncate(), Function, Implemented
@end example

@section Configurable Pathname Variable

@subsection Get Configurable Pathname Variables

@example
pathconf(), Function, Implemented
fpathconf(), Function, Implemented
_PC_LINK_MAX, Constant, Implemented
_PC_MAX_CANON, Constant, Implemented
_PC_MAX_INPUT, Constant, Implemented
_PC_MAX_INPUT, Constant, Implemented
_PC_NAME_MAX, Constant, Implemented
_PC_PATH_MAX, Constant, Implemented
_PC_PIPE_BUF, Constant, Implemented
_PC_ASYNC_IO, Constant, Implemented
_PC_CHOWN_RESTRICTED, Constant, Implemented
_PC_NO_TRUNC, Constant, Implemented
_PC_PRIO_IO, Constant, Implemented
_PC_SYNC_IO, Constant, Implemented
_PC_VDISABLE, Constant, Implemented
@end example

NOTE: The newlib unistd.h and sys/unistd.h are installed and the
include search patch is used to get the right one.  There are 
conflicts between the newlib unistd.h and RTEMS' version.
