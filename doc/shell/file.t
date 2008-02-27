@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter File and Directory Commands

@section Introduction

The RTEMS shell has the following file and directory commands:

@itemize @bullet

@item @code{umask} - Set file mode creation mask
@item @code{cp} - copy files
@item @code{pwd} - print work directory
@item @code{ls} - list files in the directory
@item @code{chdir} - change the current directory
@item @code{mkdir} - create a directory
@item @code{rmdir} - remove empty directories
@item @code{chroot} - change the root directory
@item @code{chmod} - change permissions of a file
@item @code{cat} - display file contents
@item @code{msdosfmt} - format disk
@item @code{rm} - remove files
@item @code{mount} - mount disk
@item @code{unmount} - unmount disk
@item @code{blksync} - sync the block driver
@item @code{dir} - alias for ls
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

The following is an example of how to use @code{cp}:

@example
EXAMPLE_TBD
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

The @code{cp} is implemented by a C language function
which has the following prototype:

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

The implementation and documentation for this command are from
NetBSD 4.0.

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

The following is an example of how to use @code{chroot}:

@example
EXAMPLE_TBD
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
@end itemize

When the file system type is 'msdos' the driver is a "block device driver"
node present in the file system. The driver is ignored with the 'tftp' and
'ftp' file systems. For the 'nfs' file system the driver is the 'host:/path'
string that described NFS host and the exported file system path.

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
$ mount -t msdos /dev/flashdisk0 /fd
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
@end itemize

An example configuration is:

@example
#define CONFIGURE_SHELL_MOUNT_MSDOS
#ifdef RTEMS_NETWORKING
  #define CONFIGURE_SHELL_MOUNT_TFTP
  #define CONFIGURE_SHELL_MOUNT_FTP
  #define CONFIGURE_SHELL_MOUNT_NFS
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

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

TBD

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
EXAMPLE_TBD
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
EXAMPLE_TBD
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

