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

@example
pathconf(), Function, Unimplemented
fpathconf(), Function, Unimplemented
@end example

@subsection Get Configurable Pathname Variables

