lm3s69xx
========

Tested only on Qemu simulator with git (git://git.qemu.org/qemu.git) version
1.0.50.

You have to apply the patches contained in this directory.

Command line:

qemu-system-arm -S -s -net none -nographic -M lm3s6965evb -kernel hello.bin
