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
O_RDONLY, Constant, 
O_WRONLY, Constant, 
O_RDWR, Constant, 
O_APPEND, Constant, 
O_CREAT, Constant, 
O_DSYNC, Constant, 
O_EXCL, Constant, 
O_NOCTTY, Constant, 
O_NONBLOCK, Constant, 
O_RSYNC, Constant, 
O_SYNC, Constant, 
O_TRUNC, Constant, 
@end example

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
S_ISDIR(), Function, 
S_ISCHR(), Function, 
S_ISBLK(), Function, 
S_ISREG(), Function, 
S_ISFIFO(), Function, 
S_TYPEISMQ(), Function, 
S_TYPEISSEM(), Function, 
S_TYPEISSHM(), Function, 
@end example

@subsubsection <sys/stat.h> File Modes

@example
S_IRWXU, Constant, 
S_IRUSR, Constant, 
S_IWUSR, Constant, 
S_IXUSR, Constant, 
S_IRWXG, Constant, 
S_IRGRP, Constant, 
S_IWGRP, Constant, 
S_IXGRP, Constant, 
S_IRWXO, Constant, 
S_IROTH, Constant, 
S_IWOTH, Constant, 
S_IXOTH, Constant, 
S_ISUID, Constant, 
S_ISGID, Constant, 
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
_PC_LINK_MAX, Constant, 
_PC_MAX_CANON, Constant, 
_PC_MAX_INPUT, Constant, 
_PC_MAX_INPUT, Constant, 
_PC_NAME_MAX, Constant, 
_PC_PATH_MAX, Constant, 
_PC_PIPE_BUF, Constant, 
_PC_ASYNC_IO, Constant, 
_PC_CHOWN_RESTRICTED, Constant, 
_PC_NO_TRUNC, Constant, 
_PC_PRIO_IO, Constant, 
_PC_SYNC_IO, Constant, 
_PC_VDISABLE, Constant, 
@end example

