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
opendir()
readdir()
readdir_r()
rewinddir()
closedir()
@end example

@section Working Directory

@subsection Change Current Working Directory

@example
chdir()
@end example

@subsection Get Working Directory Pathname

@example
getcwd()
@end example

@section General File Creation

@subsection Open a File

@example
open()
@end example

@subsection Create a New File or Rewrite an Existing One

@example
creat()
@end example

@subsection Set File Creation Mask

@example
umask()
@end example

@subsection Link to a File

@example
link()
@end example

@section Special File Creation

@subsection Make a Directory

@example
mkdir()
@end example

@subsection Make a FIFO Special File

@example
mkfifo()
@end example

@section File Removal

@subsection Remove Directory Entries

@example
unlink()
@end example

@subsection Remove a Directory

@example
rmdir()
@end example

@subsection Rename a File

@example
rename()
@end example

@section File Characteristics

@subsection File Characteristics Header and Data Structure

@subsection Get File Status

@example
stat()
fstat()
@end example

@subsection Check File Accessibility

@example
access()
@end example

@subsection Change File Modes

@example
chmod()
fchmod()
@end example

@subsection Change Owner and Group of a File

@example
chown()
@end example

@subsection Set File Access and Modification Times

@example
utime()
@end example

@subsection Truncate a File to a Specified Length

@example
ftruncate()
@end example

@section Configurable Pathname Variable

@example
pathconf()
fpathconf()
@end example

@subsection Get Configurable Pathname Variables

