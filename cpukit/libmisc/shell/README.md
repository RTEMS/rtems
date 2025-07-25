# Shell

This directory contains a shell user extension
primary features:

- create a user shell terminal task.

This code has not been extensively tested. It is provided as a tool
for RTEMS users to open more shell terminal.
Suggestions and comments are appreciated.

NOTES:

1. printf() & getchar() works but you can't use
   0,1,2 like fildes. You need to use fileno(stdin),fileno(stdout),...

2. You only need a termios dev to start a new session, add your new commands
   and enjoy it.

3. Telnetd daemon uses this.
   Enjoy it.

FUTURE:

1. Adding new commands in cmds.c to give file manegement to shell.

## Source locations

Below is the location of where the RTEMS Shell source was derived.

### FreeBSD

If any are empty and you know the location please submit an MR.

| RTEMS Shell        | Location                       |
| :----------------- | :----------------------------- |
| warn.c             |                                |
| warnx.c            |                                |
| vwarnx.c           |                                |
| errx.c             |                                |
| verr.c             |                                |
| verrx.c            |                                |
| vwarn.c            |                                |
| dd.h               | bin/dd/dd.h                    |
| cmp.c              | bin/ls/cmp.c                   |
| mv.c               | bin/mv/mv.c                    |
| vis.c              | contrib/libc-vis/vis.c         |
| vis.h              | contrib/libc-vis/vis.h         |
| err.h              | include/err.h                  |
| fts.h              | include/fts.h                  |
| err.c              | lib/libc/gen/err.c             |
| fts.c              | lib/libc/gen/fts.c             |
| main_cp.c          | bin/cp/cp.c                    |
| utils-cp.c         | bin/cp/utils.c                 |
| dd-args.c          | bin/dd/args.c                  |
| dd-conv.c          | bin/dd/conv.c                  |
| main_dd.c          | bin/dd/dd.c                    |
| extern-dd.h        | bin/dd/extern.h                |
| dd-misc.c          | bin/dd/misc.c                  |
| main_echo.c        | bin/echo/echo.c                |
| main_ln.c          | bin/ln/ln.c                    |
| main_ls.c          | bin/ls/ls.c                    |
| extern-ls.h        | bin/ls/ls.h                    |
| print-ls.c         | bin/ls/print.c                 |
| utils-ls.c         | bin/ls/util.c                  |
| main_rm.c          | bin/rm/rm.c                    |
| extern-cp.h        | include/extern.h               |
| sysexits.h         | include/sysexits.h             |
| pwcache.c          | contrib/libc-pwcache/pwcache.c |
| filemode.c         | lib/libc/string/strmode.c      |
| hexdump.h          | usr.bin/hexdump/hexdump.h      |
| hexsyntax.c        | usr.bin/hexdump/hexsyntax.c    |
| dd-conv_tab.c      | bin/dd/conv_tab.c              |
| dd-position.c      | bin/dd/position.c              |
| main_mknod.c       | sbin/mknod/mknod.c             |
| hexdump-conv.c     | usr.bin/hexdump/conv.c         |
| main_hexdump.c     | usr.bin/hexdump/hexdump.c      |
| hexdump-parse.c    | usr.bin/hexdump/parse.c        |
| mknod-pack_dev.c   | contrib/mknod/pack_dev.c       |
| mknod-pack_dev.h   | contrib/mknod/pack_dev.h       |
| hexdump-display.c  | usr.bin/hexdump/display.c      |
| hexdump-odsyntax.c | usr.bin/hexdump/odsyntax.c     |

### Sanos

| RTEMS Shell | Location                                      |
| :---------- | :-------------------------------------------- |
| main_edit.c | https://texteditors.org/cgi-bin/wiki.pl?Sanos |
