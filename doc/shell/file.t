@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter File and Directory Commands

@section Introduction

The RTEMS shell has the following file and directory commands:

@itemize @bullet

@item @code{umask} - Set file mode creation mask
@item @code{cp} - copy files
@item @code{mv} - move files
@item @code{pwd} - print work directory
@item @code{ls} - list files in the directory
@item @code{chdir} - change the current directory
@item @code{mkdir} - create a directory
@item @code{rmdir} - remove empty directories
@item @code{ln} - make links
@item @code{mknod} - make device special file
@item @code{chroot} - change the root directory
@item @code{chmod} - change permissions of a file
@item @code{cat} - display file contents
@item @code{msdosfmt} - format disk
@item @code{rm} - remove files
@item @code{mount} - mount disk
@item @code{unmount} - unmount disk
@item @code{blksync} - sync the block driver
@item @code{dd} - format disks
@item @code{hexdump} - format disks
@item @code{fdisk} - format disks
@item @code{dir} - alias for ls
@item @code{mkrfs} - format RFS file system
@item @code{cd} - alias for chdir

@end itemize

@section Commands

This section details the File and Directory Commands available.  A
subsection is dedicated to each of the commands and
describes the behavior and configuration of that
command as well as providing an example usage.

@c
@c
@c
@page
@subsection umask - set file mode creation mask

@pgindex umask

@subheading SYNOPSYS:

@example
umask [new_umask]
@end example

@subheading DESCRIPTION:

This command sets the user file creation mask to @code{new_umask}.  The
argument @code{new_umask} may be octal, hexadecimal, or decimal.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This command does not currently support symbolic mode masks.

@subheading EXAMPLES:

The following is an example of how to use @code{umask}:

@example
SHLL [/] $ umask
022
SHLL [/] $ umask 0666
0666
SHLL [/] $ umask
0666
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_UMASK
@findex CONFIGURE_SHELL_COMMAND_UMASK

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_UMASK} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_UMASK} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_umask

The @code{umask} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_umask(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{umask} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_UMASK_Command;
@end example

@c
@c
@c
@page
@subsection cp - copy files

@pgindex cp

@subheading SYNOPSYS:

@example
cp [-R [-H | -L | -P]] [-f | -i] [-pv] src target
cp [-R [-H | -L] ] [-f | -i] [-NpPv] source_file ... target_directory
@end example

@subheading DESCRIPTION:

In the first synopsis form, the cp utility copies the contents of the
source_file to the target_file. In the second synopsis form, the contents of
each named source_file is copied to the destination target_directory. The names
of the files themselves are not changed. If cp detects an attempt to copy a
file to itself, the copy will fail.

The following options are available:

@table @b
@item -f
For each existing destination pathname, attempt to overwrite it. If permissions
do not allow copy to succeed, remove it and create a new file, without
prompting for confirmation. (The -i option is ignored if the -f option is
specified.)

@item -H
If the -R option is specified, symbolic links on the command line are followed.
(Symbolic links encountered in the tree traversal are not followed.)

@item -i
Causes cp to write a prompt to the standard error output before copying a file
that would overwrite an existing file. If the response from the standard input
begins with the character 'y', the file copy is attempted.

@item -L
If the -R option is specified, all symbolic links are followed.

@item -N
When used with -p, do not copy file flags.

@item -P
No symbolic links are followed.

@item -p
Causes cp to preserve in the copy as many of the modification time, access
time, file flags, file mode, user ID, and group ID as allowed by permissions.

If the user ID and group ID cannot be preserved, no error message is displayed
and the exit value is not altered.

If the source file has its set user ID bit on and the user ID cannot be
preserved, the set user ID bit is not preserved in the copy's permissions. If
the source file has its set group ID bit on and the group ID cannot be
preserved, the set group ID bit is not preserved in the copy's permissions. If
the source file has both its set user ID and set group ID bits on, and either
the user ID or group ID cannot be preserved, neither the set user ID or set
group ID bits are preserved in the copy's permissions.

@item -R
If source_file designates a directory, cp copies the directory and the entire
subtree connected at that point. This option also causes symbolic links to be
copied, rather than indirected through, and for cp to create special files
rather than copying them as normal files. Created directories have the same
mode as the corresponding source directory, unmodified by the process's umask.

@item -v
Cause cp to be verbose, showing files as they are copied.

@end table

For each destination file that already exists, its contents are overwritten if
permissions allow, but its mode, user ID, and group ID are unchanged.

In the second synopsis form, target_directory must exist unless there is only
one named source_file which is a directory and the -R flag is specified.

If the destination file does not exist, the mode of the source file is used as
modified by the file mode creation mask (umask, see csh(1)). If the source file
has its set user ID bit on, that bit is removed unless both the source file and
the destination file are owned by the same user. If the source file has its set
group ID bit on, that bit is removed unless both the source file and the
destination file are in the same group and the user is a member of that group.
If both the set user ID and set group ID bits are set, all of the above
conditions must be fulfilled or both bits are removed.

Appropriate permissions are required for file creation or overwriting.

Symbolic links are always followed unless the -R flag is set, in which case
symbolic links are not followed, by default. The -H or -L flags (in conjunction
with the -R flag), as well as the -P flag cause symbolic links to be followed
as described above. The -H and -L options are ignored unless the -R option is
specified. In addition, these options override eachsubhedading other and the
command's actions are determined by the last one specified.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{cp} to
copy a file to a new name in the current directory:

@example
SHLL [/] # cat joel
cat: joel: No such file or directory
SHLL [/] # cp etc/passwd joel
SHLL [/] # cat joel
root:*:0:0:root::/:/bin/sh
rtems:*:1:1:RTEMS Application::/:/bin/sh
tty:!:2:2:tty owner::/:/bin/false
SHLL [/] # ls
drwxr-xr-x   1   root   root         536 Jan 01 00:00 dev/
drwxr-xr-x   1   root   root        1072 Jan 01 00:00 etc/
-rw-r--r--   1   root   root         102 Jan 01 00:00 joel 
3 files 1710 bytes occupied
@end example

The following is an example of how to use @code{cp} to
copy one or more files to a destination directory and
use the same @code{basename} in the destination directory:

@example
SHLL [/] # mkdir tmp
SHLL [/] # ls tmp         
0 files 0 bytes occupied
SHLL [/] # cp /etc/passwd tmp
SHLL [/] # ls /tmp
-rw-r--r--   1   root   root         102 Jan 01 00:01 passwd 
1 files 102 bytes occupied
SHLL [/] # cp /etc/passwd /etc/group /tmp
SHLL [/] # ls /tmp
-rw-r--r--   1   root   root         102 Jan 01 00:01 passwd 
-rw-r--r--   1   root   root          42 Jan 01 00:01 group 
2 files 144 bytes occupied
SHLL [/] # 
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CP
@findex CONFIGURE_SHELL_COMMAND_CP

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cp

The @code{cp} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_cp(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{cp} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CP_Command;
@end example

@subheading ORIGIN:

The implementation and portions of the documentation for this
command are from NetBSD 4.0.

@c
@c
@c
@page
@subsection mv - move files

@pgindex mv

@subheading SYNOPSYS:

@example
mv [-fiv] source_file target_file
mv [-fiv] source_file... target_file
@end example

@subheading DESCRIPTION:

In its first form, the mv utility renames the file named by the source
operand to the destination path named by the target operand.  This
form is assumed when the last operand does not name an already
existing directory.

In its second form, mv moves each file named by a source operand to a
destination file in the existing directory named by the directory
operand.  The destination path for each operand is the pathname
produced by the concatenation of the last operand, a slash, and the
final pathname component of the named file.

The following options are available:

@table @b
@item -f
Do not prompt for confirmation before overwriting the destination
path.

@item -i
Causes mv to write a prompt to standard error before moving a file
that would overwrite an existing file.  If the response from the
standard input begins with the character 'y', the move is attempted.

@item -v
Cause mv to be verbose, showing files as they are processed.

@end table

The last of any -f or -i options is the one which affects mv's
behavior.

It is an error for any of the source operands to specify a nonexistent
file or directory.

It is an error for the source operand to specify a directory if the
target exists and is not a directory.

If the destination path does not have a mode which permits writing, mv
prompts the user for confirmation as specified for the -i option.

Should the @b{rename} call fail because source and target are on
different file systems, @code{mv} will remove the destination file,
copy the source file to the destination, and then remove the source.
The effect is roughly equivalent to:

@example
rm -f destination_path && \
cp -PRp source_file destination_path && \
rm -rf source_file
@end example

@subheading EXIT STATUS:

The @code{mv} utility exits 0 on success, and >0 if an error occurs.

@subheading NOTES:

NONE

@subheading EXAMPLES:

@example
SHLL [/] mv /dev/console /dev/con1
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MV
@findex CONFIGURE_SHELL_COMMAND_MV

This command is included in the default shell command set.  When
building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MV} to have this command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MV} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mv

