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
opendir(), Function
readdir(), Function
readdir_r(), Function
rewinddir(), Function
closedir(), Function
@end example

@section Working Directory

@subsection Change Current Working Directory

@example
chdir(), Function
@end example

@subsection Get Working Directory Pathname

@example
getcwd(), Function
@end example

@section General File Creation

@subsection Open a File

@example
open(), Function
@end example

@subsection Create a New File or Rewrite an Existing One

@example
creat(), Function
@end example

@subsection Set File Creation Mask

@example
umask(), Function
@end example

@subsection Link to a File

@example
link(), Function
@end example

@section Special File Creation

@subsection Make a Directory

@example
mkdir(), Function
@end example

@subsection Make a FIFO Special File

@example
mkfifo(), Function
@end example

@section File Removal

@subsection Remove Directory Entries

@example
unlink(), Function
@end example

@subsection Remove a Directory

@example
rmdir(), Function
@end example

@subsection Rename a File

@example
rename(), Function
@end example

@section File Characteristics

@subsection File Characteristics Header and Data Structure

@subsection Get File Status

@example
stat(), Function
fstat(), Function
@end example

@subsection Check File Accessibility

@example
access(), Function
@end example

@subsection Change File Modes

@example
chmod(), Function
fchmod(), Function
@end example

@subsection Change Owner and Group of a File

@example
chown(), Function
@end example

@subsection Set File Access and Modification Times

@example
utime(), Function
@end example

@subsection Truncate a File to a Specified Length

@example
ftruncate(), Function
@end example

@section Configurable Pathname Variable

@example
pathconf(), Function
fpathconf(), Function
@end example

@subsection Get Configurable Pathname Variables

