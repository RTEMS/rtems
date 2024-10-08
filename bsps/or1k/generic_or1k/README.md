Generic or1k
============

This BSP can run on or1ksim, QEMU, jor1k [1] and OpenRISC supported FPGA boards.

```shell
$ git clone git@github.com:openrisc/or1ksim.git
$ cd or1ksim
$ mkdir builddir_or1ksim
$ cd builddir_or1ksim
$ ../configure --target=or1k-elf --prefix=/opt/or1ksim
$ make all
$ make install
$ export PATH=/opt/or1ksim/bin:$PATH
```

Configuration file "sim.cfg" should be provided for complex board
configurations at the current directory (which you run or1ksim from) or at
~/.or1k/

The current sim.cfg file that configures or1ksim emulator to RTEMS/or1ksim BSP
is at the same directory as this README. You can also use or1ksim script from
rtems-tools/sim-scripts.

From command line type:

```shell
$ or1k-elf-sim -f sim.cfg $PATH_TO_RTEMS_EXE
```
From QEMU:

```shell
$ qemu-system-or32 -serial mon:stdio -serial /dev/null -net none -nographic \
  -m 128M -kernel $PATH_TO_RTEMS_EXE
```

from sim-scripts:

```shell
$ or1ksim $PATH_TO_RTEMS_EXE
$ qemu-or1k $PATH_TO_RTEMS_EXE
```

[1] http://s-macke.github.io/jor1k/demos/rtems.html