The @code{mv} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_mv(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mv} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MV_Command;
@end example

@subheading ORIGIN:

The implementation and portions of the documentation for this command
are from NetBSD 4.0.

@c
@c
@c
@page
@subsection pwd - print work directory

@pgindex pwd

@subheading SYNOPSYS:

@example
pwd
@end example

@subheading DESCRIPTION:

This command prints the fully qualified filename of the current
working directory. 

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{pwd}:

@example
SHLL [/] $ pwd
/
SHLL [/] $ cd dev
SHLL [/dev] $ pwd
/dev
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_PWD
@findex CONFIGURE_SHELL_COMMAND_PWD

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_PWD} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_PWD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_pwd

The @code{pwd} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_pwd(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{pwd} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_PWD_Command;
@end example

@c
@c
@c
@page
@subsection ls - list files in the directory

@pgindex ls

@subheading SYNOPSYS:

@example
ls [dir]
@end example

@subheading DESCRIPTION:

This command displays the contents of the specified directory.  If
no arguments are given, then it displays the contents of the current
working directory.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This command currently does not display information on a set of 
files like the POSIX ls(1).  It only displays the contents of
entire directories.

@subheading EXAMPLES:

The following is an example of how to use @code{ls}:

@example
SHLL [/] $ ls
drwxr-xr-x   1   root   root         536 Jan 01 00:00 dev/
drwxr-xr-x   1   root   root        1072 Jan 01 00:00 etc/
2 files 1608 bytes occupied
SHLL [/] $ ls etc
-rw-r--r--   1   root   root         102 Jan 01 00:00 passwd 
-rw-r--r--   1   root   root          42 Jan 01 00:00 group 
-rw-r--r--   1   root   root          30 Jan 01 00:00 issue 
-rw-r--r--   1   root   root          28 Jan 01 00:00 issue.net 
4 files 202 bytes occupied
SHLL [/] $ ls dev etc
-rwxr-xr-x   1  rtems   root           0 Jan 01 00:00 console 
-rwxr-xr-x   1   root   root           0 Jan 01 00:00 console_b 
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_LS
@findex CONFIGURE_SHELL_COMMAND_LS

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_LS} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_LS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ls

The @code{ls} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_ls(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{ls} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_LS_Command;
@end example

@c
@c
@c
@page
@subsection chdir - change the current directory

@pgindex chdir

@subheading SYNOPSYS:

@example
chdir [dir]
@end example

@subheading DESCRIPTION:

This command is used to change the current working directory to
the specified directory.  If no arguments are given, the current
working directory will be changed to @code{/}.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{chdir}:

@example
SHLL [/] $ pwd
/
SHLL [/] $ chdir etc
SHLL [/etc] $ pwd 
/etc
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CHDIR
@findex CONFIGURE_SHELL_COMMAND_CHDIR

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CHDIR} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CHDIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_chdir

The @code{chdir} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_chdir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{chdir} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CHDIR_Command;
@end example

@c
@c
@c
@page
@subsection mkdir - create a directory

@pgindex mkdir

@subheading SYNOPSYS:

@example
mkdir  dir [dir1 .. dirN]
@end example

@subheading DESCRIPTION:

This command creates the set of directories in the order they
are specified on the command line.  If an error is encountered
making one of the directories, the command will continue to 
attempt to create the remaining directories on the command line.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

