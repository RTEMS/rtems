Shell
=====

This directory contains a shell user extension
primary features:

   + create a user shell terminal task.

This code has not been extensively tested.  It is provided as a tool
for RTEMS users to open more shell terminal.
Suggestions and comments are appreciated.

NOTES:

1.  printf() & getchar() works but you can't use 
    0,1,2 like fildes. You need to use fileno(stdin),fileno(stdout),...
    
2.  You only need a termios dev to start a new session, add your new commands
    and enjoy it.

3.  Telnetd daemon uses this (browse libnetworking/rtems_telnetd)
    Enjoy it.

FUTURE:

1.  Adding new commands in cmds.c to give file manegement to shell.
