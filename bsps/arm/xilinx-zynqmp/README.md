xilinx-zynqmp
=============

Tested on an Ultra96 board with JTAG boot.

Make sure to configure the boot mode switches for JTAG mode.

Using the xsct tool, load and run the application with

connect

targets -set -filter {name =~ "PSU"}
rst -system
source psu_init.tcl
psu_init

targets -set -filter {name =~ "Cortex-A53 #0"}
rst -processor
dow ticker.exe

# Set generic timer frequency
rwr sys 14 cntfrq_el0 100000000

# Switch to AArch32 Supervisor mode
rwr cpsr 0x2001d3

# Boot arguments
rwr r0 0
rwr r1 0
rwr r2 0
rwr r3 0

con