If this command is invoked with no arguments, nothing occurs.

The user must have sufficient permissions to create the directory.
For the @code{fileio} test provided with RTEMS, this means the user
must login as @code{root} not @code{rtems}.

@subheading EXAMPLES:

The following is an example of how to use @code{mkdir}:

@example
SHLL [/] # ls
drwxr-xr-x   1   root   root         536 Jan 01 00:00 dev/
drwxr-xr-x   1   root   root        1072 Jan 01 00:00 etc/
2 files 1608 bytes occupied
SHLL [/] # mkdir joel
SHLL [/] # ls joel
0 files 0 bytes occupied
SHLL [/] # cp etc/passwd joel
SHLL [/] # ls joel
-rw-r--r--   1   root   root         102 Jan 01 00:02 passwd 
1 files 102 bytes occupied
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MKDIR
@findex CONFIGURE_SHELL_COMMAND_MKDIR

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MKDIR} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MKDIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mkdir

The @code{mkdir} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mkdir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mkdir} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MKDIR_Command;
@end example

@c
@c
@c
@page
@subsection rmdir - remove empty directories

@pgindex rmdir

@subheading SYNOPSYS:

@example
rmdir  [dir1 .. dirN]
@end example

@subheading DESCRIPTION:

This command removes the specified set of directories.  If no 
directories are provided on the command line, no actions are taken.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This command is a implemented using the @code{rmdir(2)} system
call and all reasons that call may fail apply to this command.

@subheading EXAMPLES:

The following is an example of how to use @code{rmdir}:

@example
SHLL [/] # mkdir joeldir
SHLL [/] # rmdir joeldir
SHLL [/] # ls joeldir
joeldir: No such file or directory.
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_RMDIR
@findex CONFIGURE_SHELL_COMMAND_RMDIR

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_RMDIR} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_RMDIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_rmdir

The @code{rmdir} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_rmdir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{rmdir} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_RMDIR_Command;
@end example

@c
@c
@c
@page
@subsection ln - make links

@pgindex ln

@subheading SYNOPSYS:

@example
ln [-fhinsv] source_file [target_file]
ln [-fhinsv] source_file ... target_dir
@end example

@subheading DESCRIPTION:

The ln utility creates a new directory entry (linked file) which has
the same modes as the original file.  It is useful for maintaining
multiple copies of a file in many places at once without using up
storage for the ``copies''; instead, a link ``points'' to the original
copy.  There are two types of links; hard links and symbolic links.
How a link ``points'' to a file is one of the differences between a
hard or symbolic link.

The options are as follows:
@table @b
@item -f
Unlink any already existing file, permitting the link to occur.

@item -h
If the target_file or target_dir is a symbolic link, do not follow it.
This is most useful with the -f option, to replace a symlink which may
point to a directory.

@item -i
Cause ln to write a prompt to standard error if the target file
exists.  If the response from the standard input begins with the
character `y' or `Y', then unlink the target file so that the link may
occur.  Otherwise, do not attempt the link.  (The -i option overrides
any previous -f options.)

@item -n
Same as -h, for compatibility with other ln implementations.

@item -s
Create a symbolic link.

@item -v
Cause ln to be verbose, showing files as they are processed.
@end table

By default ln makes hard links.  A hard link to a file is
indistinguishable from the original directory entry; any changes to a
file are effective independent of the name used to reference the file.
Hard links may not normally refer to directories and may not span file
systems.

A symbolic link contains the name of the file to which it is linked.
The referenced file is used when an @i{open} operation is performed on
the link.  A @i{stat} on a symbolic link will return the linked-to
file; an @i{lstat} must be done to obtain information about the link.
The @i{readlink} call may be used to read the contents of a symbolic
link.  Symbolic links may span file systems and may refer to
directories.

Given one or two arguments, ln creates a link to an existing file
source_file.  If target_file is given, the link has that name;
target_file may also be a directory in which to place the link;
otherwise it is placed in the current directory.  If only the
directory is specified, the link will be made to the last component of
source_file.

Given more than two arguments, ln makes links in target_dir to all the
named source files.  The links made will have the same name as the
files being linked to.

@subheading EXIT STATUS:

The @code{ln} utility exits 0 on success, and >0 if an error occurs.

@subheading NOTES:

NONE

@subheading EXAMPLES:

@example
SHLL [/] ln -s /dev/console /dev/con1
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_LN
@findex CONFIGURE_SHELL_COMMAND_LN

This command is included in the default shell command set.  When
building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_LN} to have this command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_LN} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ln

