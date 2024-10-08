i386/pc386 GDB Stub
===================

The i386 GDB stub has been updated to use the libchip drivers for the
NS16550. Make sure you have detect the device and you have added a console
entry. On the PC the legacy and PCI boards are supported.

This GDB stub glue code is specific to the UART protocol defined in libbchip.

The pc386 BSP has boot command line options to manage GDB support.

a) Find the minor number of the console device:

```c
  #include <console_private.h>

  rtems_device_minor_number minor = 0;

  if (console_find_console_entry("/dev/com1",
                                 strlen("/dev/com1") - 1, &minor) == NULL)
    error("driver not found\n");
```

Note, this call is part of the private console API and may change.

b) To start GDB stub, run this:

```c
  #include <bsp.h>

  /* Init GDB glue  */
  i386_stub_glue_init(minor);

  /* Init GDB stub itself */
  set_debug_traps();

  /*
   * Init GDB break in capability,
   * has to be called after
   * set_debug_traps
   */
  i386_stub_glue_init_breakin();

  /* Put breakpoint in */
  breakpoint();

c) To run use GDB:
```

```shell
  $ i386-rtems4.12-gdb hello.exe
  GNU gdb (GDB) 7.11
  Copyright (C) 2016 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "--host=x86_64-freebsd10.1 --target=i386-rtems4.12".
  Type "show configuration" for configuration details.
  For bug reporting instructions, please see:
  <http://www.gnu.org/software/gdb/bugs/>.
  Find the GDB manual and other documentation resources online at:
  <http://www.gnu.org/software/gdb/documentation/>.
  For help, type "help".
  Type "apropos word" to search for commands related to "word"...
  Reading symbols from hello.exe...done.
  (gdb) target remote /dev/cuaU5
  Remote debugging using /dev/cuaU5
  0x00103fda in breakpoint () at i386-stub.c:1004
  1004          BREAKPOINT ();
  (gdb) b Init
  Breakpoint 1 at 0x1001e0: file init.c, line 29.
  (gdb) c
  Continuing.

  Breakpoint 1, Init (ignored=1269800) at init.c:29
  29      {
  (gdb)
```

Pressing ^C works and if running the board should halt when GDB connects.

e) Use ser2net to provide reomve access over a network to a board. Install the
ser2net package and add a configuration for the port GDB connects to. For
example:

```shell
 0005:raw:0:/dev/cuaU5:115200
```

Start ser2net running then connect GDB using:

```shell
 (gdb) target remote myhost:30005
```
