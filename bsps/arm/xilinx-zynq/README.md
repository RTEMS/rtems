Xilinx Zynq Support
===================

The Zynq is supports the common hard IP that is common to all
versions of the Zynq device family.

MMU
---

To access the PL (FPGA Logic) create an MMU table in your application
and create the entries that map to your FPGA IP.

QEMU
----

Tested only on Qemu.

git clone git://git.qemu.org/qemu.git qemu
cd qemu
git checkout 1b0d3845b454eaaac0b2064c78926ca4d739a080
mkdir build
cd build
../configure --prefix=/opt/qemu --interp-prefix=/opt/qemu
make
make install
export PATH="$PATH:/opt/qemu/bin"

qemu-system-arm -no-reboot -serial null -serial mon:stdio -net none -nographic -M xilinx-zynq-a9 -m 256M -kernel ticker.exe