The @code{ln} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_ln(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{ln} has the following
prototype:

@example
extern rtems_shell_cmd_t rtems_shell_LN_Command;
@end example

@subheading ORIGIN:

The implementation and portions of the documentation for this command
are from NetBSD 4.0.

@c
@c
@c
@page
@subsection mknod - make device special file

@pgindex mknod

@subheading SYNOPSYS:

@example
mknod [-rR] [-F fmt] [-g gid] [-m mode] [-u uid] name [c | b] 
      [driver | major] minor
mknod [-rR] [-F fmt] [-g gid] [-m mode] [-u uid] name [c | b] 
      major unit subunit
mknod [-rR] [-g gid] [-m mode] [-u uid] name [c | b] number
mknod [-rR] [-g gid] [-m mode] [-u uid] name p
@end example

@subheading DESCRIPTION:

The mknod command creates device special files, or fifos.  Normally
the shell script /dev/MAKEDEV is used to create special files for
commonly known devices; it executes mknod with the appropriate
arguments and can make all the files required for the device.

To make nodes manually, the arguments are:

@table @b
@item -r
Replace an existing file if its type is incorrect.

@item -R
Replace an existing file if its type is incorrect.  Correct the
mode, user and group.

@item -g gid
Specify the group for the device node.  The gid operand may be a
numeric group ID or a group name.  If a group name is also a numeric
group ID, the operand is used as a group name.  Precede a numeric
group ID with a # to stop it being treated as a name.

@item -m mode
Specify the mode for the device node.  The mode may be absolute or
symbolic, see @i{chmod}.

@item -u uid
Specify the user for the device node.  The uid operand may be a
numeric user ID or a user name.  If a user name is also a numeric user
ID, the operand is used as a user name.  Precede a numeric user ID
with a # to stop it being treated as a name.

@item name
Device name, for example ``tty'' for a termios serial device or ``hd''
for a disk.

@item  b | c | p
Type of device.  If the device is a block type device such as a tape
or disk drive which needs both cooked and raw special files, the type
is b.  All other devices are character type devices, such as terminal
and pseudo devices, and are type c.  Specifying p creates fifo files.

@item driver | major
The major device number is an integer number which tells the kernel
which device driver entry point to use.  If the device driver is
configured into the current kernel it may be specified by driver name
or major number.

@item minor
The minor device number tells the kernel which one of several similar
devices the node corresponds to; for example, it may be a specific
serial port or pty.

@item unit and subunit
The unit and subunit numbers select a subset of a device; for example,
the unit may specify a particular disk, and the subunit a partition on
that disk.  (Currently this form of specification is only supported
by the bsdos format, for compatibility with the BSD/OS mknod).

@item number
A single opaque device number.  Useful for netbooted computers which
require device numbers packed in a format that isn't supported by
-F.
@end table

@subheading EXIT STATUS:

The @code{mknod} utility exits 0 on success, and >0 if an error occurs.

@subheading NOTES:

NONE

@subheading EXAMPLES:

@example
SHLL [/] mknod c 3 0 /dev/ttyS10
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MKNOD
@findex CONFIGURE_SHELL_COMMAND_MKNOD

This command is included in the default shell command set.  When
building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MKNOD} to have this command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MKNOD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mknod

The @code{mknod} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_mknod(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mknod} has the following
prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MKNOD_Command;
@end example

@subheading ORIGIN:

The implementation and portions of the documentation for this command
are from NetBSD 4.0.

@c
@c
@c
@page
@subsection chroot - change the root directory

@pgindex chroot

@subheading SYNOPSYS:

@example
chroot [dir]
@end example

@subheading DESCRIPTION:

This command changes the root directory to @code{dir} for subsequent
commands.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

The destination directory @code{dir} must exist.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{chroot}
and the impact it has on the environment for subsequent
command invocations:

@example
SHLL [/] $ cat passwd
cat: passwd: No such file or directory
SHLL [/] $ chroot etc
SHLL [/] $ cat passwd
root:*:0:0:root::/:/bin/sh
rtems:*:1:1:RTEMS Application::/:/bin/sh
tty:!:2:2:tty owner::/:/bin/false
SHLL [/] $ cat /etc/passwd
cat: /etc/passwd: No such file or directory
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CHROOT
@findex CONFIGURE_SHELL_COMMAND_CHROOT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CHROOT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CHROOT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_chroot

The @code{chroot} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_chroot(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{chroot} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CHROOT_Command;
@end example

@c
@c
@c
@page
@subsection chmod - change permissions of a file

@pgindex chmod

@subheading SYNOPSYS:

@example
chmod permissions file1 [file2...]
@end example

@subheading DESCRIPTION:

This command changes the permissions on the files specified to the
indicated @code{permissions}.  The permission values are POSIX based
with owner, group, and world having individual read, write, and
executive permission bits.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

The @code{chmod} command only takes numeric representations of
the permissions.

@subheading EXAMPLES:

The following is an example of how to use @code{chmod}:

@example
SHLL [/] # cd etc
SHLL [/etc] # ls
-rw-r--r--   1   root   root         102 Jan 01 00:00 passwd 
-rw-r--r--   1   root   root          42 Jan 01 00:00 group 
-rw-r--r--   1   root   root          30 Jan 01 00:00 issue 
-rw-r--r--   1   root   root          28 Jan 01 00:00 issue.net 
4 files 202 bytes occupied
SHLL [/etc] # chmod 0777 passwd
SHLL [/etc] # ls 
-rwxrwxrwx   1   root   root         102 Jan 01 00:00 passwd 
-rw-r--r--   1   root   root          42 Jan 01 00:00 group 
-rw-r--r--   1   root   root          30 Jan 01 00:00 issue 
-rw-r--r--   1   root   root          28 Jan 01 00:00 issue.net 
4 files 202 bytes occupied
SHLL [/etc] # chmod 0322 passwd
SHLL [/etc] # ls
--wx-w--w-   1 nouser   root         102 Jan 01 00:00 passwd 
-rw-r--r--   1 nouser   root          42 Jan 01 00:00 group 
-rw-r--r--   1 nouser   root          30 Jan 01 00:00 issue 
-rw-r--r--   1 nouser   root          28 Jan 01 00:00 issue.net 
4 files 202 bytes occupied
SHLL [/etc] # chmod 0644 passwd
SHLL [/etc] # ls
-rw-r--r--   1   root   root         102 Jan 01 00:00 passwd 
-rw-r--r--   1   root   root          42 Jan 01 00:00 group 
-rw-r--r--   1   root   root          30 Jan 01 00:00 issue 
-rw-r--r--   1   root   root          28 Jan 01 00:00 issue.net 
4 files 202 bytes occupied
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CHMOD
@findex CONFIGURE_SHELL_COMMAND_CHMOD

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CHMOD} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CHMOD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_chmod

The @code{chmod} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_chmod(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{chmod} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CHMOD_Command;
@end example

@c
@c
@c
@page
@subsection cat - display file contents

@pgindex cat

@subheading SYNOPSYS:

@example
cat file1 [file2 .. fileN]
@end example

@subheading DESCRIPTION:

This command displays the contents of the specified files.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

It is possible to read the input from a device file using @code{cat}.

@subheading EXAMPLES:

The following is an example of how to use @code{cat}:

@example
SHLL [/] # cat /etc/passwd
root:*:0:0:root::/:/bin/sh
rtems:*:1:1:RTEMS Application::/:/bin/sh
tty:!:2:2:tty owner::/:/bin/false
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CAT
@findex CONFIGURE_SHELL_COMMAND_CAT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CAT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CAT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cat

The @code{cat} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_cat(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{cat} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CAT_Command;
@end example

@c
@c
@c
@page
@subsection rm - remove files

@pgindex rm

@subheading SYNOPSYS:

@example
rm file1 [file2 ... fileN]
@end example

@subheading DESCRIPTION:

This command deletes a name from the filesystem.  If the specified file name 
was the last link to a file and there are no @code{open} file descriptor
references to that file, then it is deleted and the associated space in
the file system is made available for subsequent use.

If the filename specified was the last link to a file but there
are open file descriptor references to it, then the file will
remain in existence until the last file descriptor referencing
it is closed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{rm}:

@example
SHLL [/] # cp /etc/passwd tmpfile
SHLL [/] # cat tmpfile
root:*:0:0:root::/:/bin/sh
rtems:*:1:1:RTEMS Application::/:/bin/sh
tty:!:2:2:tty owner::/:/bin/false
SHLL [/] # rm tmpfile
SHLL [/] # cat tmpfile
cat: tmpfile: No such file or directory
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_RM
@findex CONFIGURE_SHELL_COMMAND_RM

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_RM} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_RM} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_rm

The @code{rm} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_rm(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{rm} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_RM_Command;
@end example

@c
@c
@c
@page
@subsection mount - mount disk

@pgindex mount

@subheading SYNOPSYS:

@example
mount [-t fstype] [-r] [-L] device path
@end example

@subheading DESCRIPTION:

The @code{mount} command will mount a block device to a mount point
using the specified file system. The files systems are:

@itemize @bullet
@item msdos - MSDOS File System
@item tftp  - TFTP Network File System
@item ftp   - FTP Network File System
@item nfs   - Network File System
@item rfs   - RTEMS File System
@end itemize

When the file system type is 'msdos' or 'rfs' the driver is a "block
device driver" node present in the file system. The driver is ignored
with the 'tftp' and 'ftp' file systems. For the 'nfs' file system the
driver is the 'host:/path' string that described NFS host and the
exported file system path.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

The mount point must exist.

The services offered by each file-system vary. For example you cannot list the
directory of a TFTP file-system as this server is not provided in the TFTP
protocol. You need to check each file-system's documentation for the services
provided.

@subheading EXAMPLES:

Mount the Flash Disk driver to the '/fd' mount point:

@example
SHLL [/] $ mount -t msdos /dev/flashdisk0 /fd
@end example

Mount the NFS file system exported path 'bar' by host 'foo':

@example
$ mount -t nfs foo:/bar /nfs
@end example

Mount the TFTP file system on '/tftp':

@example
$ mount -t tftp /tftp
@end example

To access the TFTP files on server '10.10.10.10':

@example
$ cat /tftp/10.10.10.10/test.txt
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MOUNT
@findex CONFIGURE_SHELL_COMMAND_MOUNT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MOUNT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MOUNT} when all
shell commands have been configured.

The mount command includes references to file-system code. If you do not wish
to include file-system that you do not use do not define the mount command
support for that file-system. The file-system mount command defines are:

@itemize @bullet
@item msdos - CONFIGURE_SHELL_MOUNT_MSDOS
@item tftp - CONFIGURE_SHELL_MOUNT_TFTP
@item ftp - CONFIGURE_SHELL_MOUNT_FTP
@item nfs - CONFIGURE_SHELL_MOUNT_NFS
@item rfs - CONFIGURE_SHELL_MOUNT_RFS
@end itemize

An example configuration is:

@example
#define CONFIGURE_SHELL_MOUNT_MSDOS
#ifdef RTEMS_NETWORKING
  #define CONFIGURE_SHELL_MOUNT_TFTP
  #define CONFIGURE_SHELL_MOUNT_FTP
  #define CONFIGURE_SHELL_MOUNT_NFS
  #define CONFIGURE_SHELL_MOUNT_RFS
#endif
@end example

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mount

The @code{mount} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mount(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mount} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MOUNT_Command;
@end example

@c
@c
@c
@page
@subsection unmount - unmount disk

@pgindex unmount

@subheading SYNOPSYS:

@example
unmount path
@end example

@subheading DESCRIPTION:

This command unmounts the device at the specified @code{path}.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

TBD - Surely there must be some warnings to go here.

@subheading EXAMPLES:

The following is an example of how to use @code{unmount}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_UNMOUNT
@findex CONFIGURE_SHELL_COMMAND_UNMOUNT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_UNMOUNT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_UNMOUNT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_unmount

The @code{unmount} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_unmount(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{unmount} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_UNMOUNT_Command;
@end example

@c
@c
@c
@page
@subsection blksync - sync the block driver

@pgindex blksync

@subheading SYNOPSYS:

@example
blksync driver
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{blksync}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_BLKSYNC
@findex CONFIGURE_SHELL_COMMAND_BLKSYNC

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_BLKSYNC} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_BLKSYNC} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_blksync

The @code{blksync} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_blksync(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{blksync} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_BLKSYNC_Command;
@end example

@c
@c
@c
@page
@subsection dd - convert and copy a file

@pgindex dd

@subheading SYNOPSYS:

@example
dd [operands ...]
@end example

@subheading DESCRIPTION:

The dd utility copies the standard input to the standard output.
Input data is read and written in 512-byte blocks.  If input reads are
short, input from multiple reads are aggregated to form the output
block.  When finished, dd displays the number of complete and partial
input and output blocks and truncated input records to the standard
error output.

The following operands are available:

@table @b
@item bs=n
Set both input and output block size, superseding the ibs and obs
operands.  If no conversion values other than noerror, notrunc or sync
are specified, then each input block is copied to the output as a
single block without any aggregation of short blocks.

@item cbs=n
Set the conversion record size to n bytes.  The conversion record size
is required by the record oriented conversion values.

@item count=n
Copy only n input blocks.

@item files=n
Copy n input files before terminating.  This operand is only
applicable when the input device is a tape.

@item ibs=n
Set the input block size to n bytes instead of the default 512.

@item if=file
Read input from file instead of the standard input.

@item obs=n
Set the output block size to n bytes instead of the default 512.

@item of=file
Write output to file instead of the standard output.  Any regular
output file is truncated unless the notrunc conversion value is
specified.  If an initial portion of the output file is skipped (see
the seek operand) the output file is truncated at that point.

@item seek=n
Seek n blocks from the beginning of the output before copying.  On
non-tape devices, a @i{lseek} operation is used.  Otherwise, existing
blocks are read and the data discarded.  If the seek operation is past
the end of file, space from the current end of file to the specified
offset is filled with blocks of NUL bytes.

@item skip=n
Skip n blocks from the beginning of the input before copying.  On
input which supports seeks, a @i{lseek} operation is used.  Otherwise,
input data is read and discarded.  For pipes, the correct number of
bytes is read.  For all other devices, the correct number of blocks is
read without distinguishing between a partial or complete block being
read.

@item progress=n
Switch on display of progress if n is set to any non-zero value.  This
will cause a ``.'' to be printed (to the standard error output) for
every n full or partial blocks written to the output file.

@item conv=value[,value...]
Where value is one of the symbols from the following list.

@table @b
@item ascii, oldascii
The same as the unblock value except that characters are translated
from EBCDIC to ASCII before the records are converted.  (These values
imply unblock if the operand cbs is also specified.)  There are two
conversion maps for ASCII.  The value ascii specifies the recom-
mended one which is compatible with AT&T System V UNIX.  The value
oldascii specifies the one used in historic AT&T and pre 4.3BSD-Reno
systems.

@item block
Treats the input as a sequence of newline or end-of-file terminated
variable length records independent of input and output block
boundaries.  Any trailing newline character is discarded.  Each
input record is converted to a fixed length output record where the
length is specified by the cbs operand.  Input records shorter than
the conversion record size are padded with spaces.  Input records
longer than the conversion record size are truncated.  The number of
truncated input records, if any, are reported to the standard error
output at the completion of the copy.

@item ebcdic, ibm, oldebcdic, oldibm
The same as the block value except that characters are translated from
ASCII to EBCDIC after the records are converted.  (These values imply
block if the operand cbs is also specified.)  There are four
conversion maps for EBCDIC.  The value ebcdic specifies the
recommended one which is compatible with AT&T System V UNIX.  The
value ibm is a slightly different mapping, which is compatible with
the AT&T System V UNIX ibm value.  The values oldebcdic and oldibm are
maps used in historic AT&T and pre 4.3BSD-Reno systems.

@item lcase
Transform uppercase characters into lowercase characters.

@item noerror
Do not stop processing on an input error.  When an input error occurs,
a diagnostic message followed by the current input and output block
counts will be written to the standard error output in the same format
as the standard completion message.  If the sync conversion is also
specified, any missing input data will be replaced with NUL bytes (or
with spaces if a block oriented conversion value was specified) and
processed as a normal input buffer.  If the sync conversion is not
specified, the input block is omitted from the output.  On input files
which are not tapes or pipes, the file offset will be positioned past
the block in which the error occurred using lseek(2).

@item notrunc
Do not truncate the output file.  This will preserve any blocks in the
output file not explicitly written by dd.  The notrunc value is not
supported for tapes.

@item osync
Pad the final output block to the full output block size.  If the
input file is not a multiple of the output block size after
conversion, this conversion forces the final output block to be the
same size as preceding blocks for use on devices that require
regularly sized blocks to be written.  This option is incompatible
with use of the bs=n block size specification.

@item sparse
If one or more non-final output blocks would consist solely of NUL
bytes, try to seek the output file by the required space instead of
filling them with NULs.  This results in a sparse file on some file
systems.

@item swab
Swap every pair of input bytes.  If an input buffer has an odd number
of bytes, the last byte will be ignored during swapping.

@item sync
Pad every input block to the input buffer size.  Spaces are used for
pad bytes if a block oriented conversion value is specified, otherwise
NUL bytes are used.

@item ucase
Transform lowercase characters into uppercase characters.

@item unblock
Treats the input as a sequence of fixed length records independent of
input and output block boundaries.  The length of the input records is
specified by the cbs operand.  Any trailing space characters are
discarded and a newline character is appended.
@end table
@end table

Where sizes are specified, a decimal number of bytes is expected.  Two
or more numbers may be separated by an ``x'' to indicate a product.
Each number may have one of the following optional suffixes:
@table @b
@item b
Block; multiply by 512
@item k
Kibi; multiply by 1024 (1 KiB)
@item m
Mebi; multiply by 1048576 (1 MiB)
@item g
Gibi; multiply by 1073741824 (1 GiB)
@item t
Tebi; multiply by 1099511627776 (1 TiB)
@item w
Word; multiply by the number of bytes in an integer
@end table

When finished, dd displays the number of complete and partial input
and output blocks, truncated input records and odd-length
byte-swapping ritten.  Partial output blocks to tape devices are
considered fatal errors.  Otherwise, the rest of the block will be
written.  Partial output blocks to character devices will produce a
warning message.  A truncated input block is one where a variable
length record oriented conversion value was specified and the input
line was too long to fit in the conversion record or was not newline
terminated.

Normally, data resulting from input or conversion or both are
aggregated into output blocks of the specified size.  After the end of
input is reached, any remaining output is written as a block.  This
means that the final output block may be shorter than the output block
size.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{dd}:

@example
SHLL [/] $ dd if=/nfs/boot-image of=/dev/hda1
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DD
@findex CONFIGURE_SHELL_COMMAND_DD

This command is included in the default shell command set.  When
building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_DD} to have this command included.

This command can be excluded from the shell command set by defining
@code{CONFIGURE_SHELL_NO_COMMAND_DD} when all shell commands have been
configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_dd

The @code{dd} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_dd(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{dd} has the following
prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DD_Command;
@end example

@c
@c
@c
@page
@subsection hexdump - ascii/dec/hex/octal dump

@pgindex hexdump

@subheading SYNOPSYS:

@example
hexdump [-bcCdovx] [-e format_string] [-f format_file] [-n length]
        [-s skip] file ...
@end example

@subheading DESCRIPTION:

The hexdump utility is a filter which displays the specified files, or
the standard input, if no files are specified, in a user specified
format.

The options are as follows:

@table @b
@item -b
One-byte octal display.  Display the input offset in hexadecimal,
followed by sixteen space-separated, three column, zero-filled, bytes
of input data, in octal, per line.

@item -c
One-byte character display.  Display the input offset in hexadecimal,
followed by sixteen space-separated, three column, space-filled,
characters of input data per line.

@item -C
Canonical hex+ASCII display.  Display the input offset in hexadecimal,
followed by sixteen space-separated, two column, hexadecimal bytes,
followed by the same sixteen bytes in %_p format enclosed in ``|''
characters.

@item -d
Two-byte decimal display.  Display the input offset in hexadecimal,
followed by eight space-separated, five column, zero-filled, two-byte
units of input data, in unsigned decimal, per line.

@item -e format_string
Specify a format string to be used for displaying data.

@item -f format_file
Specify a file that contains one or more newline separated format
strings.  Empty lines and lines whose first non-blank character is a
hash mark (#) are ignored.

@item -n length
Interpret only length bytes of input.

@item -o
Two-byte octal display.  Display the input offset in hexadecimal,
followed by eight space-separated, six column, zerofilled, two byte
quantities of input data, in octal, per line.

@item -s offset
Skip offset bytes from the beginning of the input.  By default, offset
is interpreted as a decimal number.  With a leading 0x or 0X, offset
is interpreted as a hexadecimal number, otherwise, with a leading 0,
offset is interpreted as an octal number.  Appending the character b,
k, or m to offset causes it to be interpreted as a multiple of 512,
1024, or 1048576, respectively.

@item -v
The -v option causes hexdump to display all input data.  Without the
-v option, any number of groups of output lines, which would be
identical to the immediately preceding group of output lines (except
for the input offsets), are replaced with a line containing a single
asterisk.

@item -x
Two-byte hexadecimal display.  Display the input offset in
hexadecimal, followed by eight, space separated, four column,
zero-filled, two-byte quantities of input data, in hexadecimal, per
line.
@end table

For each input file, hexdump sequentially copies the input to standard
output, transforming the data according to the format strings
specified by the -e and -f options, in the order that they were
specified.

@b{Formats}

A format string contains any number of format units, separated by
whitespace.  A format unit contains up to three items: an iteration
count, a byte count, and a format.

The iteration count is an optional positive integer, which defaults to
one.  Each format is applied iteration count times.

The byte count is an optional positive integer.  If specified it
defines the number of bytes to be interpreted by each iteration of the
format.

If an iteration count and/or a byte count is specified, a single slash
must be placed after the iteration count and/or before the byte count
to disambiguate them.  Any whitespace before or after the slash is
ignored.

The format is required and must be surrounded by double quote (`` ``)
marks.  It is interpreted as a fprintf-style format string (see
@i{fprintf}), with the following exceptions:

@itemize @bullet
@item
An asterisk (*) may not be used as a field width or precision.
@item
A byte count or field precision is required for each ``s'' con-
version character (unlike the fprintf(3) default which prints the
entire string if the precision is unspecified).
@item 
The conversion characters ``h'', ``l'', ``n'', ``p'' and ``q'' are not
supported.
@item
The single character escape sequences described in the C standard
are supported:
@quotation
NUL                  \0
<alert character>    \a
<backspace>          \b
<form-feed>          \f
<newline>            \n
<carriage return>    \r
<tab>                \t
<vertical tab>       \v
@end quotation
@end itemize

Hexdump also supports the following additional conversion strings:

@table @b
@item _a[dox]
Display the input offset, cumulative across input files, of the next
byte to be displayed.  The appended characters d, o, and x specify the
display base as decimal, octal or hexadecimal respectively.

@item _A[dox]
Identical to the _a conversion string except that it is only performed
once, when all of the input data has been processed.

@item _c
Output characters in the default character set.  Nonprinting
characters are displayed in three character, zero-padded octal, except
for those representable by standard escape notation (see above), which
are displayed as two character strings.

@item _p
Output characters in the default character set.  Nonprinting
characters are displayed as a single ``.''.

@item _u
Output US ASCII characters, with the exception that control characters
are displayed using the following, lower-case, names.  Characters
greater than 0xff, hexadecimal, are displayed as hexadecimal
strings.

000 nul  001 soh  002 stx  003 etx  004 eot  005 enq
006 ack  007 bel  008 bs   009 ht   00A lf   00B vt
00C ff   00D cr   00E so   00F si   010 dle  011 dc1
012 dc2  013 dc3  014 dc4  015 nak  016 syn  017 etb
018 can  019 em   01A sub  01B esc  01C fs   01D gs
01E rs   01F us   07F del
@end table

The default and supported byte counts for the conversion characters
are as follows:

@quotation
%_c, %_p, %_u, %c       One byte counts only.

%d, %i, %o, %u, %X, %x  Four byte default, one, two, four
                        and eight byte counts supported.

%E, %e, %f, %G, %g      Eight byte default, four byte
                        counts supported.
@end quotation

The amount of data interpreted by each format string is the sum of the
data required by each format unit, which is the iteration count times
the byte count, or the iteration count times the number of bytes
required by the format if the byte count is not specified.

The input is manipulated in ``blocks'', where a block is defined as
the largest amount of data specified by any format string.  Format
strings interpreting less than an input block's worth of data, whose
last format unit both interprets some number of bytes and does not
have a specified iteration count, have the iteration count incremented
until the entire input block has been processed or there is not enough
data remaining in the block to satisfy the format string.

If, either as a result of user specification or hexdump modifying the
iteration count as described above, an iteration count is greater than
one, no trailing whitespace characters are output during the last
iteration.

It is an error to specify a byte count as well as multiple conversion
characters or strings unless all but one of the conversion characters
or strings is _a or _A.

If, as a result of the specification of the -n option or end-of-file
being reached, input data only partially satisfies a format string,
the input block is zero-padded sufficiently to display all available
data (i.e. any format units overlapping the end of data will display
some num- ber of the zero bytes).

Further output by such format strings is replaced by an equivalent
number of spaces.  An equivalent number of spaces is defined as the
number of spaces output by an s conversion character with the same
field width and precision as the original conversion character or
conversion string but with any ``+'', `` '', ``#'' conversion flag
characters removed, and ref- erencing a NULL string.

If no format strings are specified, the default display is equivalent
to specifying the -x option.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{hexdump}:

@example
SHLL [/] $ hexdump -C -n 512 /dev/hda1
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_HEXDUMP
@findex CONFIGURE_SHELL_COMMAND_HEXDUMP

This command is included in the default shell command set.  When
building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_HEXDUMP} to have this command included.

This command can be excluded from the shell command set by defining
@code{CONFIGURE_SHELL_NO_COMMAND_HEXDUMP} when all shell commands have
been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_hexdump

The @code{hexdump} command is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_hexdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{hexdump} has the following
prototype:

@example
extern rtems_shell_cmd_t rtems_shell_HEXDUMP_Command;
@end example

@c
@c
@c
@page
@subsection fdisk - format disk

@pgindex fdisk

@subheading SYNOPSYS:

@example
fdisk
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_FDISK
@findex CONFIGURE_SHELL_COMMAND_FDISK

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_FDISK} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_FDISK} when all
shell commands have been configured.

@c
@c
@c
@page
@subsection dir - alias for ls

@pgindex dir

@subheading SYNOPSYS:

@example
dir [dir]
@end example

@subheading DESCRIPTION:

This command is an alias or alternate name for the @code{ls}. 
See @ref{File and Directory Commands ls - list files in the directory, ls}
for more information.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{dir}:

@example
SHLL [/] $ dir
drwxr-xr-x   1   root   root         536 Jan 01 00:00 dev/
drwxr-xr-x   1   root   root        1072 Jan 01 00:00 etc/
2 files 1608 bytes occupied
SHLL [/] $ dir etc
-rw-r--r--   1   root   root         102 Jan 01 00:00 passwd 
-rw-r--r--   1   root   root          42 Jan 01 00:00 group 
-rw-r--r--   1   root   root          30 Jan 01 00:00 issue 
-rw-r--r--   1   root   root          28 Jan 01 00:00 issue.net 
4 files 202 bytes occupied
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DIR
@findex CONFIGURE_SHELL_COMMAND_DIR

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_DIR} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_DIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_dir

The @code{dir} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_dir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{dir} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DIR_Command;
@end example

@c
@c
@c
@page
@subsection mkrfs - format RFS file system

@pgindex mkrfs

@subheading SYNOPSYS:

@example
mkrfs [-vsbiIo] device
@end example

@subheading DESCRIPTION:

Format the block device with the RTEMS File System (RFS). The default
configuration with not parameters selects a suitable block size based
on the size of the media being formatted.

The media is broken up into groups of blocks. The number of blocks in
a group is based on the number of bits a block contains. The large a
block the more blocks a group contains and the fewer groups in the
file system.

The following options are provided:

@table @b
@item -v
Display configuration and progress of the format.

@item -s
Set the block size in bytes.

@item -b
The number of blocks in a group. The block count must be equal or less
than the number of bits in a block.

@item -i
Number of inodes in a group. The inode count must be equal or less
than the number of bits in a block.

@item -I
Initialise the inodes. The default is not to initialise the inodes and
to rely on the inode being initialised when allocated. Initialising
the inode table helps recovery if a problem appears.

@item -o
Integer percentage of the media used by inodes. The default is 1%.

@item device
Path of the device to format.
@end table

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{mkrfs}:

@example
SHLL [/] $ mkrfs /dev/fdda
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MKRFS
@findex CONFIGURE_SHELL_COMMAND_MKRFS

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MKRFS} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MKRFS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mkrfs

The @code{mkrfs} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_mkrfs(
  int    argc,
  char **argv
);
@end example

The configuration structure for @code{mkrfs} has the following
prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MKRFS_Command;
@end example

@c
@c
@c
@page
@subsection debugrfs - debug RFS file system

@pgindex debugrfs

@subheading SYNOPSYS:

@example
debugrfs [-hl] path command [options] 
@end example

@subheading DESCRIPTION:

The command provides debugging information for the RFS file system.

The options are:

@table @b
@item -h
Print a help message.

@item -l
List the commands.

@item path
Path to the mounted RFS file system. The file system has to be mounted
to view to use this command.
@end table

The commands are:

@table @b
@item block start [end]
Display the contents of the blocks from start to end.

@item data
Display the file system data and configuration. 

@item dir bno
Process the block as a directory displaying the entries.

@item group start [end]
Display the group data from the start group to the end group.

@item inode [-aef] [start] [end]
Display the inodes between start and end. If no start and end is
provides all inodes are displayed.

@table @b
@item -a
Display all inodes. That is allocated and unallocated inodes.
@item -e
Search and display on inodes that have an error.
@item -f
Force display of inodes, even when in error.
@end table
@end table

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{debugrfs}:

@example
SHLL [/] $ debugrfs /c data
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DEBUGRFS
@findex CONFIGURE_SHELL_COMMAND_DEBUGRFS

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_DEBUGRFS} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_DEBUGRFS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_debugrfs

The @code{debugrfs} command is implemented by a C language function which
has the following prototype:

@example
int rtems_shell_rtems_main_debugrfs(
  int    argc,
  char **argv
);
@end example

The configuration structure for @code{debugrfs} has the following
prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DEBUGRFS_Command;
@end example

@c
@c
@c
@page
@subsection cd - alias for chdir

@pgindex cd

@subheading SYNOPSYS:

@example
cd directory
@end example

@subheading DESCRIPTION:

This command is an alias or alternate name for the @code{chdir}. 
See @ref{File and Directory Commands chdir - change the current directory, cd}
for more information.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{cd}:

@example
SHLL [/] $ cd etc
SHLL [/etc] $ cd /
SHLL [/] $ cd /etc
SHLL [/etc] $ pwd
/etc
SHLL [/etc] $ cd /
SHLL [/] $ pwd
/
SHLL [/] $ cd etc
SHLL [/etc] $ cd ..
SHLL [/] $ pwd
/
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CD
@findex CONFIGURE_SHELL_COMMAND_CD

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CD} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cd

The @code{cd} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_cd(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{cd} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CD_Command;
@end example

